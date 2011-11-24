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
#include "server.h"
#include "client.h"
#include "db.h"


// Declarations
void check_args(prog_args *);


/**
 * Check arguments
 *
 * @param *args pointer to prog_args structure
 */
void check_args(prog_args *args) {
	sqlite3 *db;

	/* Keys arguments */
	if (args->f_list_keys == 1) {				// print available keys
		if ((db = sqlite3_open_db(SQLITE3_DEFAULT_DB)) != NULL) {
			char *sql_query;
			if ((sql_query = malloc(BUFSIZE)) == NULL)
				M_PRINT_ERROR("Can't allocate memory");

			// Build query
			sprintf(sql_query, "SELECT * FROM \"keys\"");

			M_PRINT_STATUS("Executing SQL query: %s\n", sql_query);
			sqlite3_exec_query(db, sql_query);
			sqlite3_close(db);
			free(sql_query);
		}
	} else if(args->new_nick) {					// generate new RSA key pair
		// Open sqlite3 DB
		if ((db = sqlite3_open_db(SQLITE3_DEFAULT_DB)) != NULL) {
			char *sql_query;
			if((sql_query = malloc(BUFSIZE)) == NULL)
					M_PRINT_ERROR("Can't allocate memory");

			// Build query
			sprintf(sql_query,
					"INSERT INTO \"keys\" VALUES (NULL, \"%s\", \"RSA DATA\");",
					args->new_nick
			);

			M_PRINT_STATUS("Executing SQL query: %s\n", sql_query);
			sqlite3_exec_query(db, sql_query);
			sqlite3_close(db);
			free(sql_query);
		}
	}

	/* Server/Client arguments */
	if (args->f_listen == 1) {						// start as server
		M_PRINT_STATUS("Starting server on port %d ...\n", args->port);
		chat_server_start(args->port);
	} else if ((args->host != NULL) && (args->port != 0)) {
		M_PRINT_STATUS("Connect to %s on port %d ...\n", args->host, args->port);
		chat_client_start(args->host, args->port);	// start as client
	}
}


/**
 * Main function
 */
int main(int argc, char **argv) {
	// Parse and check arguments
	check_args(parse_args(argc, argv));
	return EXIT_SUCCESS;
}
