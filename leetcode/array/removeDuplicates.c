/*
 Given a sorted array, remove the duplicates in place such that each element appear only once and return the new length.

 Do not allocate extra space for another array, you must do this in place with constant memory.

 For example,
 Given input array nums = [1,1,2],

 Your function should return length = 2, with the first two elements of nums being 1 and 2 respectively. It doesn't matter what you leave beyond the new length. 
*/


int removeDuplicates(int* nums, int numsSize)
{
    int i = 1;
    int newSize = numsSize;
    int tempCnt = 1;
    int start = 0;
    bool isInSame = false;
    int j;

    while (i < numsSize)
    {
        if (nums[i] == nums[i - 1])
        {
            if (isInSame == false)
            {
                isInSame = true;
            }

            newSize--;
        }
        else
        {
            if (isInSame == true)
            {
                isInSame = false;
                nums[++start] = nums[i];
            }
            else
            {
                nums[++start] = nums[i];
            }
        }

        i++;
    }

    return newSize;
}
