/*
    Given a non-negative number represented as an array of digits, plus one to the number.
    The digits are stored such that the most significant digit is at the head of the list.
 */

/**

 * Return an array of size *returnSize.

  * Note: The returned array must be malloced, assume caller calls free().

   */

int* plusOne(int* digits, int digitsSize, int* returnSize) {
    int i;
    
    int * newDigits = (int*)malloc((digitsSize + 1)*sizeof(int));
    memset(newDigits, 0, (digitsSize + 1)*sizeof(int));

    int j = digitsSize;
    int next = 0;

    for (i = digitsSize - 1; i >= 0; i--)
    {
        if (i == digitsSize - 1)
            newDigits[j] = digits[i] + 1;
        else
            newDigits[j] = digits[i] + next;

       if (newDigits[j] == 10)
        {
            next = 1;
            newDigits[j] = 0;
        }
        else
        {
            next = 0;
        }

        j--;
    }

    if (next == 1)
    {
        newDigits[0] = 1;
        *returnSize = digitsSize + 1;
        return newDigits;
    }
    else
    {
        *returnSize = digitsSize;
        return newDigits + 1;
    }
}
