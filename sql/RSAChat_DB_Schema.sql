DROP table IF EXISTS keys;
CREATE table "keys" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "nick" TEXT NOT NULL,
    "rsa_priv_key" TEXT NOT NULL,
    "rsa_pub_key"  TEXT NOT NULL
);
