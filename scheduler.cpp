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

// Function to extract series name and its associated time slot from the provided format
pair<string, string> extractSeriesAndTime(const string& data) {
    regex seriesRegex(R"(([^,]+)\s*\(([^\)]+)\))");
    smatch match;
    string seriesName, timeSlot;

    // Extract series name and time slot using regex
    if (regex_search(data, match, seriesRegex)) {
        seriesName = match[1];
        timeSlot = match[2];
    }

    return make_pair(seriesName, timeSlot);
}

// Function to check if the series timing overlaps with available slots
bool checkSeriesTiming(const string& seriesTime, const vector<string>& availableSlots) {
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
    // Open the file
    ifstream file("latest2.csv");

    // Check if file is opened successfully
    if (!file.is_open()) {
        cerr << "Error opening file 'latest2.csv'." << endl;
        return 1;
    } else {
        cout << "File 'latest2.csv' opened successfully." << endl;
    }

    // Vector to store family member names
    vector<string> names;

    // 4D vector to store schedule
    vector<vector<vector<vector<string>>>> schedule;

    // Map to store series names for each person
    map<string, set<string>> seriesMap;

    string line;

    // Skip the first line containing headers
    getline(file, line);

    // Read each line from the file
    while (getline(file, line)) {
        stringstream ss(line);
        vector<string> data;
        string str;

        // Split the line into individual data elements
        while (getline(ss, str, ',')) {
            if (!str.empty() && str.front() == '"' && str.back() == '"') {
                str = str.substr(1, str.size() - 2);
            }
            data.push_back(str);
        }

        // Check if the line contains at least 9 values
        if (data.size() < 9) {
            cerr << "Invalid line: " << line << endl;
            continue;
        }

        // Extract family member name and add it to the names vector
        names.push_back(data[0]);

        // Schedule for each person
        vector<vector<vector<string>>> personSchedule;

        // Iterate over days
        for (int i = 1; i <= 7; ++i) {
            stringstream timeSS(data[i]);
            vector<vector<string>> daySchedule;

            string slot;
            // Split time slots for each day
            while (getline(timeSS, slot, ';')) {
                stringstream slotSS(slot);
                vector<string> timeSlots;

                // Split time slot into start and end time
                while (getline(slotSS, slot, ' ')) {
                    timeSlots.push_back(slot);
                }
                daySchedule.push_back(timeSlots);
            }
            personSchedule.push_back(daySchedule);
        }

        // Add person's schedule to the main schedule
        schedule.push_back(personSchedule);

        // Store series for the person with their associated time slot
        for (int j = 8; j < data.size(); ++j) {
            auto seriesAndTime = extractSeriesAndTime(data[j]);
            seriesMap[data[0]].insert(seriesAndTime.first + " (" + seriesAndTime.second + ")");
        }
    }

    // Close the file
    file.close();

    // Analyze series timing and availability slots for each person
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
                        if (checkSeriesTiming(seriesTiming, {timeSlot})) {
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

    // Plan unwatched series for each free time slot
    for (int day = 0; day < 7; ++day) {
        cout << "Day " << day + 1 << ":\n";
        for (const auto& personName : names) {
            cout << personName << ":\n";
            const vector<vector<string>>& daySchedule = schedule[distance(names.begin(), find(names.begin(), names.end(), personName))][day];
            vector<string> freeSlots;

            // Collect free time slots
            for (const auto& timeSlots : daySchedule) {
                bool isOccupied = false;
                for (const auto& timeSlot : timeSlots) {
                    for (const auto& seriesPair : seriesMap[personName]) {
                        if (checkSeriesTiming(extractSeriesAndTime(seriesPair).second, {timeSlot})) {
                            isOccupied = true;
                            break;
                        }
                    }
                    if (!isOccupied) {
                        freeSlots.push_back(timeSlot);
                    }
                }
            }

            // Plan unwatched series for free time slots
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

    // Print series not watched due to unavailability
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
