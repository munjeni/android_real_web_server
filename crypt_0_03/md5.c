#include "crypt.h"

#define F(x,y,z)  ( (x&y)|((~x)&z) )
#define G(x,y,z)  ( (x&z)|(y&(~z)) )
#define H(x,y,z)  (x^y^z)
#define I(x,y,z)  (y ^ (x | (~z)))

#define FF(a,b,c,d,M,s,t) a = (a + F(b,c,d) + M + t); a = ROL(a, s); a = b + a;
#define GG(a,b,c,d,M,s,t) a = (a + G(b,c,d) + M + t); a = ROL(a, s); a = b + a;
#define HH(a,b,c,d,M,s,t) a = (a + H(b,c,d) + M + t); a = ROL(a, s); a = b + a;
#define II(a,b,c,d,M,s,t) a = (a + I(b,c,d) + M + t); a = ROL(a, s); a = b + a;

static void md5_compress(union hash_state *md)
{
    unsigned long i, W[16], a, b, c, d;

    /* copy the state into 512-bits into W[0..15] */
    for (i = 0; i < 16; i++)
        W[i] = (((unsigned long) md->md5.buf[(4 * i) + 0]) << 0) |
            (((unsigned long) md->md5.buf[(4 * i) + 1]) << 8) |
            (((unsigned long) md->md5.buf[(4 * i) + 2]) << 16) |
            (((unsigned long) md->md5.buf[(4 * i) + 3]) << 24);
 
    /* copy state */
    a = md->md5.state[0];
    b = md->md5.state[1];
    c = md->md5.state[2];
    d = md->md5.state[3];

    FF(a,b,c,d,W[0],7,0xd76aa478)
    FF(d,a,b,c,W[1],12,0xe8c7b756)
    FF(c,d,a,b,W[2],17,0x242070db)
    FF(b,c,d,a,W[3],22,0xc1bdceee)
    FF(a,b,c,d,W[4],7,0xf57c0faf)
    FF(d,a,b,c,W[5],12,0x4787c62a)
    FF(c,d,a,b,W[6],17,0xa8304613)
    FF(b,c,d,a,W[7],22,0xfd469501)
    FF(a,b,c,d,W[8],7,0x698098d8)
    FF(d,a,b,c,W[9],12,0x8b44f7af)
    FF(c,d,a,b,W[10],17,0xffff5bb1)
    FF(b,c,d,a,W[11],22,0x895cd7be)
    FF(a,b,c,d,W[12],7,0x6b901122)
    FF(d,a,b,c,W[13],12,0xfd987193)
    FF(c,d,a,b,W[14],17,0xa679438e)
    FF(b,c,d,a,W[15],22,0x49b40821)
    GG(a,b,c,d,W[1],5,0xf61e2562)
    GG(d,a,b,c,W[6],9,0xc040b340)
    GG(c,d,a,b,W[11],14,0x265e5a51)
    GG(b,c,d,a,W[0],20,0xe9b6c7aa)
    GG(a,b,c,d,W[5],5,0xd62f105d)
    GG(d,a,b,c,W[10],9,0x02441453)
    GG(c,d,a,b,W[15],14,0xd8a1e681)
    GG(b,c,d,a,W[4],20,0xe7d3fbc8)
    GG(a,b,c,d,W[9],5,0x21e1cde6)
    GG(d,a,b,c,W[14],9,0xc33707d6)
    GG(c,d,a,b,W[3],14,0xf4d50d87)
    GG(b,c,d,a,W[8],20,0x455a14ed)
    GG(a,b,c,d,W[13],5,0xa9e3e905)
    GG(d,a,b,c,W[2],9,0xfcefa3f8)
    GG(c,d,a,b,W[7],14,0x676f02d9)
    GG(b,c,d,a,W[12],20,0x8d2a4c8a)
    HH(a,b,c,d,W[5],4,0xfffa3942)
    HH(d,a,b,c,W[8],11,0x8771f681)
    HH(c,d,a,b,W[11],16,0x6d9d6122)
    HH(b,c,d,a,W[14],23,0xfde5380c)
    HH(a,b,c,d,W[1],4,0xa4beea44)
    HH(d,a,b,c,W[4],11,0x4bdecfa9)
    HH(c,d,a,b,W[7],16,0xf6bb4b60)
    HH(b,c,d,a,W[10],23,0xbebfbc70)
    HH(a,b,c,d,W[13],4,0x289b7ec6)
    HH(d,a,b,c,W[0],11,0xeaa127fa)
    HH(c,d,a,b,W[3],16,0xd4ef3085)
    HH(b,c,d,a,W[6],23,0x04881d05)
    HH(a,b,c,d,W[9],4,0xd9d4d039)
    HH(d,a,b,c,W[12],11,0xe6db99e5)
    HH(c,d,a,b,W[15],16,0x1fa27cf8)
    HH(b,c,d,a,W[2],23,0xc4ac5665)
    II(a,b,c,d,W[0],6,0xf4292244)
    II(d,a,b,c,W[7],10,0x432aff97)
    II(c,d,a,b,W[14],15,0xab9423a7)
    II(b,c,d,a,W[5],21,0xfc93a039)
    II(a,b,c,d,W[12],6,0x655b59c3)
    II(d,a,b,c,W[3],10,0x8f0ccc92)
    II(c,d,a,b,W[10],15,0xffeff47d)
    II(b,c,d,a,W[1],21,0x85845dd1)
    II(a,b,c,d,W[8],6,0x6fa87e4f)
    II(d,a,b,c,W[15],10,0xfe2ce6e0)
    II(c,d,a,b,W[6],15,0xa3014314)
    II(b,c,d,a,W[13],21,0x4e0811a1)
    II(a,b,c,d,W[4],6,0xf7537e82)
    II(d,a,b,c,W[11],10,0xbd3af235)
    II(c,d,a,b,W[2],15,0x2ad7d2bb)
    II(b,c,d,a,W[9],21,0xeb86d391)

    md->md5.state[0] += a;
    md->md5.state[1] += b;
    md->md5.state[2] += c;
    md->md5.state[3] += d;
}

void md5_init(union hash_state * md)
{
 md->md5.state[0] = 0x67452301;
 md->md5.state[1] = 0xefcdab89;
 md->md5.state[2] = 0x98badcfe;
 md->md5.state[3] = 0x10325476;
 md->md5.curlen = md->md5.length = 0;
}

void md5_process(union hash_state * md, unsigned char *buf, int len)
{
    while (len--) {
        /* copy byte */
        md->md5.buf[md->md5.curlen++] = *buf++;

        /* is 64 bytes full? */
        if (md->md5.curlen == 64) {
            md5_compress(md);
            md->md5.length += 512;
            md->md5.curlen = 0;
        }
    }
}

void md5_done(union hash_state * md, unsigned char *hash)
{
    int i;

    /* increase the length of the message */
    md->md5.length += md->md5.curlen * 8;

    /* append the '1' bit */
    md->md5.buf[md->md5.curlen++] = 0x80;

    /* if the length is currenlly above 56 bytes we append zeros
                               * then compress.  Then we can fall back to padding zeros and length
                               * encoding like normal.
                             */
    if (md->md5.curlen >= 56) {
        for (; md->md5.curlen < 64;)
            md->md5.buf[md->md5.curlen++] = 0;
        md5_compress(md);
        md->md5.curlen = 0;
    }

    /* pad upto 56 bytes of zeroes */
    for (; md->md5.curlen < 56;)
        md->md5.buf[md->md5.curlen++] = 0;

    /* since all messages are under 2^32 bits we mark the top bits zero */
    for (i = 60; i < 64; i++)
        md->md5.buf[i] = 0;

    /* append length */
    for (i = 56; i < 60; i++)
        md->md5.buf[i] = (md->md5.length >> ((4 - (60 - i)) * 8)) & 255;
    md5_compress(md);

    /* copy output */
    for (i = 0; i < 16; i++)
        hash[i] = (md->md5.state[i >> 2] >> ((i & 3) << 3)) & 255;
}

int  md5_test(void)
{
  static unsigned char hash[16] = { 
	0xf9, 0x6b, 0x69, 0x7d, 0x7c, 0xb7, 0x93, 0x8d, 0x52, 0x5a, 0x2f, 0x31, 0xaa, 0xf1, 0x61, 0xd0 };
  static unsigned char *message = "message digest";
  unsigned char tmp[16];

  if (hash_memory(find_hash("md5"), message, strlen(message), tmp) == CRYPT_ERROR) return CRYPT_ERROR;
  if (memcmp(tmp, hash, 16)) { crypt_error = "MD5 hash did not match test vector."; return CRYPT_ERROR; }
  return CRYPT_OK;
}

