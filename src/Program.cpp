#include <fstream>
#include <iostream>

#include <Functions.hpp>
#include <Program.hpp>

namespace ktn
{
    std::vector<Node> stringToTokens(int& flagCounter, std::string string)
    {
        std::vector<Node> allTokens;
        std::vector<std::string> splittedString = split(string, " ");
        std::string othStack = "";
        for (std::string cstring : splittedString) {
            if (othStack != "") {
                if (isStringNumeric(othStack)) {
                    allTokens.push_back(Node(TokenType::Number, othStack));
                }
                else {
                    allTokens.push_back(Node(TokenType::Function, othStack));
                }
                othStack = "";
            }
            for (char cchar : cstring) {
                std::string currentChar = std::string(1, cchar);
                bool tokenFound = false;
                for (const Token& token : Token::Tokens) {
                    if (token.symbol != "" && currentChar == token.symbol) {
                        if (othStack != "") {
                            if (isStringNumeric(othStack)) {
                                allTokens.push_back(Node(TokenType::Number, othStack));
                            }
                            else {
                                allTokens.push_back(Node(TokenType::Function, othStack));
                            }
                            othStack = "";
                        }
                        if (token.type == TokenType::Flag) {
                            allTokens.push_back(Node(token.type, std::to_string(++flagCounter)));
                        }
                        else {
                            allTokens.push_back(Node(token.type));
                        }
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
                allTokens.push_back(Node(TokenType::Number, othStack));
            }
            else {
                allTokens.push_back(Node(TokenType::Function, othStack));
            }
            othStack = "";
        }

        return allTokens;
    }

    //Program
    Program::Program()
    {

    }
    void Program::addOrigin(int flagNumber)
    {
        origins.push_back(flagNumber);
    }
    int Program::getNextOrigin() const
    {
        return origins.back();
    }
    void Program::removeOriginFlag(Node& flag)
    {
        if (flag.getType() == TokenType::DynamicFlag && std::stoi(flag.getValue()) == origins.back()) {
            origins.pop_back();
        }
    }
    void Program::stopExecution(TokenType cause)
    {
        pauseCause = cause;
        execution = false;
    }
    void Program::startExecution()
    {
        execution = true;
    }
    bool Program::canExecute() const
    {
        return execution;
    }
    void Program::setSeekedFlag(int flag)
    {
        flagSeeked = flag;
    }
    int Program::getSeekedFlag() const
    {
        return flagSeeked;
    }
    void Program::setDepth(int depth)
    {
        if (stackPosition.size() == 0) {
            stackPosition.push_back(std::pair<std::string, int >("", 0));
        }
        while (depth > stackPosition.size()) {
            stackPosition.push_back(stackPosition[stackPosition.size() - 1]);
        }
        while (depth < stackPosition.size()) {
            stackPosition.pop_back();
        }
    }
    void Program::setStackPosition(const std::string& pos)
    {
        stackPosition[stackPosition.size() - 1] = std::pair<std::string, int>(pos, 0);
    }
    void Program::setStackPosition(int pos)
    {
        stackPosition[stackPosition.size() - 1] = std::pair<std::string, int>(stackPosition[stackPosition.size() - 1].first, pos);
    }
    int Program::getStackSize() const
    {
        if (stack.find(stackPosition[stackPosition.size() - 1].first) == stack.end()) {
            std::cout << "[Error] : Unknown Stack Position : " << stackPosition[stackPosition.size() - 1].first << std::endl;
        }
        else {
            return stack.at(stackPosition[stackPosition.size() - 1].first).size();
        }
    }
    Node Program::getStackAt() const
    {
        if (stack.find(stackPosition[stackPosition.size() - 1].first) == stack.end()) {
            std::cout << "[Error] : Unknown Stack Position : " << stackPosition[stackPosition.size() - 1].first << std::endl;
        }
        else if (stack.at(stackPosition[stackPosition.size() - 1].first).size() < stackPosition[stackPosition.size() - 1].second) {
            std::cout << "[Error] : Unknown SubStack Position : " << stackPosition[stackPosition.size() - 1].first << "@" << 
                stackPosition[stackPosition.size() - 1].first << std::endl;
        }
        else {
            return stack.at(stackPosition[stackPosition.size() - 1].first)[stackPosition[stackPosition.size() - 1].second];
        }
    }
    void Program::storeInStack(Node token)
    {
        while (stack[stackPosition[stackPosition.size() - 1].first].size() <= stackPosition[stackPosition.size() - 1].second) {
            stack[stackPosition[stackPosition.size() - 1].first].push_back(Node(TokenType::Null));
        }
        stack[stackPosition[stackPosition.size() - 1].first][stackPosition[stackPosition.size() - 1].second] = token;
    }
    void Program::parseFile(const std::string& path)
    {
        if (compDebug) std::cout << "[Compilation] Start file parsing..." << std::endl;
        std::ifstream useFile;
        useFile.open(path);
        std::string currentLine;
        std::vector<Node> instructionBuffer;
        if (useFile.is_open()) {
            while (getline(useFile, currentLine)) {
                currentLine = replaceString(replaceString(currentLine, "	", "    "), "    ", "");
                if (currentLine != "") {
                    if (compDebug) std::cout << "[Compilation:Parsing] Current line : " << currentLine << std::endl;
                    ExtractionResult cuttedString = extractStrings(currentLine);
                    for (std::pair<int, int> parseInst : std::get<2>(cuttedString)) {
                        if (parseInst.first == 0) {
                            Node newToken(TokenType::String, std::get<0>(cuttedString)[parseInst.second]);
                            newToken.setParent(&instructions);
                            instructionBuffer.push_back(newToken);
                        }
                        else if (parseInst.first == 1) {
                            std::vector<Node> tokensFromCInst = stringToTokens(flagCounter, std::get<1>(cuttedString)[parseInst.second]);
                            instructionBuffer.insert(instructionBuffer.end(), tokensFromCInst.begin(), tokensFromCInst.end());
                        }
                    }
                }      
            }
        }
        int currentInstructionIndex = 0;
        for (Node tok : instructionBuffer) {
            instructions.addParameter(tok);
        }

        bool blocChange = true;
        if (compDebug) std::cout << "[Compilation] File parsing <Done> :: (1 / 6) tree built..." << std::endl;
        if (compDebug) instructions.inspect();
        std::vector<Node> compressionStack;
        bool compressionStackOpened = false;
        int compressionStackIndex = 0;
        int stackType;
        if (compDebug) std::cout << "[Compilation] Building Tree Organisation.." << std::endl;
        while (blocChange) {
            blocChange = false;
            int instIndex = 0;
            while (instIndex < instructions.getParameters()->size()) {
                Node& tok = instructions.getParameters()->at(instIndex);
                if (compressionStackOpened) {
                    compressionStack.push_back(tok);
                }
                if (!blocChange) {
                    for (int i = 0; i < TypeCompression.size(); i++) { //Type compression 
                        if (tok.getType() == TypeCompression[i][0]) {
                            if (compDebug) std::cout << "[Compilation:Tree] Branch opened with : " << tok.getSummary() << std::endl;
                            compressionStackOpened = true;
                            compressionStackIndex = instIndex;
                            compressionStack.clear();
                            stackType = i;
                        }
                        if (tok.getType() == TypeCompression[i][1] && i == stackType) {
                            compressionStack.pop_back();
                            Node newToken(TypeCompression[i][2]);
                            newToken.setParent(&instructions);
                            (*instructions.getParameters())[instIndex] = newToken;
                            tok = instructions.getParameters()->at(instIndex);
                            for (Node cstok : compressionStack) {
                                if (compDebug) std::cout << "      [Compilation:Tree:Branch] Add : " 
                                    << cstok.getSummary() << " to current branch (" << newToken.getSummary() << ")" << std::endl;
                                tok.addParameter(cstok);
                            }
                            if (compDebug) std::cout << "[Compilation:Tree] Branch closed with : " << tok.getSummary() << std::endl;
                            instructions.getParameters()->erase(instructions.getParameters()->begin() + compressionStackIndex, instructions.getParameters()->begin() + instIndex);
                            compressionStackOpened = false;
                            compressionStackIndex = 0;
                            compressionStack.clear();
                            blocChange = true;
                            stackType = 0;
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
        if (compDebug) std::cout << "[Compilation] Tree organisation <Done> :: (2 / 6) tree built..." << std::endl;
        if (compDebug)instructions.inspect();
        blocChange = true;
        
        if (compDebug) std::cout << "[Compilation] Creating AST" << std::endl;
        instructions.doAST();
        if (compDebug) std::cout << "[Compilation] Creating AST <Done> :: (3 / 6) tree built..." << std::endl;
        if (compDebug)instructions.inspect();
        
        //Function Imbrication
        if (compDebug) std::cout << "[Compilation] Affecting Functions Parameters" << std::endl;
        instructions.doParametersAffectation();
        if (compDebug) std::cout << "[Compilation] Affecting Functions Parameters <Done> :: (4 / 6) tree built..." << std::endl;
        if (compDebug)instructions.inspect();

        //Dynamic Flag
        
        if (compDebug) std::cout << "[Compilation] Creating DynamicFlags" << std::endl;
        instructions.doDynamicFlags(flagCounter);
        if (compDebug) std::cout << "[Compilation] Creating DynamicFlags <Done> :: (5 / 6) tree built..." << std::endl;
        if (compDebug)instructions.inspect();

        if (compDebug) std::cout << "[Compilation] Rebuilding Parent Links" << std::endl;
        instructions.doChainAdoption();
        if (compDebug) std::cout << "[Compilation] Rebuilding Parent Links <Done> :: (6 / 6) tree built..." << std::endl;
        if (compDebug)instructions.inspect();

        if (compDebug) std::cout << "[Compilation] Compilation is over ! <Success>" << std::endl;
    }
    void Program::exec()
    {
        while (!isProgramOver) {
            instructions.execute(this);
        }
    }
    void Program::import(const std::string& path)
    {
        for (int i = 0; i < 2; i++) {
            this->parseFile(path);
            this->exec();
            this->instructions = Node(TokenType::Instruction);
            isProgramOver = false;
            execution = true;
        }
    }
    Node& Program::getInstructions()
    {
        return instructions;
    }
    std::pair<std::string, int> Program::getStackPosition() const
    {
        return stackPosition[stackPosition.size() - 1];
    }
    void Program::stopProgram()
    {
        execution = false;
        isProgramOver = true;
    }
    TokenType Program::getPauseCause() const
    {
        return pauseCause;
    }
}