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

#include <sqlite3ext.h>
SQLITE_EXTENSION_INIT1

#include <icucompat.h>
#include "mm_fts.h"


const static uint8_t g_default_key[16] =
{
	0x8e, 0x65, 0xd8, 0x50, 
	0x1f, 0x94, 0x06, 0x22, 
	0xfa, 0x45, 0x68, 0xb6,
	0x69, 0x56, 0x34, 0x9c
};


#ifdef _WIN32
__declspec(dllexport)
#endif
int sqlite3_mmftsext_init(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi)
{
	int rc;
	const char *errmsg;
	SQLITE_EXTENSION_INIT2(pApi);
	
	// Load and initialize ICU library.
	if (init_icucompat() != 0)
	{
		*pzErrMsg = sqlite3_mprintf("failed to load ICU library.");
		return SQLITE_ERROR;
	}

	// Register tokenizer.
	rc = sqlite3_register_mm_tokenizer(db);
	if (rc != SQLITE_OK) goto bail;

	// Register text cipher.
	rc = sqlite3_register_mm_cipher(db, g_default_key);
	if (rc != SQLITE_OK) goto bail;

	// Register utility functions.
	rc = sqlite3_register_mm_utils(db);
	if (rc != SQLITE_OK) goto bail;

	*pzErrMsg = NULL;
	return SQLITE_OK;

bail:
	errmsg = sqlite3_errmsg(db);
	if (!errmsg) errmsg = "";
	*pzErrMsg = sqlite3_mprintf("Failed to register SQLite functions: %s, ErrCode: %d",
			errmsg, rc);
	return rc;
}
