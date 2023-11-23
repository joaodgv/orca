#include <string>
#include <vector>
#include <sstream>

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
        void reset_hand() { this->raised = false; };
        void save_to_db();
        void update_from_db(std::vector<std::string> data);
        string get_stats_string();
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

void Player::update_from_db(std::vector<std::string> data) {
    this->n_hands = std::stoi(data[0]);
    this->n_vpip = std::stoi(data[1]);
    this->n_pfr = std::stoi(data[2]);
    this->n_3bet = std::stoi(data[3]);
    this->n_3bet_opportunity = std::stoi(data[4]);
    this->raised = false;
    this->n_bets = std::stoi(data[5]);
    this->n_raises = std::stoi(data[6]);
    this->n_calls = std::stoi(data[7]);
    this->n_cbets = std::stoi(data[8]);
    this->n_cbets_opportunity = std::stoi(data[9]);
    this->n_wins = std::stoi(data[10]);
    this->n_showdowns = std::stoi(data[11]);
}

// turns stats in a csv format
string Player::get_stats_string() {
    stringstream stats;
    string stats_string;
    stats << this->n_hands << "," << this->n_vpip << "," << this->n_pfr << "," << this->n_3bet << "," << this->n_3bet_opportunity << "," << this->n_bets << "," << this->n_raises << "," << this->n_calls << "," << this->n_cbets << "," << this->n_cbets_opportunity << "," << this->n_wins << "," << this->n_showdowns;
    
    stats >> stats_string;

    return stats_string;
}