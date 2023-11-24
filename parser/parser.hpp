#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include "types.hpp" // This is the header file that contains the Player struct

using namespace std;

// TODO
// 1. check if hand was already parsed
// 3. show stats in real time

// Gets all the active players in the hand
std::vector<std::string> updateActivePlayers(std::vector<std::string> hand) {

    std::vector<std::string> active_players;
    for (int i = 2; i < hand.size(); i++) {
        std::string line = hand[i];

        if (hand[i].find("Seat") != std::string::npos && hand[i].find("button") == std::string::npos) {
            std::string player_name = hand[i].substr(hand[i].find(":") + 2, hand[i].find(" (") - hand[i].find(":") - 2);
            active_players.push_back(player_name);
        }
    }

    return active_players;
}

std::map<std::string, std::string> getAllPlayers() {
    std::map<std::string, std::string> player_map;

    std::ifstream inputFile("db/players.txt");
    if (inputFile.is_open()) {
        std::string line;
        while (getline(inputFile, line)) {
            std::string player_name = line.substr(0, line.find(","));
            std::string player_stats = line.substr(line.find(",") + 1, line.size() - line.find(",") - 1);
            player_map[player_name] = player_stats;
        }
    } else {
        std::cout << "Unable to open file" << std::endl;
    }

    return player_map;
}

// Updates the stats for each player
std::vector<Player> updatePlayers(std::vector<std::string> hand) {
    std::vector<std::string> active_players = updateActivePlayers(hand);
    std::vector<Player> players;
    std::map<std::string, std::string> player_stats;

    player_stats = getAllPlayers();
    for (int i = 0; i < active_players.size(); i++) {
        // If the player is not in the map, add it
        if (player_stats.find(active_players[i]) == player_stats.end()) {
            Player new_player(active_players[i]);
            players.push_back(new_player);
        } else {
            //if player is in map read the csv line
            std::string line = player_stats[active_players[i]];
            std::stringstream ss(line);
            std::string value;
            std::vector<std::string> data;
            
            while(std::getline(ss, value, ',')){
                data.push_back(value);
            }

            Player new_player(active_players[i]);
            new_player.update_from_db(data);
            new_player.increase_n_hands();
            players.push_back(new_player);
        }
    }

    return players;
}

// update the players map and save it to file
void savePlayers(std::vector<Player> players) {
    std::map<std::string, std::string> player_stats;

    player_stats = getAllPlayers();

    for (int i = 0; i < players.size(); i++) {
        std::string stats = players[i].get_stats_string();
        std::string player_name = players[i].get_name();
        player_stats[player_name] = stats;
    }

    std::ofstream outputFile("db/players.txt");
    if (outputFile.is_open()) {
        for (auto it = player_stats.begin(); it != player_stats.end(); it++) {
            outputFile << it->first << "," << it->second << std::endl;
        }
    } else {
        std::cout << "Unable to open file" << std::endl;
    }
}

// method to check if hand was already parsed
bool handExists(std::string handNumber, std::string date) {
    // open file of the date and check if hand number exists
    std::ifstream inputFile("db/" + date + ".txt");
    if (inputFile.is_open()) {
        std::string line;
        while (getline(inputFile, line)) {
            if (line.find(handNumber) != std::string::npos) {
                return true;
            }
        }
        inputFile.close();
    } else {
        std::ofstream outputFile("db/" + date + ".txt");
        outputFile << handNumber << std::endl;
        outputFile.close();
    }

    std::ofstream outputFile("db/" + date + ".txt", std::ios_base::app);
    outputFile << handNumber << std::endl;
    outputFile.close();
    return false;
}

// Parses a single hand from the input file for PokerStars
std::vector<Player> parseHandStars(std::vector<Player> players , std::vector<std::string> hand) {
    if (hand.size() == 0) {
        return players;
    }
    
    // parse the table name and format
    std::string handNumber = hand[0];
    std::string tableName = hand[1];
    bool wasRaised = false;

    // get the hand number from the string
    // get the table name from the string
    // TODO

    // parse the players
    players = updatePlayers(hand);
    for (int i = 0; i < players.size(); i++) {
        players[i].reset_hand();
        printf("Player: %s\n", players[i].get_name().c_str());
    }

    for (int pos = 0; pos < hand.size(); pos++)
    {
        std::string line = hand[pos];
        if (line.find("HOLE CARDS") != std::string::npos) {
            // parse the preflop actions
            pos += 2;
            line = hand[pos];

            while(line.find('*') == std::string::npos) {
                line = hand[pos];
                
                if (line.find("raises") != std::string::npos && !wasRaised) {
                    std::string playerName = line.substr(0, line.find(":"));
                    wasRaised = true;
                    for (int i = 0; i < players.size(); i++) {
                        if (players[i].get_name() == playerName) {
                            players[i].increase_n_pfr();
                            players[i].increase_n_vpip();
                            players[i].set_raised(true);
                        }
                    }
                } else if (line.find("raises") != std::string::npos && wasRaised) {
                    std::string playerName = line.substr(0, line.find(":"));
                    for (int i = 0; i < players.size(); i++) {
                        if (players[i].get_name() == playerName) {
                            players[i].increase_n_3bet();
                            players[i].increase_n_3betO();
                            players[i].set_raised(true);
                        }
                    }
                } else if (line.find("calls") != std::string::npos) {
                    std::string playerName = line.substr(0, line.find(":"));
                    for (int i = 0; i < players.size(); i++) {
                        if (players[i].get_name() == playerName) {
                            players[i].increase_n_vpip();
                            if (wasRaised) {
                                players[i].increase_n_cbetsO();
                            }
                        }
                    }
                }

                pos++;
            }
        }

        // postflop actions
        if (line.find("FLOP") != std::string::npos) {
            pos++;
            line = hand[pos];

            while (line.find("*") == std::string::npos) {
                line = hand[pos];

                if (line.find("bets") != std::string::npos) {
                    std::string playerName = line.substr(0, line.find(":"));
                    for (int i = 0; i < players.size(); i++) {
                        if (players[i].get_name() == playerName) {
                            players[i].increase_n_bets();
                            if (players[i].get_raised()) {
                                players[i].increase_n_cbets();
                                players[i].increase_n_cbetsO();
                            }
                        }
                    }
                } else if (line.find("raises") != std::string::npos) {
                    std::string playerName = line.substr(0, line.find(":"));
                    for (int i = 0; i < players.size(); i++) {
                        if (players[i].get_name() == playerName) {
                            players[i].increase_n_raises();
                            if (players[i].get_raised()) {
                                players[i].increase_n_cbets();
                                players[i].increase_n_cbetsO();
                            }
                        }
                    }
                } else if (line.find("calls") != std::string::npos) {
                    std::string playerName = line.substr(0, line.find(":"));
                    for (int i = 0; i < players.size(); i++) {
                        if (players[i].get_name() == playerName) {
                            players[i].increase_n_calls();
                            if (players[i].get_raised()) {
                                players[i].increase_n_cbetsO();
                            }
                        }
                    }
                } else if (line.find("checks") != std::string::npos) {
                    std::string playerName = line.substr(0, line.find(":"));
                    for (int i = 0; i < players.size(); i++) {
                        if (players[i].get_name() == playerName) {
                            if (players[i].get_raised()) {
                                players[i].increase_n_cbetsO();
                            }
                        }
                    }
                }

                pos++;
            }
        }

        // showdown actions
        if (line.find("SHOW DOWN") != std::string::npos) {
            pos++;
            line = hand[pos];

            while (line.find("*") == std::string::npos) {
                line = hand[pos];

                if (line.find("collected") == std::string::npos) {
                    std::string playerName = line.substr(0, line.find(":"));
                    for (int i = 0; i < players.size(); i++) {
                        if (players[i].get_name() == playerName) {
                            players[i].increase_n_showdowns();
                        }
                    }
                } else if (line.find("collected") != std::string::npos) {
                    std::string playerName = line.substr(0, line.find(" "));
                    for (int i = 0; i < players.size(); i++) {
                        if (players[i].get_name() == playerName) {
                            players[i].increase_n_wins();
                            players[i].increase_n_showdowns();
                        }
                    }
                }

                pos++;
            }
        }
    }

    return players;
}

// Parses a single hand from the input file for Poker888
std::vector<Player> parseHand888(std::vector<Player> players, std::vector<string> hand, std::string date) {
    if (hand.size() == 0) {
        return players;
    }
    
    // parse the table name and format
    std::string handNumberLine = hand[0];
    std::string handNumber = handNumberLine.substr(handNumberLine.find(": ") + 2, handNumberLine.back());
    bool wasRaised = false;

    if (handExists(handNumber, date)) {
        return players;
    }

    // parse the players
    players = updatePlayers(hand);

    for (int pos = 0; pos < hand.size(); pos++)
    {
        std::string line = hand[pos];
        if (line.find("Dealing down cards") != std::string::npos) {
            // parse the preflop actions
            pos += 2;
            line = hand[pos];

            while(line.find('*') == std::string::npos) {
                line = hand[pos];
                
                if (line.find("raises") != std::string::npos && !wasRaised) {
                    std::string playerName = line.substr(0, line.find(" "));
                    wasRaised = true;
                    for (int i = 0; i < players.size(); i++) {
                        if (players[i].get_name() == playerName) {
                            players[i].increase_n_pfr();
                            players[i].increase_n_vpip();
                            players[i].set_raised(true);
                        }
                    }
                } else if (line.find("raises") != std::string::npos && wasRaised) {
                    std::string playerName = line.substr(0, line.find(" "));
                    for (int i = 0; i < players.size(); i++) {
                        if (players[i].get_name() == playerName) {
                            players[i].increase_n_3bet();
                            players[i].increase_n_3betO();
                            players[i].set_raised(true);
                        }
                    }
                } else if (line.find("calls") != std::string::npos) {
                    std::string playerName = line.substr(0, line.find(" "));
                    for (int i = 0; i < players.size(); i++) {
                        if (players[i].get_name() == playerName) {
                            players[i].increase_n_vpip();
                            if (wasRaised) {
                                players[i].increase_n_cbetsO();
                            }
                        }
                    }
                }

                pos++;
            }
        }

        // postflop actions
        if (line.find("Dealing flop") != std::string::npos) {
            pos++;
            line = hand[pos];

            while (line.find("*") == std::string::npos) {
                line = hand[pos];

                if (line.find("bets") != std::string::npos) {
                    std::string playerName = line.substr(0, line.find(" "));
                    for (int i = 0; i < players.size(); i++) {
                        if (players[i].get_name() == playerName) {
                            players[i].increase_n_bets();
                            if (players[i].get_raised()) {
                                players[i].increase_n_cbets();
                                players[i].increase_n_cbetsO();
                            }
                        }
                    }
                } else if (line.find("raises") != std::string::npos) {
                    std::string playerName = line.substr(0, line.find(" "));
                    for (int i = 0; i < players.size(); i++) {
                        if (players[i].get_name() == playerName) {
                            players[i].increase_n_raises();
                            if (players[i].get_raised()) {
                                players[i].increase_n_cbets();
                                players[i].increase_n_cbetsO();
                            }
                        }
                    }
                } else if (line.find("calls") != std::string::npos) {
                    std::string playerName = line.substr(0, line.find(" "));
                    for (int i = 0; i < players.size(); i++) {
                        if (players[i].get_name() == playerName) {
                            players[i].increase_n_calls();
                            if (players[i].get_raised()) {
                                players[i].increase_n_cbetsO();
                            }
                        }
                    }
                } else if (line.find("checks") != std::string::npos) {
                    std::string playerName = line.substr(0, line.find(" "));
                    for (int i = 0; i < players.size(); i++) {
                        if (players[i].get_name() == playerName) {
                            if (players[i].get_raised()) {
                                players[i].increase_n_cbetsO();
                            }
                        }
                    }
                }

                pos++;
            }
        }

        // showdown actions
        if (line.find("Summary") != std::string::npos) {
            pos++;
            line = hand[pos];
            bool showdown = false;

            while (pos < hand.size()) {
                line = hand[pos];

                if (line.find("shows") != std::string::npos || line.find("mucks") != std::string::npos) {
                    std::string playerName = line.substr(0, line.find(" "));
                    showdown = true;
                    for (int i = 0; i < players.size(); i++) {
                        if (players[i].get_name() == playerName) {
                            players[i].increase_n_showdowns();
                        }
                    }
                } else if (line.find("collected") != std::string::npos) {
                    std::string playerName = line.substr(0, line.find(" "));
                    for (int i = 0; i < players.size(); i++) {
                        if (players[i].get_name() == playerName && showdown) {
                            players[i].increase_n_wins();
                            players[i].increase_n_showdowns();
                        }
                    }
                }

                pos++;
            }
        }
    }

    savePlayers(players);

    return players;
}

int parse_file(std::string path, std::string date) {
    std::ifstream inputFile(path);
    char line[256];
    std::vector<std::string> hand;
    std::vector<Player> players;

    // Opening the hand history file
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file!" << std::endl;
        return 1;
    }

    // Read the hand history file and save each hand as a vector of strings
    // Pass the hand to the parser
    while(inputFile.getline(line, 256)) {
        std::string strLine(line);
        if (strLine.empty()) {
            if (hand[1].find("PokerStars") != std::string::npos) {
                players = parseHandStars(players, hand);
            } else if (hand[1].find("888") != std::string::npos) {
                players = parseHand888(players, hand, date);
            }
            hand.clear();

            // clean screen
            std::cout << "\033[2J\033[1;1H";
            // Print the results
            std::cout << "Player:Hands VPIP PFR 3Bet AF CBet wins/showdowns" << std::endl;
            for (int i = 0; i < players.size(); i++) {
                Stats stats = players[i].get_stats();
                printf("%s: %d %.2f %.2f %.2f %.2f %.2f %.2f/%.2f\n", players[i].get_name().c_str(), stats.n_hands, stats.vpip, stats.pfr, stats.three_bet, stats.af, stats.cbet, stats.wsd, stats.wtsd);
            }
            std::cout << "*********************" << std::endl;

            continue;
        }
        hand.push_back(strLine);
    }

    inputFile.close();
    return 0;
}