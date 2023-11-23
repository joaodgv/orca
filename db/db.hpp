#include <sqlite3.h>
#include <stdio.h>
#include <fstream>

// TODO
// 1. create a method to get player
// 2. create a method to update player

void createHHdb() {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    std::ofstream outputFile("db/HH.db");

    rc = sqlite3_open("db/HH.db", &db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    } else {
        fprintf(stderr, "Opened database successfully\n");
        // Create tables
        char *sql = "CREATE TABLE IF NOT EXISTS hands("\
        "hand_id int PRIMARY KEY NOT NULL);"\
        "CREATE TABLE IF NOT EXISTS players("\
        "username varchar(255) PRIMARY KEY NOT NULL,"\
        "n_hands int NOT NULL, n_vpip int NOT NULL, n_pfr int NOT NULL,"\
        "n_3bet int NOT NULL, n_3beto int NOT NULL,"\
        "n_bets int NOT NULL, n_raises int NOT NULL, n_calls int NOT NULL,"\
        "n_cbets int NOT NULL, n_cbeto int NOT NULL,"\
        "n_wins int NOT NULL, n_showdowns int NOT NULL);";

        rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
        } else {
            fprintf(stdout, "Tables created successfully\n");
        }
    }
    sqlite3_close(db);
}