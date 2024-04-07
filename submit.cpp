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

set<string> findCommonSeries(const string& p1, const string& p2,
                             const map<string, map<string, vector<TimeSlot>>>& schedule,
                             const map<string, set<string>>& favorites,
                             map<string, string>& lastSeries) {
    set<string> common;

    for (unsigned int i = 1; i <= 7; ++i) {
        string day = "Day" + to_string(i);
        const vector<TimeSlot>& slots1 = schedule.at(p1).at(day);
        const vector<TimeSlot>& slots2 = schedule.at(p2).at(day);

        for (const auto& slot1 : slots1) {
            for (const auto& slot2 : slots2) {
                if (slot1 == slot2) {
                    const set<string>& fav1 = favorites.at(p1);
                    const set<string>& fav2 = favorites.at(p2);

                    set<string> intersect;
                    set_intersection(fav1.begin(), fav1.end(),
                                     fav2.begin(), fav2.end(),
                                     inserter(intersect, intersect.begin()));

                    for (const auto& s : intersect) {
                        common.insert(s);
                    }
                }
            }
        }
    }
    return common;
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

int main() {
    ifstream file("submit.csv");

    if (!file.is_open()) {
        cerr << "Error opening file." << endl;
        return 1;
    }

    map<string, map<string, vector<TimeSlot>>> schedule;
    map<string, set<string>> favorites;
    map<string, string> lastSeries;
    map<string, int> lastIndex;

    string line;

    getline(file, line); // Skip header line

    while (getline(file, line)) {
        vector<string> parts = split(line, ',');
        if (parts.size() < 9) {
            cerr << "Invalid line: " << line << endl;
            continue;
        }

        string person = trim(parts[0]);

        for (unsigned int i = 1; i <= 7; ++i) {
            string day = "Day" + to_string(i);
            string timeStr = trim(parts[i]);
            vector<string> times = split(timeStr, ';');

            for(const string& slot : times) {
                vector<string> slotParts = split(slot, '-');
                if (slotParts.size() != 2) {
                    cerr << "Invalid time slot: " << slot << endl;
                    continue;
                }
                schedule[person][day].emplace_back(trim(slotParts[0]), trim(slotParts[1]));
            }
        }

        string seriesList = trim(parts[8]);
        vector<string> seriesListTokens = split(seriesList, ';');
        set<string> seriesSet(seriesListTokens.begin(), seriesListTokens.end());
        favorites[person] = seriesSet;
        lastSeries[person] = "";
        lastIndex[person] = 0;
    }

    file.close();

    for (unsigned int i = 1; i <= 7; ++i) {
        string day = "Day" + to_string(i);
        cout << "Schedule for " << day << ":\n";
        cout << setfill('-') << setw(18) << "-"<< endl;
        cout << setfill('-') << setw(100) << "-"<< endl;
        cout << "Time Slot\t\t\tFamily Members\t\t\t\t\tSeries\n";
        cout << setfill('-') << setw(100) << "-"<< endl;
        
        map<string, set<string>> slotsForDay;
        for (const auto& member : schedule) {
            const map<string, vector<TimeSlot>>& dailySchedule = member.second;
            const vector<TimeSlot>& timeSlots = dailySchedule.at(day);
            for (const auto& slot : timeSlots) {
                slotsForDay[slot.start + "-" + slot.end].insert(member.first);
            }
        }

        for (const auto& slotInfo : slotsForDay) {
            const string& slot = slotInfo.first;
            const set<string>& members = slotInfo.second;

            cout << slot << "\t\t\t";
            bool first = true;
            for (const auto& member : members) {
                if (!first) {
                    cout << ", ";
                }
                print(member, "1;36");
                first = false;
            }
            cout << "\t\t\t\t\t";

            if (members.size() >= 2) {
                auto it = members.begin();
                const string& person1 = *it;
                ++it;
                const string& person2 = *it;
                set<string> commonSeries = findCommonSeries(person1, person2, schedule, favorites, lastSeries);
                if (!commonSeries.empty()) {
                    auto lastCommonSeries = prev(commonSeries.end());
                    for(const auto& series : commonSeries) {
                        print(series, "1;35");
                        if (&series != &(*lastCommonSeries)) {
                            cout << ", ";
                        }
                    }
                } else {
                    print("N/A", "1;33");
                }
            } else if (members.size() == 1) {
                const string& person = *(members.begin());
                const set<string>& seriesSet = favorites[person];
                cout<<"\t";
                print(getNext(person, seriesSet, lastSeries, lastIndex), "1;32");
            } else {
                print("N/A", "1;33");
            } 
            cout << endl;
        }

        cout << setfill('-') << setw(100) << "-"<< endl<<endl;
    }

    return 0;
}