/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <unistd.h>
#include "libcrypt.h"

char *crypt(const char *key, const char *salt)
{
	/* First, check if we are supposed to be using the MD5 replacement
	 * instead of DES...  */
	if (salt[0]=='$' && salt[1]=='1' && salt[2]=='$')
		return __md5_crypt((unsigned char*)key, (unsigned char*)salt);
	else
		return __des_crypt((unsigned char*)key, (unsigned char*)salt);
}

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <pwd.h>
#include <string.h>
//#include <crypt.h>
#include "libcrypt.h"

/* Re-entrantify me -- all this junk needs to be in 
 * struct crypt_data to make this really reentrant... */
static u_char	inv_key_perm[64];
static u_char	inv_comp_perm[56];
static u_char	u_sbox[8][64];
static u_char	un_pbox[32];
static u_int32_t en_keysl[16], en_keysr[16];
static u_int32_t de_keysl[16], de_keysr[16];
static u_int32_t ip_maskl[8][256], ip_maskr[8][256];
static u_int32_t fp_maskl[8][256], fp_maskr[8][256];
static u_int32_t key_perm_maskl[8][128], key_perm_maskr[8][128];
static u_int32_t comp_maskl[8][128], comp_maskr[8][128];
static u_int32_t saltbits;
static u_int32_t old_salt;
static u_int32_t old_rawkey0, old_rawkey1;


/* Static stuff that stays resident and doesn't change after 
 * being initialized, and therefore doesn't need to be made 
 * reentrant. */
static u_char	init_perm[64], final_perm[64];
static u_char	m_sbox[4][4096];
static u_int32_t psbox[4][256];




/* A pile of data */
static const u_char	ascii64[] = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static const u_char	IP[64] = {
	58, 50, 42, 34, 26, 18, 10,  2, 60, 52, 44, 36, 28, 20, 12,  4,
	62, 54, 46, 38, 30, 22, 14,  6, 64, 56, 48, 40, 32, 24, 16,  8,
	57, 49, 41, 33, 25, 17,  9,  1, 59, 51, 43, 35, 27, 19, 11,  3,
	61, 53, 45, 37, 29, 21, 13,  5, 63, 55, 47, 39, 31, 23, 15,  7
};

static const u_char	key_perm[56] = {
	57, 49, 41, 33, 25, 17,  9,  1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27, 19, 11,  3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,  7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29, 21, 13,  5, 28, 20, 12,  4
};

static const u_char	key_shifts[16] = {
	1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

static const u_char	comp_perm[48] = {
	14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10,
	23, 19, 12,  4, 26,  8, 16,  7, 27, 20, 13,  2,
	41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};

/*
 *	No E box is used, as it's replaced by some ANDs, shifts, and ORs.
 */

static const u_char	sbox[8][64] = {
	{
		14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
		 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
		 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
		15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13
	},
	{
		15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
		 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
		 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
		13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9
	},
	{
		10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
		13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
		13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
		 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12
	},
	{
		 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
		13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
		10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
		 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14
	},
	{
		 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
		14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
		 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
		11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3
	},
	{
		12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
		10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
		 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
		 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13
	},
	{
		 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
		13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
		 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
		 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12
	},
	{
		13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
		 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
		 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
		 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11
	}
};

static const u_char	pbox[32] = {
	16,  7, 20, 21, 29, 12, 28, 17,  1, 15, 23, 26,  5, 18, 31, 10,
	 2,  8, 24, 14, 32, 27,  3,  9, 19, 13, 30,  6, 22, 11,  4, 25
};

static const u_int32_t bits32[32] =
{
	0x80000000, 0x40000000, 0x20000000, 0x10000000,
	0x08000000, 0x04000000, 0x02000000, 0x01000000,
	0x00800000, 0x00400000, 0x00200000, 0x00100000,
	0x00080000, 0x00040000, 0x00020000, 0x00010000,
	0x00008000, 0x00004000, 0x00002000, 0x00001000,
	0x00000800, 0x00000400, 0x00000200, 0x00000100,
	0x00000080, 0x00000040, 0x00000020, 0x00000010,
	0x00000008, 0x00000004, 0x00000002, 0x00000001
};

static const u_char	bits8[8] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
static const u_int32_t *bits28, *bits24;


static int 
ascii_to_bin(char ch)
{
	if (ch > 'z')
		return(0);
	if (ch >= 'a')
		return(ch - 'a' + 38);
	if (ch > 'Z')
		return(0);
	if (ch >= 'A')
		return(ch - 'A' + 12);
	if (ch > '9')
		return(0);
	if (ch >= '.')
		return(ch - '.');
	return(0);
}

static void
des_init(void)
{
	int	i, j, b, k, inbit, obit;
	u_int32_t	*p, *il, *ir, *fl, *fr;
	static int des_initialised = 0;

	if (des_initialised==1)
	    return;

	old_rawkey0 = old_rawkey1 = 0L;
	saltbits = 0L;
	old_salt = 0L;
	bits24 = (bits28 = bits32 + 4) + 4;

	/*
	 * Invert the S-boxes, reordering the input bits.
	 */
	for (i = 0; i < 8; i++)
		for (j = 0; j < 64; j++) {
			b = (j & 0x20) | ((j & 1) << 4) | ((j >> 1) & 0xf);
			u_sbox[i][j] = sbox[i][b];
		}

	/*
	 * Convert the inverted S-boxes into 4 arrays of 8 bits.
	 * Each will handle 12 bits of the S-box input.
	 */
	for (b = 0; b < 4; b++)
		for (i = 0; i < 64; i++)
			for (j = 0; j < 64; j++)
				m_sbox[b][(i << 6) | j] =
					(u_char)((u_sbox[(b << 1)][i] << 4) |
					u_sbox[(b << 1) + 1][j]);

	/*
	 * Set up the initial & final permutations into a useful form, and
	 * initialise the inverted key permutation.
	 */
	for (i = 0; i < 64; i++) {
		init_perm[final_perm[i] = IP[i] - 1] = (u_char)i;
		inv_key_perm[i] = 255;
	}

	/*
	 * Invert the key permutation and initialise the inverted key
	 * compression permutation.
	 */
	for (i = 0; i < 56; i++) {
		inv_key_perm[key_perm[i] - 1] = (u_char)i;
		inv_comp_perm[i] = 255;
	}

	/*
	 * Invert the key compression permutation.
	 */
	for (i = 0; i < 48; i++) {
		inv_comp_perm[comp_perm[i] - 1] = (u_char)i;
	}

	/*
	 * Set up the OR-mask arrays for the initial and final permutations,
	 * and for the key initial and compression permutations.
	 */
	for (k = 0; k < 8; k++) {
		for (i = 0; i < 256; i++) {
			*(il = &ip_maskl[k][i]) = 0L;
			*(ir = &ip_maskr[k][i]) = 0L;
			*(fl = &fp_maskl[k][i]) = 0L;
			*(fr = &fp_maskr[k][i]) = 0L;
			for (j = 0; j < 8; j++) {
				inbit = 8 * k + j;
				if (i & bits8[j]) {
					if ((obit = init_perm[inbit]) < 32)
						*il |= bits32[obit];
					else
						*ir |= bits32[obit-32];
					if ((obit = final_perm[inbit]) < 32)
						*fl |= bits32[obit];
					else
						*fr |= bits32[obit - 32];
				}
			}
		}
		for (i = 0; i < 128; i++) {
			*(il = &key_perm_maskl[k][i]) = 0L;
			*(ir = &key_perm_maskr[k][i]) = 0L;
			for (j = 0; j < 7; j++) {
				inbit = 8 * k + j;
				if (i & bits8[j + 1]) {
					if ((obit = inv_key_perm[inbit]) == 255)
						continue;
					if (obit < 28)
						*il |= bits28[obit];
					else
						*ir |= bits28[obit - 28];
				}
			}
			*(il = &comp_maskl[k][i]) = 0L;
			*(ir = &comp_maskr[k][i]) = 0L;
			for (j = 0; j < 7; j++) {
				inbit = 7 * k + j;
				if (i & bits8[j + 1]) {
					if ((obit=inv_comp_perm[inbit]) == 255)
						continue;
					if (obit < 24)
						*il |= bits24[obit];
					else
						*ir |= bits24[obit - 24];
				}
			}
		}
	}

	/*
	 * Invert the P-box permutation, and convert into OR-masks for
	 * handling the output of the S-box arrays setup above.
	 */
	for (i = 0; i < 32; i++)
		un_pbox[pbox[i] - 1] = (u_char)i;

	for (b = 0; b < 4; b++)
		for (i = 0; i < 256; i++) {
			*(p = &psbox[b][i]) = 0L;
			for (j = 0; j < 8; j++) {
				if (i & bits8[j])
					*p |= bits32[un_pbox[8 * b + j]];
			}
		}

	des_initialised = 1;
}


static void
setup_salt(u_int32_t salt)
{
	u_int32_t	obit, saltbit;
	int	i;

	if (salt == old_salt)
		return;
	old_salt = salt;

	saltbits = 0L;
	saltbit = 1;
	obit = 0x800000;
	for (i = 0; i < 24; i++) {
		if (salt & saltbit)
			saltbits |= obit;
		saltbit <<= 1;
		obit >>= 1;
	}
}


static int
des_setkey(const char *key)
{
	u_int32_t	k0, k1, rawkey0, rawkey1;
	int		shifts, round;

	des_init();

	rawkey0 = ntohl(*(const u_int32_t *) key);
	rawkey1 = ntohl(*(const u_int32_t *) (key + 4));

	if ((rawkey0 | rawkey1)
	    && rawkey0 == old_rawkey0
	    && rawkey1 == old_rawkey1) {
		/*
		 * Already setup for this key.
		 * This optimisation fails on a zero key (which is weak and
		 * has bad parity anyway) in order to simplify the starting
		 * conditions.
		 */
		return(0);
	}
	old_rawkey0 = rawkey0;
	old_rawkey1 = rawkey1;

	/*
	 *	Do key permutation and split into two 28-bit subkeys.
	 */
	k0 = key_perm_maskl[0][rawkey0 >> 25]
	   | key_perm_maskl[1][(rawkey0 >> 17) & 0x7f]
	   | key_perm_maskl[2][(rawkey0 >> 9) & 0x7f]
	   | key_perm_maskl[3][(rawkey0 >> 1) & 0x7f]
	   | key_perm_maskl[4][rawkey1 >> 25]
	   | key_perm_maskl[5][(rawkey1 >> 17) & 0x7f]
	   | key_perm_maskl[6][(rawkey1 >> 9) & 0x7f]
	   | key_perm_maskl[7][(rawkey1 >> 1) & 0x7f];
	k1 = key_perm_maskr[0][rawkey0 >> 25]
	   | key_perm_maskr[1][(rawkey0 >> 17) & 0x7f]
	   | key_perm_maskr[2][(rawkey0 >> 9) & 0x7f]
	   | key_perm_maskr[3][(rawkey0 >> 1) & 0x7f]
	   | key_perm_maskr[4][rawkey1 >> 25]
	   | key_perm_maskr[5][(rawkey1 >> 17) & 0x7f]
	   | key_perm_maskr[6][(rawkey1 >> 9) & 0x7f]
	   | key_perm_maskr[7][(rawkey1 >> 1) & 0x7f];
	/*
	 *	Rotate subkeys and do compression permutation.
	 */
	shifts = 0;
	for (round = 0; round < 16; round++) {
		u_int32_t	t0, t1;

		shifts += key_shifts[round];

		t0 = (k0 << shifts) | (k0 >> (28 - shifts));
		t1 = (k1 << shifts) | (k1 >> (28 - shifts));

		de_keysl[15 - round] =
		en_keysl[round] = comp_maskl[0][(t0 >> 21) & 0x7f]
				| comp_maskl[1][(t0 >> 14) & 0x7f]
				| comp_maskl[2][(t0 >> 7) & 0x7f]
				| comp_maskl[3][t0 & 0x7f]
				| comp_maskl[4][(t1 >> 21) & 0x7f]
				| comp_maskl[5][(t1 >> 14) & 0x7f]
				| comp_maskl[6][(t1 >> 7) & 0x7f]
				| comp_maskl[7][t1 & 0x7f];

		de_keysr[15 - round] =
		en_keysr[round] = comp_maskr[0][(t0 >> 21) & 0x7f]
				| comp_maskr[1][(t0 >> 14) & 0x7f]
				| comp_maskr[2][(t0 >> 7) & 0x7f]
				| comp_maskr[3][t0 & 0x7f]
				| comp_maskr[4][(t1 >> 21) & 0x7f]
				| comp_maskr[5][(t1 >> 14) & 0x7f]
				| comp_maskr[6][(t1 >> 7) & 0x7f]
				| comp_maskr[7][t1 & 0x7f];
	}
	return(0);
}


static int
do_des(	u_int32_t l_in, u_int32_t r_in, u_int32_t *l_out, u_int32_t *r_out, int count)
{
	/*
	 *	l_in, r_in, l_out, and r_out are in pseudo-"big-endian" format.
	 */
	u_int32_t	l, r, *kl, *kr, *kl1, *kr1;
	u_int32_t	f, r48l, r48r;
	int		round;

	if (count == 0) {
		return(1);
	} else if (count > 0) {
		/*
		 * Encrypting
		 */
		kl1 = en_keysl;
		kr1 = en_keysr;
	} else {
		/*
		 * Decrypting
		 */
		count = -count;
		kl1 = de_keysl;
		kr1 = de_keysr;
	}

	/*
	 *	Do initial permutation (IP).
	 */
	l = ip_maskl[0][l_in >> 24]
	  | ip_maskl[1][(l_in >> 16) & 0xff]
	  | ip_maskl[2][(l_in >> 8) & 0xff]
	  | ip_maskl[3][l_in & 0xff]
	  | ip_maskl[4][r_in >> 24]
	  | ip_maskl[5][(r_in >> 16) & 0xff]
	  | ip_maskl[6][(r_in >> 8) & 0xff]
	  | ip_maskl[7][r_in & 0xff];
	r = ip_maskr[0][l_in >> 24]
	  | ip_maskr[1][(l_in >> 16) & 0xff]
	  | ip_maskr[2][(l_in >> 8) & 0xff]
	  | ip_maskr[3][l_in & 0xff]
	  | ip_maskr[4][r_in >> 24]
	  | ip_maskr[5][(r_in >> 16) & 0xff]
	  | ip_maskr[6][(r_in >> 8) & 0xff]
	  | ip_maskr[7][r_in & 0xff];

	while (count--) {
		/*
		 * Do each round.
		 */
		kl = kl1;
		kr = kr1;
		round = 16;
		while (round--) {
			/*
			 * Expand R to 48 bits (simulate the E-box).
			 */
			r48l	= ((r & 0x00000001) << 23)
				| ((r & 0xf8000000) >> 9)
				| ((r & 0x1f800000) >> 11)
				| ((r & 0x01f80000) >> 13)
				| ((r & 0x001f8000) >> 15);

			r48r	= ((r & 0x0001f800) << 7)
				| ((r & 0x00001f80) << 5)
				| ((r & 0x000001f8) << 3)
				| ((r & 0x0000001f) << 1)
				| ((r & 0x80000000) >> 31);
			/*
			 * Do salting for crypt() and friends, and
			 * XOR with the permuted key.
			 */
			f = (r48l ^ r48r) & saltbits;
			r48l ^= f ^ *kl++;
			r48r ^= f ^ *kr++;
			/*
			 * Do sbox lookups (which shrink it back to 32 bits)
			 * and do the pbox permutation at the same time.
			 */
			f = psbox[0][m_sbox[0][r48l >> 12]]
			  | psbox[1][m_sbox[1][r48l & 0xfff]]
			  | psbox[2][m_sbox[2][r48r >> 12]]
			  | psbox[3][m_sbox[3][r48r & 0xfff]];
			/*
			 * Now that we've permuted things, complete f().
			 */
			f ^= l;
			l = r;
			r = f;
		}
		r = l;
		l = f;
	}
	/*
	 * Do final permutation (inverse of IP).
	 */
	*l_out	= fp_maskl[0][l >> 24]
		| fp_maskl[1][(l >> 16) & 0xff]
		| fp_maskl[2][(l >> 8) & 0xff]
		| fp_maskl[3][l & 0xff]
		| fp_maskl[4][r >> 24]
		| fp_maskl[5][(r >> 16) & 0xff]
		| fp_maskl[6][(r >> 8) & 0xff]
		| fp_maskl[7][r & 0xff];
	*r_out	= fp_maskr[0][l >> 24]
		| fp_maskr[1][(l >> 16) & 0xff]
		| fp_maskr[2][(l >> 8) & 0xff]
		| fp_maskr[3][l & 0xff]
		| fp_maskr[4][r >> 24]
		| fp_maskr[5][(r >> 16) & 0xff]
		| fp_maskr[6][(r >> 8) & 0xff]
		| fp_maskr[7][r & 0xff];
	return(0);
}


#if 0
static int
des_cipher(const char *in, char *out, u_int32_t salt, int count)
{
	u_int32_t	l_out, r_out, rawl, rawr;
	int		retval;
	union {
		u_int32_t	*ui32;
		const char	*c;
	} trans;

	des_init();

	setup_salt(salt);

	trans.c = in;
	rawl = ntohl(*trans.ui32++);
	rawr = ntohl(*trans.ui32);

	retval = do_des(rawl, rawr, &l_out, &r_out, count);

	trans.c = out;
	*trans.ui32++ = htonl(l_out);
	*trans.ui32 = htonl(r_out);
	return(retval);
}
#endif


void
setkey(const char *key)
{
	int	i, j;
	u_int32_t	packed_keys[2];
	u_char	*p;

	p = (u_char *) packed_keys;

	for (i = 0; i < 8; i++) {
		p[i] = 0;
		for (j = 0; j < 8; j++)
			if (*key++ & 1)
				p[i] |= bits8[j];
	}
	des_setkey((char *)p);
}


void
encrypt(char *block, int flag)
{
	u_int32_t	io[2];
	u_char	*p;
	int	i, j;

	des_init();

	setup_salt(0L);
	p = (u_char*)block;
	for (i = 0; i < 2; i++) {
		io[i] = 0L;
		for (j = 0; j < 32; j++)
			if (*p++ & 1)
				io[i] |= bits32[j];
	}
	do_des(io[0], io[1], io, io + 1, flag ? -1 : 1);
	for (i = 0; i < 2; i++)
		for (j = 0; j < 32; j++)
			block[(i << 5) | j] = (io[i] & bits32[j]) ? 1 : 0;
}

char *__des_crypt(const unsigned char *key, const unsigned char *setting)
{
	u_int32_t	count, salt, l, r0, r1, keybuf[2];
	u_char		*p, *q;
	static char	output[21];

	des_init();

	/*
	 * Copy the key, shifting each character up by one bit
	 * and padding with zeros.
	 */
	q = (u_char *)keybuf;
	while (q - (u_char *)keybuf - 8) {
		*q++ = *key << 1;
		if (*(q - 1))
			key++;
	}
	if (des_setkey((char *)keybuf))
		return(NULL);

#if 0
	if (*setting == _PASSWORD_EFMT1) {
		int		i;
		/*
		 * "new"-style:
		 *	setting - underscore, 4 bytes of count, 4 bytes of salt
		 *	key - unlimited characters
		 */
		for (i = 1, count = 0L; i < 5; i++)
			count |= ascii_to_bin(setting[i]) << ((i - 1) * 6);

		for (i = 5, salt = 0L; i < 9; i++)
			salt |= ascii_to_bin(setting[i]) << ((i - 5) * 6);

		while (*key) {
			/*
			 * Encrypt the key with itself.
			 */
			if (des_cipher((char *)keybuf, (char *)keybuf, 0L, 1))
				return(NULL);
			/*
			 * And XOR with the next 8 characters of the key.
			 */
			q = (u_char *)keybuf;
			while (q - (u_char *)keybuf - 8 && *key)
				*q++ ^= *key++ << 1;

			if (des_setkey((char *)keybuf))
				return(NULL);
		}
		strncpy(output, setting, 9);

		/*
		 * Double check that we weren't given a short setting.
		 * If we were, the above code will probably have created
		 * wierd values for count and salt, but we don't really care.
		 * Just make sure the output string doesn't have an extra
		 * NUL in it.
		 */
		output[9] = '\0';
		p = (u_char *)output + strlen(output);
	} else 
#endif
	{
		/*
		 * "old"-style:
		 *	setting - 2 bytes of salt
		 *	key - up to 8 characters
		 */
		count = 25;

		salt = (ascii_to_bin(setting[1]) << 6)
		     |  ascii_to_bin(setting[0]);

		output[0] = setting[0];
		/*
		 * If the encrypted password that the salt was extracted from
		 * is only 1 character long, the salt will be corrupted.  We
		 * need to ensure that the output string doesn't have an extra
		 * NUL in it!
		 */
		output[1] = setting[1] ? setting[1] : output[0];

		p = (u_char *)output + 2;
	}
	setup_salt(salt);
	/*
	 * Do it.
	 */
	if (do_des(0L, 0L, &r0, &r1, (int)count))
		return(NULL);
	/*
	 * Now encode the result...
	 */
	l = (r0 >> 8);
	*p++ = ascii64[(l >> 18) & 0x3f];
	*p++ = ascii64[(l >> 12) & 0x3f];
	*p++ = ascii64[(l >> 6) & 0x3f];
	*p++ = ascii64[l & 0x3f];

	l = (r0 << 16) | ((r1 >> 16) & 0xffff);
	*p++ = ascii64[(l >> 18) & 0x3f];
	*p++ = ascii64[(l >> 12) & 0x3f];
	*p++ = ascii64[(l >> 6) & 0x3f];
	*p++ = ascii64[l & 0x3f];

	l = r1 << 2;
	*p++ = ascii64[(l >> 12) & 0x3f];
	*p++ = ascii64[(l >> 6) & 0x3f];
	*p++ = ascii64[l & 0x3f];
	*p = 0;

	return(output);
}
/*
 * MD5C.C - RSA Data Security, Inc., MD5 message-digest algorithm
 *
 * Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
 * rights reserved.
 *
 * License to copy and use this software is granted provided that it
 * is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 * Algorithm" in all material mentioning or referencing this software
 * or this function.
 *
 * License is also granted to make and use derivative works provided
 * that such works are identified as "derived from the RSA Data
 * Security, Inc. MD5 Message-Digest Algorithm" in all material
 * mentioning or referencing the derived work.
 *
 * RSA Data Security, Inc. makes no representations concerning either
 * the merchantability of this software or the suitability of this
 * software for any particular purpose. It is provided "as is"
 * without express or implied warranty of any kind.
 *
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
 *
 * $FreeBSD: src/lib/libmd/md5c.c,v 1.9.2.1 1999/08/29 14:57:12 peter Exp $
 *
 * This code is the same as the code published by RSA Inc.  It has been
 * edited for clarity and style only.
 *
 * ----------------------------------------------------------------------------
 * The md5_crypt() function was taken from freeBSD's libcrypt and contains 
 * this license: 
 *    "THE BEER-WARE LICENSE" (Revision 42):
 *     <phk@login.dknet.dk> wrote this file.  As long as you retain this notice you
 *     can do whatever you want with this stuff. If we meet some day, and you think
 *     this stuff is worth it, you can buy me a beer in return.   Poul-Henning Kamp
 *
 * $FreeBSD: src/lib/libcrypt/crypt.c,v 1.7.2.1 1999/08/29 14:56:33 peter Exp $
 *
 * ----------------------------------------------------------------------------
 * On April 19th, 2001 md5_crypt() was modified to make it reentrant 
 * by Erik Andersen <andersen@uclibc.org>
 *
 *
 * June 28, 2001             Manuel Novoa III
 *
 * "Un-inlined" code using loops and static const tables in order to
 * reduce generated code size (on i386 from approx 4k to approx 2.5k).
 *
 * June 29, 2001             Manuel Novoa III
 *
 * Completely removed static PADDING array.
 *
 * Reintroduced the loop unrolling in MD5_Transform and added the
 * MD5_SIZE_OVER_SPEED option for configurability.  Define below as:
 *       0    fully unrolled loops
 *       1    partially unrolled (4 ops per loop)
 *       2    no unrolling -- introduces the need to swap 4 variables (slow)
 *       3    no unrolling and all 4 loops merged into one with switch
 *               in each loop (glacial)
 * On i386, sizes are roughly (-Os -fno-builtin):
 *     0: 3k     1: 2.5k     2: 2.2k     3: 2k
 *
 *
 * Since SuSv3 does not require crypt_r, modified again August 7, 2002
 * by Erik Andersen to remove reentrance stuff...
 */

/*
 * Valid values are  1 (fastest/largest) to 3 (smallest/slowest).
 */
#define MD5_SIZE_OVER_SPEED 3

/**********************************************************************/

#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
//#include <crypt.h>
#include <sys/cdefs.h>
#include "libcrypt.h"

/* MD5 context. */
struct MD5Context {
  u_int32_t state[4];	/* state (ABCD) */
  u_int32_t count[2];	/* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];	/* input buffer */
};

static void   __md5_Init (struct MD5Context *);
static void   __md5_Update (struct MD5Context *, const unsigned char *, unsigned int);
static void   __md5_Pad (struct MD5Context *);
static void   __md5_Final (unsigned char [16], struct MD5Context *);
static void __md5_Transform __P((u_int32_t [4], const unsigned char [64]));


static const unsigned char __md5__magic[] = "$1$";	/* This string is magic for this algorithm.  Having 
						   it this way, we can get better later on */
static const unsigned char __md5_itoa64[] =		/* 0 ... 63 => ascii - 64 */
	"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";


#ifdef i386
#define __md5_Encode memcpy
#define __md5_Decode memcpy
#else /* i386 */

/*
 * __md5_Encodes input (u_int32_t) into output (unsigned char). Assumes len is
 * a multiple of 4.
 */

static void
__md5_Encode (unsigned char *output, u_int32_t *input, unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[j] = (unsigned char)(input[i] & 0xff);
		output[j+1] = (unsigned char)((input[i] >> 8) & 0xff);
		output[j+2] = (unsigned char)((input[i] >> 16) & 0xff);
		output[j+3] = (unsigned char)((input[i] >> 24) & 0xff);
	}
}

/*
 * __md5_Decodes input (unsigned char) into output (u_int32_t). Assumes len is
 * a multiple of 4.
 */

static void
__md5_Decode (u_int32_t *output, const unsigned char *input, unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((u_int32_t)input[j]) | (((u_int32_t)input[j+1]) << 8) |
		    (((u_int32_t)input[j+2]) << 16) | (((u_int32_t)input[j+3]) << 24);
}
#endif /* i386 */

/* F, G, H and I are basic MD5 functions. */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits. */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/*
 * FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
 * Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) { \
	(a) += F ((b), (c), (d)) + (x) + (u_int32_t)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}
#define GG(a, b, c, d, x, s, ac) { \
	(a) += G ((b), (c), (d)) + (x) + (u_int32_t)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}
#define HH(a, b, c, d, x, s, ac) { \
	(a) += H ((b), (c), (d)) + (x) + (u_int32_t)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}
#define II(a, b, c, d, x, s, ac) { \
	(a) += I ((b), (c), (d)) + (x) + (u_int32_t)(ac); \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
	}

/* MD5 initialization. Begins an MD5 operation, writing a new context. */

static void __md5_Init (struct MD5Context *context)
{
	context->count[0] = context->count[1] = 0;

	/* Load magic initialization constants.  */
	context->state[0] = 0x67452301;
	context->state[1] = 0xefcdab89;
	context->state[2] = 0x98badcfe;
	context->state[3] = 0x10325476;
}

/* 
 * MD5 block update operation. Continues an MD5 message-digest
 * operation, processing another message block, and updating the
 * context.
 */

static void __md5_Update ( struct MD5Context *context, const unsigned char *input, unsigned int inputLen)
{
	unsigned int i, idx, partLen;

	/* Compute number of bytes mod 64 */
	idx = (unsigned int)((context->count[0] >> 3) & 0x3F);

	/* Update number of bits */
	if ((context->count[0] += ((u_int32_t)inputLen << 3))
	    < ((u_int32_t)inputLen << 3))
		context->count[1]++;
	context->count[1] += ((u_int32_t)inputLen >> 29);

	partLen = 64 - idx;

	/* Transform as many times as possible. */
	if (inputLen >= partLen) {
		memcpy((void *)&context->buffer[idx], (const void *)input,
		    partLen);
		__md5_Transform (context->state, context->buffer);

		for (i = partLen; i + 63 < inputLen; i += 64)
			__md5_Transform (context->state, &input[i]);

		idx = 0;
	}
	else
		i = 0;

	/* Buffer remaining input */
	memcpy ((void *)&context->buffer[idx], (const void *)&input[i],
	    inputLen-i);
}

/*
 * MD5 padding. Adds padding followed by original length.
 */

static void __md5_Pad ( struct MD5Context *context)
{
	unsigned char bits[8];
	unsigned int idx, padLen;
	unsigned char PADDING[64];

	memset(PADDING, 0, sizeof(PADDING));
	PADDING[0] = 0x80;

	/* Save number of bits */
	__md5_Encode (bits, context->count, 8);

	/* Pad out to 56 mod 64. */
	idx = (unsigned int)((context->count[0] >> 3) & 0x3f);
	padLen = (idx < 56) ? (56 - idx) : (120 - idx);
	__md5_Update (context, PADDING, padLen);

	/* Append length (before padding) */
	__md5_Update (context, bits, 8);
}

/*
 * MD5 finalization. Ends an MD5 message-digest operation, writing the
 * the message digest and zeroizing the context.
 */

static void __md5_Final ( unsigned char digest[16], struct MD5Context *context)
{
	/* Do padding. */
	__md5_Pad (context);

	/* Store state in digest */
	__md5_Encode (digest, context->state, 16);

	/* Zeroize sensitive information. */
	memset ((void *)context, 0, sizeof (*context));
}

/* MD5 basic transformation. Transforms state based on block. */

static void
__md5_Transform (state, block)
	u_int32_t state[4];
	const unsigned char block[64];
{
	u_int32_t a, b, c, d, x[16];

#if MD5_SIZE_OVER_SPEED > 1
	u_int32_t temp;
	const char *ps;

	static const char S[] = {
		7, 12, 17, 22,
		5, 9, 14, 20,
		4, 11, 16, 23,
		6, 10, 15, 21
	};
#endif /* MD5_SIZE_OVER_SPEED > 1 */

#if MD5_SIZE_OVER_SPEED > 0
	const u_int32_t *pc;
	const char *pp;
	int i;

	static const u_int32_t C[] = {
								/* round 1 */
		0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
		0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
		0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
		0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
								/* round 2 */
		0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
		0xd62f105d, 0x2441453,  0xd8a1e681, 0xe7d3fbc8,
		0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
		0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
								/* round 3 */
		0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
		0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
		0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x4881d05,
		0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
								/* round 4 */
		0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
		0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
		0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
		0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
	};

	static const char P[] = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, /* 1 */
		1, 6, 11, 0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12, /* 2 */
		5, 8, 11, 14, 1, 4, 7, 10, 13, 0, 3, 6, 9, 12, 15, 2, /* 3 */
		0, 7, 14, 5, 12, 3, 10, 1, 8, 15, 6, 13, 4, 11, 2, 9  /* 4 */
	};

#endif /* MD5_SIZE_OVER_SPEED > 0 */

	__md5_Decode (x, block, 64);

	a = state[0]; b = state[1]; c = state[2]; d = state[3]; 

#if MD5_SIZE_OVER_SPEED > 2
	pc = C; pp = P; ps = S - 4;

	for ( i = 0 ; i < 64 ; i++ ) {
		if ((i&0x0f) == 0) ps += 4;
		temp = a;
		switch (i>>4) {
			case 0:
				temp += F(b,c,d);
				break;
			case 1:
				temp += G(b,c,d);
				break;
			case 2:
				temp += H(b,c,d);
				break;
			case 3:
				temp += I(b,c,d);
				break;
		}
		temp += x[(int)(*pp++)] + *pc++;
		temp = ROTATE_LEFT(temp, ps[i&3]);
		temp += b;
		a = d; d = c; c = b; b = temp;
	}
#elif MD5_SIZE_OVER_SPEED > 1
	pc = C; pp = P; ps = S;

	/* Round 1 */
	for ( i = 0 ; i < 16 ; i++ ) {
		FF (a, b, c, d, x[(int)(*pp++)], ps[i&0x3], *pc++);
		temp = d; d = c; c = b; b = a; a = temp;
	}

	/* Round 2 */
	ps += 4;
	for ( ; i < 32 ; i++ ) {
		GG (a, b, c, d, x[(int)(*pp++)], ps[i&0x3], *pc++);
		temp = d; d = c; c = b; b = a; a = temp;
	}
	/* Round 3 */
	ps += 4;
	for ( ; i < 48 ; i++ ) {
		HH (a, b, c, d, x[(int)(*pp++)], ps[i&0x3], *pc++);
		temp = d; d = c; c = b; b = a; a = temp;
	}

	/* Round 4 */
	ps += 4;
	for ( ; i < 64 ; i++ ) {
		II (a, b, c, d, x[(int)(*pp++)], ps[i&0x3], *pc++);
		temp = d; d = c; c = b; b = a; a = temp;
	}
#elif MD5_SIZE_OVER_SPEED > 0
	pc = C; pp = P;

	/* Round 1 */
	for ( i = 0 ; i < 4 ; i++ ) {
		FF (a, b, c, d, x[(int)(*pp++)],  7, *pc++);
		FF (d, a, b, c, x[(int)(*pp++)], 12, *pc++);
		FF (c, d, a, b, x[(int)(*pp++)], 17, *pc++);
		FF (b, c, d, a, x[(int)(*pp++)], 22, *pc++);
	}

	/* Round 2 */
	for ( i = 0 ; i < 4 ; i++ ) {
		GG (a, b, c, d, x[(int)(*pp++)],  5, *pc++);
		GG (d, a, b, c, x[(int)(*pp++)],  9, *pc++);
		GG (c, d, a, b, x[(int)(*pp++)], 14, *pc++);
		GG (b, c, d, a, x[(int)(*pp++)], 20, *pc++);
	}
	/* Round 3 */
	for ( i = 0 ; i < 4 ; i++ ) {
		HH (a, b, c, d, x[(int)(*pp++)],  4, *pc++);
		HH (d, a, b, c, x[(int)(*pp++)], 11, *pc++);
		HH (c, d, a, b, x[(int)(*pp++)], 16, *pc++);
		HH (b, c, d, a, x[(int)(*pp++)], 23, *pc++);
	}

	/* Round 4 */
	for ( i = 0 ; i < 4 ; i++ ) {
		II (a, b, c, d, x[(int)(*pp++)],  6, *pc++);
		II (d, a, b, c, x[(int)(*pp++)], 10, *pc++);
		II (c, d, a, b, x[(int)(*pp++)], 15, *pc++);
		II (b, c, d, a, x[(int)(*pp++)], 21, *pc++);
	}
#else
	/* Round 1 */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
	FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
	FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
	FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
	FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
	FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
	FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
	FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
	FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
	FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
	FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
	FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

	/* Round 2 */
#define S21 5
#define S22 9
#define S23 14
#define S24 20
	GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
	GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
	GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
	GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
	GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
	GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
	GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
	GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
	GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
	GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
	GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
	GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

	/* Round 3 */
#define S31 4
#define S32 11
#define S33 16
#define S34 23
	HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
	HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
	HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
	HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
	HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
	HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
	HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
	HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
	HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
	HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

	/* Round 4 */
#define S41 6
#define S42 10
#define S43 15
#define S44 21
	II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
	II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
	II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
	II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
	II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
	II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
	II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
	II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
	II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
	II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */
#endif

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;

	/* Zeroize sensitive information. */
	memset ((void *)x, 0, sizeof (x));
}


static void __md5_to64( char *s, unsigned long v, int n)
{
	while (--n >= 0) {
		*s++ = __md5_itoa64[v&0x3f];
		v >>= 6;
	}
}

/*
 * UNIX password
 *
 * Use MD5 for what it is best at...
 */

char *__md5_crypt(const unsigned char *pw, const unsigned char *salt)
{
	/* Static stuff */
	static const unsigned char *sp, *ep;
	static char passwd[120], *p;

	unsigned char	final[17];	/* final[16] exists only to aid in looping */
	int sl,pl,i,__md5__magic_len,pw_len;
	struct MD5Context ctx,ctx1;
	unsigned long l;

	/* Refine the Salt first */
	sp = salt;

	/* If it starts with the magic string, then skip that */
	__md5__magic_len = strlen(__md5__magic);
	if(!strncmp(sp,__md5__magic,__md5__magic_len))
		sp += __md5__magic_len;

	/* It stops at the first '$', max 8 chars */
	for(ep=sp;*ep && *ep != '$' && ep < (sp+8);ep++)
		continue;

	/* get the length of the true salt */
	sl = ep - sp;

	__md5_Init(&ctx);

	/* The password first, since that is what is most unknown */
	pw_len = strlen(pw);
	__md5_Update(&ctx,pw,pw_len);

	/* Then our magic string */
	__md5_Update(&ctx,__md5__magic,__md5__magic_len);

	/* Then the raw salt */
	__md5_Update(&ctx,sp,sl);

	/* Then just as many characters of the MD5(pw,salt,pw) */
	__md5_Init(&ctx1);
	__md5_Update(&ctx1,pw,pw_len);
	__md5_Update(&ctx1,sp,sl);
	__md5_Update(&ctx1,pw,pw_len);
	__md5_Final(final,&ctx1);
	for(pl = pw_len; pl > 0; pl -= 16)
		__md5_Update(&ctx,final,pl>16 ? 16 : pl);

	/* Don't leave anything around in vm they could use. */
	memset(final,0,sizeof final);

	/* Then something really weird... */
	for (i = pw_len; i ; i >>= 1) {
		__md5_Update(&ctx, ((i&1) ? final : (const unsigned char *) pw), 1);
	}

	/* Now make the output string */
	strcpy(passwd,__md5__magic);
	strncat(passwd,sp,sl);
	strcat(passwd,"$");

	__md5_Final(final,&ctx);

	/*
	 * and now, just to make sure things don't run too fast
	 * On a 60 Mhz Pentium this takes 34 msec, so you would
	 * need 30 seconds to build a 1000 entry dictionary...
	 */
	for(i=0;i<1000;i++) {
		__md5_Init(&ctx1);
		if(i & 1)
			__md5_Update(&ctx1,pw,pw_len);
		else
			__md5_Update(&ctx1,final,16);

		if(i % 3)
			__md5_Update(&ctx1,sp,sl);

		if(i % 7)
			__md5_Update(&ctx1,pw,pw_len);

		if(i & 1)
			__md5_Update(&ctx1,final,16);
		else
			__md5_Update(&ctx1,pw,pw_len);
		__md5_Final(final,&ctx1);
	}

	p = passwd + strlen(passwd);

	final[16] = final[5];
	for ( i=0 ; i < 5 ; i++ ) {
		l = (final[i]<<16) | (final[i+6]<<8) | final[i+12];
		__md5_to64(p,l,4); p += 4;
	}
	l = final[11];
	__md5_to64(p,l,2); p += 2;
	*p = '\0';

	/* Don't leave anything around in vm they could use. */
	memset(final,0,sizeof final);

	return passwd;
}

#include "passwd_common.h"
#include "apr_strings.h"
#include "apr_errno.h"

#if APR_HAVE_STDIO_H
#include <stdio.h>
#endif

#include "apr_md5.h"
#include "apr_sha1.h"

#if APR_HAVE_TIME_H
#include <time.h>
#endif
#if APR_HAVE_CRYPT_H
#include <crypt.h>
#endif
#if APR_HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if APR_HAVE_STRING_H
#include <string.h>
#endif
#if APR_HAVE_UNISTD_H
#include <unistd.h>
#endif
#if APR_HAVE_IO_H
#include <io.h>
#endif

#ifdef _MSC_VER
#define write _write
#endif

apr_file_t *errfile;

int abort_on_oom(int rc)
{
    const char *buf = "Error: out of memory\n";
    int written, count = strlen(buf);
    do {
        written = write(STDERR_FILENO, buf, count);
        if (written == count)
            break;
        if (written > 0) {
            buf += written;
            count -= written;
        }
    } while (written >= 0 || errno == EINTR);
    abort();
    /* NOTREACHED */
    return 0;
}

static int generate_salt(char *s, size_t size, const char **errstr,
                         apr_pool_t *pool)
{
    unsigned char rnd[32];
    static const char itoa64[] =
        "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    apr_size_t n;
    unsigned int val = 0, bits = 0;
    apr_status_t rv;

    n = (size * 6 + 7)/8;
    if (n > sizeof(rnd)) {
        apr_file_printf(errfile, "generate_salt(): BUG: Buffer too small");
        abort();
    }
    rv = apr_generate_random_bytes(rnd, n);
    if (rv) {
        *errstr = apr_psprintf(pool, "Unable to generate random bytes: %pm",
                               &rv);
        return ERR_RANDOM;
    }
    n = 0;
    while (size > 0) {
        if (bits < 6) {
            val |= (rnd[n++] << bits);
            bits += 8;
        }
        *s++ = itoa64[val & 0x3f];
        size--;
        val >>= 6;
        bits -= 6;
   }
   *s = '\0';
   return 0;
}

void putline(apr_file_t *f, const char *l)
{
    apr_status_t rv;
    if (f == NULL)
        return;
    rv = apr_file_puts(l, f);
    if (rv != APR_SUCCESS) {
        apr_file_printf(errfile, "Error writing temp file: %pm", &rv);
        apr_file_close(f);
        exit(ERR_FILEPERM);
    }
}

int get_password(struct passwd_ctx *ctx)
{
    char buf[MAX_STRING_LEN + 1];
    if (ctx->passwd_src == PW_STDIN) {
        apr_file_t *file_stdin;
        apr_size_t nread;
        if (apr_file_open_stdin(&file_stdin, ctx->pool) != APR_SUCCESS) {
            ctx->errstr = "Unable to read from stdin.";
            return ERR_GENERAL;
        }
        if (apr_file_read_full(file_stdin, buf, sizeof(buf) - 1,
                               &nread) != APR_EOF
            || nread == sizeof(buf) - 1) {
            goto err_too_long;
        }
        buf[nread] = '\0';
        if (nread >= 1 && buf[nread-1] == '\n') {
            buf[nread-1] = '\0';
            if (nread >= 2 && buf[nread-2] == '\r')
                buf[nread-2] = '\0';
        }
        apr_file_close(file_stdin);
        ctx->passwd = apr_pstrdup(ctx->pool, buf);
    }
    else if (ctx->passwd_src == PW_PROMPT_VERIFY) {
        apr_size_t bufsize = sizeof(buf);
        if (apr_password_get("Enter password: ", buf, &bufsize) != 0)
            goto err_too_long;
        ctx->passwd = apr_pstrdup(ctx->pool, buf);
    }
    else {
        apr_size_t bufsize = sizeof(buf);
        if (apr_password_get("New password: ", buf, &bufsize) != 0)
            goto err_too_long;
        ctx->passwd = apr_pstrdup(ctx->pool, buf);
        bufsize = sizeof(buf);
        buf[0] = '\0';
        apr_password_get("Re-type new password: ", buf, &bufsize);
        if (strcmp(ctx->passwd, buf) != 0) {
            ctx->errstr = "password verification error";
            memset(ctx->passwd, '\0', strlen(ctx->passwd));
            memset(buf, '\0', sizeof(buf));
            return ERR_PWMISMATCH;
        }
    }
    memset(buf, '\0', sizeof(buf));
    return 0;

err_too_long:
    ctx->errstr = apr_psprintf(ctx->pool,
                               "password too long (>%" APR_SIZE_T_FMT ")",
                               ctx->out_len - 1);
    return ERR_OVERFLOW;
}

/*
 * Make a password record from the given information.  A zero return
 * indicates success; on failure, ctx->errstr points to the error message.
 */
int mkhash(struct passwd_ctx *ctx)
{
    char *pw;
    char salt[16];
    apr_status_t rv;
    int ret = 0;
#if CRYPT_ALGO_SUPPORTED
    char *cbuf;
#endif

    if (ctx->cost != 0 && ctx->alg != ALG_BCRYPT) {
        apr_file_printf(errfile,
                        "Warning: Ignoring -C argument for this algorithm." NL);
    }

    if (ctx->passwd == NULL) {
        if ((ret = get_password(ctx)) != 0)
            return ret;
    }
    pw = ctx->passwd;

    switch (ctx->alg) {
    case ALG_APSHA:
        /* XXX out >= 28 + strlen(sha1) chars - fixed len SHA */
        apr_sha1_base64(pw, strlen(pw), ctx->out);
        break;

    case ALG_APMD5:
        ret = generate_salt(salt, 8, &ctx->errstr, ctx->pool);
        if (ret != 0)
            break;
        rv = apr_md5_encode(pw, salt, ctx->out, ctx->out_len);
        if (rv != APR_SUCCESS) {
            ctx->errstr = apr_psprintf(ctx->pool,
                                       "could not encode password: %pm", &rv);
            ret = ERR_GENERAL;
        }
        break;

    case ALG_PLAIN:
        /* XXX this len limitation is not in sync with any HTTPd len. */
        apr_cpystrn(ctx->out, pw, ctx->out_len);
        break;

#if CRYPT_ALGO_SUPPORTED
    case ALG_CRYPT:
        ret = generate_salt(salt, 8, &ctx->errstr, ctx->pool);
        if (ret != 0)
            break;
        cbuf = crypt(pw, salt);
        if (cbuf == NULL) {
            rv = APR_FROM_OS_ERROR(errno);
            ctx->errstr = apr_psprintf(ctx->pool, "crypt() failed: %pm", &rv);
            ret = ERR_PWMISMATCH;
            break;
        }

        apr_cpystrn(ctx->out, cbuf, ctx->out_len - 1);
        if (strlen(pw) > 8) {
            char *truncpw = apr_pstrdup(ctx->pool, pw);
            truncpw[8] = '\0';
            if (!strcmp(ctx->out, crypt(truncpw, salt))) {
                apr_file_printf(errfile, "Warning: Password truncated to 8 "
                                "characters by CRYPT algorithm." NL);
            }
            memset(truncpw, '\0', strlen(pw));
        }
        break;
#endif /* CRYPT_ALGO_SUPPORTED */

#if BCRYPT_ALGO_SUPPORTED
    case ALG_BCRYPT:
        rv = apr_generate_random_bytes((unsigned char*)salt, 16);
        if (rv != APR_SUCCESS) {
            ctx->errstr = apr_psprintf(ctx->pool, "Unable to generate random "
                                       "bytes: %pm", &rv);
            ret = ERR_RANDOM;
            break;
        }

        if (ctx->cost == 0)
            ctx->cost = BCRYPT_DEFAULT_COST;
        rv = apr_bcrypt_encode(pw, ctx->cost, (unsigned char*)salt, 16,
                               ctx->out, ctx->out_len);
        if (rv != APR_SUCCESS) {
            ctx->errstr = apr_psprintf(ctx->pool, "Unable to encode with "
                                       "bcrypt: %pm", &rv);
            ret = ERR_PWMISMATCH;
            break;
        }
        break;
#endif /* BCRYPT_ALGO_SUPPORTED */

    default:
        apr_file_printf(errfile, "mkhash(): BUG: invalid algorithm %d",
                        ctx->alg);
        abort();
    }
    memset(pw, '\0', strlen(pw));
    return ret;
}

int parse_common_options(struct passwd_ctx *ctx, char opt,
                          const char *opt_arg)
{
    switch (opt) {
    case 'b':
        ctx->passwd_src = PW_ARG;
        break;
    case 'i':
        ctx->passwd_src = PW_STDIN;
        break;
    case 'm':
        ctx->alg = ALG_APMD5;
        break;
    case 's':
        ctx->alg = ALG_APSHA;
        break;
    case 'p':
        ctx->alg = ALG_PLAIN;
#if !PLAIN_ALGO_SUPPORTED
        /* Backward compatible behavior: Just print a warning */
        apr_file_printf(errfile,
                        "Warning: storing passwords as plain text might just "
                        "not work on this platform." NL);
#endif
        break;
    case 'd':
#if CRYPT_ALGO_SUPPORTED
        ctx->alg = ALG_CRYPT;
#else
        /* Backward compatible behavior: Use MD5. OK since MD5 is more secure */
        apr_file_printf(errfile,
                        "Warning: CRYPT algorithm not supported on this "
                        "platform." NL
                        "Automatically using MD5 format." NL);
        ctx->alg = ALG_APMD5;
#endif
        break;
    case 'B':
#if BCRYPT_ALGO_SUPPORTED
        ctx->alg = ALG_BCRYPT;
#else
        /* Don't fall back to something less secure */
        ctx->errstr = "BCRYPT algorithm not supported on this platform";
        return ERR_ALG_NOT_SUPP;
#endif
        break;
    case 'C': {
            char *endptr;
            long num = strtol(opt_arg, &endptr, 10);
            if (*endptr != '\0' || num <= 0) {
                ctx->errstr = "argument to -C must be a positive integer";
                return ERR_SYNTAX;
            }
            ctx->cost = num;
            break;
        }
    default:
        apr_file_printf(errfile, 
                        "parse_common_options(): BUG: invalid option %c",
                        opt);
        abort();
    }
    return 0;
}
