#include <ctime>
#include <string>
#include <iostream>
#include <dirent.h>
#include <fstream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../parser/parser.hpp"

// TODO
// be able to check more than one folder

// Data types for table management
struct Table{
    std::string name;
    time_t last_modified;
    int iterations;
};

static const int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};

// get tomorrow's date
std::vector<int> get_tomorrow(int year, int month, int day) {
    std::vector<int> date;
    if (day == days[month - 1]) {
        if (month == 12) {
            date.push_back(year + 1);
            date.push_back(1);
            date.push_back(1);
        } else {
            date.push_back(year);
            date.push_back(month + 1);
            date.push_back(1);
        }
    } else {
        date.push_back(year);
        date.push_back(month);
        date.push_back(day + 1);
    }
    return date;
}

std::vector<std::string> getsettings() {
    std::vector<std::string> settings;
    std::string path = "";
    std::string hero = "";
    std::string first;

    // Import settings from file
    std::ifstream inputFile("db/settings.txt");
    if (inputFile.is_open()) {
        std::string line;
        while (getline(inputFile, line)) {
            if (line.find("dir") != std::string::npos) {
                path = line.substr(line.find('=') + 1, line.length() - 1);
            } else if(line.find("hero") != std::string::npos) {
                hero = line.substr(line.find('=') + 1, line.length());
            } else if (line.find("first") != std::string::npos) {
                first = line.substr(line.find('=') + 1, line.length());
            }
        }
        inputFile.close();
    } else {
        std::cout << "Unable to open file" << std::endl;
    }

    // ask for path if not set
    if (path.empty()) {
        std::cout << "Please enter the path to your hand history folder: ";
        std::getline(std::cin, path);
    }
    // ask for hero if not set
    if (hero.empty()) {
        std::cout << "Please enter your hero name: ";
        std::getline(std::cin, hero);
    };

    // write settings to file
    std::ofstream outputFile("db/settings.txt");
    if (outputFile.is_open()) {
        outputFile << "dir=" << path << std::endl;
        outputFile << "hero=" << hero << std::endl;
        if (first == "true")
            outputFile << "first=" << "false" << std::endl;
        else
            outputFile << "first=" << first << std::endl;
        outputFile.close();
    } else {
        std::cout << "Unable to open file" << std::endl;
    }

    // add trailing slash to path if not present
    if (path.back() != '/') {
        path = path + "/";
    }
    path = path + hero + "/";

    settings.push_back(path);
    settings.push_back(first);

    return settings;
}

void main_loop(std::string path, std::vector<int> date) {
    std::vector<Table> current_tables = std::vector<Table>();
    std::vector<std::string> settings = getsettings();
    auto dirp = opendir(settings[0].c_str());
    if (dirp == NULL) {
        std::cout << "Error opening directory" << std::endl;
        return;
    }
    // close directory
    closedir(dirp);

    int len = settings[0].length();
    while (true){
        auto dirp = opendir(settings[0].c_str());
        while (auto dp = readdir(dirp)) {
            std::string filename = dp->d_name;
            std::cout << filename << std::endl;

            std::string string_date;
            if (date[2] < 10) {
                string_date = std::to_string(date[0]) + "" + std::to_string(date[1]) + "0" + std::to_string(date[2]);
            } else {
                string_date = std::to_string(date[0]) + "" + std::to_string(date[1]) + "" + std::to_string(date[2]);
            }
            std::vector<int> tomorrow_date = get_tomorrow(date[0], date[1], date[2]);
            std::string tomorrow_string;
            if (tomorrow_date[2] < 10) {
                tomorrow_string = std::to_string(tomorrow_date[0]) + "" + std::to_string(tomorrow_date[1]) + "0" + std::to_string(tomorrow_date[2]);
            } else {
                tomorrow_string = std::to_string(tomorrow_date[0]) + "" + std::to_string(tomorrow_date[1]) + "" + std::to_string(tomorrow_date[2]);
            }
            // check if file has today's date
            if (filename.find(string_date) != std::string::npos || filename.find(tomorrow_string) != std::string::npos) {
                // check if file is already being parsed

                bool found = false;
                for (int i=0; i<current_tables.size(); i++) {
                    struct stat info;
                    stat((settings[0] + filename).c_str(), &info);
                    if (current_tables[i].name == filename) {
                        found = true;
                        current_tables[i].iterations = 0;
                        // check if file was modified
                        struct stat info;
                        stat((settings[0] + filename).c_str(), &info);
                        if (current_tables[i].last_modified != info.st_mtime) {
                            current_tables[i].last_modified = info.st_mtime;
                            parse_file(settings[0] + filename, string_date);
                        }
                        break;
                    }
                }

                std::cout << found << std::endl;

                if (!found) {
                    parse_file(settings[0] + filename, string_date);
                    Table table;
                    table.name = filename;
                    struct stat info;
                    stat((settings[0] + filename).c_str(), &info);
                    table.last_modified = info.st_mtime;
                    table.iterations = 0;
                    current_tables.push_back(table);
                }
            } else {
            }
        }

        closedir(dirp);

        // remove tables that haven't been modified in 20 iterations
        for (int i=0; i<current_tables.size(); i++) {
            if (current_tables[i].iterations > 20) {
                current_tables.erase(current_tables.begin() + i);
            } else {
                current_tables[i].iterations++;
            }
        }

        printf("Current tables: %lu\n", current_tables.size());

        sleep(30);
    }

}

int main() {
    time_t t = time(0); // get time now
    std::vector<Table> current_tables;
    std::vector<std::string> settings = getsettings();
    
    // create database file if not exists
    // csv file with player stats
    if (settings[1] == "true") {
        std::ofstream outputFile("db/players.txt");
        
        outputFile.close();
    }
    
    int year = localtime(&t)->tm_year + 1900;
    int month = localtime(&t)->tm_mon + 1;
    int day = localtime(&t)->tm_mday;

    std::string string_date = std::to_string(year) + "" + std::to_string(month) + "" + std::to_string(day);
    // create hand history file for the day if not exists
    std::ofstream outputFile("db/" + string_date + ".txt");
    if (outputFile.is_open()) {
        outputFile.close();
    } else {
        std::cout << "Unable to open file" << std::endl;
    }
    outputFile.close();

    auto dirp = opendir(settings[0].c_str());
    if (dirp == NULL) {
        std::cout << "Error opening directory" << std::endl;
        return 1;
    }
    int len = settings[0].length();
    while (auto dp = readdir(dirp)) {
        std::string filename = dp->d_name;
        if (settings[1] == "true"){
            std::string date = std::to_string(year) + "" + std::to_string(month) + "" + std::to_string(day);
            parse_file(settings[0] + filename, date);
        }
    }
    closedir(dirp);

    std::vector<int> st;
    st.push_back(year);
    st.push_back(month);
    st.push_back(day);
    main_loop(settings[0], st);

    return 0;
}