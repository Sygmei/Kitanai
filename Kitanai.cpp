#include "NoNameLang.hpp"

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

std::string join(std::vector<std::string>& vector, std::string sep, int start, int end)
{
    std::string result = "";
    if (end >= vector.size())
        end = vector.size();
    if (start >= vector.size() - 1)
        start = vector.size() - 1;
    for (int i = start; i < vector.size() - end; i++) {
        if (i != vector.size() - 1)
            result += vector[i] + sep;
        else
            result += vector[i];
    }
    return result;
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
	str.erase(remove(str.begin(), str.end(), charToRemove.c_str()[0]), str.end());
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

std::vector<Token> stringToTokens(std::string string)
{
    std::vector<Token> allTokens;
    std::vector<std::string> splittedString = split(string, " ");
    std::string othStack = "";
    for (std::string cstring : splittedString) {
        if (othStack != "") {
            if (isStringNumeric(othStack)) {
                allTokens.push_back(Token(TokenType::StaticNumber, othStack));
            }
            else {
                allTokens.push_back(Token(TokenType::Function, othStack));
            }
            othStack = "";
        }
        for (char cchar : cstring) {
            std::string currentChar = std::string(1, cchar);
            bool tokenFound = false;
            for (std::pair<std::string, std::pair<std::string, TokenType::Type>> it : TokenType::TypeDataK) {
                if (currentChar == it.first) {
                    if (othStack != "") {
                        if (isStringNumeric(othStack)) {
                            allTokens.push_back(Token(TokenType::StaticNumber, othStack));
                        }
                        else {
                            allTokens.push_back(Token(TokenType::Function, othStack));
                        }
                        othStack = "";
                    }
                    allTokens.push_back(Token(it.second.second));
                    tokenFound = true;
                    break;
                }
            }
            if (!tokenFound) {
                othStack += currentChar;
            }
        }
    }
    if (othStack != "") {
        if (isStringNumeric(othStack)) {
            allTokens.push_back(Token(TokenType::StaticNumber, othStack));
        }
        else {
            allTokens.push_back(Token(TokenType::Function, othStack));
        }
        othStack = "";
    }
    for (Token ctoken : allTokens) {
        std::cout << TokenType::TypeDataR[ctoken.getType()].first << ((ctoken.getValue() != "") ? "(" + ctoken.getValue() + ")" : "") << std::string(" / ");
    }
    std::cout << std::endl;

    return allTokens;
}

int main(int argc, char** argv)
{
    std::cout << "NoNameLang v0.0.1" << std::endl;

    if (argc == 3 && argv[1] == std::string("-f"))
    {
        std::string currentFile = std::string(argv[2]);
        std::cout << "Current file : " << currentFile << std::endl;
        Program prog;
        prog.parseFile(currentFile);
    }
    std::cout << "END" << std::endl;
    return 0;
}

//Token
Token::Token(TokenType::Type type)
{
    this->type = type;
}
Token::Token(TokenType::Type type, std::string value)
{
    this->type = type;
    this->value = value;
}
TokenType::Type Token::getType() 
{
    return this->type;
}
std::string Token::getValue()
{
    return this->value;
}

//Program
Program::Program()
{

}
void Program::parseFile(std::string path)
{
    std::ifstream useFile;
    useFile.open(path);
    std::string currentLine;
    if (useFile.is_open()) {
        std::cout << "Start Parsing File : " << path << std::endl;
        while (getline(useFile, currentLine)) {
            std::vector<Token> currentInstruction;
            currentLine = replaceString(replaceString(currentLine, "	", "    "), "    ", "");
            if (currentLine != "") {
                std::cout << ">> " << currentLine << std::endl;
                ExtractionResult cuttedString = extractStrings(currentLine);
                for (std::string staticStr : std::get<0>(cuttedString)) {
                    staticStrings.push_back(staticStr);
                }
                for (std::pair<int, int> parseInst : std::get<2>(cuttedString)) {
                    if (parseInst.first == 0) {
                        currentInstruction.push_back(Token(TokenType::StaticString));
                    }
                    else if (parseInst.first == 1) {
                        stringToTokens(std::get<1>(cuttedString)[parseInst.second]);
                    }
                }
            }      
        }
    }
}
