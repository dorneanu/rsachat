/*
 * key.h
 *
 *  Created on: Nov 30, 2011
 *      Author: S. Gene G. <gene@staubsaal>
 */
#include <openssl/bn.h>

#ifndef KEY_H
#define KEY_H
typedef struct
{
	BIGNUM* N;
	BIGNUM* v;
} Key;
Key* key_new();
void key_free(Key* _key);

int key_bytes(Key const* _key);

char* key_key2str(Key const* _key);
Key* key_str2key(char const* _from);

#endif


