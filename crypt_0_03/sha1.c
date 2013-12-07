#include "crypt.h"

#define F0(x,y,z)  ( (x&y) | ((~x)&z) )
#define F1(x,y,z)  (x ^ y ^ z)
#define F2(x,y,z)  ((x & y) | (z & (x | y)))
#define F3(x,y,z)  (x ^ y ^ z)

static void sha1_compress(union hash_state *md)
{
    unsigned long a,b,c,d,e,W[80],i,j;

    /* copy the state into 512-bits into W[0..15] */
    for (i = 0; i < 16; i++)
        W[i] = (((unsigned long) md->sha1.buf[(4 * i) + 0]) << 24) |
            (((unsigned long) md->sha1.buf[(4 * i) + 1]) << 16) |
            (((unsigned long) md->sha1.buf[(4 * i) + 2]) << 8) |
            (((unsigned long) md->sha1.buf[(4 * i) + 3]) << 0);

    /* copy state */
    a = md->sha1.state[0];
    b = md->sha1.state[1];
    c = md->sha1.state[2];
    d = md->sha1.state[3];
    e = md->sha1.state[4];

    /* expand it */
    for (i = 16; i < 80; i++) { j = W[i-3] ^ W[i-8] ^ W[i-14] ^ W[i-16]; W[i] = ROL(j, 1); }

    /* compress */
    for (i = 0;  i < 20; i++)  { j = ROL(a, 5) + F0(b,c,d) + e + W[i] + 0x5a827999; e = d; d = c; c = ROL(b, 30); b = a; a = j; }
    for (i = 20; i < 40; i++)  { j = ROL(a, 5) + F1(b,c,d) + e + W[i] + 0x6ed9eba1; e = d; d = c; c = ROL(b, 30); b = a; a = j; }
    for (i = 40; i < 60; i++)  { j = ROL(a, 5) + F2(b,c,d) + e + W[i] + 0x8f1bbcdc; e = d; d = c; c = ROL(b, 30); b = a; a = j; }
    for (i = 60; i < 80; i++)  { j = ROL(a, 5) + F3(b,c,d) + e + W[i] + 0xca62c1d6; e = d; d = c; c = ROL(b, 30); b = a; a = j; }

    /* store */
    md->sha1.state[0] += a;
    md->sha1.state[1] += b;
    md->sha1.state[2] += c;
    md->sha1.state[3] += d;
    md->sha1.state[4] += e;
}

void sha1_init(union hash_state * md)
{
 md->sha1.state[0] = 0x67452301;
 md->sha1.state[1] = 0xefcdab89;
 md->sha1.state[2] = 0x98badcfe;
 md->sha1.state[3] = 0x10325476;
 md->sha1.state[4] = 0xc3d2e1f0;
 md->sha1.curlen = md->sha1.length = 0;
}

void sha1_process(union hash_state * md, unsigned char *buf, int len)
{
    while (len--) {
        /* copy byte */
        md->sha1.buf[md->sha1.curlen++] = *buf++;

        /* is 64 bytes full? */
        if (md->sha1.curlen == 64) {
            sha1_compress(md);
            md->sha1.length += 512;
            md->sha1.curlen = 0;
        }
    }
}

void sha1_done(union hash_state * md, unsigned char *hash)
{
    int i;

    /* increase the length of the message */
    md->sha1.length += md->sha1.curlen * 8;

    /* append the '1' bit */
    md->sha1.buf[md->sha1.curlen++] = 0x80;

    /* if the length is currenlly above 56 bytes we append zeros
                               * then compress.  Then we can fall back to padding zeros and length
                               * encoding like normal.
                             */
    if (md->sha1.curlen >= 56) {
        for (; md->sha1.curlen < 64;)
            md->sha1.buf[md->sha1.curlen++] = 0;
        sha1_compress(md);
        md->sha1.curlen = 0;
    }

    /* pad upto 56 bytes of zeroes */
    for (; md->sha1.curlen < 56;)
        md->sha1.buf[md->sha1.curlen++] = 0;

    /* since all messages are under 2^32 bits we mark the top bits zero */
    for (i = 56; i < 60; i++)
        md->sha1.buf[i] = 0;

    /* append length */
    for (i = 60; i < 64; i++)
        md->sha1.buf[i] = (md->sha1.length >> ((63 - i) * 8)) & 255;
    sha1_compress(md);

    /* copy output */
    for (i = 0; i < 20; i++)
        hash[i] = (md->sha1.state[i >> 2] >> (((3 - i) & 3) << 3)) & 255;
}

int  sha1_test(void)
{
  static unsigned char hash[32] = { 
      0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a,
      0xba, 0x3e, 0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c, 0x9c, 0xd0, 0xd8, 0x9d };
  static unsigned char *message = "abc";
  unsigned char tmp[20];

  if (hash_memory(find_hash("sha1"), message, strlen(message), tmp) == CRYPT_ERROR) return CRYPT_ERROR;
  if (memcmp(tmp, hash, 20)) { crypt_error = "SHA1 hash did not match test vector."; return CRYPT_ERROR; }
  return CRYPT_OK;
}
