public class Percolation {
    // create n-by-n grid, with all sites blocked
    public Percolation(int n)  {
        mData = new [n][n];
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                mData[i][j] = 0;
        mNum = n;
    }
    // open site (row, col) if it is not open already
    public void open(int row, int col) {
        if (row < 1 || row > Num)
            return;
        if (col < 1 || col > Num)
            return;
        mData[row - 1][col - 1] = 1;
    }
    public boolean isOpen(int row, int col)  // is site (row, col) open?
    public boolean isFull(int row, int col)  // is site (row, col) full?
    public     int numberOfOpenSites()       // number of open sites
    public boolean percolates()              // does the system percolate?
    public static void main(String[] args)   // test client (optional)
private:
    int mData[][];
    int mNum;
}

