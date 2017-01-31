#include "Kitanai.hpp"

int main(int argc, char** argv)
{
    StdLib::FunctionsName["func"] = StdLib::fRightPart("func", StdLib::f_func, 3);
	RunArgParser runParser(argv, argc);
	Program prog;
	if (argc == 2 || runParser.argumentExists("-f")) {
		if (runParser.argumentExists("-e")) Token::_execDebug = true;
		if (runParser.argumentExists("-c")) {
			Token::_compDebug = true;
			prog.compDebug = true;
		}
		if (runParser.argumentExists("-l")) {
			std::ofstream out(runParser.getArgumentValue("-l"));
			std::streambuf *coutbuf = std::cout.rdbuf();
			std::cout.rdbuf(out.rdbuf());
		}
		if (runParser.argumentExists("-a")) {
			Token::_dispMemAd = true;
		}
        if (runParser.argumentExists("-i")) {
            prog.import(runParser.getArgumentValue("-i"));
        }
		
		prog.parseFile((argc == 2) ? argv[1] : runParser.getArgumentValue("-f"));
		prog.exec();
	}

	if (prog.compDebug) std::cout << "Execution over.." << std::endl;
	std::cin.get();
    return 0;
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

	return allTokens;
}

bool Token::_execDebug = false;
bool Token::_compDebug = false;
bool Token::_dispMemAd = false;
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
    return "[<" + std::to_string(id) + "> " + this->getPrintableType() 
		+ ((this->getValue() != "") ? " (" + this->value + ")" : "") +
		((_dispMemAd) ? std::string(" @") + address.str() : std::string()) + "]";
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
	doChainAdoption();
    if (getType() == TokenType::Condition && prg->canExecute()) {
        parametersExecution[0].execute(prg);
    }
    else if (getType() == TokenType::Function && getValue() == "func") {
    }
    else
    {
        for (Token& tok : parametersExecution) {
            tok.execute(prg);
        }
    }
	if (getType() != TokenType::Null && parent != nullptr) {
		prg->setDepth(getDepth());
		if (prg->canExecute()) {
			if (_execDebug) std::cout << "[Exe] Execute Token : " << getSummary() << std::endl;
			if (getType() == TokenType::Function) {
				if (_execDebug) std::cout << "[Exe::Function] Call function : " << getValue()
					<< " (" << StdLib::FunctionsName[getValue()].second << " args)" << std::endl;
				if (_execDebug) std::cout << "[Exe::Function] Arg List : " << std::endl;
				for (Token fctArg : parametersExecution) {
					if (_execDebug) std::cout << "    [Exe::FunctionArg] : " << fctArg.getSummary() << std::endl;
				}
				Token returnValue = StdLib::FunctionsName[getValue()].first(parametersExecution);
				returnValue.doChainAdoption();
				returnValue.setParent(this);
				returnValue.execute(prg);
				if (_execDebug) std::cout << "[Exe::Function] " << getValue() << " returned : " << returnValue.getValue() << std::endl;
				this->type = returnValue.getType();
				this->value = returnValue.getValue();
			}
			else if (getType() == TokenType::StackAt) {
				if (getInstructionContent(parametersExecution).getType() == TokenType::String) {
					prg->setStackPosition(getInstructionContent(parametersExecution).getValue());
				}
				else if (getInstructionContent(parametersExecution).getType() == TokenType::Number) {
					prg->setStackPosition(std::stoi(getInstructionContent(parametersExecution).getValue()));
				}
				if (_execDebug) std::cout << "[Exe:StackAt] StackPosition is now at : $" << prg->getStackPosition().first << "+"
					<< prg->getStackPosition().second << std::endl;
				this->type = TokenType::Null;
				this->value = "";
			}
			else if (getType() == TokenType::Condition) {
				Token condition = getInstructionContent(parametersExecution);
				if (_execDebug) std::cout << "[Exe:Condition] Condition result : " << condition.getSummary() << std::endl;
				if (condition.getType() == TokenType::Number && condition.getValue() == "1") {
					if (_execDebug) std::cout << "[Exe::Condition] Condition is fulfilled (First Branch)" << std::endl;
					parametersExecution[1].execute(prg);
				}
				else if (parametersExecution.size() > 2 && condition.getType() == TokenType::Number && condition.getValue() == "0") {
					if (_execDebug) std::cout << "[Exe::Condition] Condition is not fulfilled (Second Branch)" << std::endl;
					parametersExecution[2].execute(prg);
				}
			}
			else if (getType() == TokenType::CurrentStackValue) {
				Token valueAt = prg->getStackAt();
				this->type = valueAt.getType();
				this->value = valueAt.getValue();
				if (_execDebug) std::cout << "[Exe:CurrentStackValue] : Current StackValue (at $" << prg->getStackPosition().first
					<< "+" << prg->getStackPosition().second << "): " << valueAt.getSummary() << std::endl;
			}
			else if (getType() == TokenType::StackAccess) {
				if (_execDebug) std::cout << "[Exe:StackAccess] : Store value : "
					<< getInstructionContent(parametersExecution).getSummary() << " at $"
					<< prg->getStackPosition().first << "+" << prg->getStackPosition().second << std::endl;

				prg->storeInStack(getInstructionContent(parametersExecution));
			}
			else if (getType() == TokenType::Instruction && getValue() != "Ignore") {
				this->type = getInstructionContent(parametersExecution).getType();
				this->value = getInstructionContent(parametersExecution).getValue();
			}
			else if (getType() == TokenType::Goto) {
				Token jumpDest = getInstructionContent(parametersExecution);
				if (jumpDest.getType() == TokenType::Origin) {
					prg->setSeekedFlag(prg->getNextOrigin());
					if (_execDebug) std::cout << "[Exe:Goto] : Goto Origin : " << prg->getNextOrigin() << std::endl;
					prg->stopExecution(TokenType::Origin);
				}
				else {
					if (_execDebug) std::cout << "[Exe:Goto] : Goto flag : "
						<< getInstructionContent(parametersExecution).getValue() << std::endl;
					if (_execDebug) std::cout << "[Exe::Goto] : Add Origin Marker : "
						<< getInstructionContent(parametersExecution, 1).getValue() << std::endl;
					prg->addOrigin(std::stoi(getInstructionContent(parametersExecution, 1).getValue()));
					prg->setSeekedFlag(std::stoi(getInstructionContent(parametersExecution).getValue()));
					prg->stopExecution(TokenType::Goto);
				}
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
			if (_execDebug) std::cout << "[Exe:Flag] Flag found : Restarting execution" << std::endl;
			prg->startExecution();
		}
		else if (TokenType::DynamicFlag == this->getType() && prg->getSeekedFlag() == std::stoi(this->getValue())) {
			if (_execDebug) std::cout << "[Exe:Flag] DynamicFlag found : Restarting execution" << std::endl;
			prg->startExecution();
			if (prg->getPauseCause() == TokenType::Origin)
				prg->removeOriginFlag(*this);
		}
	}
}
Token Token::getInstructionContent(std::vector<Token>& tokens, int index)
{
    int cIndex = 0;
    for (Token token : tokens) {
        if (token.getType() == TokenType::Instruction && token.getValue() == "Ignore") {
        }
        else if (token.getType() != TokenType::Null && cIndex == index) {
            return token;
        }
        else if (token.getType() != TokenType::Null) {
            cIndex++;
        }
    }
    return Token(TokenType::Null);
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
void Token::doChainAdoption()
{
	for (Token& token : parameters) {
		token.setParent(this);
		token.doChainAdoption();
	}
}
void Token::doAST()
{
    for (Token& token : parameters) {
        token.doAST();
    }
    int i = 0;
    while (i < parameters.size()) {
        Token& tok = parameters[i];
        if (_compDebug) std::cout << "    [Compilation:AST] Current Token : " << tok.getSummary() << std::endl;
        for (int it = 0; it < TokenType::TypeParameters.size(); it++) {
            int blocIndex = i + 1;
            if (tok.getType() == TokenType::TypeParameters[it][0]) {
				if (_compDebug) std::cout << "        [Compilation:AST:Rule] Trying to apply following rule : ";
                for (int ik = 0; ik < TokenType::TypeParameters[it].size(); ik++) {
					if (_compDebug) std::cout << TokenType::TypeDataR[TokenType::TypeParameters[it][ik]].first << ";";
                }
				if (_compDebug) std::cout << " (Size : " << TokenType::TypeParameters[it].size() << ")" << std::endl;
                bool noAdd = true;
                std::vector<Token> toAdd;
                for (int iPara = 1; iPara < TokenType::TypeParameters[it].size(); iPara++) {
					if (_compDebug) std::cout << "            [Compilation:AST:Rule] Expect : " 
						<< TokenType::TypeDataR[TokenType::TypeParameters[it][iPara]].first << std::endl;
					if (_compDebug) std::cout << "            [Compilation:AST:Rule] Got : " 
						<< parameters[blocIndex].getSummary() << std::endl;
                    if (TokenType::TypeParameters[it][iPara] == parameters[blocIndex].getType()) {
						if (_compDebug) std::cout << "                [Compilation:AST:Rule] Add Parameter : " 
							<< parameters[blocIndex].getSummary() << std::endl;
                        noAdd = false;
                        toAdd.push_back(parameters[blocIndex++]);
                    }
                    else {
                        noAdd = true;
                        break;
                    }
                }
                if (!noAdd) {
                    for (int i = 0; i < toAdd.size(); i++) {
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
        Token& tok = parameters[i];
        if (tok.getType() == TokenType::Function) {
            if (_compDebug) std::cout << "[Compilation:FunctionArgs] Searching args for Function : " 
				<< tok.getSummary() << std::endl;
            std::vector<int> indexToErase;
            for (int j = i + 1; j < StdLib::FunctionsName[tok.getValue()].second + i + 1; j++) {
				if (_compDebug) std::cout << "    [Compilation:FunctionsArgs:Parameter] : Adding parameter : " 
					<< parameters[j].getSummary() << std::endl;
                tok.addParameter(parameters[j]);
                indexToErase.push_back(j);
            }
            std::reverse(indexToErase.begin(), indexToErase.end());
            for (int j = 0; j < indexToErase.size(); j++) {
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
            if (getAtID(tokID)->getType() == TokenType::Goto && getAtID(tokID)->getParameters()->size() == 1 
				&& getAtID(tokID)->getParameters()->at(0).getType() == TokenType::Number) {
                if (_compDebug) std::cout << "    [Compilation:DynaFlag] Creating DynamicFlag for Goto Instruction : " 
					<< getAtID(tokID)->getSummary() << std::endl;
                this->insertAfter(tokID, Token(TokenType::DynamicFlag, std::to_string(++flagCounter)));
                getAtID(tokID)->addParameter(Token(TokenType::Number, std::to_string(flagCounter)));
            }
            else if (getAtID(tokID)->getType() == TokenType::Condition && getAtID(tokID)->getParameters()->size() == 3 
				&& getAtID(tokID)->getParameters()->at(1).getType() == TokenType::Instruction) {
				if (_compDebug) std::cout << "[Compilation:DynaFlag] Creating DynamicFlag for Condition Instruction : " 
					<< getAtID(tokID)->getSummary() << std::endl;
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
void Program::setStackPosition(std::string pos)
{
	stackPosition[stackPosition.size() - 1] = std::pair<std::string, int>(pos, 0);
}
void Program::setStackPosition(int pos)
{
	stackPosition[stackPosition.size() - 1] = std::pair<std::string, int>(stackPosition[stackPosition.size() - 1].first, pos);
}
Token Program::getStackAt()
{
    if (stack.find(stackPosition[stackPosition.size() - 1].first) == stack.end()) {
        std::cout << "[Error] : Unknown Stack Position : " << stackPosition[stackPosition.size() - 1].first << std::endl;
    }
    else if (stack[stackPosition[stackPosition.size() - 1].first].size() < stackPosition[stackPosition.size() - 1].second) {
        std::cout << "[Error] : Unknown SubStack Position : " << stackPosition[stackPosition.size() - 1].first << "@" << 
			stackPosition[stackPosition.size() - 1].first << std::endl;
    }
    else {
        return stack[stackPosition[stackPosition.size() - 1].first][stackPosition[stackPosition.size() - 1].second];
    }
}
void Program::storeInStack(Token token)
{
    while (stack[stackPosition[stackPosition.size() - 1].first].size() <= stackPosition[stackPosition.size() - 1].second) {
        stack[stackPosition[stackPosition.size() - 1].first].push_back(Token(TokenType::Null));
    }
    stack[stackPosition[stackPosition.size() - 1].first][stackPosition[stackPosition.size() - 1].second] = token;
}
void Program::parseFile(std::string path)
{
	if (compDebug) std::cout << "[Compilation] Start file parsing..." << std::endl;
    std::ifstream useFile;
    useFile.open(path);
    std::string currentLine;
    std::vector<Token> instructionBuffer;
    if (useFile.is_open()) {
        while (getline(useFile, currentLine)) {
            currentLine = replaceString(replaceString(currentLine, "	", "    "), "    ", "");
            if (currentLine != "") {
				if (compDebug) std::cout << "[Compilation:Parsing] Current line : " << currentLine << std::endl;
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
    int currentInstructionIndex = 0;
    for (Token tok : instructionBuffer) {
        instructions.addParameter(tok);
    }

    bool blocChange = true;
	if (compDebug) std::cout << "[Compilation] File parsing <Done> :: (1 / 6) tree built..." << std::endl;
	if (compDebug) instructions.inspect();
    std::vector<Token> compressionStack;
    bool compressionStackOpened = false;
    int compressionStackIndex = 0;
    int stackType;
	if (compDebug) std::cout << "[Compilation] Building Tree Organisation.." << std::endl;
    while (blocChange) {
        blocChange = false;
        int instIndex = 0;
        while (instIndex < instructions.getParameters()->size()) {
            Token& tok = instructions.getParameters()->at(instIndex);
            if (compressionStackOpened) {
                compressionStack.push_back(tok);
            }
            if (!blocChange) {
                for (int i = 0; i < TokenType::TypeCompression.size(); i++) { //Type compression 
                    if (tok.getType() == TokenType::TypeCompression[i][0]) {
						if (compDebug) std::cout << "[Compilation:Tree] Branch opened with : " << tok.getSummary() << std::endl;
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
void Program::import(std::string path)
{
    for (int i = 0; i < 2; i++) {
        this->parseFile(path);
        this->exec();
        this->instructions = Token(TokenType::Instruction);
        isProgramOver = false;
        execution = true;
    }
}
Token& Program::getInstructions()
{
    return instructions;
}
std::pair<std::string, int> Program::getStackPosition()
{
    return stackPosition[stackPosition.size() - 1];
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
