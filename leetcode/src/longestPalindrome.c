/*
* Copyright (C) 2015 2nd Foundation
* All rights reserved
*
*/

//It's the low performance version
char* longestPalindrome(char* s) {
    int length = strlen(s);
    
    int MaxLength = 1;
    int low = 0;
    int high = 0;
    int i;
    int start = 0;
    
    for (i = 1; i < length; i++)
    {
        low  = i - 1;
        high = i;
        while (low >= 0 && high < length && s[low] == s[high])
        {
            if (high - low + 1 > MaxLength)
            {
                start = low;
                MaxLength = high - low + 1;
            }
            low--;
            high++;
        }
        
        low = i - 1;
        high = i + 1;
        while (low >= 0 && high < length && s[low] == s[high])
        {
            if (high - low + 1 > MaxLength)
            {
                start = low;
                MaxLength = high - low + 1;
            }
            low--;
            high++;
        }
    }
    
    char * sub = (char*)malloc((MaxLength + 1)*sizeof(char));
    memset(sub, 0, (MaxLength + 1)*sizeof(char));
    memcpy(sub, &s[start], MaxLength);
    
    return sub;
}
