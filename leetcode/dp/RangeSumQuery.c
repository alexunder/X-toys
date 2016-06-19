/*
 Given an integer array nums, find the sum of the elements between indices i and j (i ≤ j), inclusive.

 Example:

 Given nums = [-2, 0, 3, -5, 2, -1]

 sumRange(0, 2) -> 1
 sumRange(2, 5) -> -1
 sumRange(0, 5) -> -3

 Note:

     You may assume that the array does not change.
         There are many calls to sumRange function.

 */

typedef struct NumArray {
    int size;
    int * prefix;
} NumArray;

/** Initialize your data structure here. */
struct NumArray* NumArrayCreate(int* nums, int numsSize) {
    int i;

    NumArray * pArray = (NumArray *)malloc(sizeof(NumArray));
    pArray->size = numsSize;
    pArray->prefix = (int*)malloc((numsSize + 1)*sizeof(int));

    pArray->prefix[0] = 0;
    for (i = 1; i < numsSize + 1; i++)
    {
        pArray->prefix[i] = pArray->prefix[i - 1] + nums[i - 1];
    }

    return pArray;
}

int sumRange(struct NumArray* numArray, int i, int j) {
    return numArray->prefix[j + 1] - numArray->prefix[i];
}

/** Deallocates memory previously allocated for the data structure. */
void NumArrayFree(struct NumArray* numArray) {
    free(numArray->prefix);
    free(numArray);
}

// Your NumArray object will be instantiated and called as such:
// struct NumArray* numArray = NumArrayCreate(nums, numsSize);
// sumRange(numArray, 0, 1);
// sumRange(numArray, 1, 2);
// NumArrayFree(numArray);
