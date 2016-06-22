/*
 Follow up for "Unique Paths":
 Now consider if some obstacles are added to the grids. How many unique paths would there be?
 An obstacle and empty space is marked as 1 and 0 respectively in the grid.
 */

int uniquePathsWithObstacles(int** obstacleGrid, int obstacleGridRowSize, int obstacleGridColSize) {
    int m = obstacleGridRowSize;
    int n = obstacleGridColSize;
    int * table = (int *)malloc(m*n*sizeof(int));
    memset(table, 0, m*n*sizeof(int));

    int i;
    int j;

    for (j = 0; j < m; j++)
        for (i = 0; i < n; i++) {
            if (i == 0 || j == 0) {
                if (obstacleGrid[j][i] == 1)
                    table[i + j*n] = 0;
                else {
                    if (i == 0 && j == 0)
                        table[0] = 1;
                    else if (i == 0)
                        table[i + j*n] = table[i + (j - 1)*n];
                    else if (j == 0)
                        table[i + j*n] = table[i - 1 + j*n];
                }
            } else {
                if (obstacleGrid[j][i] == 1)
                    table[i + j*n] = 0;
                else
                    table[i + j*n] = table[i - 1 + j*n] + table[i + (j - 1)*n];
            }
        }

    int ret = table[n*m - 1];
    free(table);
    return ret;
}
