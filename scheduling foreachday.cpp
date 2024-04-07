#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <set>
#include <map>
#include <regex>

using namespace std;

pair<string, string> extractseriesandtime(const string& data) {
    regex serreg(R"(([^,]+)\s*\(([^\)]+)\))");
    smatch match;
    string seriesName, timeSlot;

    if (regex_search(data, match, serreg)) {
        seriesName = match[1];
        timeSlot = match[2];
    }

    return make_pair(seriesName, timeSlot);
}

bool seriestimecalc(const string& seriesTime, const vector<string>& availableSlots) {
    for (const auto& slot : availableSlots) {
        int startSlot, endSlot, startSeries, endSeries;
        stringstream ssSlot(slot);
        stringstream ssSeries(seriesTime);
        string startSlotStr, endSlotStr, startSeriesStr, endSeriesStr;
        
        getline(ssSlot, startSlotStr, '-');
        getline(ssSlot, endSlotStr, '-');
        getline(ssSeries, startSeriesStr, '-');
        getline(ssSeries, endSeriesStr, '-');
        
        startSlot = stoi(startSlotStr);
        endSlot = stoi(endSlotStr);
        startSeries = stoi(startSeriesStr);
        endSeries = stoi(endSeriesStr);
        
        if (startSeries >= startSlot && endSeries <= endSlot) {
            return true;
        }
    }
    return false;
}

int main() {
    ifstream file("latest2.csv");

    if (!file.is_open()) {
        cerr << "Error opening file 'latest2.csv'." << endl;
        return 1;
    } else {
        // cout << "File 'latest2.csv' opened successfully." << endl;
    }

    vector<string> names;
    vector<vector<vector<vector<string>>>> schedule;
    map<string, set<string>> seriesMap;

    string line;

    getline(file, line);

    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> data;
        string str;

        while (getline(ss, str, ',')) {
            if (!str.empty() && str.front() == '"' && str.back() == '"') {
                str = str.substr(1, str.size() - 2);
            }
            data.push_back(str);
        }

        if (data.size() < 9) {
            cerr << "Invalid line: " << line << endl;
            continue;
        }

        names.push_back(data[0]);

        vector<vector<vector<string>>> personSchedule;

        for (int i = 1; i <= 7; ++i) {
            stringstream timeSS(data[i]);
            vector<vector<string>> daySchedule;

            string slot;
            while (getline(timeSS, slot, ';')) {
                stringstream slotSS(slot);
                vector<string> timeSlots;

                while (getline(slotSS, slot, ' ')) {
                    timeSlots.push_back(slot);
                }
                daySchedule.push_back(timeSlots);
            }
            personSchedule.push_back(daySchedule);
        }

        schedule.push_back(personSchedule);

        for (int j = 8; j < data.size(); ++j) {
            auto seriesAndTime = extractseriesandtime(data[j]);
            seriesMap[data[0]].insert(seriesAndTime.first + " (" + seriesAndTime.second + ")");
        }
    }

    file.close();

    map<string, map<int, vector<string>>> seriesWatched;
    map<string, map<int, vector<string>>> seriesNotWatched;
    for (const auto& personSeriesPair : seriesMap) {
        string personName = personSeriesPair.first;
        const set<string>& personSeries = personSeriesPair.second;

     for (int day = 0; day < 7; ++day) {
            const vector<vector<string>>& daySchedule = schedule[distance(names.begin(), find(names.begin(), names.end(), personName))][day];

    for (const auto& series : personSeries) {
     string seriesName = series.substr(0, series.find('(') - 1);
      string seriesTiming = series.substr(series.find('(') + 1, series.find(')') - series.find('(') - 1);

     bool watched = false;
          for (const auto& timeSlots : daySchedule) {
        for (const auto& timeSlot : timeSlots) {
       if (seriestimecalc(seriesTiming, {timeSlot})) {
          seriesWatched[personName][day].push_back(seriesName);
              watched = true;
                  break;
                 }
             }
             if (watched) {
                  break;
              }
        }

         if (!watched) {
             seriesNotWatched[personName][day].push_back(seriesName);
                }
            }
        }
    }

    for (int day = 0; day < 7; ++day) {
        cout << "Day " << day + 1 << ":\n";
        for (const auto& personName : names) {
            cout << personName << ":\n";
            const vector<vector<string>>& daySchedule = schedule[distance(names.begin(), find(names.begin(), names.end(), personName))][day];
            vector<string> freeSlots;

            for (const auto& timeSlots : daySchedule) {
                bool isOccupied = false;
                for (const auto& timeSlot : timeSlots) {
                    for (const auto& seriesPair : seriesMap[personName]) {
                        if (seriestimecalc(extractseriesandtime(seriesPair).second, {timeSlot})) {
                            isOccupied = true;
                            break;
                        }
                    }
                    if (!isOccupied) {
                        freeSlots.push_back(timeSlot);
                    }
                }
            }

            for (const auto& slot : freeSlots) {
                if (seriesNotWatched.find(personName) != seriesNotWatched.end() && seriesNotWatched[personName].find(day) != seriesNotWatched[personName].end()) {
                    if (!seriesNotWatched[personName][day].empty()) {
                        string series = seriesNotWatched[personName][day].front();
                        seriesNotWatched[personName][day].erase(seriesNotWatched[personName][day].begin());
                        seriesWatched[personName][day].push_back(series);
                        cout << "Planned series '" << series << "' for " << personName << " in time slot: " << slot << endl;
                    }
                }
            }

            if (freeSlots.empty()) {
                cout << "No free slots available for planning series." << endl;
            }
        }
        cout << endl;
    }

    cout << "Series Watched by Each Person at Each Day and Time Slot:\n";
    for (int day = 0; day < 7; ++day) {
        cout << "Day " << day + 1 << ":\n";
        for (const auto& personName : names) {
            cout << "Person: " << personName << endl;
            const vector<vector<string>>& daySchedule = schedule[distance(names.begin(), find(names.begin(), names.end(), personName))][day];
            for (int slot = 0; slot < daySchedule.size(); ++slot) {
                if (slot != 1) {
                    cout << "Time Slot " << slot + 1 << ":\n";
                    bool seriesFound = false;
                    for (const auto& series : seriesMap[personName]) {
                        if (seriestimecalc(extractseriesandtime(series).second, daySchedule[slot])) {
                            cout << "  Series: " << series << endl;
                            seriesFound = true;
                        }
                    }
                    if (!seriesFound) {
                        cout << "  No series watched.\n";
                    }
                }
            }
        }
    }

    cout << "Series Not Watched Due to Unavailability:\n";
    for (const auto& personSeriesPair : seriesNotWatched) {
        const string& personName = personSeriesPair.first;
        const auto& notWatched = personSeriesPair.second;
        for (const auto& daySeriesPair : notWatched) {
            int day = daySeriesPair.first;
            const auto& seriesList = daySeriesPair.second;
            for (const auto& series : seriesList) {
                cout << "Person: " << personName << ", Day: " << day + 1 << ", Series: " << series << endl;
            }
        }
    }

    return 0;
}