/*------------------------------ ConfigParser.h -----------------------------*/

#ifndef CONFIGPARSER_H_
#define CONFIGPARSER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <map>

using namespace std;

class ConfigParser {
public:
	ConfigParser(char* file_name);
	map<string, string> GetParamSet();
	string GetValue(string param);


private:
	map<string, string> param_set;
	void ParseLine(string line, char delimiter);
	bool IsComment(string line, string delimiter);
	bool IsValidParam(string param);
};

#endif
