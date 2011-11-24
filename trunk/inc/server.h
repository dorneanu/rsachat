/*
 * server.h
 *
 *  Created on: Nov 23, 2011
 *      Author: victor
 */

#ifndef SERVER_H_
#define SERVER_H_

// Declarations
int   chat_server_init(int);
void  chat_server_start();
int   chat_server_client_handler(int);
void  chat_server_loop(int);
void* chat_server_read(void *);
int   chat_server_write(int, char[], int);
void  chat_server_write_to_clients(int, char[]);



#endif /* SERVER_H_ */
