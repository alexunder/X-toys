/* *****************************************************************************
 *  Name:              Alan Turing
 *  Coursera User ID:  123456
 *  Last modified:     1/1/2019
 **************************************************************************** */

import edu.princeton.cs.algs4.WeightedQuickUnionUF;

public class Percolation {

    private static final int BLOCKED = 0;
    private static final int OPENED  = 1;

    private int mGrid[][];
    private int mEdgeSize = 10;
    private WeightedQuickUnionUF mWQUnionFind;
    private int mNumberComponents;

    // creates n-by-n grid, with all sites initially blocked
    public Percolation(int n) {
        if (n <= 0)
            throw new IllegalArgumentException("n is illegal!");

        mGrid = new int[n][n];
        mEdgeSize = n;
        mWQUnionFind = new WeightedQuickUnionUF(n*n);
        mNumberComponents = n*n;

        int i, j;
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                mGrid[i][j] = 0;
            }
        }
    }

    // opens the site (row, col) if it is not open already
    public void open(int row, int col) {
        if (row < 1 || row > mEdgeSize )
            throw new IllegalArgumentException("Row is not valid!");

        if (col < 1 || col > mEdgeSize )
            throw new IllegalArgumentException("Col is not valid!");

        int real_row = row - 1;
        int real_col = col - 1;

        if (mGrid[real_row][real_col] != OPENED)
            mGrid[real_row][real_col] = OPENED;

        //Check the surrounding cells
        if (real_row - 1 >= 0) {
            if (mGrid[real_row - 1][real_col] == OPENED) {
                mWQUnionFind.union((real_row - 1) * mEdgeSize + real_col,
                                   real_row * mEdgeSize + real_col);
                mNumberComponents--;
            }
        }
        if (real_row + 1 < mEdgeSize) {
            if (mGrid[real_row + 1][real_col] == OPENED) {
                mWQUnionFind.union((real_row + 1) * mEdgeSize + real_col,
                                   real_row * mEdgeSize + real_col);
                mNumberComponents--;
            }
        }
        if (real_col - 1 >= 0) {
            if (mGrid[real_row][real_col - 1] == OPENED) {
                mWQUnionFind.union(real_row * mEdgeSize + real_col - 1,
                                   real_row * mEdgeSize + real_col);
                mNumberComponents--;
            }
        }
        if (real_col + 1 < mEdgeSize) {
            if (mGrid[real_row][real_col + 1] == OPENED) {
                mWQUnionFind.union(real_row * mEdgeSize + real_col + 1,
                                   real_row * mEdgeSize + real_col);
                mNumberComponents--;
            }
        }
    }

    // is the site (row, col) open?
    public boolean isOpen(int row, int col) {
        if (row < 1 || row > mEdgeSize )
            throw new IllegalArgumentException("Row is not valid!");

        if (col < 1 || col > mEdgeSize )
            throw new IllegalArgumentException("Col is not valid!");

        return mGrid[row - 1][col - 1] == OPENED;
    }

    // is the site (row, col) full?
    public boolean isFull(int row, int col) {
        if (row < 1 || row > mEdgeSize )
            throw new IllegalArgumentException("Row is not valid!");

        if (col < 1 || col > mEdgeSize )
            throw new IllegalArgumentException("Col is not valid!");

        int i;

        for (i = 0; i < mEdgeSize; i++) {
            if (mWQUnionFind.find(row*mEdgeSize + col) == i) {
                return true;
            }
        }

        return false;
    }

    // returns the number of open sites
    public int numberOfOpenSites() {
        return mWQUnionFind.count() - mNumberComponents;
    }

    // does the system percolate?
    public boolean percolates() {
        int i, j;
        for (i = 0; i < mEdgeSize; i++) {
            for (j = 0; j < mEdgeSize; j++) {
                if (mWQUnionFind.find((mEdgeSize - 1)*mEdgeSize + i) == j) {
                    return true;
                }
            }
        }

        return false;
    }
    public static void main(String[] args) {

    }
}
