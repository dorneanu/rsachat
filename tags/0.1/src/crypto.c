/*
============================================================================
 Name        : crypto.c
 Author      : S. Gene G.
 Version     :
 Copyright   : check doc/COPYING
 Description : Encrypt/Decrypt data using RSA
 ============================================================================
*/

#include <openssl/bn.h>
#include <assert.h>
#include <string.h>
#include "crypto.h"
#include "key.h"


/** 
 * calculates ψ(N) = (p-1)(q-1)
 *
 * @param _result result of phi
 * @param _p first parameter
 * @param _q second parameter
 * @param _ctx helper vaiable
 * @return 0 if failed, otherwise succede
 */
int euler(BIGNUM* _result,BIGNUM const* _p, BIGNUM const* _q,BN_CTX* _ctx)
{
	BIGNUM* _tmp1 = BN_new();
	BIGNUM* _tmp2 = BN_new();
	int ret = 1;

	if(BN_sub(_tmp1,_p,BN_value_one()) == 0) ret = 0;
	if(BN_sub(_tmp2,_q,BN_value_one()) == 0) ret = 0;

	if(BN_mul(_result,_tmp1,_tmp2,_ctx) == 0) ret = 0;

	BN_clear_free(_tmp1);
	BN_clear_free(_tmp2);

	return ret;
}



/**
 * internalImp of generateKeyPair
 * @return 0 return on error, 1 returned on on success
 *
 */
int _implGenerateKeyPair(Key* _key1, Key* _key2, int bitLength,BIGNUM* p, BIGNUM* q, BIGNUM* eul, BIGNUM* _tmp,BN_CTX* _ctx)
{
	//Generate 2 prime numbers p and q
	if(BN_generate_prime(p,bitLength/2,0,0,0,0,0) == 0) return 0;
	if(BN_generate_prime(q,bitLength/2+1,0,0,0,0,0) == 0) return 0;

	//compute N
	if(BN_mul(_key1->N,p,q,_ctx) == 0) return 0;
	if(BN_copy(_key2->N,_key1->N) == 0) return 0;

	//compute e
	if(euler(eul,p,q,_ctx) == 0) return 0;
	//find an e without common divisor
	// !ToDo check that e is larger than 1
	do {
		if(BN_rand_range(_key1->v,eul) == 0) return 0;

		if(BN_gcd(_tmp,eul,_key1->v,_ctx) == 0) return 0;
	} while(BN_cmp(_tmp,BN_value_one()) != 0);

 	if(BN_mod_inverse(_key2->v, _key1->v,eul,_ctx) == 0) return 0;

	return 1;
}


/**
 * generates one pair of key so that N has bitLength bits.
 * this means the that the bit the highest value is always one.
 * @param[out] _key1 - first key, if _key == 0 then a key will be created which have to be freed with key_free()
 * @param[out] _key2 - second key
 * @param bitLength Length of the N value
 * @return 0 return on error, 1 returned on on success
 */
int generateKeyPair(Key** _key1,Key** _key2,int bitLength)
{
	assert(_key1 != 0);
	assert(_key2 != 0);
	assert(bitLength % 2 == 0);

	if(*_key1 == 0) *_key1 = key_new();
	if(*_key2 == 0) *_key2 = key_new();

	//Used for temporary computation of cryptolib
 	BN_CTX* _ctx =BN_CTX_new();

	BIGNUM* p = BN_new();
	BIGNUM* q = BN_new();
	BIGNUM* eul = BN_new();
	BIGNUM* _tmp = BN_new();

	int ret =_implGenerateKeyPair(*_key1,*_key2,bitLength,p,q,eul,_tmp,_ctx);
	
	BN_clear_free(_tmp);
	BN_clear_free(eul);
	BN_clear_free(p);
	BN_clear_free(q);

	BN_CTX_free(_ctx);
	return ret;
}
/**
 * computes the block size of encrypted data
 * if it is encrypted with _key
 * or if it was encrypted with counter part of _key
 * (public key <-> private key)
 *
 * @param _key involved key, must not be 0 pointer
 * @return block size in bytes
 */
int encryptedBlockSize(Key* _key)
{
	assert(_key != 0);
	return BN_num_bytes(_key->N);
}


/**
 * computes the block size of decrypted data
 * if it is decrypted with _key
 * or if it was decrypted with counter part of _key
 * (public key <-> private key)
 *
 * @param _key involved key, must not be 0 pointer
 * @return block size in bytes
 */
int decryptedBlockSize(Key* _key)
{
	assert(_key != 0);
	return BN_num_bytes(_key->N)-1;
}


/**
 * computes the number of blocks of encrypted data of
 * length _length
 *
 * @param _key invoved key, must not be 0 pointer
 * @param _length, length of encrypted data
 * @return number of blocks
 */
int encryptedBlockCount(Key* _key,int _length)
{
	assert(_key != 0);
	assert(_length >= 0);
	int block = encryptedBlockSize(_key);
	int ret = _length / block;
	if(_length % block > 0) ++ret;
	return ret;
}


/**
 * computes the number of blocks of decrypted data of
 * length _length
 *
 * @param _key invoved key, must not be 0 pointer
 * @param _length, length of decrypted data
 * @return number of blocks
 */
int decryptedBlockCount(Key* _key,int _length)
{
	assert(_key != 0);
	assert(_length >= 0);
	int block = decryptedBlockSize(_key);
	int ret = _length / block;
	if(_length % block > 0) ++ret;
	return ret;
}


/**
 * computes size of encrypted data, which takes
 * _size bytes in decrypted form
 *
 * @param _key involved key
 * @param _size length of encrypted data
 * @return size of bytes
 */
int encryptedSize(Key* _key,int _size)
{
	assert(_key != 0);
	assert(_size >= 0);
	return encryptedBlockSize(_key) * encryptedBlockCount(_key,_size);
}


/**
 * computes size of decrypted data, which takes
 * _size bytes in encrypted form
 *
 * @param _key involved key
 * @param _size length of decrypted data
 * @return size of bytes
 */
int decryptedSize(Key* _key,int _size)
{
	assert(_key != 0);
	assert(_size >= 0);
	return decryptedBlockSize(_key) * decryptedBlockCount(_key,_size);
}


/**
 * encrypts srcBuffer with a length of encryptedBlockSize(_key) and saves the result in destBuffer
 *
 * @param _key involved key
 * @param srcBuffer source Buffer
 * @param destBuffer destination Buffer
 * @return 0 return on error, 1 returned on on success
 */
int encryptBlock(Key* _key,unsigned char const* srcBuffer, unsigned char* destBuffer)
{
	BN_CTX* _ctx =BN_CTX_new();

	int ret = 1;

	int deBlockSize = decryptedBlockSize(_key);
	int enBlockSize = encryptedBlockSize(_key);


	BIGNUM* srcNum = BN_new();
	BIGNUM* destNum = BN_new();

	if(BN_bin2bn(srcBuffer,deBlockSize,srcNum) == 0) ret = 0;
	if(BN_mod_exp(destNum,srcNum,_key->v,_key->N,_ctx) == 0) ret = 0;

	memset(destBuffer,0,enBlockSize);
	int offset = enBlockSize - BN_num_bytes(destNum);
	if(offset >= 0)
		BN_bn2bin(destNum,destBuffer+offset);
	else
		ret = 0;


	BN_free(destNum);
	BN_free(srcNum);

	BN_CTX_free(_ctx);

	return ret;
}


/**
 * decryptes srcBuffer with a length of decryptedBlockSize(_key) and saves the result in destBuffer
 *
 * @param _key involved key
 * @param srcBuffer source Buffer
 * @param destBuffer destination Buffer
 * @return 0 return on error, 1 returned on on success
 */
int decryptBlock(Key* _key,unsigned char const* srcBuffer, unsigned char* destBuffer)
{
	BN_CTX* _ctx =BN_CTX_new();

	int ret = 1;

	int enBlockSize = encryptedBlockSize(_key);
	int deBlockSize = decryptedBlockSize(_key);


	BIGNUM* srcNum = BN_new();
	BIGNUM* destNum = BN_new();

	if(BN_bin2bn(srcBuffer,enBlockSize,srcNum) == 0) ret = 0;
	if(BN_mod_exp(destNum,srcNum,_key->v,_key->N,_ctx) == 0) ret = 0;

	memset(destBuffer,0,deBlockSize);
	int offset = deBlockSize - BN_num_bytes(destNum);
	if(offset >= 0)
		BN_bn2bin(destNum,destBuffer+offset);
	else
		ret = 0;


	BN_free(destNum);
	BN_free(srcNum);

	BN_CTX_free(_ctx);

	return ret;
}


/**
 * encrypts a buffer with a given key, and stores it in a different buffer
 *
 * @param _key to encrypt the buffer with
 * @param srcBuffer contains the data to encrypt
 * @param length amount of data in bytes, which should be encrypted
 * @param destBuffer buffer to store the encrypted data. This buffer needs to contain encryptedSize(_key,length) bytes
 * @return 0 return on error, 1 returned on on success
 */
int encrypt(Key* _key, char const* srcBuffer, int length, char* destBuffer)
{

	int deBlockSize = decryptedBlockSize(_key);
	int enBlockSize = encryptedBlockSize(_key);

	int deBlockCount = decryptedBlockCount(_key,length);
	int i;

	//compute all blocks < deBlockCount
	//convert buffer to BIGNUM, compute M^e % N, convert BIGNUM to buffer
	for(i=0;i<deBlockCount-1;++i)
	{
		if(encryptBlock(_key,(unsigned char*)srcBuffer+(i*deBlockSize),(unsigned char*)destBuffer+(i*enBlockSize)) == 0) return 0;
	}

	//compute block = deBlockCount
	unsigned char* block = malloc(sizeof(unsigned char)*deBlockSize);
	memset(block,0,deBlockSize);
	memcpy(block,srcBuffer+(deBlockCount-1)*deBlockSize, length % deBlockSize);

	int ret = encryptBlock(_key,(unsigned char*)block,(unsigned char*)destBuffer+(deBlockCount-1)*enBlockSize);

	free(block);

	return ret;
}


/**
 * decrypts a buffer with a given key, and stores it in a different buffer
 *
 * @param _key to encrypt the buffer with
 * @param srcBuffer contains the data to decrypt
 * @param length amount of data in bytes, which should be decrypted
 * @param destBuffer buffer to store the decrypted data. This buffer needs to contain decryptedSize(_key,length) bytes
 * @return 0 return on error, 1 returned on on success
 */

int decrypt(Key* _key,char const* srcBuffer,int length,char* destBuffer)
{
	int deBlockSize = decryptedBlockSize(_key);
	int enBlockSize = encryptedBlockSize(_key);

	int enBlockCount = encryptedBlockCount(_key,length);

	//compute all blocks < deBlockCount
	//convert buffer to BIGNUM, compute M^e % N, convert BIGNUM to buffer
	int i;
	for(i=0;i<enBlockCount;++i)
		if(decryptBlock(_key,(unsigned char*)srcBuffer+(i*enBlockSize),(unsigned char*)destBuffer+(i*deBlockSize)) == 0) return 0;

	return 1;
}

