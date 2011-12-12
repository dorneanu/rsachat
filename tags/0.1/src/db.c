/*
============================================================================
 Name        : db.c
 Author      : Victor Dorneanu
 Version     :
 Copyright   : check doc/COPYING
 Description : DB functions
 ============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>
#include <errno.h>
#include <wordexp.h>
#include "RSAChat.h"
#include "db.h"


/**
 * sqlite3 callback function
 */
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   NotUsed=0;
   azColName = NULL;
   int i;
   for(i=0; i<argc; i++){
     printf("%s\t", argv[i] ? argv[i]: "NULL");
   }
   printf("\n");
   return 0;
 }


/**
 * Opens connection to specified db
 *
 * @param db_name path to sqlite3 db
 * @return if successful return pointer to sqlite3 otherwise NULL
 */
sqlite3 *sqlite3_open_db(char *db_name) {
	sqlite3 *db;
	int rc;
	wordexp_t exp_result;

	// Perform word expansion like a posix-shell
	// ~ -> /home/<user>
	wordexp(db_name, &exp_result, 0);

	rc = sqlite3_open(exp_result.we_wordv[0], &db);
	if (rc) {
		M_PRINT_ERROR(sqlite3_errmsg(db));
		sqlite3_close(db);
		db = NULL;
	}

	wordfree(&exp_result);
	return db;
}


/**
 * Execute <query> on database
 */
void sqlite3_exec_query(sqlite3 *db, char *query) {
	int rc;
	char *sqlite_err_msg = NULL;

	if ((rc = sqlite3_exec(db, query, callback, 0, &sqlite_err_msg)) != SQLITE_OK) {
		M_ERROR;

		if (sqlite_err_msg) {
			M_PRINT_ERROR(sqlite_err_msg);
			free(sqlite_err_msg);
		}

		sqlite3_close(db);
	}
}
