/*
* Copyright (C) 2015 2nd Foundation
* All rights reserved
*
*/

double findMedianSortedArrays(int* nums1, int nums1Size, int* nums2, int nums2Size) 
{
    int nm = nums1Size + nums2Size;
    int * pTemp = (int *)malloc(nm*sizeof(int));
    memset(pTemp, 0, nm*sizeof(int));
    int count = 0;
    int i = 0;
    int j = 0;
    
    while (i < nums1Size && j < nums2Size)
    {
        if (nums1[i] < nums2[j])
            pTemp[count++] = nums1[i++];
        else
            pTemp[count++] = nums2[j++];
    }
    
    while (i < nums1Size)
        pTemp[count++] = nums1[i++];
        
    while (j < nums2Size)
        pTemp[count++] = nums2[j++];
        
    int even_1 = 0;
    int even_2 = 0;
    int odd = 0;
    int temp = count / 2;
    temp--;
    double ret = 0.0;
    
    if (count % 2 == 0)
    {
        even_1 = temp;
        even_2 = temp + 1;
        ret = (pTemp[even_1] + pTemp[even_2]) / 2.0;
    }
    else
    {
        odd = temp + 1;
        ret = pTemp[odd];
    }
    
    free(pTemp);
    return ret;
}
