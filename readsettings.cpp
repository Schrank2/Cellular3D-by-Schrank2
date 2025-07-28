#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "functions.h"
#include "defs.h"
using namespace std;
// https://www.w3schools.com/cpp/cpp_files.asp

vector<string> readSettings() {
	std::vector<std::string> Variables(7);
	cout << endl << "Reading Settings from options.txt" << endl;
	string settingsFile;
	// Read from the text file
	ifstream MyReadFile("options.txt");
	vector<string> lines;
	vector<vector<string>> settings;
	string line;
	// Print the file content to the console
	while (getline(MyReadFile, line)) {
		lines.push_back(line);
		cout << line << endl;
	}
	cout << endl;
	// Interpret the lines
	for (int i = 0; i < lines.size(); i++) {
		istringstream iss(lines[i]);  // create a string stream from the line
		string key;
		size_t pos = lines[i].find_first_not_of(" \t");
		if (pos != string::npos) {
			if (lines[i][pos] != '#') {
				settings.push_back(vector<string>());
				while (getline(iss, key, '=')) {
					settings.back().push_back(key);
				}
			}
		}
	}
	// Close the file
	MyReadFile.close();
	for (int i = 0; i < settings.size(); i++) {
		string key = settings[i][0];
		string value = settings[i][1];
		if (key== "GameScale") {
			Variables[0] = value;
		} else if (key == "mapDensity") {
			Variables[1] = value;
		} else if (key == "ScreenWidth") {
			Variables[2] = value;
		} else if (key == "ScreenHeight") {
			Variables[3] = value;
		} else if (key == "ThreadCountUsage") {
			Variables[4] = value;
		} else if (key == "TickTime") {
			Variables[5] = value;
		} else if (key == "Debug") {
			Variables[6] = value;
		}
	}
	return Variables;
}