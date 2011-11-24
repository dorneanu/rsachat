/*
 * client.h
 *
 *  Created on: Nov 23, 2011
 *      Author: victor
 */

#ifndef CLIENT_H_
#define CLIENT_H_

// Declarations
int   chat_client_connect (char *, int);
void  chat_client_start(char *, int);
void  chat_client_read_and_send(int);
int   chat_client_write(int, char [], int);
int   chat_client_read(int, char [], int);
void* chat_client_receive(void *);


#endif /* CLIENT_H_ */
