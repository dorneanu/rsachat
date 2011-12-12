#ifndef CRYPTO_H
#define CRYPTO_H

#include "key.h"

int generateKeyPair(Key** _key1,Key** _key2,int bitLength);
int encrypt(Key* _key,char const* srcBuffer,int length,char* destBuffer);
int decrypt(Key* _key,char const* srcBuffer,int length,char* destBuffer);
int encryptedSize(Key* _key,int _size);
int decryptedSize(Key* _key,int _size);
#endif
