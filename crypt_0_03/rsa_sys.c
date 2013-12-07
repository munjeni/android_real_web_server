#include "crypt.h"

/* Encrypted Message Format:

offset    |  length   |    Contents
----------------------------------------------------------------------
0         |    3      |  0x1A, 0x2B, 0x3C
3         |    1      |  length of symmetric key cipher name + NULL
4         |    n      |  name of cipher + NULL
4+n       |    4      |  length of RSA encrypted value (little endian format)
8+n       |    q      |  the rsa_pad()'ed RSA encrypted value (little endian format)
8+n+q     |    p      |  The CTR IV value used (varies in size based on cipher used)
8+n+q+p   |    4      |  length of message (little endian format)
8+n+q+p   |    j      |  ciphertext
----------------------------------------------------------------------

*/

int rsa_encrypt(unsigned char *in, int len, unsigned char *out, int *outlen,
                union prng_state *prng, int wprng, int cipher, struct rsa_key *key)
{
   unsigned char sym_IV[32], sym_key[32], rsa_in[4096], rsa_out[4096];
   struct symmetric_CTR ctr;
   int x, y, z, keylen, blklen, rsa_size;

   /* are the parameters valid? */
   if (wprng  == -1) { crypt_error = "Invalid PRNG passed to rsa_encrypt()."; return CRYPT_ERROR; }
   if (cipher == -1) { crypt_error = "Invalid Cipher passed to rsa_encrypt()."; return CRYPT_ERROR; }

   /* setup the CTR key */
   keylen = MIN(cipher_descriptor[cipher].max_key_length, 32);
   blklen = cipher_descriptor[cipher].block_length;
   if (prng_descriptor[wprng].read(sym_key, keylen, prng) != keylen) {
      crypt_error = "Error reading PRNG in rsa_encrypt()."; 
      return CRYPT_ERROR;
   }
   if (prng_descriptor[wprng].read(sym_IV, blklen, prng) != blklen) {
      crypt_error = "Error reading PRNG in rsa_encrypt()."; 
      return CRYPT_ERROR;
   }
   if (ctr_start(cipher, sym_IV, sym_key, keylen, 0, &ctr) == CRYPT_ERROR) return CRYPT_ERROR;

   /* rsa_pad the symmetric key */
   y = 4096; 
   if (rsa_pad(sym_key, keylen, rsa_in, &y, wprng, prng) == CRYPT_ERROR) return CRYPT_ERROR;
   
   /* rsa encrypt it */
   rsa_size = 4096;
   if (rsa_exptmod(rsa_in, y, rsa_out, &rsa_size, PK_PUBLIC, key) == CRYPT_ERROR) return CRYPT_ERROR;

   /* check size */
   if (*outlen < (9+rsa_size+blklen+len+strlen(cipher_descriptor[cipher].name))) { 
      crypt_error = "Buffer overrun in rsa_encrypt().";
      return CRYPT_ERROR; 
   }

   /* now lets make the header */
   out[0] = 0x1A; 
   out[1] = 0x2B;
   out[2] = 0x3C;
   out[3] = strlen(cipher_descriptor[cipher].name) + 1;
   for (y = 4, x = 0; x <= strlen(cipher_descriptor[cipher].name); x++, y++)
       out[y] = cipher_descriptor[cipher].name[x];

   /* store the size of the RSA value */
   STORE32L(rsa_size, (out+y));
   y += 4;

   /* store the rsa value */
   for (x = 0; x < rsa_size; x++, y++)
       out[y] = rsa_out[x];

   /* store the IV used */
   for (x = 0; x < blklen; x++, y++)
       out[y] = sym_IV[x];
       
   /* store the length */
   STORE32L(len, (out+y));
   y += 4;

   /* encrypt the message */
   for (x = 0; x < len; ) {
       z = ((len-x)>blklen)?blklen:(len-x);
       ctr_encrypt(&in[x], &out[y], z, &ctr);
       x += z;
       y += z;
   }
   
   /* clean up */
   memset(sym_key, 0, sizeof(sym_key));
   memset(sym_IV, 0, sizeof(sym_IV));
   memset(&ctr, 0, sizeof(ctr));
   memset(rsa_in, 0, sizeof(rsa_in));
   memset(rsa_out, 0, sizeof(rsa_out));
   *outlen = y;
   return CRYPT_OK;
}

int rsa_decrypt(unsigned char *in, int len, unsigned char *out, int *outlen, struct rsa_key *key)
{
   unsigned char sym_IV[32], sym_key[32], rsa_in[4096], rsa_out[4096];
   struct symmetric_CTR ctr;
   int x, y, z, keylen, blklen, rsa_size, cipher;

   if (key->type != PK_PRIVATE) { crypt_error = "Invalid key type for rsa_decrypt()."; return CRYPT_ERROR; }

   /* check the header */
   if (in[0] != 0x1A || in[1] != 0x2B || in[2] != 0x3C) {
      crypt_error = "Invalid header for input in rsa_decrypt().";
      return CRYPT_ERROR;
   }

   /* grab cipher name */
   cipher = find_cipher(&in[4]);
   if (cipher == -1) {
      crypt_error = "Invalid cipher name for rsa_decrypt().";
      return CRYPT_ERROR;
   }
   keylen = MIN(cipher_descriptor[cipher].max_key_length, 32);
   blklen = cipher_descriptor[cipher].block_length;

   /* skip over the name */
   y = 4 + (int)(in[3]&255);

   /* grab length of the rsa key */
   LOAD32L(rsa_size, (in+y))
   y += 4;

   /* read it in */
   for (x = 0; x < rsa_size; x++, y++)
       rsa_in[x] = in[y];

   /* decrypt it */
   x = sizeof(rsa_out);
   if (rsa_exptmod(rsa_in, rsa_size, rsa_out, &x, PK_PRIVATE, key) == CRYPT_ERROR) return CRYPT_ERROR;

   /* depad it */
   z = sizeof(sym_key);
   if (rsa_depad(rsa_out, x, sym_key, &z) == CRYPT_ERROR) return CRYPT_ERROR;

   /* read the IV in */
   for (x = 0; x < blklen; x++, y++)
       sym_IV[x] = in[y];
   if (ctr_start(cipher, sym_IV, sym_key, keylen, 0, &ctr) == CRYPT_ERROR) return CRYPT_ERROR;

   /* get len */
   LOAD32L(len, (in+y));
   y += 4;

   /* check size */
   if (*outlen < len) { crypt_error = "Buffer overrun in rsa_decrypt()."; return CRYPT_ERROR; }

   /* decrypt the message */
   for (x = 0; x < len; ) {
       z = ((len-x)>blklen)?blklen:(len-x);
       ctr_decrypt(&in[y], &out[x], z, &ctr);
       x += z;
       y += z;
   }
   
   /* clean up */
   memset(sym_key, 0, sizeof(sym_key));
   memset(sym_IV, 0, sizeof(sym_IV));
   memset(&ctr, 0, sizeof(ctr));
   memset(rsa_in, 0, sizeof(rsa_in));
   memset(rsa_out, 0, sizeof(rsa_out));
   *outlen = len;
   return CRYPT_OK;
}

/* Signature Message Format 
offset    |  length   |    Contents
----------------------------------------------------------------------
0         |    3      | 0x3c, 0x2b, 0x1a
3         |    1      | length of hash name including NULL
4         |    n      | name of hash including NULL
4+n       |    4      | length of rsa_pad'ed signature
8+n       |    p      | the rsa_pad'ed signature
*/

int rsa_sign(unsigned char *in, int inlen, unsigned char *out, int *outlen, int hash, 
             union prng_state *prng, int wprng, struct rsa_key *key)
{
   int hashlen, rsa_size, x, y;
   unsigned char rsa_in[4096], rsa_out[4096];

   /* are the parameters valid? */
   if (wprng  == -1) { crypt_error = "Invalid PRNG passed to rsa_sign()."; return CRYPT_ERROR; }
   if (hash == -1)   { crypt_error = "Invalid Hash passed to rsa_sign()."; return CRYPT_ERROR; }

   /* hash it */
   hashlen = hash_descriptor[hash].hashsize;
   if (hash_memory(hash, in, inlen, rsa_in) == CRYPT_ERROR) return CRYPT_ERROR;

   /* pad it */
   x = sizeof(rsa_in);
   if (rsa_pad(rsa_in, hashlen, rsa_out, &x, wprng, prng) == CRYPT_ERROR) return CRYPT_ERROR;

   /* sign it */
   rsa_size = sizeof(rsa_in);
   if (rsa_exptmod(rsa_out, x, rsa_in, &rsa_size, PK_PRIVATE, key) == CRYPT_ERROR) return CRYPT_ERROR;

   /* check size */
   if (*outlen < (9+rsa_size+strlen(hash_descriptor[hash].name))) {
      crypt_error = "Buffer Overrun in rsa_sign()."; 
      return CRYPT_ERROR; 
   }

   /* now lets output the message */
   out[0] = 0x3C; 
   out[1] = 0x2B;
   out[2] = 0x1A;
   out[3] = strlen(hash_descriptor[hash].name) + 1;
   for (y = 4, x = 0; x <= strlen(hash_descriptor[hash].name); x++, y++)
       out[y] = hash_descriptor[hash].name[x];

   /* output the len */
   STORE32L(rsa_size, (out+y));
   y += 4;

   /* store the signature */
   for (x = 0; x < rsa_size; x++, y++)
       out[y] = rsa_in[x];

   /* clean up */
   memset(rsa_in, 0, sizeof(rsa_in));
   memset(rsa_out, 0, sizeof(rsa_out));
   *outlen = y;
   return CRYPT_OK;
}

int rsa_verify(unsigned char *sig, unsigned char *msg, int inlen, int *stat, struct rsa_key *key)
{
   int hash, hashlen, rsa_size, x, y, z;
   unsigned char rsa_in[4096], rsa_out[4096];

   /* always be correct by default */
   *stat = 1;

   /* verify header */
   if (sig[0] != 0x3C || sig[1] != 0x2B || sig[2] != 0x1A) {
      crypt_error = "Invalid header for input in rsa_verify().";
      return CRYPT_ERROR;
   }

   /* grab cipher name */
   hash = find_hash(&sig[4]);
   if (hash == -1) {
      crypt_error = "Invalid hash name for rsa_verify().";
      return CRYPT_ERROR;
   }
   hashlen = hash_descriptor[hash].hashsize;
   y = 4 + (int)(sig[3]&255);

   /* get the len */
   LOAD32L(rsa_size, (sig+y));
   y += 4;

   /* load the signature */
   for (x = 0; x < rsa_size; x++, y++)
       rsa_in[x] = sig[y];

   /* exptmod it */
   x = sizeof(rsa_in);
   if (rsa_exptmod(rsa_in, rsa_size, rsa_out, &x, PK_PUBLIC, key) == CRYPT_ERROR) return CRYPT_ERROR;

   /* depad it */
   z = sizeof(rsa_in);
   if (rsa_depad(rsa_out, x, rsa_in, &z) == CRYPT_ERROR) return CRYPT_ERROR;

   /* check? */
   if (z != hashlen) *stat = 0;
   hash_memory(hash, msg, inlen, rsa_out);
   if (memcmp(rsa_out, rsa_in, hashlen)) *stat = 0;

   memset(rsa_in, 0, sizeof(rsa_in));
   memset(rsa_out, 0, sizeof(rsa_out));
   return CRYPT_OK;
}