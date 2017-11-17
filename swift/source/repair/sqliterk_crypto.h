/*
 * Tencent is pleased to support the open source community by making
 * WCDB available.
 *
 * Copyright (C) 2017 THL A29 Limited, a Tencent company.
 * All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use
 * this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *       https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef sqliterk_crypto_h
#define sqliterk_crypto_h

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sqliterk_file sqliterk_file;
typedef struct sqliterk_pager sqliterk_pager;
typedef struct codec_ctx sqliterk_codec;
typedef struct sqliterk_cipher_conf sqliterk_cipher_conf;

int sqliterkCryptoSetCipher(sqliterk_pager *pager,
                            sqliterk_file *fd,
                            const sqliterk_cipher_conf *conf);
void sqliterkCryptoFreeCodec(sqliterk_pager *pager);
int sqliterkCryptoDecode(sqliterk_codec *codec, int pgno, void *data);

#ifdef __cplusplus
}
#endif

#endif
