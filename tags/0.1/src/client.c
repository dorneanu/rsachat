/*
============================================================================
 Name        : client.c
 Author      : Victor Dorneanu
 Version     :
 Copyright   : check doc/COPYING
 Description : Connect to server, read from stdin, encrypt data
 ============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>
#include "RSAChat.h"
#include "client.h"
#include "server.h"
#include "utils.h"


// Global variable
client_session *cs;
server_session *ss;


/**
 * Start a new client
 *
 * @param host hostname of server
 * @param port port number
 */
void chat_client_start(prog_args *args) {
	int server_socket_fd = -1;
	struct thread_info *tinfo;
	pthread_t threadID;

	// Init sessions
	if((cs = malloc(sizeof(client_session))) == NULL)
		M_EXIT_ON_ERROR("Can't allocate memory");

	if ((ss = malloc(sizeof(server_session))) == NULL)
		M_EXIT_ON_ERROR("Can't allocate memory");

	cs->nickname = args->nick;

	// Get keys from DB
	cs->rsa_priv_key = key_get_private_key(args->db, args->nick);
	cs->rsa_pub_key  = key_get_public_key(args->db, args->nick);
	M_PRINT_STATUS("Got own RSA key from DB\n");


	// Allocate mem for struct thread_info
	if ((tinfo = malloc(sizeof(struct thread_info))) == NULL)
		M_EXIT_ON_ERROR("Can't allocate memory");

	server_socket_fd = chat_client_connect(args->host, args->port);
	tinfo->socket_fd = server_socket_fd;


	// Send public key to server and receive server's public key
	chat_client_exchange_keys_and_nicknames(server_socket_fd);

	// 2 Threads: One for receiving data from server, one for sending data
	pthread_create(&threadID, NULL, chat_client_receive, tinfo);
	chat_client_read_and_send(server_socket_fd);

	pthread_exit(NULL);
}


/**
 * Connect to given host and port number
 *
 * @param host hostname of server
 * @param port port number
 * @return server socket fd
 */
int chat_client_connect(char *host, int port) {
	struct sockaddr_in sock;
	struct hostent *server;
	int server_socket_fd, ret;

	if ((server_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
		M_EXIT_ON_ERROR("Can't create socket");

	// Zero sock structure
	memset((char *) &sock, 0, sizeof(sock));

	// Get host by name
	if ((server = gethostbyname(host)) == NULL)
		M_EXIT_ON_ERROR("No such host");

	// Connect to server
	sock.sin_family = AF_INET;
	sock.sin_port   = ntohs(port);
	bcopy((char *)server->h_addr, (char *)&sock.sin_addr.s_addr, server->h_length);

	if ((ret = connect(server_socket_fd,
					  (struct sockaddr *) &sock, sizeof(sock))) < 0 )
		M_EXIT_ON_ERROR("Error connecting to server");

	M_PRINT_STATUS("Connected to server!\n");
	return server_socket_fd;
}



/**
 * Reads data from server
 * @param socket_fd servers socket
 * @param buf data buffer
 * @param buflen length of data buffer
 *
 */
int chat_client_read(int socket_fd, char buf[], int buflen) {
	int ret;

	if ((ret = read(socket_fd, buf, buflen)) < 0) {
		M_PRINT_ERROR("Error reading from socket");
		return -1;
	} else {
		return ret;
	}
}


/**
 * Sends data to server
 *
 * @param socket_fd servers socket
 * @param buf data buffer
 * @param buflen length of data buffer
 * @return -1 if error, 0 otherwise
 */
int chat_client_write(int socket_fd, char buf[], int buflen) {
	int ret;

	if ((ret = write(socket_fd, buf, buflen)) != buflen)
		return -1;
	else
		return 0;

}



/**
 * Reads from stdin and sends data to server
 *
 * @param server_socket_fd socket to server
 */
void chat_client_read_and_send(int server_socket_fd) {
	char buf[BUFSIZE];

	// Read input and send it to server
	while (1) {
		memset(buf, 0, BUFSIZE);

		if(!(mygetline(buf, BUFSIZE)))
			M_PRINT_ERROR("fgets error");

		// Encrypt line with servers public key
		int encryptedLen = encryptedSize(ss->rsa_pub_key, strlen(buf)+1);
		char *encryptedBuffer = malloc(encryptedLen);
		encrypt(ss->rsa_pub_key, buf, strlen(buf)+1, encryptedBuffer);

		// Send to server
		chat_client_write(server_socket_fd, encryptedBuffer,BUFSIZE);
	}
}



/**
 * Thread to receive from server
 */
void *chat_client_receive(void *arg) {
	struct thread_info *tinfo = (struct thread_info *) arg;
	int server_fd = tinfo->socket_fd;
	char buf[BUFSIZE];
	int buflen = 0;

	while (1) {
		memset(buf, 0, BUFSIZE);
		buflen = chat_client_read(server_fd, buf, BUFSIZE);

		// Check if server closed the connection
		if (buflen == 0) {
			M_PRINT_ERROR("Lost server connection");
			close(server_fd);
			exit(EXIT_FAILURE);
		} else {
			// Decrypt message
			int decryptedLen = decryptedSize(cs->rsa_priv_key, buflen);
			char *decryptedBuffer = malloc(decryptedLen);
			decrypt(cs->rsa_priv_key, buf, buflen, decryptedBuffer);

			printf("<%s> %s\n", ss->nickname, decryptedBuffer);
			free(decryptedBuffer);
		}
	}

	return NULL;
}


/**
 * Exchange public keys (between client and server)
 *
 * @param server_fd server's fd
 */
void chat_client_exchange_keys_and_nicknames(int server_fd) {
	char *hello_message_to_server;			/* consists of: <nickname>HELLO_MSG_DELIM<public key> */
	char *hello_message_from_server;
	char *own_public_key = key_key2str(cs->rsa_pub_key);

	// Allocate memory
	if((hello_message_to_server = malloc(BUFSIZE)) == NULL)
		M_PRINT_ERROR("Can't allocate memory");

	if((hello_message_from_server = malloc(BUFSIZE)) == NULL)
		M_PRINT_ERROR("Can't allocate memory");

	// Send hello message to server
	sprintf(hello_message_to_server, "%s%s%s", cs->nickname, HELLO_MSG_DELIM, own_public_key);
	if(chat_client_write(server_fd, hello_message_to_server, BUFSIZE))
		M_PRINT_ERROR("Error on sending hello message to server");

	// Get server's hello message
	if(chat_client_read(server_fd, hello_message_from_server, BUFSIZE) < 0) {
		M_PRINT_ERROR("Error receiving hello message from server");
	} else {
		char *saveptr = NULL;

		// Save nickname
		ss->nickname = strtok_r(hello_message_from_server, HELLO_MSG_DELIM, &saveptr);

		// Save public key
		ss->rsa_pub_key = key_str2key(strtok_r(NULL, HELLO_MSG_DELIM, &saveptr));
	}
}


