/*
 ============================================================================
 Name        : RSAChat.c
 Author      : Victor Dorneanu
 Version     :
 Copyright   : Your copyright notice
 Description : Secure RSA based chat
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sqlite3.h>
#include "RSAChat.h"
#include "args.h"
#include "db.h"


int main(int argc, char **argv) {
	prog_args *args;
	//sqlite3* db;

	// Check arguments
	args = check_args(argc, argv);

	// Create chat server
	if (args->f_listen == 1) {
		chat_server_start(args->port);
	} else if ((args->host != NULL) && (args->port != 0)) {
		chat_client_start(args->host, args->port);
	}
	// Opens DB
	//db = sqlite3_open_db(SQLITE3_DEFAULT_DB);

	return EXIT_SUCCESS;
}
