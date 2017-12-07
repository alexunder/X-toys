#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* convert(char* s, int numRows) {
    if (numRows == 1)
        return s;

    int lens = strlen(s);
    int basic_width = 2*numRows - 2;
    int abby = lens/basic_width;
    int bitch = lens%basic_width;

    int totalWidth = abby * (numRows - 1);
    if (bitch <= numRows)
        totalWidth += 1;
    else
        totalWidth += 1 + bitch - numRows;

	printf("totalWidth=%d\n", totalWidth);
	printf("numRows=%d\n", numRows);
    char * matrix = (char*)malloc(totalWidth*numRows*sizeof(char));
    memset(matrix, 0, totalWidth*numRows*sizeof(char));

    int i;
    int startx = 0;
    int starty = 0;
    matrix[0] = s[0];
    for (i = 1; i < lens; i++)
    {
        int temp = i % basic_width;
        if ((temp < numRows) && (temp > 0))
        {
            starty++;
        }
        else
        {
            startx++;
            starty--;

        }
        matrix[startx + starty*totalWidth] = s[i];
    }

	//Dump the matrix
	printf("The Matrices are:\n");
    for (i = 0; i < totalWidth*numRows; i++)
    {
		if (matrix[i] > 0)
			printf("%c", matrix[i]);
		else
			printf("%d", matrix[i]);

		if (i % totalWidth == (totalWidth - 1))
			printf("\n");
		else
			printf(" ");
	}

    int cnt = 0;
    for (i = 0; i < totalWidth*numRows; i++)
    {
        if (matrix[i] != 0)
            s[cnt++] = matrix[i];
    }

    free(matrix);
    return s;
}

int main()
{
    char input[] = "PAHNAPLSIIGYIR";
    printf("Before input=%s\n", input);
    printf("s=%s\n", convert(input, 3));
}
