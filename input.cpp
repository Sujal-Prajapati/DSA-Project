#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <string>

using namespace std;

struct Slots {
    string startTime;
    string endTime;

    Slots(const string& start, const string& end)
        : startTime(start), endTime(end) {}
};

int main() {
    ifstream file("temp.csv");

    if (!file.is_open()) {
        cerr << "Error opening file." << endl;
        return 1;
    }

    map<string, map<string, vector<Slots>>> timeslots;
    map<string, set<string>> favoriteSeries;

    string line;
    getline(file, line); 

    while (getline(file, line)) {
        istringstream ss(line);
        vector<string> parts;
        string str;

        while (getline(ss, str, ',')) {
            parts.push_back(str);
        }

        if (parts.size() < 9) {
            cerr << "Invalid line: " << line << endl;
            continue;
        }

        string name = parts[0];

        for (int i = 1; i <= 7; ++i) {
            string day = "Day" + to_string(i);
            istringstream SS(parts[i]);
            string timeSlot;

            while (getline(SS, timeSlot, ';')) {
                istringstream slot(timeSlot);
                string startTime, endTime;

                getline(slot, startTime, '-');
                getline(slot, endTime, '-');
                
                timeslots[name][day].emplace_back(startTime, endTime);
            }
        }

        istringstream seriesSS(parts[8]);
        string series;

        while (getline(seriesSS, series, ';')) {
            favoriteSeries[name].insert(series);
        }
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
