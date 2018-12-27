/* 
** SQLCipher
** sqlcipher.h developed by Stephen Lombardo (Zetetic LLC) 
** sjlombardo at zetetic dot net
** http://zetetic.net
** 
** Copyright (c) 2008, ZETETIC LLC
** All rights reserved.
** 
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of the ZETETIC LLC nor the
**       names of its contributors may be used to endorse or promote products
**       derived from this software without specific prior written permission.
** 
** THIS SOFTWARE IS PROVIDED BY ZETETIC LLC ''AS IS'' AND ANY
** EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL ZETETIC LLC BE LIABLE FOR ANY
** DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
** (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
** ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**  
*/
/* BEGIN SQLCIPHER */
#ifdef SQLITE_HAS_CODEC
#ifndef SQLCIPHER_H
#define SQLCIPHER_H

#define SQLCIPHER_HMAC_SHA1 0
#define SQLCIPHER_HMAC_SHA1_LABEL "HMAC_SHA1"
#define SQLCIPHER_HMAC_SHA256 1
#define SQLCIPHER_HMAC_SHA256_LABEL "HMAC_SHA256"
#define SQLCIPHER_HMAC_SHA512 2
#define SQLCIPHER_HMAC_SHA512_LABEL "HMAC_SHA512"


#define SQLCIPHER_PBKDF2_HMAC_SHA1 0
#define SQLCIPHER_PBKDF2_HMAC_SHA1_LABEL "PBKDF2_HMAC_SHA1"
#define SQLCIPHER_PBKDF2_HMAC_SHA256 1
#define SQLCIPHER_PBKDF2_HMAC_SHA256_LABEL "PBKDF2_HMAC_SHA256"
#define SQLCIPHER_PBKDF2_HMAC_SHA512 2
#define SQLCIPHER_PBKDF2_HMAC_SHA512_LABEL "PBKDF2_HMAC_SHA512"


typedef struct {
  int (*activate)(void *ctx);
  int (*deactivate)(void *ctx);
  const char* (*get_provider_name)(void *ctx);
  int (*add_random)(void *ctx, void *buffer, int length);
  int (*random)(void *ctx, void *buffer, int length);
  int (*hmac)(void *ctx, int algorithm, unsigned char *hmac_key, int key_sz, unsigned char *in, int in_sz, unsigned char *in2, int in2_sz, unsigned char *out);
  int (*kdf)(void *ctx, int algorithm, const unsigned char *pass, int pass_sz, unsigned char* salt, int salt_sz, int workfactor, int key_sz, unsigned char *key);
  int (*cipher)(void *ctx, int mode, unsigned char *key, int key_sz, unsigned char *iv, unsigned char *in, int in_sz, unsigned char *out);
  const char* (*get_cipher)(void *ctx);
  int (*get_key_sz)(void *ctx);
  int (*get_iv_sz)(void *ctx);
  int (*get_block_sz)(void *ctx);
  int (*get_hmac_sz)(void *ctx, int algorithm);
  int (*ctx_copy)(void *target_ctx, void *source_ctx);
  int (*ctx_cmp)(void *c1, void *c2);
  int (*ctx_init)(void **ctx);
  int (*ctx_free)(void **ctx);
  int (*fips_status)(void *ctx);
  const char* (*get_provider_version)(void *ctx);
} sqlcipher_provider;

/* provider interfaces */
int sqlcipher_register_provider(sqlcipher_provider *p);
sqlcipher_provider* sqlcipher_get_provider();

#endif
#endif
/* END SQLCIPHER */

