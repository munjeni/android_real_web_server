#include "crypt.h"

int rsa_make_key(union prng_state *prng, int wprng, int size, long e, struct rsa_key *key)
{
   mp_int p, q, tmp1, tmp2;
   int res;   

   if ((size < (1024/8)) || (size > (4096/8))) {
      crypt_error = "Invalid key size in rsa_make_key()."; 
      return CRYPT_ERROR;
   }

   if ((e < 3) || (!(e & 1))) {
      crypt_error = "Invalid value of e in rsa_make_key().";
      return CRYPT_ERROR;
   }
 
   if (wprng == -1) {
      crypt_error = "Invalid prng given to rsa_make_key().";
      return CRYPT_ERROR; 
   }

   mp_init(&p);
   mp_init(&q);
   mp_init(&tmp1);
   mp_init(&tmp2);

   /* make primes */
   do { 
       if (rand_prime(&p, size/2, prng, wprng) == CRYPT_ERROR) { res = CRYPT_ERROR; goto done; }
       if (rand_prime(&q, size/2, prng, wprng) == CRYPT_ERROR) { res = CRYPT_ERROR; goto done; }
       mp_sub_d(&p, 1, &tmp1);
       mp_sub_d(&q, 1, &tmp2);
       mp_lcm(&tmp1, &tmp2, &tmp1);
       mp_set_int(&tmp2, e);
       mp_gcd(&tmp1, &tmp2, &tmp2);
   } while (mp_cmp_d(&tmp2, 1));
 
   /* make key */
   mp_init(&key->e);
   mp_init(&key->d);
   mp_init(&key->N);

   mp_set_int(&key->e, e);
   mp_invmod(&key->e, &tmp1, &key->d);
   mp_mul(&p, &q, &key->N);
   res = CRYPT_OK;
   key->type = PK_PRIVATE;
done:
   mp_clear(&tmp2);
   mp_clear(&tmp1);
   mp_clear(&p);
   mp_clear(&q);
   return res;
}

void rsa_free(struct rsa_key *key)
{
   mp_clear(&key->e);
   mp_clear(&key->d);
   mp_clear(&key->N);
}

int rsa_exptmod(unsigned char *in, int inlen, unsigned char *out, int *outlen, int which, struct rsa_key *key)
{
   mp_int tmp;
   unsigned char buf[4096];
   int x;

   if (which == PK_PRIVATE && key->type != PK_PRIVATE) {
      crypt_error = "Invalid key type in rsa_exptmod()."; 
      return CRYPT_ERROR;
   }

   /* init and copy into tmp */
   mp_init(&tmp);
   buf[0] = 0;
   memcpy(buf+1, in, inlen);
   mp_read_raw(&tmp, buf, inlen+1);

   /* exptmod it */
   mp_exptmod(&tmp, which==PK_PRIVATE?&key->d:&key->e, &key->N, &tmp);

   /* read it back */
   x = mp_raw_size(&tmp)-1;
   if (x > *outlen) {
      mp_clear(&tmp);
      memset(buf, 0, sizeof(buf));
      crypt_error = "Not enough room for result in rsa_exptmod().";
      return CRYPT_ERROR;
   }
   mp_toraw(&tmp, buf);
   memcpy(out, buf+1, x);
   *outlen = x;

   /* clean up and return */
   mp_clear(&tmp);
   memset(buf, 0, sizeof(buf));
   return CRYPT_OK;
}

int rsa_pad(unsigned char *in, int inlen, unsigned char *out, int *outlen, int wprng, union prng_state *prng)
{
   unsigned char buf[4096];
   int x;

   /* is output big enough? */
   if (*outlen < (3 * inlen)) { crypt_error = "Output not big enough in rsa_pad()."; return CRYPT_ERROR; }

   /* get random padding required */
   if (wprng == -1) { crypt_error = "Invalid PRNG given to rsa_pad()."; return CRYPT_ERROR; }
   if (prng_descriptor[wprng].read(buf, inlen*2-2, prng) != (inlen*2 - 2)) {
      crypt_error = "Error reading PRNG in rsa_pad()."; 
      return CRYPT_ERROR;
   }

   /* pad it */
   out[0] = 0xFF;
   for (x = 0; x < inlen-1; x++) out[x+1] = buf[x];
   for (x = 0; x < inlen; x++)   out[x+inlen] = in[x];
   for (x = 0; x < inlen-1; x++) out[x+inlen+inlen] = buf[x+inlen-1];
   out[inlen+inlen+inlen-1] = 0xFF;

   /* clear up and return */
   memset(buf, 0, sizeof(buf));
   *outlen = inlen*3;
   return CRYPT_OK;
}

int rsa_depad(unsigned char *in, int inlen, unsigned char *out, int *outlen)
{
   int x;
   if (*outlen < inlen/3) { crypt_error = "Output not big enough in rsa_depad()."; return CRYPT_ERROR; }
   for (x = 0; x < inlen/3; x++) out[x] = in[x+(inlen/3)];
   *outlen = inlen/3;
   return CRYPT_OK;
}

int rsa_export(unsigned char *out, int *outlen, int type, struct rsa_key *key)
{
   unsigned char buf[4096], buf2[4096];
   int x, y, z, outsize;

   /* type valid? */
   if (key->type != PK_PRIVATE && type == PK_PRIVATE) { crypt_error = "Invalid key type in rsa_export()."; return CRYPT_ERROR; }

   /* size valid ? */
   outsize = *outlen;
   if (outsize < 9) { crypt_error = "Invalid buffer size for rsa_export()."; return CRYPT_ERROR; }

   /* output type */
   buf2[0] = type;

   /* output modulus */
   mp_toraw(&key->N, buf);  
   z = mp_raw_size(&key->N);

   STORE32L(z, buf2+1);
   y = 5;
   for (x = 0; x < z; x++, y++)
       buf2[y] = buf[x];
  
   /* output public key */
   mp_toraw(&key->e, buf);  
   z = mp_raw_size(&key->e);
   STORE32L(z, buf2+y);
   y += 4;
   for (x = 0; x < z; x++, y++)
       buf2[y] = buf[x];
   
   if (type == PK_PRIVATE) {
      /* output public key */
      mp_toraw(&key->d, buf);  
      z = mp_raw_size(&key->d);
      STORE32L(z, buf2+y);
      y += 4;
      for (x = 0; x < z; x++, y++)
          buf2[y] = buf[x];
   }

   /* check size */
   if (outsize < y) { crypt_error = "Buffer overrun in rsa_export()."; return CRYPT_ERROR; }
   memcpy(out, buf2, y);
   *outlen = y;
   memset(buf, 0, sizeof(buf));
   memset(buf2, 0, sizeof(buf2));
   return CRYPT_OK;
}

int rsa_import(unsigned char *in, struct rsa_key *key)
{
   int x, y;

   /* init key */
   mp_init(&key->e);
   mp_init(&key->d);
   mp_init(&key->N);
   key->type = in[0];

   /* load modulus */
   LOAD32L(x, in+1);
   y = 5;
   mp_read_raw(&key->N, in+y, x);
   y += x;
  
   /* load public key*/
   LOAD32L(x, in+y);
   y += 4;
   mp_read_raw(&key->e, in+y, x);
   y += x;

   if (key->type == PK_PRIVATE) {
      /* load public key*/
      LOAD32L(x, in+y);
      y += 4;
      mp_read_raw(&key->e, in+y, x);
      y += x;
   }
   return CRYPT_OK;
}
