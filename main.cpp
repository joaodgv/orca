#include <ctime>
#include <string>
#include <iostream>
#include <filesystem>
#include <dirent.h>

int main() {
    time_t t = time(0);   // get time now
    int year, month, day;
    std::string path = "/Users/j/Library/Application Support/PokerStarsPT/HandHistory/AceOw1/";

    year = 2023;//localtime(&t)->tm_year + 1900;
    month = 11;//localtime(&t)->tm_mon + 1;
    day = 18;//localtime(&t)->tm_mday;

    std::string string_date = std::to_string(year) + "" + std::to_string(month) + "" + std::to_string(day);

    auto dirp = opendir(path.c_str());
    if (dirp == NULL) {
        std::cout << "Error opening directory" << std::endl;
        return 1;
    }
    int len = path.length();
    while (auto dp = readdir(dirp)) {
        std::string filename = dp->d_name;
        if (filename.find(string_date) != std::string::npos) {
            std::string date = filename.substr(0, 10);
            std::cout << date << std::endl;
        }
    }

    return 0;
}