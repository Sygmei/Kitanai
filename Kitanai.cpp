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
std::vector<Token> stringToTokens(int& flagCounter, std::string string)
{
    std::vector<Token> allTokens;
    std::vector<std::string> splittedString = split(string, " ");
    std::string othStack = "";
    for (std::string cstring : splittedString) {
        if (othStack != "") {
            if (isStringNumeric(othStack)) {
                allTokens.push_back(Token(TokenType::Number, othStack));
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
                            allTokens.push_back(Token(TokenType::Number, othStack));
                        }
                        else {
                            allTokens.push_back(Token(TokenType::Function, othStack));
                        }
                        othStack = "";
                    }
                    if (it.second.second == TokenType::Flag) {
                        allTokens.push_back(Token(it.second.second, std::to_string(++flagCounter)));
                    }
                    else {
                        allTokens.push_back(Token(it.second.second));
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
            allTokens.push_back(Token(TokenType::Number, othStack));
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

    /*std::ofstream out("log.txt");
    std::streambuf *coutbuf = std::cout.rdbuf();
    std::cout.rdbuf(out.rdbuf());*/

    if (argc == 3 && argv[1] == std::string("-f"))
    {
        std::string currentFile = std::string(argv[2]);
        std::cout << "Current file : " << currentFile << std::endl;
        Program prog;
        prog.parseFile(currentFile);
        prog.exec();
    }
    else if (argc == 2) {
        std::string currentFile = std::string(argv[1]);
        std::cout << "Current file : " << currentFile << std::endl;
        Program prog;
        prog.parseFile(currentFile);
        prog.exec();
    }
    std::cout << "END" << std::endl;
    return 0;
}

//Token
int Token::TokenAmount = 0;
Token::Token(TokenType::Type type)
{
    this->type = type;
    this->id = TokenAmount++;
}
Token::Token(TokenType::Type type, std::string value)
{
    this->type = type;
    this->value = value;
    this->id = TokenAmount++;
}
void Token::setParent(Token* parent)
{
    std::cout << getSummary() << " now have parent : " << parent->getSummary() << std::endl;
    this->parent = parent;
    this->parentID = this->parent->getID();
}
int Token::getID()
{
    return this->id;
}
std::string Token::getSummary()
{
    std::ostringstream address;
    address << (void const *)this;
    return "[<" + std::to_string(id) + "> " + this->getPrintableType() + ((this->getValue() != "") ? " (" + this->value + ")" : "") + std::string(" @") + address.str() + "]";
}
TokenType::Type Token::getType() 
{
    return this->type;
}
std::string Token::getPrintableType()
{
    return TokenType::TypeDataR[this->getType()].first;
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
    token.setParent(this);
    this->parameters.push_back(token);
}
std::vector<Token>* Token::getParameters()
{
    return &parameters;
}
bool Token::operator==(Token& other)
{
    return (this->getType() == other.getType() && this->getValue() == other.getValue());
}
void Token::insertAfter(int id, Token token)
{
    for (int i = 0; i < parameters.size(); i++) {
        if (parameters[i].getID() == id) {
            token.setParent(this);
            parameters.insert(parameters.begin() + i + 1, token);
            break;
        }
    }
}
Token* Token::getParent()
{
    return this->parent;
}
int Token::getParentID()
{
    return this->parentID;
}
void Token::inspect(int depth)
{
    for (int i = 0; i < depth; i++) std::cout << "    ";
    std::cout << getSummary() << std::endl;
    for (Token childToken : parameters) {
        childToken.inspect(depth + 1);
    }
}
void Token::execute(Program* prg)
{
    std::vector<Token> parametersExecution = parameters;
    if (getType() != TokenType::Condition) {
        for (Token& tok : parametersExecution) {
            tok.execute(prg);
        }
    }
    else {
        parametersExecution[0].execute(prg);
    }
    if (prg->canExecute()) {
        //std::cout << "Execute Token : " << getSummary() << std::endl;
        if (getType() == TokenType::Function) {
            //std::cout << "[Exe::Function] Call function : " << getValue() << std::endl;
            Token returnValue = StdLib::FunctionsName[getValue()].first(parametersExecution);
            //std::cout << "[Exe::Function] " << getValue() << " returned : " << returnValue.getSummary() << std::endl;
            this->type = returnValue.getType();
            this->value = returnValue.getValue();
        }
        else if (getType() == TokenType::StackAt) {
            if (getInstructionContent(parametersExecution).getType() == TokenType::String) {
                prg->setStackPosition(getDepth(), getInstructionContent(parametersExecution).getValue());
            }
            else if (getInstructionContent(parametersExecution).getType() == TokenType::String) {
                prg->setStackPosition(getDepth(), std::stoi(getInstructionContent(parametersExecution).getValue()));
            }
            this->type = TokenType::Null;
            this->value = "";
        }
        else if (getType() == TokenType::Condition) {
            Token condition = getInstructionContent(parametersExecution);
            //std::cout << "[Exe:Condition] : Condition result : " << condition.getSummary() << std::endl;
            if (condition.getType() == TokenType::Number && condition.getValue() == "1") {
                //std::cout << "[Exe::Condition] : Condition is fulfilled (First Branch)" << std::endl;
                parametersExecution[1].execute(prg);
            }
            else if (parametersExecution.size() > 2 && condition.getType() == TokenType::Number && condition.getValue() == "0") {
                //std::cout << " (Second Branch)";
                parametersExecution[2].execute(prg);
                //std::cout << std::endl;
            }
        }
        else if (getType() == TokenType::CurrentStackValue) {
            //std::cout << "Reading Stack.." << std::endl;
            Token valueAt = prg->getStackAt(getDepth());
            this->type = valueAt.getType();
            this->value = valueAt.getValue();
            //std::cout << "[Exe:CurrentStackValue] : Returns current StackValue : " << getSummary() << std::endl;
        }
        else if (getType() == TokenType::StackAccess) {
            //std::cout << "[Exe:StackAccess] : Store value : " << getInstructionContent(parametersExecution).getSummary() << " at $" << prg->getStackIndex() << std::endl;

            prg->storeInStack(getDepth(), getInstructionContent(parametersExecution));
        }
        else if (getType() == TokenType::Instruction) {
            this->type = getInstructionContent(parametersExecution).getType();
            this->value = getInstructionContent(parametersExecution).getValue();
        }
        else if (getType() == TokenType::Goto) {
            Token jumpDest = getInstructionContent(parametersExecution);
            if (jumpDest.getType() == TokenType::Origin) {
                prg->setSeekedFlag(prg->getNextOrigin());
                //std::cout << "[Exe:Goto] : Goto Origin : " << prg->getNextOrigin() << std::endl;
            }
            else {
                //std::cout << "[Exe:Goto] : Goto flag : " << getInstructionContent(parametersExecution).getValue() << std::endl;
                //std::cout << "[Exe::Goto] : Add Origin Marker : " << getInstructionContent(parametersExecution, 1).getValue() << std::endl;
                prg->addOrigin(std::stoi(getInstructionContent(parametersExecution, 1).getValue()));
                prg->setSeekedFlag(std::stoi(getInstructionContent(parametersExecution).getValue()));
            }
            prg->stopExecution(TokenType::Goto);            
        }
        else if (getType() == TokenType::GotoNoOrigin) {
            prg->setSeekedFlag(std::stoi(getInstructionContent(parametersExecution).getValue()));
            prg->stopExecution(TokenType::Goto);
        }
        else if (getType() == TokenType::ToggleExecution) {
            prg->stopExecution(TokenType::ToggleExecution);
        }
        else if (getType() == TokenType::End) {
            prg->stopProgram();
        }
    }
    else if (getType() == TokenType::ToggleExecution && prg->getPauseCause() == TokenType::ToggleExecution) {
        prg->startExecution();
    }
    else if (TokenType::Flag == this->getType() && prg->getSeekedFlag() == std::stoi(this->getValue()) && prg->getPauseCause() == TokenType::Goto) {
        //std::cout << "[Exe:Flag] Flag found : Restarting execution" << std::endl;
        prg->startExecution();
    }
    else if (TokenType::DynamicFlag == this->getType() && prg->getSeekedFlag() == std::stoi(this->getValue()) && prg->getPauseCause() == TokenType::Goto) {
        //std::cout << "[Exe:Flag] DynamicFlag found : Restarting execution" << std::endl;
        prg->startExecution();
        prg->removeOriginFlag(*this);
    }
}
Token Token::getInstructionContent(std::vector<Token>& tokens, int index)
{
    int cIndex = 0;
    for (Token token : tokens) {
        if (token.getType() != TokenType::Null && cIndex == index) {
            return token;
        }
        else if (token.getType() != TokenType::Null) {
            cIndex++;
        }
    }
    return Token(TokenType::Null);
}
void Token::explore(std::vector<Token*>& list, int depth)
{
    if (getDepth() == depth) {
        list.push_back(this);
        std::cout << "Pushing : " << this->getSummary() << " to Crawler" << std::endl;
    }
    
    for (Token& childToken : parameters) {
        childToken.explore(list, depth);
    }
}
Token* Token::getAtID(int id)
{
    if (this->id == id) {
        return this;
    }
    else
    {
        for (Token& childToken : parameters) {
            if (childToken.getAtID(id) != nullptr) {
                return childToken.getAtID(id);
            }
        }
        return nullptr;
    }
}
void Token::eraseTokenAtID(int id)
{
    for (int i = 0; i < parameters.size(); i++) {
        if (parameters[i].getID() == id) {
            parameters.erase(parameters.begin() + i);
            break;
        }
    }
}
int Token::getDepth()
{
    int depth = 0;
    Token* tParent = getParent();
    while (tParent != nullptr) {
        depth++;
        tParent = tParent->getParent();
    }
    return depth;
}

//TokenCrawler
TokenCrawler::TokenCrawler(Token& tokens, Token* iterator)
{
    this->iterator = iterator;
    tokens.explore(tokenVector);
    index = 0;
}
void TokenCrawler::refresh(Token& tokens, int depth)
{
    tokenVector.clear();
    tokens.explore(tokenVector, depth);
}
void TokenCrawler::reset()
{
    index = 0;
}
bool TokenCrawler::hasNext()
{
    return tokenVector.size() > index;
}
Token* TokenCrawler::get()
{
    return tokenVector[index];
}
Token* TokenCrawler::getAtOffset(int offset)
{
    return tokenVector[index + offset];
}
bool TokenCrawler::next()
{
    if (index >= tokenVector.size()) {
        return false;
    }
    else {
        *this->iterator = *tokenVector[index++];
        return true;
    }
}
std::vector<Token*> TokenCrawler::getList()
{
    return tokenVector;
}
void Token::doSemantics()
{
    for (Token& token : parameters) {
        token.doSemantics();
    }
    std::cout << "Do semantics for : " << getSummary() << std::endl;
    int i = 0;
    while (i < parameters.size()) {
        Token& tok = parameters[i];
        std::cout << "  Current SubToken : " << tok.getSummary() << std::endl;
        for (int it = 0; it < TokenType::TypeParameters.size(); it++) {
            int blocIndex = i + 1;
            if (tok.getType() == TokenType::TypeParameters[it][0]) {
                std::cout << "Currently trying ParaSet :";
                for (int ik = 0; ik < TokenType::TypeParameters[it].size(); ik++) {
                    std::cout << TokenType::TypeDataR[TokenType::TypeParameters[it][ik]].first << ";";
                }
                std::cout << std::endl;
                std::cout << "    @Type Parameters Inspection " << TokenType::TypeDataR[tok.getType()].first << std::endl;
                bool noAdd = true;
                std::vector<Token> toAdd;
                std::cout << "ParaSize " << TokenType::TypeParameters[it].size() << std::endl;
                std::cout << "BlocSize " << blocIndex << "/" << tok.getParent()->getParameters()->size() << std::endl;
                std::cout << "Current Elem : " << tok.getSummary() << " @" << blocIndex << std::endl;
                for (int iPara = 1; iPara < TokenType::TypeParameters[it].size(); iPara++) {
                    std::cout << "Current iPara : " << iPara << std::endl;
                    std::cout << "Current blkIndex " << blocIndex << std::endl;
                    std::cout << "        Expect : " << TokenType::TypeDataR[TokenType::TypeParameters[it][iPara]].first << std::endl;
                    std::cout << "        Got : " << parameters[blocIndex].getSummary() << std::endl;
                    if (TokenType::TypeParameters[it][iPara] == parameters[blocIndex].getType()) {
                        std::cout << "        Add Parameter : " << parameters[blocIndex].getSummary() << std::endl;
                        noAdd = false;
                        toAdd.push_back(parameters[blocIndex++]);
                    }
                    else {
                        noAdd = true;
                        break;
                    }
                }
                std::cout << "EOL" << std::endl;
                if (!noAdd) {
                    if (tok.getType() == TokenType::Condition) {
                        std::cout << "BOUIT" << std::endl;
                    }
                    for (int i = 0; i < toAdd.size(); i++) {
                        std::cout << "ADD : " << toAdd[i].getSummary() << std::endl; 
                        tok.addParameter(toAdd[i]);
                        this->eraseTokenAtID(toAdd[i].getID());
                    }
                    break;
                }
            }
        }

        i++;
    }
}
void Token::doParametersAffectation() 
{
    for (Token& tok : parameters) {
        tok.doParametersAffectation();
    }
    for (int i = 0; i < parameters.size(); i++) {
        std::cout << "L00p" << std::endl;
        Token& tok = parameters[i];
        if (tok.getType() == TokenType::Function) {
            std::cout << "Parametrics of : " << tok.getSummary() << std::endl;
            std::vector<int> indexToErase;
            for (int j = i + 1; j < StdLib::FunctionsName[tok.getValue()].second + i + 1; j++) {
                std::cout << "[ADDING PARAMETER] : " << parameters[j].getPrintableType() << std::endl;
                //parameters[j].doParametersAffectation();
                tok.addParameter(parameters[j]);
                indexToErase.push_back(j);
                std::cout << "Closure" << std::endl;
            }
            std::reverse(indexToErase.begin(), indexToErase.end());
            tok.inspect();
            for (int j = 0; j < indexToErase.size(); j++) {
                std::cout << "Erase : " << indexToErase[j] << " of size" << parameters.size() << "?" << j << std::endl;
                parameters.erase(parameters.begin() + indexToErase[j]);
            }
        }
    }
}
void Token::doDynamicFlags(int& flagCounter)
{
    for (int i = 0; i < parameters.size(); i++) {
        int tokID = parameters[i].getID();
        getAtID(tokID)->doDynamicFlags(flagCounter);
        if (getAtID(tokID)->getType() == TokenType::Goto || this->getAtID(tokID)->getType() == TokenType::Condition) {
            getAtID(tokID)->inspect();
            std::cout << "Dynamic Flagging : " << getAtID(tokID)->getSummary() << std::endl;
            std::cout << "<" << i + 1 << " / " << getAtID(tokID)->getParameters()->size() - 1 << "> ";
            if (i + 1 < parameters.size()) {
                std::cout << "GetNextParameter : " << parameters[i + 1].getSummary();
            }
            std::cout << std::endl;
            if (getAtID(tokID)->getType() == TokenType::Goto && getAtID(tokID)->getParameters()->size() == 1 &&getAtID(tokID)->getParameters()->at(0).getType() == TokenType::Number) {
                std::cout << "[Compilation] : Creating DynamicFlag for Goto Instruction" << std::endl;
                std::cout << "CCurrent : " << getAtID(tokID)->getSummary() << std::endl;
                this->insertAfter(tokID, Token(TokenType::DynamicFlag, std::to_string(++flagCounter)));
                std::cout << "Add Parameter to : " << getAtID(tokID)->getSummary() << std::endl;
                getAtID(tokID)->addParameter(Token(TokenType::Number, std::to_string(flagCounter)));
            }
            else if (getAtID(tokID)->getType() == TokenType::Condition && getAtID(tokID)->getParameters()->size() == 3 && getAtID(tokID)->getParameters()->at(1).getType() == TokenType::Instruction) {
                std::cout << "[Compilation] : Creating DynamicFlag for Condition Instruction" << std::endl;
                this->insertAfter(tokID, Token(TokenType::DynamicFlag, std::to_string(++flagCounter)));
                Token firstBranchJump = Token(TokenType::GotoNoOrigin);
                firstBranchJump.addParameter(Token(TokenType::Number, std::to_string(flagCounter)));
                Token& firstBranch = getAtID(tokID)->getParameters()->at(1);
                firstBranch.addParameter(firstBranchJump);
            }
        }
    }
}


//Program
Program::Program()
{

}
void Program::addOrigin(int flagNumber)
{
    origins.push_back(flagNumber);
}
int Program::getNextOrigin()
{
    return origins.back();
}
void Program::removeOriginFlag(Token& flag)
{
    if (flag.getType() == TokenType::DynamicFlag && std::stoi(flag.getValue()) == origins.back()) {
        origins.pop_back();
    }
}
void Program::stopExecution(TokenType::Type cause)
{
    pauseCause = cause;
    execution = false;
}
void Program::startExecution()
{
    execution = true;
}
bool Program::canExecute()
{
    return execution;
}
void Program::setSeekedFlag(int flag)
{
    flagSeeked = flag;
}
int Program::getSeekedFlag()
{
    return flagSeeked;
}
void Program::setStackPosition(int depth, std::string pos)
{
    //std::cout << "[Exe] StackPosition switched to : " << pos << std::endl;
    if (depth == stackPosition.size() - 1) {
        stackPosition[stackPosition.size() - 1] = std::pair<std::string, int>(pos, 0);
    }
    else if (depth >= stackPosition.size()) {
        //Follow
    }
    stackPosition = pos;
}
void Program::setStackPosition(int depth, int pos)
{
    //std::cout << "[Exe] SubStackPosition switched to : " << pos << std::endl;
    subStackPosition = pos;
}
Token Program::getStackAt(int depth)
{
    /*std::cout << "Reading Position : " << stackPosition << " / " << subStackPosition << std::endl;
    std::cout << "Stack returned : " << stack[stackPosition][subStackPosition].getSummary() << std::endl;*/
    if (stack.find(stackPosition) == stack.end()) {
        std::cout << "[Error] : Unknown Stack Position : " << stackPosition << std::endl;
    }
    else if (stack[stackPosition].size() < subStackPosition) {
        std::cout << "[Error] : Unknown SubStack Position : " << subStackPosition << std::endl;
    }
    else {
        return stack[stackPosition][subStackPosition];
    }
}
void Program::storeInStack(Token token)
{
    while (stack[stackPosition].size() <= subStackPosition) {
        stack[stackPosition].push_back(Token(TokenType::Null));
    }
    stack[stackPosition][subStackPosition] = token;
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
                        Token newToken(TokenType::String, std::get<0>(cuttedString)[parseInst.second]);
                        newToken.setParent(&instructions);
                        instructionBuffer.push_back(newToken);
                    }
                    else if (parseInst.first == 1) {
                        std::vector<Token> tokensFromCInst = stringToTokens(flagCounter, std::get<1>(cuttedString)[parseInst.second]);
                        instructionBuffer.insert(instructionBuffer.end(), tokensFromCInst.begin(), tokensFromCInst.end());
                    }
                }
            }      
        }
    }
    std::cout << "Buffering..." << std::endl;
    int currentInstructionIndex = 0;
    for (Token tok : instructionBuffer) {
        instructions.addParameter(tok);
    }

    bool blocChange = true;
    instructions.inspect();
    std::cout << "Beginning Compression" << std::endl;
    std::vector<Token> compressionStack;
    bool compressionStackOpened = false;
    int compressionStackIndex = 0;
    int stackType;
    while (blocChange) {
        std::cout << "Bloc iteration" << std::endl;
        blocChange = false;
        int instIndex = 0;
        while (instIndex < instructions.getParameters()->size()) {
            Token& tok = instructions.getParameters()->at(instIndex);
            if (compressionStackOpened) {
                compressionStack.push_back(tok);
            }
            if (!blocChange) {
                std::cout << "NoBC" << std::endl;
                for (int i = 0; i < TokenType::TypeCompression.size(); i++) { //Type compression 
                    if (tok.getType() == TokenType::TypeCompression[i][0]) {
                        std::cout << "    Stack opened for : " << TokenType::TypeDataR[tok.getType()].first << std::endl;
                        compressionStackOpened = true;
                        compressionStackIndex = instIndex;
                        compressionStack.clear();
                        stackType = i;
                    }
                    if (tok.getType() == TokenType::TypeCompression[i][1] && i == stackType) {
                        compressionStack.pop_back();
                        Token newToken(TokenType::TypeCompression[i][2]);
                        newToken.setParent(&instructions);
                        (*instructions.getParameters())[instIndex] = newToken;
                        tok = instructions.getParameters()->at(instIndex);
                        for (Token cstok : compressionStack) {
                            std::cout << "      Add : " << TokenType::TypeDataR[cstok.getType()].first << std::endl;
                            tok.addParameter(cstok);
                        }
                        std::cout << "    Stack closed for : " << TokenType::TypeDataR[tok.getType()].first << std::endl;
                        instructions.getParameters()->erase(instructions.getParameters()->begin() + compressionStackIndex, instructions.getParameters()->begin() + instIndex);
                        compressionStackOpened = false;
                        compressionStackIndex = 0;
                        compressionStack.clear();
                        blocChange = true;
                        stackType = 0;
                    }
                    else if (tok.getType() == TokenType::TypeCompression[i][1] && i != stackType) {
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
    instructions.inspect();
    blocChange = true;
    
    std::cout << "Start Instruction Emb" << std::endl;
    std::cout << "[Compilation] Creating Semantic Tree" << std::endl;
    instructions.doSemantics();
    
    
    std::cout << "End of Imb" << std::endl;
    instructions.inspect();
    //Function Imbrication
    std::cout << "[Compilation] Affecting Functions Parameters" << std::endl;
    instructions.doParametersAffectation();

    //Dynamic Flag
    instructions.inspect();
    std::cout << "[Compilation] Creating DynamicFlags" << std::endl;
    instructions.doDynamicFlags(flagCounter);
    std::cout << "Crawled" << std::endl;
}
void Program::exec()
{
    instructions.inspect();
    while (!isProgramOver) {
        instructions.execute(this);
    }
}
std::string Program::getStackIndex()
{
    return stackPosition;
}
void Program::stopProgram()
{
    execution = false;
    isProgramOver = true;
}
TokenType::Type Program::getPauseCause()
{
    return pauseCause;
}
