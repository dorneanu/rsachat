DROP table IF EXISTS keys;
CREATE table "keys" (
    "id" INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    "nick" TEXT NOT NULL,
    "data" TEXT NOT NULL
);

-- Create dump data"
INSERT INTO "keys" VALUES(NULL, "nick1", "Phuik1zia1");
INSERT INTO "keys" VALUES(NULL, "nick2", "Eiyae4ohng");

