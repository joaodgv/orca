#include <string>

using namespace std;

// TODO
// 1. Add a function to update the stats from the database
// 2. add money won/lost
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
        void increase_n_vpip() { this->n_vpip++; printf("%d", this->n_vpip); };
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
        int get_n_hands() { return this->n_hands; };
        int get_n_vpip() { return this->n_vpip; };
        int get_n_pfr() { return this->n_pfr; };
        int get_n_3bet() { return this->n_3bet; };
        int get_n_3betO() { return this->n_3bet_opportunity; };
        int get_n_bets() { return this->n_bets; };
        int get_n_raises() { return this->n_raises; };
        int get_n_calls() { return this->n_calls; };
        int get_n_cbets() { return this->n_cbets; };
        int get_n_cbetsO() { return this->n_cbets_opportunity; };
        int get_n_wins() { return this->n_wins; };
        int get_n_showdowns() { return this->n_showdowns; };
        void reset_hand() { this->raised = false; };
        void save_to_db();
        void update_from_db();
        Stats get_stats() {
            Stats stats;
            stats.n_hands = this->n_hands;
            stats.vpip = (float)this->n_vpip / (float)this->n_hands;
            stats.pfr = (float)this->n_pfr / (float)this->n_hands;
            float three_bet = (float)this->n_3bet / (float)this->n_3bet_opportunity;
            stats.three_bet = (three_bet > 0) ? three_bet : 0;
            float af = (float)(this->n_bets + this->n_raises) / (float)this->n_calls;
            stats.af = (af > 0) ? af : 0;
            float cbet = (float)this->n_cbets / (float)this->n_cbets_opportunity;
            stats.cbet = (cbet > 0) ? cbet : 0;
            stats.wtsd = (float)this->n_showdowns / (float)this->n_hands;
            float wsd = (float)this->n_wins / (float)this->n_showdowns;
            stats.wsd = (wsd > 0) ? wsd : 0;
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

void Player::save_to_db() {
    // TODO
}

void Player::update_from_db() {
    // TODO
}