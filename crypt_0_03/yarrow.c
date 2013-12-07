#include "crypt.h"

int yarrow_start(union prng_state *prng)
{
   prng->yarrow.cipher = find_cipher("blowfish");
   prng->yarrow.hash   = find_hash("sha1");
   memset(prng->yarrow.pool, 0, 32);
   memset(prng->yarrow.buf, 0, 32);
   prng->yarrow.bl = -1;
   return CRYPT_OK;
}

int yarrow_add_entropy(unsigned char *buf, int len, union prng_state *prng)
{
   int x;
   unsigned char tmp[32];
   if (hash_memory(prng->yarrow.hash, buf, len, tmp) == CRYPT_ERROR) return CRYPT_ERROR;
   for (x = 0; x < hash_descriptor[prng->yarrow.hash].hashsize; x++)
       prng->yarrow.pool[x] ^= tmp[x];
   memset(tmp, 0, 32);
   return CRYPT_OK;
}

int yarrow_ready(union prng_state *prng)
{
   memset(prng->yarrow.buf, 0, 32);
   if (ctr_start(prng->yarrow.cipher, prng->yarrow.buf, prng->yarrow.pool, 
       MIN(cipher_descriptor[prng->yarrow.cipher].max_key_length, hash_descriptor[prng->yarrow.hash].hashsize),
       0, &prng->yarrow.ctr) == CRYPT_ERROR) return CRYPT_ERROR;
   prng->yarrow.bl = -1;
   return CRYPT_OK;
}

int yarrow_read(unsigned char *buf, int len, union prng_state *prng)
{
   int x = len;
   while (x--) {
      if (prng->yarrow.bl == -1) {
         memset(prng->yarrow.buf, 0, 32);
         ctr_encrypt(prng->yarrow.buf, prng->yarrow.buf, cipher_descriptor[prng->yarrow.cipher].block_length, &prng->yarrow.ctr);
         prng->yarrow.bl = cipher_descriptor[prng->yarrow.cipher].block_length - 1;
      }
      *buf++ = prng->yarrow.buf[prng->yarrow.bl--];
   }
   return len;
}