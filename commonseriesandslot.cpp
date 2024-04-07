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
    regex seriesRegex(R"(([^ ]+)\s*\(([^\)]+)\))");
    smatch match;
    string seriesName, timeSlot;

    // Extract series name and time slot using regex
    if (regex_search(data, match, seriesRegex)) {
        seriesName = match[1];
        timeSlot = match[2];
    }

    return make_pair(seriesName, timeSlot);
}

// Function to find common series between two schedules
set<string> findCommonSeries(const set<string>& schedule1, const set<string>& schedule2) {
    set<string> commonSeries;
    for (const auto& series : schedule1) {
        if (schedule2.find(series) != schedule2.end()) {
            commonSeries.insert(series);
        }
    }
    return commonSeries;
}

// Function to check if there is an intersection of time slots between two schedules
vector<string> getIntersection(const vector<vector<string>>& schedule1, const vector<vector<string>>& schedule2) {
    vector<string> intersection;
    for (const auto& slots1 : schedule1) {
        for (const auto& slots2 : schedule2) {
            for (const auto& slot1 : slots1) {
                int start1, end1, start2, end2;
                stringstream ss1(slot1);
                string start1_str, end1_str;
                getline(ss1, start1_str, '-');
                getline(ss1, end1_str, '-');
                start1 = stoi(start1_str);
                end1 = stoi(end1_str);

                for (const auto& slot2 : slots2) {
                    stringstream ss2(slot2);
                    string start2_str, end2_str;
                    getline(ss2, start2_str, '-');
                    getline(ss2, end2_str, '-');
                    start2 = stoi(start2_str);
                    end2 = stoi(end2_str);

                    if (start1 < end2 && end1 > start2) {
                        int start = max(start1, start2);
                        int end = min(end1, end2);
                        intersection.push_back(to_string(start) + "-" + to_string(end));
                    }
                }
            }
        }
    }
    return intersection;
}

int main() {
    // Open the file
    ifstream file("latest2.csv");

    // Check if file is opened successfully
    if (!file.is_open()) {
        cerr << "Error opening file 'latest.csv'." << endl;
        return 1;
    } else {
        cout << "File 'latest.csv' opened successfully." << endl;
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
    // cout << "Headers: " << line << endl; // Suppressing this line

    // Read each line from the file
    while (getline(file, line)) {
        // cout << "Reading line: " << line << endl; // Suppressing this line
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

    // Print the extracted series and their associated time slots
    cout << "Series with their associated time slots:\n";
    for (const auto& personSeriesPair : seriesMap) {
        cout << "Name: " << personSeriesPair.first << endl;
        cout << "Series: ";
        for (const auto& series : personSeriesPair.second) {
            cout << series << " ";
        }
        cout << endl;
    }

    // Print the intersection of common slots between Alice, Bob, and Charlie
    cout << "Intersection of common slots between Alice, Bob, and Charlie:\n";
    for (int day = 0; day < 7; ++day) {
        cout << "Day " << day + 1 << ":\n";

        // Intersection of common slots between Alice and Bob
        vector<string> aliceBobIntersection = getIntersection(schedule[0][day], schedule[1][day]);
        cout << "Intersection between Alice and Bob: ";
        if (aliceBobIntersection.empty()) {
            cout << "No" << endl;
        } else {
            cout << "Yes: ";
            for (const auto& slot : aliceBobIntersection) {
                cout << slot << " ";
            }
            cout << endl;
        }

        // Intersection of common slots between Bob and Charlie
        vector<string> bobCharlieIntersection = getIntersection(schedule[1][day], schedule[2][day]);
        cout << "Intersection between Bob and Charlie: ";
        if (bobCharlieIntersection.empty()) {
            cout << "No" << endl;
        } else {
            cout << "Yes: ";
            for (const auto& slot : bobCharlieIntersection) {
                cout << slot << " ";
            }
            cout << endl;
        }

        // Intersection of common slots between Alice and Charlie
        vector<string> aliceCharlieIntersection = getIntersection(schedule[0][day], schedule[2][day]);
        cout << "Intersection between Alice and Charlie: ";
        if (aliceCharlieIntersection.empty()) {
            cout << "No" << endl;
        } else {
            cout << "Yes: ";
            for (const auto& slot : aliceCharlieIntersection) {
                cout << slot << " ";
            }
            cout << endl;
        }

        // Intersection of common slots between Alice, Bob, and Charlie
        if (!aliceBobIntersection.empty() && !bobCharlieIntersection.empty() && !aliceCharlieIntersection.empty()) {
            cout << "Intersection among Alice, Bob, and Charlie: ";
            for (const auto& slot : aliceBobIntersection) {
                cout << slot << " ";
            }
            cout << endl;
        } else {
            cout << "Intersection among Alice, Bob, and Charlie: No" << endl;
        }
    }

    // Find common series pairwise and for all three of them
    set<string> aliceSeries = seriesMap["Alice"];
    set<string> bobSeries = seriesMap["Bob"];
    set<string> charlieSeries = seriesMap["Charlie"];

    set<string> aliceBobCommon = findCommonSeries(aliceSeries, bobSeries);
    set<string> aliceCharlieCommon = findCommonSeries(aliceSeries, charlieSeries);
    set<string> bobCharlieCommon = findCommonSeries(bobSeries, charlieSeries);
    set<string> allCommonSeries;
    set_intersection(aliceBobCommon.begin(), aliceBobCommon.end(),
                     aliceCharlieCommon.begin(), aliceCharlieCommon.end(),
                     inserter(allCommonSeries, allCommonSeries.begin()));
    set_intersection(allCommonSeries.begin(), allCommonSeries.end(),
                     bobCharlieCommon.begin(), bobCharlieCommon.end(),
                     inserter(allCommonSeries, allCommonSeries.begin()));

    // Output common series
    cout << "Common series between Alice and Bob:\n";
    for (const auto& series : aliceBobCommon) {
        cout << series << endl;
    }
    cout << endl;

    cout << "Common series between Alice and Charlie:\n";
    for (const auto& series : aliceCharlieCommon) {
        cout << series << endl;
    }
    cout << endl;

    cout << "Common series between Bob and Charlie:\n";
    for (const auto& series : bobCharlieCommon) {
        cout << series << endl;
    }
    cout << endl;

    cout << "Common series among Alice, Bob, and Charlie:\n";
    for (const auto& series : allCommonSeries) {
        cout << series << endl;
    }
    cout << endl;

    return 0;
}
