rsachat
========
It's all about a project I had to implement at the university some years
ago. Basically *rsachat* is a RSA based chat which lacks of proper 
security features, e.g. replay attacks etc. Don't *ever* think this 
tool might be secure enough. It is **not**! It was implemented just for 
educational purposes.


Requirements
------------

* gcc
* make / gmake
* SQLite
* openssl / openssl-dev
    
    
Compiling
-------------
First of all make sure you have following libraries installed:

* libsqlite3.so  (sqlite / sqlite-dev)
* libcrypto.so  (openssl / openssl-dev)

Now you can run *make*:

    % make
    ...
    gcc -o RSAChat src/RSAChat.o src/args.o src/server.o src/client.o src/db.o src/key.o src/crypto.o src/utils.o -lsqlite3 -lpthread -lcrypto
    
Afterwards your *RSAChat* binary should be ready to run. 


Running
-------------
At the beginning we need to create our keys DB. Therefore we need to 
create one using SQLite:

    % sqlite3 -init sql/RSAChat_DB_Schema.sql ~/.chatstore
    -- Loading resources from sql/RSAChat_DB_Schema.sql
    SQLite version 3.7.2
    Enter ".help" for instructions
    Enter SQL statements terminated with a ";"
    sqlite> .tables
    keys
    sqlite> .schema keys
    CREATE TABLE "keys" (
        "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
        "nick" TEXT NOT NULL,
        "rsa_priv_key" TEXT NOT NULL,
        "rsa_pub_key"  TEXT NOT NULL
    );
    sqlite> .quit

Now your keys will be stored at *~/.chatstore*. In the next step we
need to genrate a private/public key pair:

    % ./RSAChat -i victor    
    [--] Executing SQL query: INSERT INTO "keys" VALUES (NULL, "victor", "8ABE8B681432DBE1D80F92C1D88DC692B1F3CFA61CE64830E56571D2B337A8825F13837B78375A8A36D37075124DDC9460A16EA73AD6D8FBDFB5CEC3793AA8DFE42FBEB7D22527567CC24A679C2190ED58AC59F30D3B9C3271194838BD7B9E3D874C9134A1475F5DA8216562CB0D2C32C19FB8CED57CD5A8217810321A4846A5 019ECB93B4BEF67E9DD29B5BCDE6BD51CF154F6E6F7CEDC538D073DDA1EB1375B5283707C9CE6E3B9BDA5EADECB8A22DDFEC761662DDE25785338BFFD805203CE3AB1254A1A21F12CEFD6F4F176693A4EE254C24BC49A78A00A318FFD8DB68216874CFB4AC3C413C1A688D290598AE054665575B37872413EF80E8DDFF28A84AF9", "0B9143191FE337C529350B1ADBBADB8ABA9D511A121E693F2AB15220A3AC0D60751DDA3BC4495DDAE5D018F9AFA78C8C16C9F79E8EEBD5800508C89239252E1AF3189429FA9D89F040637A579069824BCFE1D7CB39528B5E4C0E911B983538B3317E766CBDAEF51BCD76E586F85DC7B39C58DF578B4D51C1D1609E261DB54CDD 019ECB93B4BEF67E9DD29B5BCDE6BD51CF154F6E6F7CEDC538D073DDA1EB1375B5283707C9CE6E3B9BDA5EADECB8A22DDFEC761662DDE25785338BFFD805203CE3AB1254A1A21F12CEFD6F4F176693A4EE254C24BC49A78A00A318FFD8DB68216874CFB4AC3C413C1A688D290598AE054665575B37872413EF80E8DDFF28A84AF9");


You can also verify that the keys were actually stored in the DB:


    % sqlite3 ~/.chatstore   
    SQLite version 3.7.2
    Enter ".help" for instructions
    Enter SQL statements terminated with a ";"
    sqlite> SELECT id, nick FROM keys;
    1|victor
    sqlite> .quit


You can run *RSAChat* in 3 modi:

* server
* client
* server _and_ client


Multiple clients may connect to the server at the same time. Let's start 
the server:

    % ./RSAChat -n victor -l 1337
    [--] Starting server on port 1337 ...
    [--] Got own RSA key from DB


Now we can wait for our conversation partners. Meanwhile we can connect
to the server as *simon*...

    % ./RSAChat -c 'localhost 1337' -n simon
   
    
...and chat!

* Victor's session

    % ./RSAChat -n victor -l 1337
    [--] Starting server on port 1337 ...
    [--] Got own RSA key from DB
    <simon> hey victor
    hi simon
    <simon> bla bla
    bla bla
    ^C
    
* Simon's session

    % ./RSAChat -c 'localhost 1337' -n simon
    [--] Connect to localhost on port 1337 ...
    [--] Got own RSA key from DB
    [--] Connected to server!
    hey victor
    <victor> hi simon
    bla bla
    <victor> bla bla
    ^C
