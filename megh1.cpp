#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>

using namespace std;

// Function to trim whitespace from the beginning and end of a string
string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    if (string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

// Function to split a string by a delimiter and return a vector of tokens
vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    stringstream ss(s);
    string token;
    while (getline(ss, token, delimiter)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

int main() {
    // List of series
    set<string> seriesList = {
        "Game of Thrones", "Breaking Bad", "Stranger Things", "Friends", "The Office", "The Crown",
        "Sherlock", "Peaky Blinders", "Narcos", "Black Mirror", "The Mandalorian", "Money Heist",
        "The Witcher", "The Big Bang Theory", "House of Cards", "The Simpsons", "Grey's Anatomy",
        "The Walking Dead", "Vikings", "Westworld", "Rick and Morty", "The Handmaid's Tale",
        "Better Call Saul", "The Haunting of Hill House", "Mindhunter", "Dexter", "Brooklyn Nine-Nine",
        "The Marvelous Mrs. Maisel", "How I Met Your Mother", "Lost", "Stranger Things", "Chernobyl",
        "The Umbrella Academy", "Fargo", "Dark", "Breaking Bad", "Prison Break", "The Good Place",
        "Homeland", "Arrested Development", "Ozark", "True Detective", "Parks and Recreation",
        "Orange Is the New Black", "BoJack Horseman", "The Sopranos", "Suits", "Modern Family",
        "Big Little Lies", "Black Sails", "The Boys", "Avatar: The Last Airbender", "Futurama"
    };

    ifstream file("name2.csv"); // Change file name if needed

    if (!file.is_open()) {
        cerr << "Error opening file." << endl;
        return 1;
    }

    map<string, map<string, vector<pair<string, string>>>> familySchedule; // Map to store family member's schedule by day
    map<string, set<string>> favoriteSeries; // Map to store favorite series for each family member

    string line;

    // Skip the header line
    getline(file, line);

    while (getline(file, line)) {
        vector<string> parts = split(line, ',');
        if (parts.size() < 9) {
            cerr << "Invalid line: " << line << endl;
            continue;
        }

        string name = trim(parts[0]);

        // Store available time slots for each day
        for (int i = 1; i <= 7; ++i) {
            string day = "Day" + to_string(i);
            string timeSlot = trim(parts[i]);
            vector<string> timeSlots = split(timeSlot, ';');

            familySchedule[name][day] = {};
            for (const string& slot : timeSlots) {
                vector<string> slotParts = split(slot, '-');
                if (slotParts.size() != 2) {
                    cerr << "Invalid time slot: " << slot << endl;
                    continue;
                }
                familySchedule[name][day].push_back(make_pair(trim(slotParts[0]), trim(slotParts[1])));
            }
        }

        // Store favorite series
        string seriesList = trim(parts[8]);
        vector<string> seriesListTokens = split(seriesList, ',');
        set<string> seriesSet(seriesListTokens.begin(), seriesListTokens.end());
        favoriteSeries[name] = seriesSet;
    }

    file.close();

    // Output the TV schedule day by day
    for (int i = 1; i <= 7; ++i) {
        string day = "Day" + to_string(i);
        cout << "Schedule for " << day << ":\n";
        cout << "Time Slot\tFamily Members\tSuggested Series\n";

        // Collect time slots for the day from all family members
        map<string, set<string>> timeSlotsForDay;
        for (const auto& familyMember : familySchedule) {
            const map<string, vector<pair<string, string>>>& scheduleByDay = familyMember.second;
            const vector<pair<string, string>>& timeSlots = scheduleByDay.at(day);
            for (const auto& timeSlot : timeSlots) {
                timeSlotsForDay[timeSlot.first + "-" + timeSlot.second].insert(familyMember.first);
            }
        }

        // Output time slots in ascending order
        for (const auto& timeSlot : timeSlotsForDay) {
            const string& slot = timeSlot.first;
            const set<string>& familyMembers = timeSlot.second;

            cout << slot << "\t";
            for (auto it = familyMembers.begin(); it != familyMembers.end(); ++it) {
                cout << *it;
                if (next(it) != familyMembers.end()) {
                    cout << ", ";
                }
            }

            cout << "\t";

            // Find common series among family members available at this time slot
            set<string> commonSeries;
            for (const auto& member : familyMembers) {
                if (commonSeries.empty()) {
                    commonSeries = favoriteSeries[member];
                } else {
                    set<string> temp;
                    set_intersection(commonSeries.begin(), commonSeries.end(),
                                     favoriteSeries[member].begin(), favoriteSeries[member].end(),
                                     inserter(temp, temp.begin()));
                    commonSeries = temp;
                }
            }

            // If no common series, suggest a series not watched by any family member
            if (commonSeries.empty()) {
                for (const auto& series : seriesList) {
                    bool found = false;
                    for (const auto& member : familyMembers) {
                        if (favoriteSeries[member].count(series) > 0) {
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        commonSeries.insert(series);
                        break;
                    }
                }
            }

            // Output suggested series
            for (auto it = commonSeries.begin(); it != commonSeries.end(); ++it) {
                cout << *it;
                if (next(it) != commonSeries.end()) {
                    cout << ", ";
                }
            }

            cout << endl;
        }

        cout << endl;
    }

    return 0;
}
