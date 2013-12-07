#include "crypt.h"

const static unsigned char *codes = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789{}";

const static unsigned char map[256] = {
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
 52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255, 
255, 255, 255, 255, 255,  26,  27,  28,  29,  30,  31,  32, 
 33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44, 
 45,  46,  47,  48,  49,  50,  51, 255, 255, 255, 255, 255, 
255,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10, 
 11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22, 
 23,  24,  25,  62, 255,  63, 255, 255, 255, 255, 255, 255, 
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 
255, 255, 255, 255};

int base64_encode(unsigned char *in, int len, unsigned char *out, int *outlen)
{
 unsigned long t;
 int x, y;

 /* valid output size ? */
 if (*outlen < (4*((len/3)+1))) {
    crypt_error = "Buffer overrun in base64_encode().";
    return CRYPT_ERROR;
 }

 /* output size (only upto 16MB) */
 t = len;
 y = 0;
 out[y++] = codes[(t>>18)&63]; t <<= 6;
 out[y++] = codes[(t>>18)&63]; t <<= 6;
 out[y++] = codes[(t>>18)&63]; t <<= 6;
 out[y++] = codes[(t>>18)&63];

 for (x = 0; x < len; ) {
     /* form a 24-bit word */
     t = in[x++];
     t = (t<<8)|((x>=len)?0:in[x++]);
     t = (t<<8)|((x>=len)?0:in[x++]);

     /* output 4 base 64 chars */
     out[y++] = codes[(t>>18)&63]; t <<= 6;
     out[y++] = codes[(t>>18)&63]; t <<= 6;
     out[y++] = codes[(t>>18)&63]; t <<= 6;
     out[y++] = codes[(t>>18)&63];
 }
 *outlen = y;
 return CRYPT_OK;
}

int base64_decode(unsigned char *in, int len, unsigned char *out, int *outlen)
{
 unsigned long t, stop;
 int x, y, z;

 /* first four chars cannot be messed up */
 t = map[in[0]];
 t = (t<<6)|map[in[1]];
 t = (t<<6)|map[in[2]];
 t = (t<<6)|map[in[3]];
 stop = t;

 if (*outlen < t) {
    crypt_error = "Buffer overrun in base64_decode().";
    return CRYPT_ERROR;
 }
 
 for (x = 4, y = z = t = 0; x < len; x++) {
     if (map[in[x]] != 255) {
        t = (t<<6)|map[in[x]];
        if (++y == 4) {
           out[z++] = (t>>16)&255; if (z == stop) break;
           out[z++] = (t>>8)&255;  if (z == stop) break;
           out[z++] = t&255;       if (z == stop) break; 
           y = t = 0;
        }
     }
 }
 *outlen = z;
 return CRYPT_OK;
}