public class Percolation {
    // create n-by-n grid, with all sites blocked
    public Percolation(int n)  {
        mData = new int[n][n];
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                mData[i][j] = 0;
        mNum = n;
    }
    // open site (row, col) if it is not open already
    public void open(int row, int col) {
        if (row < 1 || row > mNum)
            throw new IndexOutOfBoundsException();
        if (col < 1 || col > mNum)
            throw new IndexOutOfBoundsException();

        if (mData[row - 1][col - 1] == 0)
            mData[row - 1][col - 1] = 1;
    }
    // is site (row, col) open?
    public boolean isOpen(int row, int col) {
        if (row < 1 || row > mNum)
            throw new IndexOutOfBoundsException();
        if (col < 1 || col > mNum)
            throw new IndexOutOfBoundsException();
        return (mData[row - 1][col - 1] == 1);
    }
    // is site (row, col) full?
    public boolean isFull(int row, int col) {
        if (row < 1 || row > mNum)
            throw new IndexOutOfBoundsException();
        if (col < 1 || col > mNum)
            throw new IndexOutOfBoundsException();
        return (mData[row - 1][col - 1] == 2);
    }
    // number of open sites
    public int numberOfOpenSites() {
        int i;
        int j;
        int num_open_sites = 0;
        for (i = 0; i < mNum; i++)
            for (j = 0; j < mNum; j++) {
            if (mData[i][j] != 0)
                num_open_sites++;
        }
        return num_open_sites;
    }
    // does the system percolate?
    public boolean percolates() {
       return false;
    }
    // test client (optional)
    public static void main(String[] args) {
        return;
    }

    private int mData[][];
    private  int mNum;
}

