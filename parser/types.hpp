#include <string>

using namespace std;

// TODO
// 1. Add a function to update the stats from the database
/*
*   Data structures for the hand history parser
*/
// Represents the stats
struct Stats {
    int n_hands;
    // pre-flop stats
    float vpip;
    float pfr;
    float three_bet;
    float af;
    float cbet;
    float wtsd;
    float wsd;
};

class Player {
    private:
        string name;
        int n_hands;
        // Pre-flop stats
        int n_vpip;
        int n_pfr;
        int n_3bet;
        int n_3bet_opportunity;
        bool raised;
        //Post-flop stats
        int n_bets;
        int n_raises;
        int n_calls;
        int n_cbets;
        int n_cbets_opportunity;
        // Showdown stats
        int n_wins;
        int n_showdowns;

    public:
        Player(string name);
        string get_name() { return this->name; };
        void increase_n_hands() { this->n_hands++; };
        void increase_n_vpip() { this->n_vpip++; };
        void increase_n_pfr() { this->n_pfr++; };
        void increase_n_3bet() { this->n_3bet++; };
        void increase_n_3betO() { this->n_3bet_opportunity++; };
        void set_raised(bool raised) { this->raised = raised; };
        bool get_raised() { return this->raised; };
        void increase_n_bets() { this->n_bets++; };
        void increase_n_raises() { this->n_raises++; };
        void increase_n_calls() { this->n_calls++; };
        void increase_n_cbets() { this->n_cbets++; };
        void increase_n_cbetsO() { this->n_cbets_opportunity++; };
        void increase_n_wins() { this->n_wins++; };
        void increase_n_showdowns() { this->n_showdowns++; };
        void reset_hand() { this->raised = false; };
        void update_from_db();
        Stats get_stats() {
            Stats stats;
            stats.n_hands = this->n_hands;
            stats.vpip = (float)this->n_vpip / (float)this->n_hands;
            stats.pfr = (float)this->n_pfr / (float)this->n_hands;
            stats.three_bet = (float)this->n_3bet / (float)this->n_3bet_opportunity;
            stats.af = (float)(this->n_bets + this->n_raises) / (float)this->n_calls;
            stats.cbet = (float)this->n_cbets / (float)this->n_cbets_opportunity;
            stats.wtsd = (float)this->n_showdowns / (float)this->n_hands;
            stats.wsd = (float)this->n_wins / (float)this->n_showdowns;
            return stats;
        };
};

Player::Player(string name) {
    this->name = name;
    this->n_hands = 0;
    this->n_vpip = 0;
    this->n_pfr = 0;
    this->n_3bet = 0;
    this->n_3bet_opportunity = 0;
    this->raised = false;
    this->n_bets = 0;
    this->n_raises = 0;
    this->n_calls = 0;
    this->n_cbets = 0;
    this->n_cbets_opportunity = 0;
    this->n_wins = 0;
    this->n_showdowns = 0;
}

void Player::update_from_db() {
    // TODO
}