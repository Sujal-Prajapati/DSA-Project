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

string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    stringstream ss(s);
    string token;
    while (getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

struct TimeSlot {
    string start;
    string end;

    TimeSlot(const string& s, const string& e)
        : start(s), end(e) {}

    bool operator==(const TimeSlot& other) const {
        return (start == other.start) && (end == other.end);
    }
};
