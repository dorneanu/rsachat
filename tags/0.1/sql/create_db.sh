#!/bin/sh

SQLITE_BIN=/usr/local/bin/sqlite3
echo -ne "\033[43mPrint .quit to exit sqlite3\033[0m\n"
$SQLITE_BIN -init RSAChat_DB_Schema.sql $HOME/.chatstore
