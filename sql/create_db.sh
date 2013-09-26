#!/bin/sh

SQLITE_BIN=`which sqlite3`
echo -ne "\033[43mPrint .quit to exit sqlite3\033[0m\n"
$SQLITE_BIN -init RSAChat_DB_Schema.sql $HOME/.chatstore
