#include <string>

using namespace std;

/*
*   Data structures for the hand history parser
*/
// Represents a player in the hand
struct Player {
    string name;
    int n_hands;
    // Pre-flop stats
    int n_vpip;
    int n_pfr;
    int n_3bet;
    int n_3bet_opportunity;
    //Post-flop stats
    int n_bets;
    int n_raises;
    int n_calls;
    int n_cbets;
    int n_cbets_opportunity;
    // Showdown stats
    int n_wins;
    int n_showdowns;
};
