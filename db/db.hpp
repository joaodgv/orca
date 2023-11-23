#include <sqlite3.h>
#include <stdio.h>
#include <fstream>
#include <sstream>

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

void updatePlayerStats(std::string username, int n_hands, int n_vpip, int n_pfr,
        int n_3bet, int n_3beto, int n_bets, int n_raises, int n_calls,
        int n_cbets, int n_cbeto, int n_wins, int n_showdowns) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open("db/HH.db", &db);

    std::stringstream ss;
    ss << "REPLACE INTO players (username, n_hands, n_vpip, n_pfr,"\
        "n_3bet, n_3beto, n_bets, n_raises, n_calls, n_cbets, n_cbeto,"\
        "n_wins, n_showdowns) VALUES ('" << username << "', " << n_hands << ", "\
        << n_vpip << ", " << n_pfr << ", " << n_3bet << ", " << n_3beto << ", "\
        << n_bets << ", " << n_raises << ", " << n_calls << ", " << n_cbets << ", "\
        << n_cbeto << ", " << n_wins << ", " << n_showdowns << ");";

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    } else {
        fprintf(stderr, "Opened database successfully\n");
        // Create tables
        char *sql = (char *)ss.str().c_str();

        rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
        } else {
            fprintf(stdout, "Tables created successfully\n");
        }
    }
    sqlite3_close(db);
}

void updateHand(int hand_id) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open("db/HH.db", &db);

    std::stringstream ss;
    ss << "REPLACE INTO hands (hand_id) VALUES (" << hand_id << ");";

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    } else {
        fprintf(stderr, "Opened database successfully\n");
        // Create tables
        char *sql = (char *)ss.str().c_str();

        rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
        } else {
            fprintf(stdout, "Tables created successfully\n");
        }
    }
    sqlite3_close(db);
}

static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);
   
   for(i = 0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   
   printf("\n");
   return 0;
}

std::vector<std::string> getPlayerStats(std::string username) {
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    std::vector<std::string> stats;

    rc = sqlite3_open("db/HH.db", &db);

    std::stringstream ss;
    ss << "SELECT * FROM players WHERE username = '" << username << "';";

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    } else {
        fprintf(stderr, "Opened database successfully\n");
        // Create tables
        char *sql = (char *)ss.str().c_str();

        rc = sqlite3_exec(db, sql, callback, NULL, &zErrMsg);
        if (rc != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
        } else {
            fprintf(stdout, "Tables created successfully\n");
        }
    }
    sqlite3_close(db);
    return stats;
}