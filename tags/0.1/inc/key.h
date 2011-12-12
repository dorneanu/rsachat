#include <openssl/bn.h>
#include <sqlite3.h>

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
Key* key_get_private_key(sqlite3 *, char *);
Key* key_get_public_key(sqlite3 *, char *);

#endif


