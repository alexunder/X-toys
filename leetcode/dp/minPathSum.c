/*
 Given a m x n grid filled with non-negative numbers, 
 find a path from top left to bottom right which minimizes the sum of all numbers along its path.
 */
int min(int v1, int v2)
{
    return v1 < v2 ? v1 : v2;
}

int minPathSum(int** grid, int gridRowSize, int gridColSize) {
    int m = gridRowSize;
    int n = gridColSize;

    int * table = (int *)malloc(m*n*sizeof(int));
    memset(table, 0, m*n*sizeof(int));

    int i;
    int j;

    for (j = 0; j < m; j++)
        for (i = 0; i < n; i++) {
            if (i == 0 || j == 0) {
                if (i == 0 && j == 0)
                    table[0] = grid[0][0];
                else if (i == 0)
                    table[i + j*n] = grid[j][i] + table[i + (j - 1)*n];
                else if (j == 0)
                    table[i + j*n] = grid[j][i] + table[i - 1 + j*n]; 
            } else
                table[i + j*n] = grid[j][i] + min(table[i - 1 + j*n], table[i + (j - 1)*n]);
        }

    int ret = table[n*m - 1];
    free(table);
    return ret;
}
