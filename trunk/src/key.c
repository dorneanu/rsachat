/*
 * key.c
 *
 *  Created on: Nov 30, 2011
 *      Author: S. Gene G. <gene@staubsaal.de>
 */

#include "key.h"
#include <assert.h>
#include <string.h>
#include <openssl/bn.h>
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
 * frees the Memory which _key pointsto
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

