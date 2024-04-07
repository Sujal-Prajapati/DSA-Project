#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <iomanip>

using namespace std;

// Function to trim whitespace from the beginning and end of a string
string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// Function to split a string by a delimiter and return a vector of tokens
vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    stringstream ss(s);
    string token;
    while (getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

// Struct to represent a time slot
struct TimeSlot {
    string startTime;
    string endTime;

    TimeSlot(const string& start, const string& end)
        : startTime(start), endTime(end) {}

    bool operator==(const TimeSlot& other) const {
        return (startTime == other.startTime) && (endTime == other.endTime);
    }
};
