#pragma once

#include <algorithm>
#include <map>
#include <string>
#include <vector>

class RunArgParser
{
	private:
		char** start;
		int size;
	public:
		RunArgParser(char** start, int size);
		bool argumentExists(std::string arg);
		std::string getArgumentValue(std::string arg);
};

std::vector<std::string> split(const std::string &str, const std::string &delimiters = " ");
std::string replaceString(std::string subject, const std::string& search, const std::string &replace);
typedef std::tuple<std::vector<std::string>, std::vector<std::string>, std::vector<std::pair<int, int>>> ExtractionResult;
ExtractionResult extractStrings(std::string string);
void removeCharFromString(std::string &str, std::string charToRemove);
int occurencesInString(std::string str, std::string occur);
bool isStringNumeric(std::string str);
bool isStringInt(std::string str);
bool isStringFloat(std::string str);