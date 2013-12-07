#include "crypt.h"

int rc5_setup(unsigned char *key, int keylen, int num_rounds, union symmetric_key *skey)
{
    unsigned long L[64], S[50], A, B, i, j, v, s, t, l;

    /* test parameters */
    if (num_rounds == 0) num_rounds = 12;
    if (num_rounds < 12 || num_rounds > 24) { 
       crypt_error = "Invalid number of rounds for RC5.";
       return CRYPT_ERROR;
    }

    if (keylen < 8 || keylen > 128) {
       crypt_error = "Invalid key size for RC5.";
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
    t = 2 * (num_rounds + 1);
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
    for (i = 0; i < t; i++) skey->rc5.K[i] = S[i];
    skey->rc5.rounds = num_rounds;

    memset(L, 0, sizeof(L));
    memset(S, 0, sizeof(S));
    A = B = 0;
    return CRYPT_OK;
}

void rc5_ecb_encrypt(unsigned char *pt, unsigned char *ct, union symmetric_key *key)
{
   unsigned long A, B;
   int r;

   LOAD32L(A, &pt[0]);
   LOAD32L(B, &pt[4]);
   A += key->rc5.K[0];
   B += key->rc5.K[1];
   for (r = 0; r < key->rc5.rounds; r++) {
       A = ROL(A ^ B, B) + key->rc5.K[r+r+2];
       B = ROL(B ^ A, A) + key->rc5.K[r+r+3];
   }
   STORE32L(A, &ct[0]);
   STORE32L(B, &ct[4]);
   A = B = 0;
}

void rc5_ecb_decrypt(unsigned char *ct, unsigned char *pt, union symmetric_key *key)
{
   unsigned long A, B;
   int r;

   LOAD32L(A, &ct[0]);
   LOAD32L(B, &ct[4]);
   for (r = key->rc5.rounds - 1; r >= 0; r--) {
       B = ROR(B - key->rc5.K[r+r+3], A) ^ A;
       A = ROR(A - key->rc5.K[r+r+2], B) ^ B;
   }
   A -= key->rc5.K[0];
   B -= key->rc5.K[1];
   STORE32L(A, &pt[0]);
   STORE32L(B, &pt[4]);
   A = B = 0;
}

int rc5_test(void)
{
   static unsigned char key[16] =
          { 0x91, 0x5f, 0x46, 0x19, 0xbe, 0x41, 0xb2, 0x51,  0x63, 0x55, 0xa5, 0x01, 0x10, 0xa9, 0xce, 0x91 };
   static unsigned char pt[8] = 
          { 0x21, 0xa5, 0xdb, 0xee, 0x15, 0x4b, 0x8f, 0x6d };
   static unsigned char ct[8] = 
          { 0xf7, 0xc0, 0x13, 0xac, 0x5b, 0x2b, 0x89, 0x52 };
   unsigned char buf[2][8];
   union symmetric_key skey;

   if (rc5_setup(key, 16, 12, &skey) == CRYPT_ERROR) return CRYPT_ERROR;
   rc5_ecb_encrypt(pt, buf[0], &skey);
   rc5_ecb_decrypt(buf[0], buf[1], &skey);

   /* compare */
   if (memcmp(buf[0], &ct, 8)) { crypt_error = "Plaintext did not encrypt to test vector."; return CRYPT_ERROR; }
   if (memcmp(buf[1], &pt, 8)) { crypt_error = "Ciphertext did not decrypt to test vector."; return CRYPT_ERROR; }

   return CRYPT_OK;
}