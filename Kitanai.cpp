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
void Token::setType(TokenType::Type type)
{
    this->type = type;
}
std::string Token::getValue()
{
    return this->value;
}
void Token::addParameter(Token token)
{
    this->parameters.push_back(token);
}
std::vector<Token> Token::getParameters()
{
    return parameters;
}
void Token::inspect(int depth)
{
    for (int i = 0; i < depth; i++) std::cout << "    ";
    std::cout << "- Token : " << TokenType::TypeDataR[type].first << ((value != "") ? "(" + value + ")" : "") << std::endl;
    for (Token& childToken : parameters) {
        childToken.inspect(depth + 1);
    }
}

InstructionBloc::InstructionBloc()
{

}
void InstructionBloc::addToken(Token token)
{
    this->instructions.push_back(token);
}
std::vector<Token>* InstructionBloc::getAllTokens()
{
    return &instructions;
}
void InstructionBloc::inspect(int depth)
{
    for (int i = 0; i < depth; i++) std::cout << "    ";
    std::cout << "Instruction Bloc" << std::endl;
    for (Token& rootToken : instructions) {
        rootToken.inspect(depth + 1);
    }
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
    std::vector<Token> instructionBuffer;
    if (useFile.is_open()) {
        std::cout << "Start Parsing File : " << path << std::endl;
        while (getline(useFile, currentLine)) {
            currentLine = replaceString(replaceString(currentLine, "	", "    "), "    ", "");
            if (currentLine != "") {
                std::cout << ">> " << currentLine << std::endl;
                ExtractionResult cuttedString = extractStrings(currentLine);
                for (std::string staticStr : std::get<0>(cuttedString)) {
                    staticStrings.push_back(staticStr);
                }
                for (std::pair<int, int> parseInst : std::get<2>(cuttedString)) {
                    if (parseInst.first == 0) {
                        instructionBuffer.push_back(Token(TokenType::StaticString, std::get<0>(cuttedString)[parseInst.second]));
                    }
                    else if (parseInst.first == 1) {
                        std::vector<Token> tokensFromCInst = stringToTokens(std::get<1>(cuttedString)[parseInst.second]);
                        instructionBuffer.insert(instructionBuffer.end(), tokensFromCInst.begin(), tokensFromCInst.end());
                    }
                }
            }      
        }
    }
    std::cout << "Buffering..." << std::endl;
    int currentInstructionIndex = 0;
    instructions.push_back(InstructionBloc());
    for (Token tok : instructionBuffer) {
        if (tok.getType() == TokenType::NewInstruction) {
        }
        else {
            instructions[currentInstructionIndex].addToken(tok);
        }
    }

    for (InstructionBloc& inst : instructions) {
        bool blocChange = true;
        std::cout << "Beginning instruction" << std::endl;
        std::vector<Token*> compressionStack;
        bool compressionStackOpened = false;
        int compressionStackIndex = 0;
        int stackType;
        while (blocChange) {
            //std::cout << "Bloc changed, restart" << std::endl;
            blocChange = false;
            int instIndex = 0;
            while (instIndex < inst.getAllTokens()->size()) {
                Token* tok = &inst.getAllTokens()->at(instIndex);
                std::cout << "Current Token : " << TokenType::TypeDataR[tok->getType()].first << std::endl;
                for (int it = 0; it < TokenType::TypeParameters.size(); it++) { //Type Parameters
                    if (tok->getType() == TokenType::TypeParameters[it][0] && instIndex + TokenType::TypeParameters[it].size() - 1 < inst.getAllTokens()->size()) {
                        //std::cout << "    @Type Parameters Inspection " << TokenType::TypeDataR[tok->getType()].first << std::endl;
                        int pindex = 1;
                        bool noAdd = true;
                        std::vector<Token> toAdd;
                        //std::cout << "VecSize " << TokenType::TypeParameters[it].size() << std::endl;
                        //std::cout << "InsVecSize " << inst.getAllTokens()->size() << "/" << instIndex << std::endl;
                        //std::cout << "Current Elem : " << TokenType::TypeDataR[inst.getAllTokens()->at(instIndex).getType()].first << " @" << instIndex << std::endl;
                        for (int iPara = 1; iPara < TokenType::TypeParameters[it].size(); iPara++) {
                            //std::cout << "Current iPara : " << iPara << std::endl;
                            //std::cout << "Current ndIndex " << instIndex + pindex << std::endl;
                            //std::cout << "        Expect : " << TokenType::TypeDataR[TokenType::TypeParameters[it][iPara]].first << std::endl;
                            //std::cout << "        Got : " << TokenType::TypeDataR[inst.getAllTokens()->at(instIndex + pindex).getType()].first << std::endl;
                            if (TokenType::TypeParameters[it][iPara] == inst.getAllTokens()->at(instIndex + pindex).getType()) {
                                //std::cout << "        Add Parameter : " << TokenType::TypeDataR[inst.getAllTokens()->at(instIndex + pindex).getType()].first << std::endl;
                                noAdd = false;
                                toAdd.push_back(inst.getAllTokens()->at(instIndex + pindex));
                                pindex++;
                            }
                            else {
                                noAdd = true;
                                break;
                            }
                        }
                        //std::cout << "EOL" << std::endl;
                        if (!noAdd) {
                            for (int i = 0; i < toAdd.size(); i++) {
                                //std::cout << "ADD : " << TokenType::TypeDataR[toAdd[i].getType()].first << std::endl; 
                                tok->addParameter(toAdd[i]);
                            }
                            inst.getAllTokens()->erase(inst.getAllTokens()->begin() + instIndex + 1, inst.getAllTokens()->begin() + instIndex + toAdd.size() + 1);
                            //std::cout << "BLOC CHANGED !" << std::endl;
                            blocChange = true;
                        }
                    }
                }
                if (compressionStackOpened) {
                    compressionStack.push_back(tok);
                }
                if (!blocChange) {
                    for (int i = 0; i < TokenType::TypeCompression.size(); i++) { //Type compression 
                        if (tok->getType() == TokenType::TypeCompression[i][0]) {
                            std::cout << "    Stack opened for : " << TokenType::TypeDataR[tok->getType()].first << std::endl;
                            compressionStackOpened = true;
                            compressionStackIndex = instIndex;
                            compressionStack.clear();
                            stackType = i;
                        }
                        if (tok->getType() == TokenType::TypeCompression[i][1] && i == stackType) {
                            compressionStack.pop_back();
                            (*inst.getAllTokens())[instIndex] = Token(TokenType::TypeCompression[i][2]);
                            tok = &inst.getAllTokens()->at(instIndex);
                            for (Token* cstok : compressionStack) {
                                std::cout << "      Add : " << TokenType::TypeDataR[cstok->getType()].first << std::endl;
                                tok->addParameter(*cstok);
                            }
                            std::cout << "    Stack closed for : " << TokenType::TypeDataR[tok->getType()].first << std::endl;
                            inst.getAllTokens()->erase(inst.getAllTokens()->begin() + compressionStackIndex, inst.getAllTokens()->begin() + instIndex);
                            compressionStackOpened = false;
                            compressionStackIndex = 0;
                            compressionStack.clear();
                            blocChange = true;
                            stackType = 0;
                        }
                        else if (tok->getType() == TokenType::TypeCompression[i][1] && i != stackType) {
                            std::cout << "ERROR STACK CROSSING : " << std::endl;
                        }
                    }
                }
                else {
                    compressionStackOpened = false;
                    compressionStack.clear();
                    compressionStackIndex = 0;
                }
                instIndex++;
            }
        }
    }
    for (InstructionBloc& inst : instructions) {
        inst.inspect();
    }
}
