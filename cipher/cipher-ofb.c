/* cipher-ofb.c  - Generic OFB mode implementation
 * Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003
 *               2005, 2007, 2008, 2009, 2011 Free Software Foundation, Inc.
 *
 * This file is part of Libgcrypt.
 *
 * Libgcrypt is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser general Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * Libgcrypt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "g10lib.h"
#include "cipher.h"
#include "ath.h"
#include "bufhelp.h"
#include "./cipher-internal.h"


gcry_err_code_t
_gcry_cipher_ofb_encrypt (gcry_cipher_hd_t c,
                          unsigned char *outbuf, unsigned int outbuflen,
                          const unsigned char *inbuf, unsigned int inbuflen)
{
  unsigned char *ivp;
  size_t blocksize = c->cipher->blocksize;

  if (outbuflen < inbuflen)
    return GPG_ERR_BUFFER_TOO_SHORT;

  if ( inbuflen <= c->unused )
    {
      /* Short enough to be encoded by the remaining XOR mask. */
      /* XOR the input with the IV */
      ivp = c->u_iv.iv + c->cipher->blocksize - c->unused;
      buf_xor(outbuf, ivp, inbuf, inbuflen);
      c->unused -= inbuflen;
      return 0;
    }

  if( c->unused )
    {
      inbuflen -= c->unused;
      ivp = c->u_iv.iv + blocksize - c->unused;
      buf_xor(outbuf, ivp, inbuf, c->unused);
      outbuf += c->unused;
      inbuf += c->unused;
      c->unused = 0;
    }

  /* Now we can process complete blocks. */
  while ( inbuflen >= blocksize )
    {
      /* Encrypt the IV (and save the current one). */
      memcpy( c->lastiv, c->u_iv.iv, blocksize );
      c->cipher->encrypt ( &c->context.c, c->u_iv.iv, c->u_iv.iv );
      buf_xor(outbuf, c->u_iv.iv, inbuf, blocksize);
      outbuf += blocksize;
      inbuf += blocksize;
      inbuflen -= blocksize;
    }
  if ( inbuflen )
    { /* process the remaining bytes */
      memcpy( c->lastiv, c->u_iv.iv, blocksize );
      c->cipher->encrypt ( &c->context.c, c->u_iv.iv, c->u_iv.iv );
      c->unused = blocksize;
      c->unused -= inbuflen;
      buf_xor(outbuf, c->u_iv.iv, inbuf, inbuflen);
      outbuf += inbuflen;
      inbuf += inbuflen;
      inbuflen = 0;
    }
  return 0;
}


gcry_err_code_t
_gcry_cipher_ofb_decrypt (gcry_cipher_hd_t c,
                          unsigned char *outbuf, unsigned int outbuflen,
                          const unsigned char *inbuf, unsigned int inbuflen)
{
  unsigned char *ivp;
  size_t blocksize = c->cipher->blocksize;

  if (outbuflen < inbuflen)
    return GPG_ERR_BUFFER_TOO_SHORT;

  if( inbuflen <= c->unused )
    {
      /* Short enough to be encoded by the remaining XOR mask. */
      ivp = c->u_iv.iv + blocksize - c->unused;
      buf_xor(outbuf, ivp, inbuf, inbuflen);
      c->unused -= inbuflen;
      return 0;
    }

  if ( c->unused )
    {
      inbuflen -= c->unused;
      ivp = c->u_iv.iv + blocksize - c->unused;
      buf_xor(outbuf, ivp, inbuf, c->unused);
      outbuf += c->unused;
      inbuf += c->unused;
      c->unused = 0;
    }

  /* Now we can process complete blocks. */
  while ( inbuflen >= blocksize )
    {
      /* Encrypt the IV (and save the current one). */
      memcpy( c->lastiv, c->u_iv.iv, blocksize );
      c->cipher->encrypt ( &c->context.c, c->u_iv.iv, c->u_iv.iv );
      buf_xor(outbuf, c->u_iv.iv, inbuf, blocksize);
      outbuf += blocksize;
      inbuf += blocksize;
      inbuflen -= blocksize;
    }
  if ( inbuflen )
    { /* Process the remaining bytes. */
      /* Encrypt the IV (and save the current one). */
      memcpy( c->lastiv, c->u_iv.iv, blocksize );
      c->cipher->encrypt ( &c->context.c, c->u_iv.iv, c->u_iv.iv );
      c->unused = blocksize;
      c->unused -= inbuflen;
      buf_xor(outbuf, c->u_iv.iv, inbuf, inbuflen);
      outbuf += inbuflen;
      inbuf += inbuflen;
      inbuflen = 0;
    }
  return 0;
}
