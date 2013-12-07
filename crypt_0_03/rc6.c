#include "crypt.h"

int rc6_setup(unsigned char *key, int keylen, int num_rounds, union symmetric_key *skey)
{
    unsigned long L[64], S[50], A, B, i, j, v, s, t, l;

    /* test parameters */
    if (num_rounds != 0 && num_rounds != 20) { 
       crypt_error = "Invalid number of rounds for RC6.";
       return CRYPT_ERROR;
    }

    if (keylen < 8 || keylen > 128) {
       crypt_error = "Invalid key size for RC6.";
       return CRYPT_ERROR;
    }

    /* copy the key into the L array */
    for (A = i = j = 0; i < keylen; ) { 
        A = (A << 8) | ((unsigned long)key[i++]);
        if (!(i & 3)) {
           L[j++] = BSWAP(A);
           A = 0;
        }
    }
    if (keylen & 3) { A <<= (8 * (3 - (keylen&3))); L[j++] = BSWAP(A); }

    /* setup the S array */
    t = 44;				/* fixed at 20 rounds */
    S[0] = 0xB7E15163;
    for (i = 1; i < t; i++) S[i] = S[i - 1] + 0x9E3779B9;

    /* mix buffer */
    s = 3 * MAX(t, j);
    l = j;
    for (A = B = i = j = v = 0; v < s; v++) { 
        A = S[i] = ROL(S[i] + A + B, 3);
        B = L[j] = ROL(L[j] + A + B, (A+B));
        i = (i + 1) % t;
        j = (j + 1) % l;
    }
    
    /* copy to key */
    for (i = 0; i < t; i++) skey->rc6.K[i] = S[i];

    memset(L, 0, sizeof(L));
    memset(S, 0, sizeof(S));
    A = B = 0;
    return CRYPT_OK;

}

void rc6_ecb_encrypt(unsigned char *pt, unsigned char *ct, union symmetric_key *key)
{
   unsigned long a,b,c,d,t,u;
   int r;
   
   LOAD32L(a,&pt[0]);LOAD32L(b,&pt[4]);LOAD32L(c,&pt[8]);LOAD32L(d,&pt[12]);
   b += key->rc6.K[0];
   d += key->rc6.K[1];
   for (r = 0; r < 20; r++) {
       t = (b * (b + b + 1)); t = ROL(t, 5);
       u = (d * (d + d + 1)); u = ROL(u, 5);
       a = ROL(a^t,u) + key->rc6.K[r+r+2];
       c = ROL(c^u,t) + key->rc6.K[r+r+3];
       t = a; a = b; b = c; c = d; d = t;
   }
   a += key->rc6.K[42];
   c += key->rc6.K[43];
   STORE32L(a,&ct[0]);STORE32L(b,&ct[4]);STORE32L(c,&ct[8]);STORE32L(d,&ct[12]);
   a = b = c = d = t = 0;
}

void rc6_ecb_decrypt(unsigned char *ct, unsigned char *pt, union symmetric_key *key)
{
   unsigned long a,b,c,d,t,u;
   int r;
   
   LOAD32L(a,&ct[0]);LOAD32L(b,&ct[4]);LOAD32L(c,&ct[8]);LOAD32L(d,&ct[12]);
   a -= key->rc6.K[42];
   c -= key->rc6.K[43];
   for (r = 19; r >= 0; r--) {
       t = d; d = c; c = b; b = a; a = t;
       t = (b * (b + b + 1)); t = ROL(t, 5);
       u = (d * (d + d + 1)); u = ROL(u, 5);
       c = ROR(c - key->rc6.K[r+r+3], t) ^ u;
       a = ROR(a - key->rc6.K[r+r+2], u) ^ t;
   }
   b -= key->rc6.K[0];
   d -= key->rc6.K[1];
   STORE32L(a,&pt[0]);STORE32L(b,&pt[4]);STORE32L(c,&pt[8]);STORE32L(d,&pt[12]);
}

int rc6_test(void)
{
   static unsigned char key[16] =
          { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78 };
   static unsigned char pt[16] = 
          { 0x02, 0x13, 0x24, 0x35, 0x46, 0x57, 0x68, 0x79, 0x8a, 0x9b, 0xac, 0xbd, 0xce, 0xdf, 0xe0, 0xf1 };
   static unsigned char ct[16] = 
          { 0x52, 0x4e, 0x19, 0x2f, 0x47, 0x15, 0xc6, 0x23, 0x1f, 0x51, 0xf6, 0x36, 0x7e, 0xa4, 0x3f, 0x18 };
   unsigned char buf[2][16];
   union symmetric_key skey;

   if (rc6_setup(key, 16, 0, &skey) == CRYPT_ERROR) return CRYPT_ERROR;
   rc6_ecb_encrypt(pt, buf[0], &skey);
   rc6_ecb_decrypt(buf[0], buf[1], &skey);

   /* compare */
   if (memcmp(buf[0], &ct, 16)) { crypt_error = "Plaintext did not encrypt to test vector."; return CRYPT_ERROR; }
   if (memcmp(buf[1], &pt, 16)) { crypt_error = "Ciphertext did not decrypt to test vector."; return CRYPT_ERROR; }

   return CRYPT_OK;
}
