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
    vector<string> vect;
    stringstream ss(s);
    string token;
    while (getline(ss, token, delimiter)) {
        vect.push_back(trim(token));
    }
    return vect;
}

struct TimeSlot {
    string startTime;
    string endTime;

    TimeSlot(const string& start, const string& end) : startTime(trim(start)), endTime(trim(end)) {}

    bool operator==(const TimeSlot& other) const {
        return (startTime == other.startTime) && (endTime == other.endTime);
    }
};

//Finding common series
set<string> findCommonFavoriteSeries(const string& p1, const string& p2,const map<string, map<string, vector<TimeSlot>>>& schedule,const map<string, set<string>>& series,map<string, string>& lastSeries) {
    set<string> common;
    for (unsigned int i = 1; i <= 7; ++i) {
        string day = "Day" + to_string(i);
        const vector<TimeSlot>& slots1 = schedule.at(p1).at(day);
        const vector<TimeSlot>& slots2 = schedule.at(p2).at(day);

        for (const auto& t1 : slots1) {
            for (const auto& t2 : slots2) {
                if (t1 == t2) {                                             // Time slots are same or not
                    const set<string>& series1 = series.at(p1);
                    const set<string>& series2 = series.at(p2);

                    set<string> s;                                          // Storing comman series
                    set_intersection(series1.begin(), series1.end(),series2.begin(), series2.end(),inserter(s, s.begin()));

                    // Add common series to the set
                    for(const auto& series : s) {
                        common.insert(series);
                    }
                }
            }
        }
    }
    return common;                                                          //Returning common series
}
string getNext(const string& person, const set<string>& seriesSet, map<string, string>& last,
               map<string, int>& lastIndex) {
    auto it = seriesSet.begin();
    int lastIdx = lastIndex[person];
    if (lastIdx == seriesSet.size() - 1) {
        lastIndex[person] = 0;
        return *it;
    }
    advance(it, lastIdx + 1);
    lastIndex[person] = lastIdx + 1;
    return *it;
}

void print(const string& text, const string& color) {
    cout << "\033[" << color << "m" << text << "\033[0m";
}
