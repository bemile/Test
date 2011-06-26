/*
 * ConfigParser.cpp
 *
 *  Created on: Jun 25, 2011
 *      Author: jie
 */

#include "ConfigParser.h"

ConfigParser::ConfigParser(char* file_name) {
	string line;
	ifstream fs(file_name);
	if (fs.is_open()) {
		while (fs.good()) {
			getline(fs, line);
			ParseLine(line, '=');
			cout << line << endl;
		}
		fs.close();
	}
	else {
		cout << "Cannot open config file to read." << endl;
	}

}


void ConfigParser::ParseLine(string line, char delimiter) {
	if (!IsComment(line, "#")) {
		int length = line.length();
		int index = line.find(delimiter, 0);
		if (index > 0) {
			string param = line.substr(0, index);
			string value = line.substr(index + 1, length - 1 - index);
			if (IsValidParam(param)) {
				param_set.insert(pair<string, string>(param, value));
			}
		}
	}
}


bool ConfigParser::IsComment(string line, string delimiter) {
	if (line.find(delimiter, 0) == 0)
		return true;
	else
		return false;
}


bool ConfigParser::IsValidParam(string param) {
	return true;
}


map<string, string> ConfigParser::GetParamSet() {
	return param_set;
}

string ConfigParser::GetValue(char* param) {
	return param_set[param];
}


