#ifndef SERVER_H_
#define SERVER_H_

#include "key.h"
#include "client.h"

// Session variables
typedef struct {
	Key *rsa_priv_key;
	Key *rsa_pub_key;
	char *nickname;
} server_session;

typedef struct {
	Key *rsa_priv_key;
	Key *rsa_pub_key;
	char *nickname;
} client_session;


// Declarations
int   chat_server_init(int);
void  chat_server_start(prog_args *);
int   chat_server_client_handler(int);
void  chat_server_loop(int);
void* chat_server_read(void *);
void  chat_server_read_and_send(int, client_session *);
int   chat_server_write(int, char[], int);
void  chat_server_write_to_clients(int, char[]);
void  char_server_exchange_keys(int, client_session *);



#endif /* SERVER_H_ */
