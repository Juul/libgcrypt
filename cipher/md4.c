/* md4.c - MD4 Message-Digest Algorithm
 * Copyright (C) 2002, 2003 Free Software Foundation, Inc.
 *
 * This file is part of Libgcrypt.
 *
 * Libgcrypt is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * Libgcrypt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 *
 * Based on md5.c in libgcrypt, but rewritten to compute md4 checksums
 * using a public domain md4 implementation with the following comments:
 *
 * Modified by Wei Dai from Andrew M. Kuchling's md4.c
 * The original code and all modifications are in the public domain.
 *
 * This is the original introductory comment:
 *
 *  md4.c : MD4 hash algorithm.
 *
 * Part of the Python Cryptography Toolkit, version 1.1
 *
 * Distribute and use freely; there are no restrictions on further
 * dissemination and usage except those imposed by the laws of your
 * country of residence.
 *
 */

/* MD4 test suite:
 * MD4 ("") = 31d6cfe0d16ae931b73c59d7e0c089c0
 * MD4 ("a") = bde52cb31de33e46245e05fbdbd6fb24
 * MD4 ("abc") = a448017aaf21d8525fc10ae87aa6729d
 * MD4 ("message digest") = d9130a8164549fe818874806e1c7014b
 * MD4 ("abcdefghijklmnopqrstuvwxyz") = d79e1c308aa5bbcdeea8ed63df412da9
 * MD4 ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789") =
 * 043f8582f241db351ce627e153e7f0e4
 * MD4 ("123456789012345678901234567890123456789012345678901234567890123456
 * 78901234567890") = e33b4ddc9c38f2199c3e7b164fcc0536
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "g10lib.h"
#include "memory.h"
#include "dynload.h"

#include "bithelp.h"


typedef struct {
    u32 A,B,C,D;	  /* chaining variables */
    u32  nblocks;
    byte buf[64];
    int  count;
} MD4_CONTEXT;


static void
md4_init( MD4_CONTEXT *ctx )
{
    ctx->A = 0x67452301;
    ctx->B = 0xefcdab89;
    ctx->C = 0x98badcfe;
    ctx->D = 0x10325476;

    ctx->nblocks = 0;
    ctx->count = 0;
}

#define F(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))


/****************
 * transform 64 bytes
 */
static void
transform( MD4_CONTEXT *ctx, byte *data )
{
    u32 in[16];
    register u32 A = ctx->A;
    register u32 B = ctx->B;
    register u32 C = ctx->C;
    register u32 D = ctx->D;

#ifdef BIG_ENDIAN_HOST
    { int i;
      byte *p2, *p1;
      for(i=0, p1=data, p2=(byte*)in; i < 16; i++, p2 += 4 ) {
	p2[3] = *p1++;
	p2[2] = *p1++;
	p2[1] = *p1++;
	p2[0] = *p1++;
      }
    }
#else
    memcpy (in, data, 64);
#endif

    /* Round 1.  */
#define function(a,b,c,d,k,s) a=rol(a+F(b,c,d)+in[k],s);
          function(A,B,C,D, 0, 3);
          function(D,A,B,C, 1, 7);
          function(C,D,A,B, 2,11);
          function(B,C,D,A, 3,19);
          function(A,B,C,D, 4, 3);
          function(D,A,B,C, 5, 7);
          function(C,D,A,B, 6,11);
          function(B,C,D,A, 7,19);
          function(A,B,C,D, 8, 3);
          function(D,A,B,C, 9, 7);
          function(C,D,A,B,10,11);
          function(B,C,D,A,11,19);
          function(A,B,C,D,12, 3);
          function(D,A,B,C,13, 7);
          function(C,D,A,B,14,11);
          function(B,C,D,A,15,19);

#undef function

    /* Round 2.  */
#define function(a,b,c,d,k,s) a=rol(a+G(b,c,d)+in[k]+0x5a827999,s);

          function(A,B,C,D, 0, 3);
          function(D,A,B,C, 4, 5);
          function(C,D,A,B, 8, 9);
          function(B,C,D,A,12,13);
          function(A,B,C,D, 1, 3);
          function(D,A,B,C, 5, 5);
          function(C,D,A,B, 9, 9);
          function(B,C,D,A,13,13);
          function(A,B,C,D, 2, 3);
          function(D,A,B,C, 6, 5);
          function(C,D,A,B,10, 9);
          function(B,C,D,A,14,13);
          function(A,B,C,D, 3, 3);
          function(D,A,B,C, 7, 5);
          function(C,D,A,B,11, 9);
          function(B,C,D,A,15,13);

#undef function

    /* Round 3.  */
#define function(a,b,c,d,k,s) a=rol(a+H(b,c,d)+in[k]+0x6ed9eba1,s);

          function(A,B,C,D, 0, 3);
          function(D,A,B,C, 8, 9);
          function(C,D,A,B, 4,11);
          function(B,C,D,A,12,15);
          function(A,B,C,D, 2, 3);
          function(D,A,B,C,10, 9);
          function(C,D,A,B, 6,11);
          function(B,C,D,A,14,15);
          function(A,B,C,D, 1, 3);
          function(D,A,B,C, 9, 9);
          function(C,D,A,B, 5,11);
          function(B,C,D,A,13,15);
          function(A,B,C,D, 3, 3);
          function(D,A,B,C,11, 9);
          function(C,D,A,B, 7,11);
          function(B,C,D,A,15,15);


    /* Put checksum in context given as argument.  */
    ctx->A += A;
    ctx->B += B;
    ctx->C += C;
    ctx->D += D;
}



/* The routine updates the message-digest context to
 * account for the presence of each of the characters inBuf[0..inLen-1]
 * in the message whose digest is being computed.
 */
static void
md4_write( MD4_CONTEXT *hd, byte *inbuf, size_t inlen)
{
    if( hd->count == 64 ) { /* flush the buffer */
	transform( hd, hd->buf );
        _gcry_burn_stack (80+6*sizeof(void*));
	hd->count = 0;
	hd->nblocks++;
    }
    if( !inbuf )
	return;
    if( hd->count ) {
	for( ; inlen && hd->count < 64; inlen-- )
	    hd->buf[hd->count++] = *inbuf++;
	md4_write( hd, NULL, 0 );
	if( !inlen )
	    return;
    }
    _gcry_burn_stack (80+6*sizeof(void*));

    while( inlen >= 64 ) {
	transform( hd, inbuf );
	hd->count = 0;
	hd->nblocks++;
	inlen -= 64;
	inbuf += 64;
    }
    for( ; inlen && hd->count < 64; inlen-- )
	hd->buf[hd->count++] = *inbuf++;

}



/* The routine final terminates the message-digest computation and
 * ends with the desired message digest in mdContext->digest[0...15].
 * The handle is prepared for a new MD4 cycle.
 * Returns 16 bytes representing the digest.
 */

static void
md4_final( MD4_CONTEXT *hd )
{
    u32 t, msb, lsb;
    byte *p;

    md4_write(hd, NULL, 0); /* flush */;

    t = hd->nblocks;
    /* multiply by 64 to make a byte count */
    lsb = t << 6;
    msb = t >> 26;
    /* add the count */
    t = lsb;
    if( (lsb += hd->count) < t )
	msb++;
    /* multiply by 8 to make a bit count */
    t = lsb;
    lsb <<= 3;
    msb <<= 3;
    msb |= t >> 29;

    if( hd->count < 56 ) { /* enough room */
	hd->buf[hd->count++] = 0x80; /* pad */
	while( hd->count < 56 )
	    hd->buf[hd->count++] = 0;  /* pad */
    }
    else { /* need one extra block */
	hd->buf[hd->count++] = 0x80; /* pad character */
	while( hd->count < 64 )
	    hd->buf[hd->count++] = 0;
	md4_write(hd, NULL, 0);  /* flush */;
	memset(hd->buf, 0, 56 ); /* fill next block with zeroes */
    }
    /* append the 64 bit count */
    hd->buf[56] = lsb	   ;
    hd->buf[57] = lsb >>  8;
    hd->buf[58] = lsb >> 16;
    hd->buf[59] = lsb >> 24;
    hd->buf[60] = msb	   ;
    hd->buf[61] = msb >>  8;
    hd->buf[62] = msb >> 16;
    hd->buf[63] = msb >> 24;
    transform( hd, hd->buf );
    _gcry_burn_stack (80+6*sizeof(void*));

    p = hd->buf;
  #ifdef BIG_ENDIAN_HOST
    #define X(a) do { *p++ = hd->a      ; *p++ = hd->a >> 8;      \
		      *p++ = hd->a >> 16; *p++ = hd->a >> 24; } while(0)
  #else /* little endian */
    #define X(a) do { *(u32*)p = (*hd).a ; p += 4; } while(0)
  #endif
    X(A);
    X(B);
    X(C);
    X(D);
  #undef X

}

static byte *
md4_read( MD4_CONTEXT *hd )
{
    return hd->buf;
}

/****************
 * Return some information about the algorithm.  We need algo here to
 * distinguish different flavors of the algorithm.
 * Returns: A pointer to string describing the algorithm or NULL if
 *	    the ALGO is invalid.
 */
static const char *
md4_get_info( int algo, size_t *contextsize,
	       byte **r_asnoid, int *r_asnlen, int *r_mdlen,
	       void (**r_init)( void *c ),
	       void (**r_write)( void *c, byte *buf, size_t nbytes ),
	       void (**r_final)( void *c ),
	       byte *(**r_read)( void *c )
	     )
{
    static byte asn[18] = /* Object ID is 1.2.840.113549.2.4 */
		    { 0x30, 0x20, 0x30, 0x0c, 0x06, 0x08, 0x2a, 0x86,0x48,
		      0x86, 0xf7, 0x0d, 0x02, 0x04, 0x05, 0x00, 0x04, 0x10 };

    if( algo != 301 )
	return NULL;

    *contextsize = sizeof(MD4_CONTEXT);
    *r_asnoid = asn;
    *r_asnlen = DIM(asn);
    *r_mdlen = 16;
    *(void  (**)(MD4_CONTEXT *))r_init		       = md4_init;
    *(void  (**)(MD4_CONTEXT *, byte*, size_t))r_write = md4_write;
    *(void  (**)(MD4_CONTEXT *))r_final 	       = md4_final;
    *(byte *(**)(MD4_CONTEXT *))r_read		       = md4_read;

    return "MD4";
}


#ifndef IS_MODULE
static
#endif
const char * const gnupgext_version = "MD4 ($Revision$)";

static struct {
    int class;
    int version;
    int  value;
    void (*func)(void);
} func_table[] = {
    { 10, 1, 0, (void(*)(void))md4_get_info },
    { 11, 1, 301 },
};


#ifndef IS_MODULE
static
#endif
void *
gnupgext_enum_func( int what, int *sequence, int *class, int *vers )
{
    void *ret;
    int i = *sequence;

    do {
	if( i >= DIM(func_table) || i < 0 )
	    return NULL;
	*class = func_table[i].class;
	*vers  = func_table[i].version;
	switch( *class ) {
	  case 11: case 21: case 31: ret = &func_table[i].value; break;
	  default:		     ret = func_table[i].func; break;
	}
	i++;
    } while( what && what != *class );

    *sequence = i;
    return ret;
}




#ifndef IS_MODULE
void
_gcry_md4_constructor(void)
{
    _gcry_register_internal_cipher_extension( gnupgext_version, gnupgext_enum_func );
}
#endif

/* end of file */