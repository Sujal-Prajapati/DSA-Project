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

    // Print the schedule in a table format
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
        cout << "Schedule for " << dayName << ":\n";

        // Header for the table
        cout << "Time Slot\tSeries\n";

        // Store printed series for deduplication
        set<string> printedSeries;

        for (int i = 0; i < names.size(); ++i) {
            for (const auto& timeSlot : schedule[i][day]) {
                for (int j = 0; j < timeSlot.size(); ++j) {
                    cout << timeSlot[j];
                    if (j < timeSlot.size() - 1) {
                        cout << ",";
                    }
                }
                cout << "\t";
                // Print series only if not already printed
                for (const auto& series : seriesMap[names[i]]) {
                    if (printedSeries.find(series) == printedSeries.end()) {
                        cout << series << ",";
                        printedSeries.insert(series);
                    }
                }
                cout << "\n";
            }
        }
        cout << endl;

        // Clear printed series for the next day
        printedSeries.clear();
    }

    return 0;
}