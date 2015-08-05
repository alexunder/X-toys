/*
* Copyright (C) 2015 2nd Foundation
* All rights reserved
*
*/

int lengthOfLongestSubstring(char* s) {
    int length = strlen(s);
    char * temp = (char*)malloc(length + 1);
    memset(temp, 0, length + 1);
    int cnt_temp = 0;
    
    int ret = 0;
    
    int i = 0;
    while(i < length)
    {
        if (strchr(temp, s[i]) == NULL)
        {
            temp[cnt_temp++] = s[i];
        }
        else
        {
            if (cnt_temp > ret)
                ret = cnt_temp;
            
            char c = s[i];
            int j = i - 1;
            
            for (j = i - 1;s[j] != c; j--);
            
            i = j + 1;
            memset(temp, 0, length + 1);
            cnt_temp = 0;
            temp[cnt_temp++] = s[i];
        }
        
        i++;
    }
    
    if (i == length && cnt_temp > ret)
        ret = cnt_temp;
    
    if (ret == 0)
        ret = cnt_temp;
    
    free(temp);
    return ret;
}
