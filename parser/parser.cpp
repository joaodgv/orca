#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "types.cpp" // This is the header file that contains the Player struct

using namespace std;

// Updates the player vector with the players in the hand
std::vector<Player> updatePlayers(std::vector<Player> players, std::vector<std::string> hand) {
    // Checks if this is the first hand in the file
    if (players.size() == 0) {
        for (int i = 2; i < hand.size(); i++) {
            std::string line = hand[i];
            if (line.find("Seat") != std::string::npos) {
                Player player;
                player.name = line.substr(line.find(":") + 2, line.find("(") - line.find(":") - 3);
                player.n_hands = 1;
                player.n_vpip = 0;
                player.n_pfr = 0;
                player.n_3bet = 0;
                player.n_3bet_opportunity = 0;
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
                    if (players[j].name == player_name) {
                        players[j].n_hands++;
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
            Player new_player;
            new_player.name = hand[i].substr(0, hand[i].find(" "));
            new_player.n_hands = 0;
            new_player.n_vpip = 0;
            new_player.n_pfr = 0;
            new_player.n_3bet = 0;
            new_player.n_3bet_opportunity = 0;
            players.push_back(new_player);
            hand.erase(hand.begin() + i);
        } else if (hand[i].find("leaves") != std::string::npos) {
            std::string player_name = hand[i].substr(0, hand[i].find(" "));
            for (int j = 0; j < players.size(); j++) {
                if (players[j].name == player_name) {
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
                        if (players[i].name == playerName) {
                            players[i].n_pfr++;
                            players[i].n_vpip++;
                        }
                    }
                } else if (line.find("raises") != std::string::npos && wasRaised) {
                    std::string playerName = line.substr(0, line.find(":"));
                    for (int i = 0; i < players.size(); i++) {
                        if (players[i].name == playerName) {
                            players[i].n_3bet++;
                            players[i].n_3bet_opportunity++;
                        }
                    }
                } else if (line.find("calls") != std::string::npos) {
                    std::string playerName = line.substr(0, line.find(":"));
                    printf("Player %s calls\n", playerName.c_str());
                    for (int i = 0; i < players.size(); i++) {
                        if (players[i].name == playerName) {
                            players[i].n_vpip++;
                            if (wasRaised) {
                                players[i].n_3bet_opportunity++;
                            }
                        }
                    }
                }

                pos++;
            }
        }

        // postflop actions
        // if (line.find("FLOP") != std::string::npos) {
        //     pos++;
        //     line = hand[pos];

        //     while (line.find("*") == std::string::npos) {
        //         line = hand[pos];

        //         if (line.find("bets") != std::string::npos) {

        //         }
        //     }
        // }
    }

    // parse the flop actions

    // parse the turn actions

    // parse the river actions

    // parse the showdown actions

    // parse the pot size

    return players;
}

int main() {
    std::ifstream inputFile("handhistory.txt");
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
    std::cout << "Player:Hands VPIP PFR 3Bet 3BetOpportunity" << std::endl;
    for (int i = 0; i < players.size(); i++) {
        float vpip = (float)players[i].n_vpip / (float)players[i].n_hands;
        float pfr = (float)players[i].n_pfr / (float)players[i].n_hands;
        float threeBet = (float)players[i].n_3bet / (float)players[i].n_3bet_opportunity;
        threeBet = (players[i].n_3bet_opportunity == 0)? 0 : threeBet;
        std::cout << players[i].name << ":" << players[i].n_hands << " " << vpip << " " << pfr << " " << threeBet << std::endl;
    }


    inputFile.close();
    return 0;
}