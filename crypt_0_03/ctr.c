#include "crypt.h"

int ctr_start(int cipher, unsigned char *count, unsigned char *key, int keylen, int num_rounds, struct symmetric_CTR *ctr)
{
   int x;

   /* bad param? */
   if (cipher == -1) { crypt_error = "Invalid cipher id passed to ctr_start()."; return CRYPT_ERROR; }

   /* setup cipher */
   if (cipher_descriptor[cipher].setup(key, keylen, num_rounds, &ctr->key) == CRYPT_ERROR) return CRYPT_ERROR;

   /* copy ctr */
   ctr->blocklen = cipher_descriptor[cipher].block_length;
   ctr->cipher   = cipher;
   for (x = 0; x < ctr->blocklen; x++) ctr->ctr[x] = count[x];
   return CRYPT_OK;
}

void ctr_encrypt(unsigned char *pt, unsigned char *ct, int len, struct symmetric_CTR *ctr)
{
   unsigned char buf[32];
   int x;

   /* increment counter */
   for (x = 0; x < ctr->blocklen; x++) if (++ctr->ctr[x]) break;

   /* copy counter */
   for (x = 0; x < ctr->blocklen; x++) buf[x] = ctr->ctr[x];

   /* encrypt it */
   cipher_descriptor[ctr->cipher].ecb_encrypt(buf, buf, &ctr->key);
   for (x = 0; x < len; x++) ct[x] = pt[x] ^ buf[x];
   memset(buf, 0, sizeof(buf));
}

void ctr_decrypt(unsigned char *ct, unsigned char *pt, int len, struct symmetric_CTR *ctr)
{
   ctr_encrypt(ct, pt, len, ctr);
}
