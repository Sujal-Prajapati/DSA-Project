#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <set>
#include <map>

using namespace std;

int main() {
    ifstream file("temp.csv");

    if (!file.is_open()) {
        cerr << "Error opening file." << endl;
        return 1;
    }

    vector<string> names;
    vector<vector<vector<vector<string>>>> schedule; // 4D vector to store schedule
    map<string, set<string>> seriesMap; // Map to store series names for each person

    string line;

    // Skip the first line containing headers
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

        if (data.size() < 9) { // Assuming we need at least 9 values in each line
            cerr << "Invalid line: " << line << endl;
            continue;
        }

        // Skip the first element which is the name
        names.push_back(data[0]);

        vector<vector<vector<string>>> personSchedule; // Schedule for each person

        for (int i = 1; i <= 7; ++i) { // Iterate over days
            stringstream timeSS(data[i]);
            vector<vector<string>> daySchedule; // Schedule for each day

            string slot;
            while (getline(timeSS, slot, ';')) { // Split time slots
                stringstream slotSS(slot);
                vector<string> timeSlots; // Time slots for each day

                while (getline(slotSS, slot, ' ')) { // Split time slot into start and end time
                    timeSlots.push_back(slot);
                }
                daySchedule.push_back(timeSlots); // Add time slots to the day's schedule
            }
            personSchedule.push_back(daySchedule); // Add day's schedule to person's schedule
        }

        schedule.push_back(personSchedule); // Add person's schedule to the main schedule

        // Store series for the person
        vector<string> personSeries(data.begin() + 8, data.end());
        set<string> personSeriesSet(personSeries.begin(), personSeries.end());
        seriesMap[data[0]] = personSeriesSet;
    }

    file.close();

    // Sorting the schedule based on the first time slot of the first day
    for (int i = 0; i < names.size(); ++i) {
        for (int j = i + 1; j < names.size(); ++j) {
            if (schedule[i][0][0][0] > schedule[j][0][0][0]) {
                swap(schedule[i], schedule[j]);
                swap(names[i], names[j]);
            }
        }
    }

    // Print the sorted schedule
    cout << "Schedule:\n";
    for (int i = 0; i < names.size(); ++i) {
        cout << "Name: " << names[i] << endl;
        for (int j = 0; j < 7; ++j) {
            cout << "Day " << j + 1 << ":\n";
            for (int k = 0; k < schedule[i][j].size(); ++k) {
                cout << "  Time Slot: ";
                for (int l = 0; l < schedule[i][j][k].size(); ++l) {
                    cout << schedule[i][j][k][l] << " ";
                }
                cout << endl;
            }
        }
        cout << endl;
    }

    // Print the intersection of series between each pair of persons
    cout << "Intersection of series between each pair of persons:\n";
    for (int i = 0; i < names.size(); ++i) {
        for (int j = i + 1; j < names.size(); ++j) {
            set<string> intersection;
            set_intersection(seriesMap[names[i]].begin(), seriesMap[names[i]].end(),
                             seriesMap[names[j]].begin(), seriesMap[names[j]].end(),
                             inserter(intersection, intersection.begin()));
            if (!intersection.empty()) {
                cout << "Intersection between " << names[i] << " and " << names[j] << ":\n";
                for (const auto& series : intersection) {
                    cout << "  Series: " << series << endl;
                }
                cout << endl;
            }
        }
    }

    // Print the intersection of time slots between each pair of names for each day
    cout << "Intersection of time slots between each pair of names for each day:\n";
    for (int day = 0; day < 7; ++day) {
        string dayName;
        switch (day) {
            case 0:
                dayName = "Monday";
                break;
            case 1:
                dayName = "Tuesday";
                break;
            case 2:
                dayName = "Wednesday";
                break;
            case 3:
                dayName = "Thursday";
                break;
            case 4:
                dayName = "Friday";
                break;
            case 5:
                dayName = "Saturday";
                break;
            case 6:
                dayName = "Sunday";
                break;
        }
        cout << dayName << ":\n";
        for (int i = 0; i < names.size(); ++i) {
            for (int j = i + 1; j < names.size(); ++j) {
                cout << "  Intersection between " << names[i] << " and " << names[j] << ":\n";
                set<string> intersection;
                for (const auto& timeSlot : schedule[i][day]) {
                    if (find(schedule[j][day].begin(), schedule[j][day].end(), timeSlot) != schedule[j][day].end()) {
                        intersection.insert(timeSlot.begin(), timeSlot.end());
                    }
                }
                // Plan series for each person based on intersection
                if (!intersection.empty()) {
                    cout << "    Common Time Slots:\n";
                    for (const auto& slot : intersection) {
                        cout << "      Time Slot: " << slot << endl;
                        // Plan series for common time slot
                        cout << "      Series for Common Time Slot:\n";
                        for (const auto& series : seriesMap[names[i]]) {
                            if (intersection.find(series) != intersection.end()) {
                                cout << "        Series: " << series << endl;
                            }
                        }
                    }
                } else {
                    // Plan series for individual person if no common time slot
                    cout << "    No Common Time Slots. Planning Series for Individual Time Slots:\n";
                    for (const auto& timeSlot : schedule[i][day]) {
                        cout << "      Time Slot: ";
                        for (const auto& slot : timeSlot) {
                            cout << slot << " ";
                        }
                        cout << endl;
                        cout << "      Series for Individual Time Slot:\n";
                        for (const auto& series : seriesMap[names[i]]) {
                            cout << "        Series: " << series << endl;
                        }
                    }
                }
            }
        }
    }

    return 0;
}