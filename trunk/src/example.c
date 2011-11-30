/*
 * example.c
 *
 *  Created on: Nov 30, 2011
 *      Author: S. Gene G. <gene@staubsaal.de>
 */

#include "crypto.h"
#include <string.h>
#include <openssl/rand.h>

//Public/Private Key pair, used for demonstrating
char const* const gstr_privateKey = "1B9B6ECCCE6C1329E6A359FE5FE9F2B583D694214308CB55A45198A33AC9AE933A0A10F3974ABD140C15076F3F0296092EB67B1EF3BBF8CF5BD74880C863E8D020B51302AACE0B19DA88120AD6478F0D9DF918FCA5121B960437691A1FC4690773B28CD86A001207EA233384F9E44349A8AD733D3D5A0F74D19933623A1414B7 017DD6EBC04645A6232EB5B68B827C591AB83FCCE5919009B1F4E4196DE605B91BF14ECE0D6357F9D37D8F9FD1AA5CB04FA0C7CCFDCECFB2E499A8EFCE69432CE9D4ED16A1FFC7E99009EF3E5BB82EC2B0D2D41678E2A94F4105AD7740D2652950F919E3E033734F6B982F8B648D65794D07B5168321C32A25248CB3FA4E230EA7";
char const* const gstr_publicKey = "011E11AD07695B8C7DFC39345A8527BCCE361BB7B04A3DECA0E896841C72ECEFA8BDF7F4E1F973A025F893D44C3B78A7CE6C134CDD5FD5F8DE0043EA184FB2A32C96A54A821722CD97E44C014A2601FF96440F931A6688BE4715DDBC0D1262DDB83093D4770EBEB375B428E520EC0AFA10DA5C898C634871C04AD96BEE2643239F 017DD6EBC04645A6232EB5B68B827C591AB83FCCE5919009B1F4E4196DE605B91BF14ECE0D6357F9D37D8F9FD1AA5CB04FA0C7CCFDCECFB2E499A8EFCE69432CE9D4ED16A1FFC7E99009EF3E5BB82EC2B0D2D41678E2A94F4105AD7740D2652950F919E3E033734F6B982F8B648D65794D07B5168321C32A25248CB3FA4E230EA7";
//A decrypted message
char const* const gstr_message = "Ein öffentlicher Text, der zur demonstration genutzt wird";

//A encrypted message, it was encrypted with the public Key
char const gstr_secret[]  = {0x1,0x54,0x28,0xcc,0x2b,0xb7,0x19,0x7b,0x29,0x61,0x7c,0xa7,0xc4,0xa7,0x66,0xdc,0x3a,0x82,0x1f,0x75,0xd0,0xda,0x88,0x4a,0x1b,0xbf,0x68,0x7d,0xbf,0x67,0xd8,0x8a,0x46,0x8c,0x4c,0x66,0x52,0x5a,0xea,0xc3,0x7f,0xbe,0x79,0x7c,0x41,0x41,0x89,0x8f,0x2e,0xb,0x19,0x85,0x44,0x6,0x41,0x28,0xc,0x16,0xd3,0x92,0x63,0x78,0x98,0xae,0x6b,0xb8,0xd3,0xb8,0xa,0x97,0x71,0xd2,0x5f,0x87,0xe1,0xc6,0xce,0x76,0x83,0x75,0xf9,0xf0,0xe5,0x3c,0xfe,0x24,0xec,0x7,0x61,0x71,0x1e,0x6b,0x3,0xd4,0x6f,0xe7,0xfa,0x47,0x73,0xf4,0xef,0x2f,0x4,0x7c,0x37,0xa,0xf9,0x33,0x5c,0x60,0xd0,0xcf,0x8d,0xd6,0xe9,0x2,0xc1,0xfa,0xe4,0x76,0x41,0x5b,0xec,0xae,0x6b,0x80,0xf4,0xbf,0x81};
//Number of Bytes of the encrypted Mesage
const int gint_secretLen = 129;

// Dieses Beispiel macht gar nichts
void example00(void)
{
}
//Erstellt ein Key Paar und zerstört diese wieder
void example01(void)
{
	// 2 Variablen die auf die Keys zeigen
	Key *k1 = 0,
		*k2 = 0;
	
	generateKeyPair(&k1,&k2,1024); // erstellt ein zufälliges Schlüsselpaar

	// Sicheres löschen der Schlüssel aus dem Speicher
	key_free(k1);
	key_free(k2);
}

//Erstellt ein Key Paar wandelt diese in Strings um und räumt danach auf.
void example02(void)
{
	// 2 Variablen die auf die Keys zeigen
	Key *k1 = 0,
		*k2 = 0;
	
	generateKeyPair(&k1,&k2,1024); // erstellt ein zufälliges Schlüsselpaar

	//Keys nach Strings umwandeln(es wird speicher erstellt der freigegeben werden muss
	char* strkey1 = key_key2str(k1);
	char* strkey2 = key_key2str(k2);

	printf("Key1: %s\nKey2: %s\n",strkey1,strkey2);

	//Speicher wieder freigeben
	free(strkey1);
	free(strkey2);


	// Sicheres löschen der Schlüssel aus dem Speicher
	key_free(k1);
	key_free(k2);
}
//Wandelt einen String in einen Key um.
void example03(void)
{
	Key *k1 = key_str2key(gstr_privateKey);
	Key *k2 = key_str2key(gstr_publicKey);

	// -- do something --

	// Sicheres löschen der Schlüssel aus dem Speicher
	key_free(k1);
	key_free(k2);
}

//Verschlüsselung eines Textes
void example04(void)
{
	Key *k1 = key_str2key(gstr_privateKey);
	Key *k2 = key_str2key(gstr_publicKey);

	// es soll der Text in gstr_message verschlüsselt werden
	// die verschlüsselte länge, eines nicht verschlüsselten text
	int encryptedLen = encryptedSize(k2,strlen(gstr_message)+1);

	// neuen Speicher anlegen, der die verschlüsselte Version handhaben kann
	char *encryptedBuffer = malloc(encryptedLen);

	//Speicher verschlüsseln
	encrypt(k2,gstr_message,strlen(gstr_message)+1,encryptedBuffer);

	printf("Verschüsselter Tex hat länge: %i\n",encryptedLen);
	
	// -- Do Something
	
	free(encryptedBuffer);

	// Sicheres löschen der Schlüssel aus dem Speicher
	key_free(k1);
	key_free(k2);
}
//Entschlüsselung eines Textes
void example05(void)
{
	Key *k1 = key_str2key(gstr_privateKey);
	Key *k2 = key_str2key(gstr_publicKey);

	// es soll der Text in gstr_secret entschlüsselt werden
	// die entschlüsselte länge, eines verschlüsselten text
	int decryptedLen = decryptedSize(k1,gint_secretLen);

	// neuen Speicher anlegen, der die entschlüsselte Version handhaben kann
	char *decryptedBuffer = malloc(decryptedLen);

	//Speicher entschlüsseln (achtung, strlen() funktioniert auf verschlüsselte nachrichten nicht zu verlässig)
	decrypt(k1,gstr_secret,gint_secretLen,decryptedBuffer);

	printf("Entschüsselter Text: %s\n",decryptedBuffer);

	// -- Do Something
	
	free(decryptedBuffer);

	// Sicheres löschen der Schlüssel aus dem Speicher
	key_free(k1);
	key_free(k2);
}

int main(int argc, char** args)
{
	if(argc < 2) return 0;

	int example = atoi(args[1]);
	switch(example)
	{
	case 0: example00(); break;
	case 1: example01(); break;
	case 2: example02(); break;
	case 3: example03(); break;
	case 4: example04(); break;
	case 5: example05(); break;

	}
	return 0;
}
