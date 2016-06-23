/*
  A message containing letters from A-Z is being encoded to numbers using the following mapping:

  'A' -> 1
  'B' -> 2
  ...
  'Z' -> 26

  Given an encoded message containing digits, determine the total number of ways to decode it.
 */

int numDecodings(char* s) {
    int len = strlen(s);
    int * table = (int *)malloc(len*sizeof(int));
    memset(table, 0, len*sizeof(int));

    if (s[0] == '0')
        return 0;
    else
        table[0] = 1;

    int i;

    for (i = 1; i < len; i++) {
        if (s[i] == '0') {
            if (s[i - 1] == '1' || s[i - 1] == '2') {
                if (i >= 2)
                    table[i] = table[i - 2];
                else
                    table[i] = table[i - 1];
            } else
                table[i] = 0;
        } else if (s[i] >= '1' && s[i] <= '9') {
            if (s[i - 1] == '2' && s[i] <= '6' ||
                    s[i - 1] == '1') {
                if (i >= 2)
                    table[i] = table[i - 1] + table[i - 2];
                else
                    table[i] = table[i - 1] + 1;
            }
            else
                table[i] = table[i - 1];
        }
    }

    int ret = table[len - 1];
    free(table);
    return ret;
}
