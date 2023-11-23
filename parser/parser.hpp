#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "types.hpp" // This is the header file that contains the Player struct

using namespace std;

// TODO
// 1. check if hand was already parsed
// 2. update stats in database
// 3. show stats in real time

// Updates the player vector with the players in the hand
std::vector<Player> updatePlayers(std::vector<Player> players, std::vector<std::string> hand) {
    // Checks if this is the first hand in the file
    if (players.size() == 0) {
        for (int i = 2; i < hand.size(); i++) {
            std::string line = hand[i];
            if (line.find("Seat") != std::string::npos) {
                Player player(line.substr(line.find(":") + 2, line.find("(") - line.find(":") - 3));
                player.increase_n_hands();
                players.push_back(player);
            } else {
                break;
            }
        }
    } else {
        // Updates the number of hands played by each player
        // if the player is already in the vector
        for (int i = 2; i < hand.size(); i++) {
            std::string line = hand[i];
            if (line.find("Seat") != std::string::npos) {
                std::string player_name = line.substr(line.find(":") + 2, line.find("(") - line.find(":") - 3);
                for (int j = 0; j < players.size(); j++) {
                    if (players[j].get_name() == player_name) {
                        players[j].increase_n_hands();
                        break;
                    }
                }
            } else {
                break;
            }
        }
    }

    // Checks for new players joining the table
    // Also deletes players that leave the table
    for (int i = 0; i < hand.size(); i++){
        if (hand[i].find("joins") != std::string::npos) {
            Player new_player(hand[i].substr(0, hand[i].find(" ")));
            players.push_back(new_player);
            hand.erase(hand.begin() + i);
        } else if (hand[i].find("leaves") != std::string::npos) {
            std::string player_name = hand[i].substr(0, hand[i].find(" "));
            for (int j = 0; j < players.size(); j++) {
                if (players[j].get_name() == player_name) {
                    players.erase(players.begin() + j);
                    break;
                }
            }
        }
    }

    return players;
}

// Parses a single hand from the input file
std::vector<Player> parseHand(std::vector<Player> players , std::vector<std::string> hand) {
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
    players = updatePlayers(players, hand);

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

int parse_file(std::string path) {
    printf("Parsing file: %s\n", path.c_str());
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
            players = parseHand(players, hand);
            hand.clear();
            continue;
        }
        hand.push_back(strLine);
    }
    
    // Print the results
    std::cout << "Player:Hands VPIP PFR 3Bet 3BetO AF CBet wins/showdowns" << std::endl;
    for (int i = 0; i < players.size(); i++) {
        Stats stats = players[i].get_stats();
        std::cout << players[i].get_name() << ":" << stats.n_hands << " " << stats.vpip << " " << stats.pfr << " " << stats.three_bet << " " << stats.af << " " << stats.cbet << " " << stats.wsd << "/" << stats.wtsd << std::endl;
    }
    std::cout << "*********************" << std::endl;

    inputFile.close();
    return 0;
}