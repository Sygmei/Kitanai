#include <algorithm>
#include <iostream>
#include <sstream>

#include <Node.hpp>
#include <Program.hpp>
#include <StdLib.hpp>

namespace ktn
{
    bool Node::_execDebug = false;
    bool Node::_compDebug = false;
    bool Node::_dispMemAd = false;

    
    int Node::NodeAmount = 0;
    Node::Node(TokenType type)
    {
        this->type = type;
        this->id = NodeAmount++;
    }
    Node::Node(TokenType type, const std::string& value)
    {
        this->type = type;
        this->value = value;
        this->id = NodeAmount++;
    }
    
    void Node::setParent(Node* parent)
    {
        this->parent = parent;
        this->parentID = this->parent->getID();
    }
    int Node::getID() const
    {
        return this->id;
    }
    std::string Node::getSummary() const
    {
        std::ostringstream address;
        address << (void const *)this;
        return "[<" + std::to_string(id) + "> " + this->getPrintableType() 
            + ((this->getValue() != "") ? " (" + this->value + ")" : "") +
            ((_dispMemAd) ? std::string(" @") + address.str() : std::string()) + "]";
    }
    TokenType Node::getType() const
    {
        return this->type;
    }
    std::string Node::getPrintableType() const
    {
        return Token::FindByType(this->type).name;
    }
    void Node::setType(TokenType type)
    {
        this->type = type;
    }
    std::string Node::getValue() const
    {
        return this->value;
    }
    void Node::addParameter(Node token)
    {
        token.setParent(this);
        this->parameters.push_back(token);
    }
    std::vector<Node>* Node::getParameters()
    {
        return &parameters;
    }
    void Node::insertAfter(int id, Node token)
    {
        for (int i = 0; i < parameters.size(); i++) {
            if (parameters[i].getID() == id) {
                token.setParent(this);
                parameters.insert(parameters.begin() + i + 1, token);
                break;
            }
        }
    }
    Node* Node::getParent() const
    {
        return this->parent;
    }
    int Node::getParentID() const
    {
        return this->parentID;
    }
    void Node::inspect(int depth)
    {
        for (int i = 0; i < depth; i++) std::cout << "    ";
        std::cout << getSummary() << std::endl;
        for (Node childToken : parameters) {
            childToken.inspect(depth + 1);
        }
    }
    void Node::execute(Program* prg)
    {
        std::vector<Node> parametersExecution = parameters;
        doChainAdoption();
        if (getType() == TokenType::Condition && prg->canExecute()) {
            parametersExecution[0].execute(prg);
        }
        else if (getType() == TokenType::Function && getValue() == "func") {
        }
        else
        {
            for (Node& tok : parametersExecution) {
                tok.execute(prg);
            }
        }
        if (getType() != TokenType::Null && parent != nullptr) {
            prg->setDepth(getDepth());
            if (prg->canExecute()) {
                if (_execDebug) std::cout << "[Exe] Execute Node : " << getSummary() << std::endl;
                if (getType() == TokenType::Function) {
                    if (_execDebug) std::cout << "[Exe::Function] Call function : " << getValue()
                        << " (" << StdLib::FunctionsName[getValue()].second << " args)" << std::endl;
                    if (_execDebug) std::cout << "[Exe::Function] Arg List : " << std::endl;
                    for (Node fctArg : parametersExecution) {
                        if (_execDebug) std::cout << "    [Exe::FunctionArg] : " << fctArg.getSummary() << std::endl;
                    }
                    Node returnValue = StdLib::FunctionsName[getValue()].first(parametersExecution);
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
                    Node condition = getInstructionContent(parametersExecution);
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
                    Node valueAt = prg->getStackAt();
                    this->type = valueAt.getType();
                    this->value = valueAt.getValue();
                    if (_execDebug) std::cout << "[Exe:CurrentStackValue] : Current StackValue (at $" << prg->getStackPosition().first
                        << "+" << prg->getStackPosition().second << "): " << valueAt.getSummary() << std::endl;
                }
                else if (getType() == TokenType::StackSize) {
                    this->type = TokenType::Number;
                    this->value = std::to_string(prg->getStackSize());
                }
                else if (getType() == TokenType::StackLeft) {
                    prg->setStackPosition(prg->getStackPosition().second - 1);
                    this->type = TokenType::Null;
                    this->value = "";
                }
                else if (getType() == TokenType::StackRight) {
                    prg->setStackPosition(prg->getStackPosition().second + 1);
                    this->type = TokenType::Null;
                    this->value = "";
                }
                else if (getType() == TokenType::CurrentStackPosition) {
                    this->type = TokenType::String;
                    this->value = prg->getStackPosition().first;
                }
                else if (getType() == TokenType::CurrentSubStackPosition) {
                    this->type = TokenType::Number;
                    this->value = std::to_string(prg->getStackPosition().second);
                }
                else if (getType() == TokenType::StackAccess) {
                    if (_execDebug) std::cout << "[Exe:StackAccess] : Store value : "
                        << getInstructionContent(parametersExecution).getSummary() << " at $"
                        << prg->getStackPosition().first << "+" << prg->getStackPosition().second << std::endl;

                    prg->storeInStack(getInstructionContent(parametersExecution));
                    this->type = TokenType::Null;
                    this->value = "";
                }
                else if (getType() == TokenType::Instruction && getValue() != "Ignore") {
                    this->type = getInstructionContent(parametersExecution).getType();
                    this->value = getInstructionContent(parametersExecution).getValue();
                }
                else if (getType() == TokenType::Goto) {
                    Node jumpDest = getInstructionContent(parametersExecution);
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
                    this->type = TokenType::Null;
                    this->value = "";
                }
                else if (getType() == TokenType::ToggleExecution) {
                    prg->stopExecution(TokenType::ToggleExecution);
                    this->type = TokenType::Null;
                    this->value = "";
                }
                else if (getType() == TokenType::End) {
                    prg->stopProgram();
                    this->type = TokenType::Null;
                    this->value = "";
                }
            }
            else if (getType() == TokenType::ToggleExecution && prg->getPauseCause() == TokenType::ToggleExecution) {
                prg->startExecution();
                this->type = TokenType::Null;
                this->value = "";
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
    Node Node::getInstructionContent(std::vector<Node>& tokens, int index)
    {
        int cIndex = 0;
        for (Node token : tokens) {
            if (token.getType() == TokenType::Instruction && token.getValue() == "Ignore") {
            }
            else if (token.getType() != TokenType::Null && cIndex == index) {
                return token;
            }
            else if (token.getType() != TokenType::Null) {
                cIndex++;
            }
        }
        return Node(TokenType::Null);
    }
    Node* Node::getAtID(int id)
    {
        if (this->id == id) {
            return this;
        }
        else
        {
            for (Node& childToken : parameters) {
                if (childToken.getAtID(id) != nullptr) {
                    return childToken.getAtID(id);
                }
            }
            return nullptr;
        }
    }
    void Node::eraseNodeAtId(int id)
    {
        for (int i = 0; i < parameters.size(); i++) {
            if (parameters[i].getID() == id) {
                parameters.erase(parameters.begin() + i);
                break;
            }
        }
    }
    int Node::getDepth() const
    {
        int depth = 0;
        Node* tParent = getParent();
        while (tParent != nullptr) {
            depth++;
            tParent = tParent->getParent();
        }
        return depth;
    }
    void Node::doChainAdoption()
    {
        for (Node& token : parameters) {
            token.setParent(this);
            token.doChainAdoption();
        }
    }
    void Node::doAST()
    {
        for (Node& token : parameters) {
            token.doAST();
        }
        int i = 0;
        while (i < parameters.size()) {
            Node& tok = parameters[i];
            if (_compDebug) std::cout << "    [Compilation:AST] Current Node : " << tok.getSummary() << std::endl;
            for (int it = 0; it < TypeParameters.size(); it++) {
                int blocIndex = i + 1;
                if (tok.getType() == TypeParameters[it][0]) {
                    if (_compDebug) std::cout << "        [Compilation:AST:Rule] Trying to apply following rule : ";
                    for (int ik = 0; ik < TypeParameters[it].size(); ik++) {
                        if (_compDebug) std::cout << Token::FindByType(TypeParameters[it][ik]).name << ";";
                    }
                    if (_compDebug) std::cout << " (Size : " << TypeParameters[it].size() << ")" << std::endl;
                    bool noAdd = true;
                    std::vector<Node> toAdd;
                    for (int iPara = 1; iPara < TypeParameters[it].size(); iPara++) {
                        if (_compDebug) std::cout << "            [Compilation:AST:Rule] Expect : " 
                            << Token::FindByType(TypeParameters[it][iPara]).name << std::endl;
                        if (_compDebug) std::cout << "            [Compilation:AST:Rule] Got : " 
                            << parameters[blocIndex].getSummary() << std::endl;
                        if (TypeParameters[it][iPara] == parameters[blocIndex].getType()) {
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
                            this->eraseNodeAtId(toAdd[i].getID());
                        }
                        break;
                    }
                }
            }
            i++;
        }
    }
    void Node::doParametersAffectation() 
    {
        for (Node& tok : parameters) {
            tok.doParametersAffectation();
        }
        for (int i = 0; i < parameters.size(); i++) {
            Node& tok = parameters[i];
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
    void Node::doDynamicFlags(int& flagCounter)
    {
        for (int i = 0; i < parameters.size(); i++) {
            int tokID = parameters[i].getID();
            getAtID(tokID)->doDynamicFlags(flagCounter);
            if (getAtID(tokID)->getType() == TokenType::Goto || this->getAtID(tokID)->getType() == TokenType::Condition) {
                if (getAtID(tokID)->getType() == TokenType::Goto && getAtID(tokID)->getParameters()->size() == 1 
                    && getAtID(tokID)->getParameters()->at(0).getType() == TokenType::Number) {
                    if (_compDebug) std::cout << "    [Compilation:DynaFlag] Creating DynamicFlag for Goto Instruction : " 
                        << getAtID(tokID)->getSummary() << std::endl;
                    this->insertAfter(tokID, Node(TokenType::DynamicFlag, std::to_string(++flagCounter)));
                    getAtID(tokID)->addParameter(Node(TokenType::Number, std::to_string(flagCounter)));
                }
                else if (getAtID(tokID)->getType() == TokenType::Condition && getAtID(tokID)->getParameters()->size() == 3 
                    && getAtID(tokID)->getParameters()->at(1).getType() == TokenType::Instruction) {
                    if (_compDebug) std::cout << "[Compilation:DynaFlag] Creating DynamicFlag for Condition Instruction : " 
                        << getAtID(tokID)->getSummary() << std::endl;
                    this->insertAfter(tokID, Node(TokenType::DynamicFlag, std::to_string(++flagCounter)));
                    Node firstBranchJump = Node(TokenType::GotoNoOrigin);
                    firstBranchJump.addParameter(Node(TokenType::Number, std::to_string(flagCounter)));
                    Node& firstBranch = getAtID(tokID)->getParameters()->at(1);
                    firstBranch.addParameter(firstBranchJump);
                }
            }
        }
    }
}