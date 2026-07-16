#ifndef UTILS_H
#define UTILS_H

int hex_char_to_val(char c);
int hex_to_bytes(const char *hex_str, unsigned char *output, int max_len);

#endif