#include "utils.h"
#include <string.h>

int hex_char_to_val(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

int hex_to_bytes(const char *hex_str, unsigned char *output, int max_len)
{
    int len = strlen(hex_str);
    if (len % 2 != 0)
        return -1;

    int byte_count = len / 2;
    if (byte_count > max_len)
        return -1;

    for (int i = 0; i < byte_count; i++)
    {
        int high_val = hex_char_to_val(hex_str[i * 2]);
        int low_val = hex_char_to_val(hex_str[i * 2 + 1]);

        if (high_val == -1 || low_val == -1)
            return -1;

        output[i] = (unsigned char)((high_val << 4) | low_val);
    }

    return byte_count;
}