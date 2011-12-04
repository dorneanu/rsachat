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
#include "crypto.h"
#include "key.h"



/**
 * Main function
 */
int main(int argc, char **argv) {
	// Parse and check arguments
	check_args(parse_args(argc, argv));
	return EXIT_SUCCESS;
}


/**
 * Check arguments
 *
 * @param *args pointer to prog_args structure
 */
void check_args(prog_args *args) {
	sqlite3 *db;

	// Open sqlite3 DB
	if ((db = sqlite3_open_db(SQLITE3_DEFAULT_DB)) == NULL)
		M_EXIT_ON_ERROR("Can't connect to DB");

	args->db = db;

	/* Keys arguments */
	if (args->f_list_keys == 1) {				// print available keys
			char *sql_query;
			if ((sql_query = malloc(BUFSIZE)) == NULL)
				M_PRINT_ERROR("Can't allocate memory");

			// Build query
			sprintf(sql_query, "SELECT * FROM \"keys\"");

			M_PRINT_STATUS("Executing SQL query: %s\n", sql_query);
			sqlite3_exec_query(db, sql_query);
			free(sql_query);
	} else if(args->new_nick) {					// generate new RSA key pair
		// Open sqlite3 DB
		generate_new_rsa_key_pair(db, args);
	}

	/* Server/Client arguments */
	if (args->f_listen == 1) {									// start as server
		M_PRINT_STATUS("Starting server on port %d ...\n", args->port);
		chat_server_start(args);
	} else if ((args->host != NULL) && (args->port != 0)) {		// start as client
		M_PRINT_STATUS("Connect to %s on port %d ...\n", args->host, args->port);
		chat_client_start(args);
	}
}


/**
 * Generate new RSA key pair and store it to DB
 *
 * @param db DB connection
 * @param args programms arguments
 */
void generate_new_rsa_key_pair(sqlite3 *db, prog_args *args) {
	char *private_key = NULL;
	char *public_key = NULL;
	char *sql_query = NULL;

	Key *k1 = 0,
		*k2 = 0;

	// Generates random key pair
	// FIXME: 1024 should be defined elsewhere
	generateKeyPair(&k1, &k2, 1024);

	private_key = key_key2str(k1);
	public_key = key_key2str(k2);

	// TODO: Dynamically allocate buffer
	if ((sql_query = malloc(BUFSIZE * 4)) == NULL)
		M_PRINT_ERROR("Can't allocate memory");

	// Build query
	sprintf(sql_query,
			"INSERT INTO \"keys\" VALUES (NULL, \"%s\", \"%s\", \"%s\");",
			args->new_nick,
			private_key,
			public_key);

	M_PRINT_STATUS("Executing SQL query: %s\n", sql_query);
	sqlite3_exec_query(db, sql_query);

	sqlite3_close(db);

	free(sql_query);
	key_free(k1);
	key_free(k2);

}


