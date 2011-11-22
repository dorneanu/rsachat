/*
 * db.h
 *
 *  Created on: Nov 22, 2011
 *      Author: victor
 */

#ifndef DB_H_
#define DB_H_
#include <sqlite3.h>

#define SQLITE3_DEFAULT_DB			"file://home/victor/.chatstore"


// Function declarations
sqlite3 *sqlite3_open_db(char *);
void sqlite3_exec_query(sqlite3 *, char *);


#endif /* DB_H_ */
