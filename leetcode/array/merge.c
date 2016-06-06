/*
  Given two sorted integer arrays nums1 and nums2, merge nums2 into nums1 as one sorted array.
 */

void merge(int* nums1, int m, int* nums2, int n)
{
    int i = 0;
    int j = 0;
    int k;

    while ( i < m && j < n)
    {
        if (nums1[i] > nums2[j])
        {
            for (k = m - 1; k >= i; k--)
                nums1[k + 1] = nums1[k];

            nums1[i] = nums2[j];
            m++;
            j++;
            i++;
        }
        else
        {
            i++;
        }
    }

    if (j < n)
    {
        for (k = j; k < n; k++)
            nums1[i++] = nums2[k]; 
    }
}
