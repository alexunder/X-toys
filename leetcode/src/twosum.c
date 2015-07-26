/*
* Copyright (C) 2015 2nd Foundation
* All rights reserved
*
* Note: The returned array must be malloced, assume caller calls free().
*/
 
typedef int (*compfn)(const void*, const void*);

typedef struct _data_with_index
{
    int v;
    int i;
} data_index;

int compare(data_index *elem1, data_index *elem2)
{
    if ( elem1->v < elem2->v)
       return -1;

    else if (elem1->v > elem2->v)
       return 1;

    else
       return 0;
}
 
int* twoSum(int* nums, int numsSize, int target) 
{
    data_index * p = (data_index*)malloc(numsSize*sizeof(data_index));
    memset(p, 0, numsSize*sizeof(int));
    
    int i;
    
    for (i = 0; i < numsSize; i++)
    {
        p[i].v = nums[i];
        p[i].i = i + 1;
    }
    
    qsort((void *)p, numsSize, sizeof(data_index), (compfn)compare);
    
    int * result = (int *)malloc(2*sizeof(int));
    memset(result, 0, 2*sizeof(int));
    
    int l = 0;
    int r = numsSize - 1;
    
    while (l < r)
    {
        if (p[l].v + p[r].v > target)
        {
            r--;
        }
        else if (p[l].v + p[r].v < target)
        {
            l++;
        }
        else
        {
            if (p[l].i > p[r].i)
            {
                result[0] = p[r].i;
                result[1] = p[l].i;
            }
            else
            {
                result[0] = p[l].i;
                result[1] = p[r].i;
            }
            free(p);
            return result;
        }
    }
    
    free(p);
    return NULL;
}
