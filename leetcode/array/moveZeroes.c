/*
  Given an array nums, write a function to move all 0's to the end of it while maintaining the relative order of the non-zero elements.
 */

void moveZeroes(int* nums, int numsSize)
{
    int i;
    int j = 0;

    for (i = 0; i < numsSize; i++)
    {
        if (nums[i] == 0)
            continue;
            
        if (i > j)
        {
            nums[j] = nums[i];
            nums[i] = 0;
        }
        
        j++;
    }
}
