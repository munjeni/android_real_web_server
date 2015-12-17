#include "crypt.h"
#include <openssl/des.h>

char *crypt_error;

struct _cipher_descriptor cipher_descriptor[] = {
{
	"blowfish",
	8, 56, 8, 16,
	&blowfish_setup,
	&blowfish_ecb_encrypt,
	&blowfish_ecb_decrypt,
        &blowfish_test
},
{
	"rc5",
	8, 128, 8, 12,
	&rc5_setup,
	&rc5_ecb_encrypt,
	&rc5_ecb_decrypt,
        &rc5_test
},
{
	"rc6",
	8, 128, 16, 20,
	&rc6_setup,
	&rc6_ecb_encrypt,
	&rc6_ecb_decrypt,
        &rc6_test
},
{
	"safer+",
	16, 32, 16, 8,
	&saferp_setup,
	&saferp_ecb_encrypt,
	&saferp_ecb_decrypt,
        &saferp_test
},
{
	"serpent",
	16, 32, 16, 32,
	&serpent_setup,
	&serpent_ecb_encrypt,
	&serpent_ecb_decrypt,
        &serpent_test
},
{
	NULL,
	0, 0, 0, 0,
	NULL,
	NULL,
	NULL,
	NULL
}
};

struct _hash_descriptor hash_descriptor[] = {
{
	"sha256",
	32,
	&sha256_init,
	&sha256_process,
	&sha256_done,
        &sha256_test
},
{
	"tiger",
	24,
	&tiger_init,
	&tiger_process,
	&tiger_done,
        &tiger_test
},
{
	"sha1",
	20,
	&sha1_init,
	&sha1_process,
	&sha1_done,
        &sha1_test
},
{
	"md5",
	16,
	&md5_init,
	&md5_process,
	&md5_done,
        &md5_test
},
{
	NULL,
	0,
	NULL,
	NULL,
	NULL,
	NULL
}
};

struct _prng_descriptor prng_descriptor[] = {
{
	"yarrow",
	&yarrow_start,
	&yarrow_add_entropy,
	&yarrow_ready,
	&yarrow_read
},
{
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
}
};
int find_cipher(char *name)
{
   int x;

   for (x = 0; cipher_descriptor[x].name != NULL; x++)
       if (!strcmp(cipher_descriptor[x].name, name)) return x;
   return -1;
}

int find_hash(char *name)
{
   int x;

   for (x = 0; hash_descriptor[x].name != NULL; x++)
       if (!strcmp(hash_descriptor[x].name, name)) return x;
   return -1;
}

int find_prng(char *name)
{
   int x;

   for (x = 0; prng_descriptor[x].name != NULL; x++)
       if (!strcmp(prng_descriptor[x].name, name)) return x;
   return -1;
}

static char buffer[14];

char *crypt(const char *key, const char *salt)
{
	memset(buffer, '\0', 14);
	DES_fcrypt(key, salt, buffer);
	return buffer;
}

