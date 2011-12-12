/*
============================================================================
 Name        : key.c
 Author      : S. Gene G.
 Version     :
 Copyright   : check doc/COPYING
 Description : Create, modify RSA key pairs
 ============================================================================
*/

#include <assert.h>
#include <string.h>
#include <openssl/bn.h>
#include <sqlite3.h>
#include <errno.h>
#include "RSAChat.h"
#include "key.h"
#include "db.h"

/** 
 * creates a new Key
 *
 * Key needs to be freed with key_free
 * This Key contains to parts the v and N Value (v is usually e or d)
 * @return Pointer to Key
 */
Key* key_new()
{
	Key* _key = malloc(sizeof(Key));
	_key->N = BN_new();
	_key->v = BN_new();
	return _key;

}


/**
 * frees the Memory which _key points to
 */
void key_free(Key* _key)
{
	assert(_key != 0);
	BN_free(_key->N);
	BN_free(_key->v);
	free(_key);
}


/**
 * converts the _key into a String
 *
 * @param _key the which you want the String of
 * @return Pointer to a string, this memory has to be freed with free()
 */
char* key_key2str(Key const* _key)
{
	char *t1 = BN_bn2hex(_key->v);
	char *t2 = BN_bn2hex(_key->N);
	int len = strlen(t1) + strlen(t2) + 2;
	char *ret = malloc(len);

	sprintf(ret,"%s %s",t1,t2);
	OPENSSL_free(t1);
	OPENSSL_free(t2);
	return ret;
}


/**
 * converts a str to a Key
 *
 * @param _from needs to be a nullterminated String
 * @return Pointer to the Key, musst be freed with key_free()
 */

Key* key_str2key(char const* _from)
{
	Key* _key = key_new();

	char const* s_key1 = _from;
	char const* s_key2 = _from;
	while(*(s_key2++) != ' ' && s_key2 != '\0');

	BN_hex2bn(&_key->v,s_key1);
	BN_hex2bn(&_key->N,s_key2);
	return _key;
}



/**
 * Gets RSA private key from DB specified by nickname
 *
 * @param db DB connection
 */
Key *key_get_private_key(sqlite3 *db, char *nickname){
	sqlite3_stmt *stmt;
	int rc;
	char *sql_query = NULL;
	char *rsa_priv_key = NULL;

	if ((sql_query = malloc(BUFSIZE * 4)) == NULL)
		M_PRINT_ERROR("Can't allocate memory");

	sprintf(sql_query, "SELECT rsa_priv_key FROM \"keys\" WHERE nick = \"%s\";", nickname);

	if ((rc = sqlite3_prepare(db, sql_query, -1, &stmt, 0)) != SQLITE_OK)
		M_PRINT_ERROR("Can't prepare SQL statement");

	// Take last key available
	while(sqlite3_step(stmt) == SQLITE_ROW) {
		const char *tmp_result = (char *) sqlite3_column_text(stmt, 0);

		if ((rsa_priv_key = malloc(BUFSIZE)) == NULL)
			M_PRINT_ERROR("Can't allocate memory");

		memcpy(rsa_priv_key, tmp_result, strlen(tmp_result));
	}

	sqlite3_finalize(stmt);

	assert(rsa_priv_key != NULL);
	return key_str2key(rsa_priv_key);
}


/**
 * Gets RSA public key from DB specified by nickname
 *
 * @param db DB connection
 */
Key *key_get_public_key(sqlite3 *db, char *nickname){
	sqlite3_stmt *stmt;
	int rc;
	char *sql_query = NULL;
	char *rsa_pub_key = NULL;

	if ((sql_query = malloc(BUFSIZE * 4)) == NULL)
		M_PRINT_ERROR("Can't allocate memory");

	sprintf(sql_query, "SELECT rsa_pub_key FROM \"keys\" WHERE nick = \"%s\";", nickname);

	if ((rc = sqlite3_prepare(db, sql_query, -1, &stmt, 0)) != SQLITE_OK)
		M_PRINT_ERROR("Can't prepare SQL statement");

	// Take last key available
	while(sqlite3_step(stmt) == SQLITE_ROW) {
		const char *tmp_result = (char *) sqlite3_column_text(stmt, 0);

		if ((rsa_pub_key = malloc(BUFSIZE))== NULL)
			M_PRINT_ERROR("Can't allocate memory");

		memcpy(rsa_pub_key, tmp_result, strlen(tmp_result));
	}

	sqlite3_finalize(stmt);

	assert(rsa_pub_key != NULL);
	return key_str2key(rsa_pub_key);
}



