#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <string>

using namespace std;

struct TimeSlot {
    string startTime;
    string endTime;

    TimeSlot(const string& start, const string& end)
        : startTime(start), endTime(end) {}
};

vector<string> split(const string &s, char delimiter) {
    vector<string> tokens;
    stringstream ss(s);
    string token;
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

int main() {
    ifstream file("name2.csv");

    if (!file.is_open()) {
        cerr << "Error opening file." << endl;
        return 1;
    }

    map<string, map<string, vector<TimeSlot>>> timeslots;
    map<string, set<string>> favoriteSeries;

    string line;
    getline(file, line);  // Skip header

    while (getline(file, line)) {
        vector<string> parts = split(line, ',');
        if (parts.size() < 9) {
            cerr << "Invalid line: " << line << endl;
            continue;
        }

        string name = parts[0];

        for (int i = 1; i <= 7; ++i) {
            string day = "Day" + to_string(i);
            vector<string> timeSlots = split(parts[i], ';');

            for(const string& slot : timeSlots) {
                vector<string> slotParts = split(slot, '-');
                if (slotParts.size() != 2) {
                    cerr << "Invalid time slot: " << slot << endl;
                    continue;
                }
                timeslots[name][day].emplace_back(slotParts[0], slotParts[1]);
            }
        }

        vector<string> list = split(parts[8], ';');
        set<string> seriesSet(list.begin(), list.end());
        favoriteSeries[name] = seriesSet;
    }

    file.close();

    for (const auto& family : timeslots) {
        cout << "Member: " << family.first << endl;

        cout << "Time Slots:" << endl;
        for (const auto& daySchedule : family.second) {
            cout << "Day: " << daySchedule.first << endl;
            for (const auto& timeSlot : daySchedule.second) {
                cout << "  " << timeSlot.startTime << " - " << timeSlot.endTime << endl;
            }
        }

        cout << "Series: ";
        for (const auto& series : favoriteSeries[family.first]) {
            cout << series << " ";
        }
        cout << endl << endl;
    }

    return 0;
}