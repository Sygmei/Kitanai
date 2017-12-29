#include <chrono>
#include <cmath>
#include <iostream>
#include <fstream>
#include <functional>
#include <random>

#include <Functions.hpp>
#include <Node.hpp>
#include <StdLib.hpp>
#include <Token.hpp>

namespace ktn
{
    namespace StdLib
    {
        Node f_add(const std::vector<Node> tokens) {
            Node a = tokens[0];
            Node b = tokens[1];
            if (a.getType() == TokenType::Number && b.getType() == TokenType::Number)
                return Node(TokenType::Number, std::to_string(std::stoll(a.getValue()) + std::stoll(b.getValue())));
            else {
                return Node(TokenType::String, a.getValue() + b.getValue());
            }
        }
        Node f_sub(const std::vector<Node> tokens) {
            Node a = tokens[0];
            Node b = tokens[1];
            if (a.getType() == TokenType::Number && b.getType() == TokenType::Number)
                return Node(TokenType::Number, std::to_string(std::stoll(a.getValue()) - std::stoll(b.getValue())));
        }
        Node f_mul(const std::vector<Node> tokens) {
            Node a = tokens[0];
            Node b = tokens[1];
            if (a.getType() == TokenType::Number && b.getType() == TokenType::Number)
                return Node(TokenType::Number, std::to_string((long long int)(std::stod(a.getValue()) * std::stod(b.getValue()))));
        }
        Node f_div(const std::vector<Node> tokens) {
            Node a = tokens[0];
            Node b = tokens[1];
            if (a.getType() == TokenType::Number && b.getType() == TokenType::Number)
                return Node(TokenType::Number, std::to_string((long long int)std::stod(a.getValue()) / std::stod(b.getValue())));
        }
        Node f_mod(const std::vector<Node> tokens) {
            Node a = tokens[0];
            Node b = tokens[1];
            if (a.getType() == TokenType::Number && b.getType() == TokenType::Number)
                return Node(TokenType::Number, std::to_string(std::stoll(a.getValue()) % std::stoll(b.getValue())));
        }
        Node f_not(const std::vector<Node> tokens) {
            Node a = tokens[0];
            if (a.getType() == TokenType::Number) {
                if (a.getValue() == "0") {
                    return Node(TokenType::Number, "1");
                }
                else {
                    return Node(TokenType::Number, "0");
                }
            }
        }
        Node f_eq(const std::vector<Node> tokens) {
            Node a = tokens[0];
            Node b = tokens[1];
            if (a.getType() == b.getType() && a.getValue() == b.getValue()) {
                return Node(TokenType::Number, "1");
            } else {
                return Node(TokenType::Number, "0");
            }
        }
        Node f_neq(const std::vector<Node> tokens) {
            Node a = tokens[0];
            Node b = tokens[1];
            if (a.getType() == b.getType() && a.getValue() == b.getValue()) {
                return Node(TokenType::Number, "0");
            } else {
                return Node(TokenType::Number, "1");
            }
        }
        Node f_gt(const std::vector<Node> tokens) {
            Node a = tokens[0];
            Node b = tokens[1];
            if (a.getType() == TokenType::Number && b.getType() == TokenType::Number) {
                return Node(TokenType::Number, std::to_string(std::stod(a.getValue()) > std::stod(b.getValue())));
            }
            if (a.getType() == TokenType::String && b.getType() == TokenType::String) {
                return Node(TokenType::Number, std::to_string(a.getValue().size() > b.getValue().size()));
            }
        }
        Node f_ge(const std::vector<Node> tokens) {
            Node a = tokens[0];
            Node b = tokens[1];
            if (a.getType() == TokenType::Number && b.getType() == TokenType::Number) {
                return Node(TokenType::Number, std::to_string(std::stod(a.getValue()) >= std::stod(b.getValue())));
            }
            if (a.getType() == TokenType::String && b.getType() == TokenType::String) {
                return Node(TokenType::Number, std::to_string(a.getValue().size() >= b.getValue().size()));
            }
        }
        Node f_lt(const std::vector<Node> tokens) {
            Node a = tokens[0];
            Node b = tokens[1];
            if (a.getType() == TokenType::Number && b.getType() == TokenType::Number) {
                return Node(TokenType::Number, std::to_string(std::stod(a.getValue()) < std::stod(b.getValue())));
            }
            if (a.getType() == TokenType::String && b.getType() == TokenType::String) {
                return Node(TokenType::Number, std::to_string(a.getValue().size() < b.getValue().size()));
            }
        }
        Node f_le(const std::vector<Node> tokens) {
            Node a = tokens[0];
            Node b = tokens[1];
            if (a.getType() == TokenType::Number && b.getType() == TokenType::Number) {
                return Node(TokenType::Number, std::to_string(std::stod(a.getValue()) <= std::stod(b.getValue())));
            }
            if (a.getType() == TokenType::String && b.getType() == TokenType::String) {
                return Node(TokenType::Number, std::to_string(a.getValue().size() <= b.getValue().size()));
            }
        }
        Node f_print(const std::vector<Node> tokens) {
            Node a = tokens[0];
            std::cout << a.getValue() << std::endl;
            return Node(TokenType::Null);
        }
        Node f_input(const std::vector<Node> tokens) {
            Node a = tokens[0];
            std::cout << a.getValue();
            std::string userInput;
            std::getline(std::cin, userInput);
            return Node(TokenType::String, userInput);
        }
        Node f_int(const std::vector<Node> tokens) {
            Node toInt = tokens[0];
            return Node(TokenType::Number, std::to_string((int) std::round(std::stod(toInt.getValue()))));
        }
        Node f_string(const std::vector<Node> tokens) {
            Node toString = tokens[0];
            return Node(TokenType::String, toString.getValue());
        }
        Node f_random(const std::vector<Node> tokens) {
            std::mt19937_64 rng;
            uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
            rng.seed(ss);
            std::uniform_real_distribution<double> unif(0, 1);
            return Node(TokenType::Number, std::to_string(unif(rng)));
        }
        Node f_time(const std::vector<Node> tokens) {
            std::string retTime = std::to_string(std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1));
            return Node(TokenType::Number, retTime);
        }
        Node f_split(const std::vector<Node> tokens) {
            Node saveTo = tokens[0];
            Node string = tokens[1];
            Node delimiter = tokens[2];
            Node storeInstruction(TokenType::Instruction);
            Node accessMemory = Node(TokenType::StackAt);
            accessMemory.addParameter(saveTo);
            storeInstruction.addParameter(accessMemory);
            std::vector<std::string> splittedString = split(string.getValue(), delimiter.getValue());
            for (int i = 0; i < splittedString.size(); i++) {
                Node incMemory(TokenType::StackAt);
                incMemory.addParameter(Node(TokenType::Number, std::to_string(i)));
                Node writeMemory(TokenType::StackAccess);
                writeMemory.addParameter(Node(TokenType::String, splittedString[i]));
                storeInstruction.addParameter(incMemory);
                storeInstruction.addParameter(writeMemory);
            }
            return storeInstruction;
        }
        Node f_read(const std::vector<Node> tokens) {
            Node filenameToken = tokens[0];
            std::string filename = filenameToken.getValue();
            Node stackPlace = tokens[1];
            Node storeInstruction(TokenType::Instruction);
            Node accessMemory = Node(TokenType::StackAt);
            accessMemory.addParameter(stackPlace);
            Node zeroPos = Node(TokenType::Number, "0");
            Node accessSubMem = Node(TokenType::StackAt);
            accessSubMem.addParameter(zeroPos);
            storeInstruction.addParameter(accessMemory);
            storeInstruction.addParameter(accessSubMem);
            std::ifstream infile(filename);
            std::string line;
            while (std::getline(infile, line))
            {
                Node stackAcc(TokenType::StackAccess);
                stackAcc.addParameter(Node(TokenType::String, line));
                storeInstruction.addParameter(stackAcc);
                storeInstruction.addParameter(Node(TokenType::StackRight));
            }
            return storeInstruction;
        }
        Node f_write(const std::vector<Node> tokens) {
            return Node(TokenType::Null);
        }
        Node fBuild(std::string funcname, int amount, std::function<Node(const std::vector<Node>)> func, std::vector<Node> parameters) {
            if (amount != parameters.size()) {
                std::cout << "[Error] Number of parameters not correct in function : " << funcname << std::endl;
                return Node(TokenType::Null);
            }
            return func(parameters);
        }
        std::pair<std::function<Node(const std::vector<Node>)>, int> fRightPart (std::string name, std::function<Node(const std::vector<Node>)> func, int amount) {
            return std::pair<std::function<Node(const std::vector<Node>)>, int>(std::bind(fBuild, name, amount, func, std::placeholders::_1), amount);
        }
        std::pair<std::string, std::pair<std::function<Node(const std::vector<Node>)>, int>> f(std::string name, std::function<Node(const std::vector<Node>)> func, int amount) {
            std::pair<std::function<Node(const std::vector<Node>)>, int> rpart = fRightPart(name, func, amount);
            return std::pair<std::string, std::pair<std::function<Node(const std::vector<Node>)>, int>>(name, rpart);
        }
       
        Node fCall(std::string funcName, int expectedArgs, const std::vector<Node> tokens) {
            if (expectedArgs == tokens.size()) {
                Node callInstruction(TokenType::Instruction);
                Node argDef(TokenType::Instruction, "Ignore");
                Node toArgs(TokenType::StackAt);
                toArgs.addParameter(Node(TokenType::String, "args"));
                argDef.addParameter(toArgs);
                for (int i = 0; i < tokens.size(); i++) {
                    Node incMemory(TokenType::StackAt);
                    incMemory.addParameter(Node(TokenType::Number, std::to_string(i)));
                    Node writeMemory(TokenType::StackAccess);
                    writeMemory.addParameter(tokens[i]);
                    argDef.addParameter(incMemory);
                    argDef.addParameter(writeMemory);
                }
                Node funcCode(TokenType::Instruction, "Code");
                funcCode.addParameter(toArgs);
                for (Node& tok : *customFunctions[funcName]->getParameters()) {
                    funcCode.addParameter(tok);
                }
                callInstruction.addParameter(argDef);
                callInstruction.addParameter(funcCode);
                return callInstruction;
            }
            else {
                std::cout << "[Error] Number of parameters not correct in custom function : " << funcName << " (expected "<< expectedArgs << " got " << tokens.size() << ")" << std::endl;
                for (int i = 0; i < tokens.size(); i++) {
                    Node cTok = tokens[i];
                    std::cout << "Arg#" << i << " : " << cTok.getSummary() << std::endl;
                }
                return Node(TokenType::Null);
            }
        }
        Node f_func(const std::vector<Node> tokens) {
            Node tFuncName = tokens[0];
            Node tFuncArgs = tokens[1];
            std::string funcName = tFuncName.getValue();
            int funcArgs = std::stoi(tFuncArgs.getValue());
            Node funcBody = tokens[2];
            customFunctions[funcName] = new Node(funcBody);
            FunctionsName[funcName] = std::pair<std::function<Node(const std::vector<Node>)>, int>(std::bind(fCall, funcName, funcArgs, std::placeholders::_1), funcArgs);
            return Node(TokenType::Null);
        }
    }
}