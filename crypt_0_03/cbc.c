#include "crypt.h"

int cbc_start(int cipher, unsigned char *IV, unsigned char *key, int keylen, int num_rounds, struct symmetric_CBC *cbc)
{
   int x;

   /* bad param? */
   if (cipher == -1) { crypt_error = "Invalid cipher id passed to cbc_start()."; return CRYPT_ERROR; }

   /* setup cipher */
   if (cipher_descriptor[cipher].setup(key, keylen, num_rounds, &cbc->key) == CRYPT_ERROR) return CRYPT_ERROR;

   /* copy IV */
   cbc->blocklen = cipher_descriptor[cipher].block_length;
   cbc->cipher   = cipher;
   for (x = 0; x < cbc->blocklen; x++) cbc->IV[x] = IV[x];
   return CRYPT_OK;
}

void cbc_encrypt(unsigned char *pt, unsigned char *ct, struct symmetric_CBC *cbc)
{
   int x;
   unsigned char tmp[32];

   /* xor IV */
   for (x = 0; x < cbc->blocklen; x++) tmp[x] = pt[x] ^ cbc->IV[x];
   
   /* encrypt */
   cipher_descriptor[cbc->cipher].ecb_encrypt(tmp, ct, &cbc->key);

   /* store IV */
   for (x = 0; x < cbc->blocklen; x++) cbc->IV[x] = ct[x];
   memset(tmp, 0, 32);
}

void cbc_decrypt(unsigned char *ct, unsigned char *pt, struct symmetric_CBC *cbc)
{
   int x;
   unsigned char tmp[32];

   /* decrypt */
   cipher_descriptor[cbc->cipher].ecb_decrypt(ct, tmp, &cbc->key);

   /* xor IV in */
   for (x = 0; x < cbc->blocklen; x++) pt[x] = tmp[x] ^ cbc->IV[x];

   /* replace IV */ 
   for (x = 0; x < cbc->blocklen; x++) cbc->IV[x] = ct[x];
   memset(tmp, 0, 32);
}