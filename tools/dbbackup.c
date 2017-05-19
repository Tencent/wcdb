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
 
#include <sqlite3.h>
#include "../mm_backup.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>


#define DBBAK_OP_BACKUP		1
#define DBBAK_OP_RECOVER	2

static int g_verbose = 0;
static int g_operation = 0;
static uint32_t g_flags = 0;
static const char *g_db_path = NULL;
static const char *g_dump_path = NULL;
static const uint8_t *g_key = NULL;
static const uint8_t *g_dbkey = NULL;
static const char *g_tabdesc_path = NULL;
static char *g_tabdesc_buffer = NULL;
static mm_backup_tabdesc *g_tabdesc = NULL;
static int g_num_tabdesc = 0;


extern void sqlcipher_set_default_kdf_iter(int iter);
extern void sqlcipher_set_default_use_hmac(int use);
extern void sqlcipher_set_default_pagesize(int page_size);


static const struct option g_options[] =
{
	{"verbose",			no_argument,		NULL, 'v'},
	{"compress",		no_argument,		NULL, 'z'},
	{"no-compress",		no_argument,		NULL, 'Z'},
	{"fix",				no_argument, 		NULL, 'f'},
	{"no-fix",			no_argument,		NULL, 'F'},
	{"create-table",	    no_argument, 		NULL, 't'},
	{"no-create-table",	no_argument, 		NULL, 'T'},
	{"incremental",		no_argument, 		NULL, 'i'},
	{"output",			required_argument, 	NULL, 'o'},
	{"tabdesc",			required_argument,	NULL, 'd'},
	{"dbkey",			required_argument,	NULL, 'K'},
	{"key",				required_argument,	NULL, 'k'},
	{"version",			required_argument,	NULL, 0x100},
	{"page-size",		required_argument,	NULL, 0x101},
};


static void usage(const char *argv0)
{
	printf("USAGE: %s <backup|recover> <db_path> <dump_path> <passwd> [encrypt]\n",
			argv0);
	exit(1);
}

static void print_log(int prio, const char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
}

static void parse_options(int argc, char *argv[])
{
	int opt;
	int value;
	
	// parse operation
	if (argc < 2) usage(argv[0]);
	
	const char *opstr = argv[1];
	if (!strcmp(opstr, "backup"))
		g_operation = DBBAK_OP_BACKUP;
	else if (!strcmp(opstr, "recover"))
		g_operation = DBBAK_OP_RECOVER;
	else usage(argv[0]);

	// default to SQLCipher version 1, thanks to kkdb.
	sqlcipher_set_default_kdf_iter(4000);
	sqlcipher_set_default_use_hmac(0);
	
	// parse options
	optind = 2;
	while ((opt = getopt_long(argc, argv, "vzZfFtTio:d:K:k:", g_options, NULL)) != -1)
	{
		switch (opt)
		{
		case 'v':	// verbose
			g_verbose = 1;
			break;
		case 'z':	// compress
			g_flags &= (~MMBAK_FLAG_NO_COMPRESS);
			break;
		case 'Z':	// no-compress
			g_flags |= MMBAK_FLAG_NO_COMPRESS;
			break;
		case 'f':	// fix
			g_flags |= MMBAK_FLAG_FIX_CORRUPTION;
			break;
		case 'F':	// no-fix
			g_flags &= (~MMBAK_FLAG_FIX_CORRUPTION);
			break;
		case 't':	// create-table
			g_flags &= (~MMBAK_FLAG_NO_CREATE_TABLE);
			break;
		case 'T':	// no-create-table
			g_flags |= MMBAK_FLAG_NO_CREATE_TABLE;
			break;
		case 'i':	// incremental
			g_flags |= MMBAK_FLAG_INCREMENTAL;
			break;
		case 'o':	// output
			g_dump_path = optarg;
			break;
		case 'd':	// tabdesc
			g_tabdesc_path = optarg;
			break;
		case 'K':	// dbkey
			g_dbkey = optarg;
			break;
		case 'k':	// key
			g_key = optarg;
			break;
		case 0x100:	// version
			value = atoi(optarg);
			if (value == 1)
			{
				sqlcipher_set_default_kdf_iter(4000);
				sqlcipher_set_default_use_hmac(0);
			}
			else if (value == 2)
			{
				sqlcipher_set_default_kdf_iter(4000);
				sqlcipher_set_default_use_hmac(1);
			}
			else if (value == 3)
			{
				sqlcipher_set_default_kdf_iter(64000);
				sqlcipher_set_default_use_hmac(1);
			}
			else
			{
				print_log(MMBAK_LOG_ERROR, "Version must be 1, 2 or 3");
				exit(-1);
			}
			break;
		case 0x101:	// page-size
			value = atoi(optarg);
			if (value != 512 && value != 1024 && value != 2048 && value != 4096 &&
					value != 8192 && value != 16384 && value != 32768 && value != 65536)
			{
				print_log(MMBAK_LOG_ERROR, "Page size must be 512, 1024, 2048, ..., 65536");
				exit(-1);
			}
			sqlcipher_set_default_pagesize(value);
			break;
		default:	// ?
			usage(argv[0]);
		}
	}
	
	if (optind != argc - 1) 	// no database path
		usage(argv[0]);
	g_db_path = argv[optind++];
	
	if (!g_dump_path)
	{
		print_log(MMBAK_LOG_ERROR, "Output path must be specified.");
		usage(argv[0]);
	}
	
	if (g_key == NULL)
		g_flags |= MMBAK_FLAG_NO_CIPHER;
}

static void parse_tabdesc(const char *path)
{
	FILE *fp = fopen(path, "r");
	if (!fp) goto bail;
	
	// get file length
	fseek(fp, 0, SEEK_END);
	long file_len = ftell(fp) + 1;
	fseek(fp, 0, SEEK_SET);
	
	// read whole file into buffer
	g_tabdesc_buffer = (char *) malloc(file_len);
	if (!g_tabdesc_buffer) goto bail;
	char *p = g_tabdesc_buffer;
	int count = 0;
	while (!feof(fp) && !ferror(fp) && p - g_tabdesc_buffer < file_len)
	{
		char *ret = fgets(p, g_tabdesc_buffer + file_len - p, fp);
		if (ret)
		{
			count++;
			char c;
			while (c = *p++)
			{
				if (c == '\n')
				{
					*(p - 1) = '\0';
					break;
				}
			}
		}
	}
	fclose(fp);
	fp = NULL;
	
	// translate strings to tabdesc
	g_tabdesc = (mm_backup_tabdesc *) calloc(count, sizeof(mm_backup_tabdesc));
	if (!g_tabdesc) goto bail;
	
	p = g_tabdesc_buffer;
	
	int i;
	for (i = 0; i < count; i++)
	{
		g_tabdesc[i].table = p;
		char c;
		while (c = *p++)
		{
			if (c == '|')
			{
				*(p - 1) = '\0';
				g_tabdesc[i].condition = p;
			}
		}
	}
	g_num_tabdesc = count;
	return;
		
bail:
	print_log(MMBAK_LOG_ERROR, "Cannot parse table descriptor.");
	if (fp) fclose(fp);
	exit(1);
}

int main(int argc, char *argv[])
{
	int ret;
	sqlite3 *db;
	
	parse_options(argc, argv);
	if (g_tabdesc_path) parse_tabdesc(g_tabdesc_path);
	
	int key_len = g_key ? strlen(g_key) : 0;
	
	ret = sqlite3_open(g_db_path, &db);
	if (ret != SQLITE_OK) return -3;
	
	if (g_dbkey)
		sqlite3_key(db, g_dbkey, strlen(g_dbkey));
	
	if (g_operation == DBBAK_OP_BACKUP)
	{
		mm_backup_ctx *ctx = mm_backup_init((const uint8_t *) g_key, key_len, g_dump_path,
				g_flags, print_log);
		if (!ctx) return -2;
		
		ret = mm_backup_run(ctx, db, g_tabdesc, g_num_tabdesc);
		if (ret != 0) return -4;
		
		mm_backup_finish(ctx);
	}
	else if (g_operation == DBBAK_OP_RECOVER)
	{
		mm_recover_ctx *ctx = mm_recover_init(g_dump_path, (const uint8_t *) g_key, key_len, print_log);
		if (!ctx) return -2;
		
		ret = mm_recover_run(ctx, db, 1);
		if (ret != 0) return -4;
		
		mm_recover_finish(ctx);
	}
	else usage(argv[0]);
	
	free(g_tabdesc_buffer);
	free(g_tabdesc);
	
	sqlite3_close(db);
	return 0;
}
