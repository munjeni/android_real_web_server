#include "crypt.h"

void ecb_tests(void)
{
 int x;
 unsigned char buf[8];
 unsigned long L;
 unsigned long long LL;

 L = 0x12345678;
 STORE32L(L, &buf[0]);
 L = 0;
 LOAD32L(L, &buf[0]);
 if (L != 0x12345678) printf("LOAD/STORE32 Little don't work\n");
 LL = 0x01020304050607;
 STORE64L(LL, &buf[0]);
 LL = 0;
 LOAD64L(LL, &buf[0])
 if (LL != 0x01020304050607) printf("LOAD/STORE64 Little don't work\n");

 for (x = 0; cipher_descriptor[x].name != NULL; x++) {
     printf("Testing: %15s, Key Size: %4d to %4d, Block Size: %3d, Default # of rounds: %2d, ", cipher_descriptor[x].name,
            cipher_descriptor[x].min_key_length*8,cipher_descriptor[x].max_key_length*8,
            cipher_descriptor[x].block_length*8, cipher_descriptor[x].default_rounds);
     if (cipher_descriptor[x].test() == CRYPT_ERROR)
        printf("Failed. Reason: %s\n", crypt_error);
     else 
        printf("Passed.\n");
 }
}

void cbc_tests(void)
{
 struct symmetric_CBC cbc;
 int x, y;
 unsigned char blk[32], ct[32], key[32], IV[32];

 /* ---- CBC ENCODING ---- */
 /* make up a block and IV */
 for (x = 0; x < 32; x++) blk[x] = IV[x] = x;

 /* now lets start a cbc session */
 if (cbc_start(find_cipher("safer+"), IV, key, 16, 0, &cbc) == CRYPT_ERROR) { printf("Error: %s\n", crypt_error); return; }

 /* now lets encode 32 bytes */
 for (x = 0; x < 2; x++)
    cbc_encrypt(blk+16*x, ct+16*x, &cbc);


 /* ---- CBC DECODING ---- */
 /* make up a IV */
 for (x = 0; x < 32; x++) IV[x] = x;

 /* now lets start a cbc session */
 if (cbc_start(find_cipher("safer+"), IV, key, 16, 0, &cbc) == CRYPT_ERROR) { printf("Error: %s\n", crypt_error); return; }

 /* now lets decode 32 bytes */
 for (x = 0; x < 2; x++)
    cbc_decrypt(ct+16*x, blk+16*x, &cbc);

 /* print output */
 printf("CBC    : ");
 for (x = y = 0; x < 32; x++) if (blk[x] != x) y = 1;
 printf("%s\n", y?"failed":"passed");
}

void ctr_tests(void)
{
 struct symmetric_CTR ctr;
 int x, y;
 unsigned char blk[32], ct[32], key[32], count[32];

 /* ---- CTR ENCODING ---- */
 /* make up a block and IV */
 for (x = 0; x < 32; x++) blk[x] = count[x] = x;

 /* now lets start a cbc session */
 if (ctr_start(find_cipher("rc5"), count, key, 8, 0, &ctr) == CRYPT_ERROR) { printf("Error: %s\n", crypt_error); return; }

 /* now lets encode 32 bytes */
 for (x = 0; x < 4; x++)
    ctr_encrypt(blk+8*x, ct+8*x, 8, &ctr);

 /* ---- CTR DECODING ---- */
 /* make up a IV */
 for (x = 0; x < 32; x++) count[x] = x;

 /* now lets start a cbc session */
 if (ctr_start(find_cipher("rc5"), count, key, 8, 0, &ctr) == CRYPT_ERROR) { printf("Error: %s\n", crypt_error); return; }

 /* now lets decode 32 bytes */
 for (x = 0; x < 4; x++)
    ctr_decrypt(ct+8*x, blk+8*x, 8, &ctr);

 /* print output */
 printf("CTR    : ");
 for (x = y = 0; x < 32; x++) if (blk[x] != x) y = 1;
 printf("%s\n", y?"failed":"passed");
}

void hash_tests(void)
{
 int x;
 for (x = 0; hash_descriptor[x].name != NULL; x++) {
     printf("Testing: %15s ", hash_descriptor[x].name);
     if (hash_descriptor[x].test() == CRYPT_ERROR)
        printf("Failed. Reason: %s\n", crypt_error);
     else 
        printf("Passed.\n");
 }
}

void pad_test(void)
{
 unsigned char in[100], out[100];
 int x, y;
 union prng_state prng;
 
 /* start the PRNG with some static junk */
 yarrow_start(&prng);
 yarrow_add_entropy("hello", 5, &prng);
 yarrow_ready(&prng);

 /* make a dummy message */
 for (x = 0; x < 16; x++) in[x] = x;

 /* pad the message so that random filler is placed before and after it */
 y = 100;
 if (rsa_pad(in, 16, out, &y, find_prng("yarrow"), &prng) == CRYPT_ERROR) { printf("Error: %s\n", crypt_error); return; }

 /* depad the message to get the original content */
 memset(in, 0, sizeof(in));
 x = 100;
 if (rsa_depad(out, y, in, &x) == CRYPT_ERROR) { printf("Error: %s\n", crypt_error); return; }

 /* check outcome */
 printf("rsa_pad: ");
 if (x != 16) { printf("Failed.  Wrong size.\n"); return; }
 for (x = 0; x < 16; x++) if (in[x] != x) { printf("Failed.  Expected %02x and got %02x.\n", x, in[x]); return; }
 printf("passed.\n");
}

void rsa_test(void)
{
 unsigned char in[4096], out[4096];
 int x, y;
 struct rsa_key key;
 union prng_state prng;
 
 /* start prng */
 yarrow_start(&prng);
 yarrow_add_entropy("hello", 5, &prng);
 yarrow_ready(&prng);
 for (x = 0; x < 8; x++) in[x] = x;

 /* make a 1024-bit RSA key */
 if (rsa_make_key(&prng, find_prng("yarrow"), 1024/8, 65537, &key) == CRYPT_ERROR) { printf("Error: %s\n", crypt_error); return; }

 /* ---- SINGLE ENCRYPT ---- */
 /* encrypt a short 8 byte string */
 y = sizeof(in);
 if (rsa_exptmod(in, 8, out, &y, PK_PUBLIC, &key) == CRYPT_ERROR) { printf("Error: %s\n", crypt_error); return; }

 /* decrypt it */
 x = sizeof(out);
 if (rsa_exptmod(out, y, in, &x, PK_PRIVATE, &key) == CRYPT_ERROR) { printf("Error: %s\n", crypt_error); return; }

 /* compare */
 printf("RSA    : ");
 for (x = 0; x < 8; x++) if (in[x] != x) { printf("Failed.\n"); return; }
 printf("passed.\n");

 /* ---- BLOCK ENCRYPT ---- */
 /* now lets test rsa_encrypt() */
 for (x = 0; x < 8; x++) in[x] = x;
 x = sizeof(out);
 if (rsa_encrypt(in, 8, out, &x, &prng, find_prng("yarrow"), find_cipher("blowfish"), &key) == CRYPT_ERROR) {
    printf("Error: %s\n", crypt_error); 
    return;
 }

 /* test rsa_decrypt() */
 memset(in, 0, sizeof(in));
 y = sizeof(in);
 if (rsa_decrypt(out, x, in, &y, &key) == CRYPT_ERROR) {
    printf("Error: %s\n", crypt_error); 
    return;
 }
 printf("rsa_encrypt()/rsa_decrypt(): ");
 for (y = 0; y < 8; y++) if (in[y] != y) { printf("failed.\n"); return; }
 printf("Passed.\n");

 /* ---- SIGNATURES ---- */
 x = sizeof(in);
 if (rsa_sign("hello", 5, in, &x, find_hash("md5"), &prng, find_prng("yarrow"), &key) == CRYPT_ERROR) {
    printf("Error: %s\n", crypt_error);
    return;
 }
 if (rsa_verify(in, "hello", 5, &y, &key) == CRYPT_ERROR) { 
    printf("Error: %s\n", crypt_error);
    return;
 }
 printf("RSA Signatures: %s, ", (y==1)?"pass":"fail");
 if (rsa_verify(in, "abcde", 5, &y, &key) == CRYPT_ERROR) { 
    printf("Error: %s\n", crypt_error);
    return;
 }
 printf("%s\n", (y==0)?"pass":"fail");

 /* ---- EXPORT/IMPORT ---- */
 x = sizeof(out);
 if (rsa_export(out, &x, PK_PRIVATE, &key) == CRYPT_ERROR) {
    printf("Error: %s\n", crypt_error);
    return;
 }
 printf("RSA Export takes %d bytes\n", x);
 rsa_free(&key);
 if (rsa_import(out, &key) == CRYPT_ERROR) {
    printf("Error: %s\n", crypt_error);
    return;
 }
 printf("RSA Import: ");
 if (rsa_verify(in, "abcde", 5, &y, &key) == CRYPT_ERROR) { 
    printf("Error: %s\n", crypt_error);
    return;
 }
 printf("%s\n", (y==0)?"pass":"fail");
 rsa_free(&key);
}

void base64_test(void)
{
   unsigned char buf[2][100];
   int x, y;

   memset(buf, 0, 200);
   for (x = 0; x < 16; x++) buf[0][x] = x;
   
   x = 100;
   if (base64_encode(buf[0], 16, buf[1], &x) == CRYPT_ERROR) {
      printf("Error: %s\n", crypt_error);
      return;
   }
   printf("Base64 encoded 16 bytes to %d bytes...[%s]\n", x, buf[1]);
   memset(buf[0], 0, 100);
   y = 100;
   if (base64_decode(buf[1], x, buf[0], &y) == CRYPT_ERROR) {
      printf("Error: %s\n", crypt_error);
      return;
   }
   printf("Base64 decoded %d bytes to %d bytes\n", x, y);
   printf("base64 : ");
   for (x = 0; x < 16; x++) if (buf[0][x] != x) { printf("failed.\n"); return; }
   printf("passed.\n");
}

int main(void)
{
 ecb_tests();
 cbc_tests();
 ctr_tests();
 hash_tests();
 pad_test();
 rsa_test();
 base64_test();
 return 0;
}