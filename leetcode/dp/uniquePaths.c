/*
 A robot is located at the top-left corner of a m x n grid (marked 'Start' in the diagram below).
 The robot can only move either down or right at any point in time. The robot is trying to reach the bottom-right corner of the grid (marked 'Finish' in the diagram below).
 How many possible unique paths are there?
 */

// My fist recursive solution

int uniquePaths(int m, int n) {
    if (m == 1 || n == 1)
        return 1;
    else
        return uniquePaths(m - 1, n) + uniquePaths(m, n - 1);
}

// The second version

int uniquePaths(int m, int n) {
    int * table = (int *)malloc(m*n*sizeof(int));
    memset(table, 0, m*n*sizeof(int));

    int i;
    int j;

    for (j = 0; j < m; j++)
        for (i = 0; i < n; i++) {
            if (i == 0 || j == 0) {
                if (i == 0 && j == 0)
                    table[0] = 1;
                else if (i == 0)
                    table[i + j*n] = table[i + (j - 1)*n];
                else if (j == 0)
                    table[i + j*n] = table[i - 1 + j*n]; 
            } else
                table[i + j*n] = table[i - 1 + j*n] + table[i + (j - 1)*n];
        }

    int ret = table[n*m - 1];
    free(table);
    return ret;
}
