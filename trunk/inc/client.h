/*
 * client.h
 *
 *  Created on: Nov 23, 2011
 *      Author: victor
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include "key.h"
#include "crypto.h"

// Declarations
int   chat_client_connect (char *, int);
void  chat_client_start(prog_args *);
void  chat_client_read_and_send(int);
int   chat_client_write(int, char [], int);
int   chat_client_read(int, char [], int);
void* chat_client_receive(void *);
void  chat_client_exchange_keys_and_nicknames(int);


#endif /* CLIENT_H_ */
