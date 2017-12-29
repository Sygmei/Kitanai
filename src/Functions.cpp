#include "Functions.hpp"

RunArgParser::RunArgParser(char** start, int size)
{
	this->start = start;
	this->size = size;
}
bool RunArgParser::argumentExists(std::string arg)
{
	return std::find(start, start + size, arg) != (start + size);
}
std::string RunArgParser::getArgumentValue(std::string arg)
{
	char ** itr = std::find(start, start + size, arg);
	if (itr != (start + size) && ++itr != (start + size))
	{
		return std::string(*itr);
	}
	return "";
}

std::vector<std::string> split(const std::string &str, const std::string &delimiters)
{
	std::vector<std::string> tokens;
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	std::string::size_type pos = str.find_first_of(delimiters, lastPos);
	while (std::string::npos != pos || std::string::npos != lastPos) {
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		lastPos = str.find_first_not_of(delimiters, pos);
		pos = str.find_first_of(delimiters, lastPos);
	}
	return tokens;
}
std::string replaceString(std::string subject, const std::string& search, const std::string &replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return subject;
}
ExtractionResult extractStrings(std::string string)
{
	std::vector<std::string> strings;
	std::vector<std::string> others;
	std::vector<std::pair<int, int>> order;
	std::string stack = "";
	bool stackOpened = false;
	for (char cchar : string) {
		std::string currentChar = std::string(1, cchar);
		if (currentChar == "\"") {
			if (stack != "") {
				if (stackOpened) {
					order.push_back(std::pair<int, int>(0, strings.size()));
					strings.push_back(stack);
				}
				else {
					order.push_back(std::pair<int, int>(1, others.size()));
					others.push_back(stack);
				}
				stack = "";
			}
			stackOpened = !stackOpened;
		}
		else {
			stack = stack + currentChar;
		}
	}
	if (stack != "" && !stackOpened) {
		order.push_back(std::pair<int, int>(1, others.size()));
		others.push_back(stack);
	}
	std::tuple<std::vector<std::string>, std::vector<std::string>, std::vector<std::pair<int, int>>> result = std::make_tuple(strings, others, order);
	return result;
}
void removeCharFromString(std::string &str, std::string charToRemove)
{
	str.erase(std::remove(str.begin(), str.end(), charToRemove.c_str()[0]), str.end());
}
int occurencesInString(std::string str, std::string occur)
{
	int occurrences = 0;
	std::string::size_type start = 0;
	while ((start = str.find(occur, start)) != std::string::npos) {
		++occurrences;
		start += occur.length(); // see the note
	}
	return occurrences;
}
bool isStringInt(std::string str)
{
	if (str.size() > 0)
	{
		if (str.substr(0, 1) == "-")
			str = str.substr(1);
		return std::all_of(str.begin(), str.end(), ::isdigit);
	}
	else
		return false;
}
bool isStringFloat(std::string str)
{
	bool isFloat = false;
	if (str.size() > 0)
	{
		if (str.substr(0, 1) == "-")
			str = str.substr(1);
		if (occurencesInString(str, ".") == 1)
		{
			isFloat = true;
			removeCharFromString(str, ".");
		}
		return (std::all_of(str.begin(), str.end(), ::isdigit) && isFloat);
	}
	else
		return false;
}
bool isStringNumeric(std::string str)
{
	return (isStringFloat(str) || isStringInt(str));
}
