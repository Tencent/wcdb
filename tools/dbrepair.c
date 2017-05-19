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

#include "../SQLiteRepairKit.h"
#include <sqlite3.h>
#include <stdio.h>
#include <getopt.h>

static char g_verbose = 0;
static const char *g_in_path = NULL;
static const char *g_out_path = NULL;
static const char *g_out_key = NULL;
static const char *g_save_master = NULL;
static const char *g_load_master = NULL;
static sqliterk_cipher_conf g_cipher_conf = {0};
static const char **g_filter = NULL;
static size_t g_num_filter = 0;
static size_t g_filter_capacity = 0;


extern void sqlcipher_set_default_kdf_iter(int iter);
extern void sqlcipher_set_default_use_hmac(int use);
extern void sqlcipher_set_default_pagesize(int page_size);


static const struct option g_options[] =
{
    {"help",            no_argument,        NULL, 'h'},
	{"verbose",			no_argument,		NULL, 'v'},
	{"output",			required_argument, 	NULL, 'o'},
	{"out-key",			required_argument,	NULL, 'K'},
	{"in-key",			required_argument,	NULL, 'k'},
	{"version",			required_argument,	NULL, 0x100},
	{"page-size",		required_argument,	NULL, 0x101},
	{"save-master",		required_argument,	NULL, 'M'},
	{"load-master",		required_argument,	NULL, 'm'},
	{"filter",			required_argument,	NULL, 'f'},
};


static void usage(const char *argv0)
{
	printf("USAGE:\n"
		"  %1$s --output=<output_path> [OPTIONS] <db_path>\n"
		"  %1$s --save-master=<master_path> [OPTIONS] <db_path>\n",
		argv0);
	puts("\n"
	"OUTPUT OPTIONS:\n"
	"  -o, --output=<path>        Output recovered data to new SQLite database.\n"
	"  -m, --load-master=<path>   Load previously saved master info from <path>.\n"
	"  -M, --save-master=<path>   Save master info and do no recover.\n"
	"                             <db_path> should be database that's not corrupted.\n"
	"  -f, --filter=<table>       Add <table> to the filter. If one or more table is in the\n"
	"                             filter, only filtered tables is read and recovered.\n"
	"\n"
	"CIPHER OPTIONS:\n"
	"  -k, --in-key=<key>         Specify the input key used to read database in <db_path>.\n"
	"  -K, --out-key=<key>        Specify the output key used to write '--output' database.\n"
	"      --page-size=<size>     Specify page size of the encrypted input database.\n"
	"      --version=<1|2|3>      Use SQLCipher format version 1.x, 2.x or 3.x.\n"
	"\n"
    "MISCELLANEOUS OPTIONS:\n"
    "  -h, --help                 Show this help message and exit.\n"
    "\n");
	exit(1);
}

static void print_log(sqliterk_loglevel level, int result, const char* msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
}

static void inline LOGI(const char *msg)
{
	print_log(sqliterk_loglevel_info, SQLITERK_OK, msg);
}

static void parse_options(int argc, char *argv[])
{
	int opt;
	int value;
	
	if (argc < 2) usage(argv[0]);

	// default to SQLCipher version 1, for compatibility to KKDB.
	sqlcipher_set_default_kdf_iter(4000);
	sqlcipher_set_default_use_hmac(0);
	
	g_cipher_conf.use_hmac = -1;

	// parse options
	optind = 1;
	while ((opt = getopt_long(argc, argv, "hvo:K:k:M:m:", g_options, NULL)) != -1)
	{
		switch (opt)
		{
        case 'h':   // help
            usage(argv[0]);
            break;
		case 'v':	// verbose
			g_verbose = 1;
			break;
		case 'o':	// output
			g_out_path = optarg;
			break;
		case 'K':	// out-key
			g_out_key = optarg;
			break;
		case 'k':	// in-key
			g_cipher_conf.key = optarg;
			g_cipher_conf.key_len = strlen(optarg);
			break;
		case 'f':	// filter
			if (g_num_filter == g_filter_capacity)
			{
				size_t sz = g_filter_capacity ? g_filter_capacity * 2 : 64;
				void *ptr = realloc(g_filter, sz * sizeof(const char *));
				if (!ptr) exit(-5);
				g_filter = (const char **) ptr;
				g_filter_capacity = sz;
			}
			g_filter[g_num_filter++] = optarg;
			break;
		case 'M':	// save-master
			g_save_master = optarg;
			break;
		case 'm':	// load-master
			g_load_master = optarg;
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
				LOGI("Version must be 1, 2 or 3");
				exit(-1);
			}
			break;
		case 0x101:	// page-size
			value = atoi(optarg);
			if (value != 512 && value != 1024 && value != 2048 && value != 4096 &&
					value != 8192 && value != 16384 && value != 32768 && value != 65536)
			{
				LOGI("Page size must be 512, 1024, 2048, ..., or 65536");
				exit(-1);
			}
            sqlcipher_set_default_pagesize(value);
			break;
        default:    // ?
            usage(argv[0]);
		}
	}
	
	if (optind != argc - 1) 	// no database path
		usage(argv[0]);
	g_in_path = argv[optind++];
	
	if (g_save_master && (g_load_master || g_out_path))
	{
		LOGI("--save-master must be used without --load-master or --output.");
		usage(argv[0]);
	}

	if (!g_out_path && !g_save_master)
	{
		LOGI("Output path must be specified.");
		usage(argv[0]);
	}
}

int main(int argc, char *argv[])
{
	int ret;
	sqlite3 *db;
	sqliterk *rk;
	
    sqliterk_os reg = { print_log };
    sqliterk_register(reg);

	parse_options(argc, argv);
	
	if (g_save_master)
	{
		ret = sqlite3_open(g_in_path, &db);
		if (ret != SQLITE_OK) return -3;
		if (g_cipher_conf.key)
			sqlite3_key(db, g_cipher_conf.key, g_cipher_conf.key_len);

		int out_key_len = g_out_key ? strlen(g_out_key) : 0;
		ret = sqliterk_save_master(db, g_save_master, NULL, 0);
		
		sqlite3_close(db);
		return (ret == SQLITERK_OK) ? 0 : -4;
	}
	else
	{
		sqliterk_master_info *master = NULL;
		unsigned char salt[16];
		if (g_load_master)
		{
			ret = sqliterk_load_master(g_load_master, NULL, 0, g_filter, g_num_filter,
				&master, salt);
			g_cipher_conf.kdf_salt = salt;
		}
		else
			ret = sqliterk_make_master(g_filter, g_num_filter, &master);
		if (ret != SQLITERK_OK) return -6;

		ret = sqliterk_open(g_in_path, g_cipher_conf.key ? &g_cipher_conf : NULL, &rk);
		if (ret != SQLITERK_OK) return -3;

		ret = sqlite3_open(g_out_path, &db);
		if (ret != SQLITE_OK) return -3;
		if (g_out_key)
			sqlite3_key(db, g_out_key, strlen(g_out_key));

		ret = sqliterk_output(rk, db, master, 0);

		sqliterk_free_master(master);
		sqlite3_close(db);
		sqliterk_close(rk);
		free(g_filter);

		return (ret == SQLITERK_OK) ? 0 : -4;
	}
}
