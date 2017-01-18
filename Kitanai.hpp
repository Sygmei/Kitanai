#pragma once

#include <algorithm>
#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>

#include "Functions.hpp"

namespace TokenType
{
    enum Type
    {
        OpenInstruction,
        CloseInstruction,
        Instruction,
        String,
        Number,
        Condition,
        OpenStackAccess,
        CloseStackAccess,
        StackAccess,
        Function,
        StackAt,
        CurrentStackValue,
        Flag,
        DynamicFlag,
        Goto,
        GotoNoOrigin,
        Origin,
        NewInstruction,
        End,
        ToggleExecution,
        Null
    };

    static std::map<std::string, std::pair<std::string, Type>> TypeDataK = {
        {"(", {"OpenInstruction", OpenInstruction}},
        {")", {"CloseInstruction", CloseInstruction}},
        {"[", {"OpenStackAccess", OpenStackAccess}},
        {"]", {"CloseStackAccess", CloseStackAccess}},
        {"?", {"Condition", Condition}},
        {"$", {"StackAt", StackAt}},
        {"@", {"CurrentStackValue", CurrentStackValue}},
        {"#", {"Flag", Flag}},
        {"&", {"Goto", Goto}},
        {"*", {"GotoNoOrigin", GotoNoOrigin}},
        {"~", {"Origin", Origin}},
        {";", {"NewInstruction", NewInstruction}},
        {"!", {"ToggleExecution", ToggleExecution}},
        {"%", {"End", End}}
    };
    static std::map<Type, std::pair<std::string, std::string>> TypeDataR = {
        {OpenInstruction, {"OpenInstruction", "("}},
        {CloseInstruction, {"CloseInstruction", ")"}},
        {OpenStackAccess, {"OpenStackAccess", "["}},
        {CloseStackAccess, {"CloseStackAccess", "]"}},
        {Condition, {"Condition", "?"}},
        {StackAt, {"StackAt", "$"}},
        {CurrentStackValue, {"CurrentStackValue", "@"}},
        {Flag, {"Flag", "#"}},
        {DynamicFlag, {"DynamicFlag", ""}},
        {Goto, {"Goto", "&"}},
        {GotoNoOrigin, {"GotoNoOrigin", "-&"}},
        {Origin, {"Origin", "~"}},
        {NewInstruction, {"NewInstruction", ";"}},
        {End, {"End", "%"}},
        {ToggleExecution, {"ToggleExecution", "!"}},
        {Function, {"Function", ""}},
        {String, {"String", ""}},
        {Number, {"Number", ""}},
        {Instruction, {"Instruction", ""}},
        {StackAccess, {"StackAccess", ""}},
        {Null, {"Null", ""}}
    };
    static std::vector<std::vector<Type>> TypeParameters = {
        {Goto, Origin},
        {Goto, Number},
        {GotoNoOrigin, Number},
        {StackAt, String},
        {StackAt, Number},
		{StackAt, Instruction},
        {Condition, Instruction, Instruction, Instruction},
        {Condition, Instruction, Instruction}
    };
    static std::vector<std::vector<Type>> TypeCompression = {
        {OpenInstruction, CloseInstruction, Instruction},
        {OpenStackAccess, CloseStackAccess, StackAccess},
    };
}

class Program;
class Token
{
    private:
        static int TokenAmount;
        int id = 0;
        int parentID = -1;
        Token* parent = nullptr;
        TokenType::Type type;
        std::string value;
        std::vector<Token> parameters;
    public:
		static bool _execDebug;
		static bool _compDebug;
		static bool _dispMemAd;
        Token(TokenType::Type type);
        Token(TokenType::Type type, std::string value);
        void setParent(Token* parent);
        int getID();
        int getParentID();
        Token* getAtID(int id);
        std::string getSummary();
        TokenType::Type getType();
        std::string getPrintableType();
        void setType(TokenType::Type type);
        std::string getValue();
        void addParameter(Token token);
        std::vector<Token>* getParameters();
        void eraseTokenAtID(int id);
        void inspect(int depth = 0);
        void execute(Program* prg);
        Token getInstructionContent(std::vector<Token>& tokens, int index = 0);
        Token* getParent();
        void insertAfter(int id, Token token);
        int getDepth();
        void doAST();
        void doParametersAffectation();
        void doDynamicFlags(int& flagCounter);
		void doChainAdoption();
};

std::vector<Token> stringToTokens(int& flagCounter, std::string string);

namespace StdLib
{
    Token f_add(const std::vector<Token> tokens) {
        Token a = tokens[0];
        Token b = tokens[1];
        if (a.getType() == TokenType::Number && b.getType() == TokenType::Number)
            return Token(TokenType::Number, std::to_string(std::stoi(a.getValue()) + std::stoi(b.getValue())));
        else {
            return Token(TokenType::String, a.getValue() + b.getValue());
        }
    }
    Token f_sub(const std::vector<Token> tokens) {
        Token a = tokens[0];
        Token b = tokens[1];
        if (a.getType() == TokenType::Number && b.getType() == TokenType::Number)
            return Token(TokenType::Number, std::to_string(std::stoi(a.getValue()) - std::stoi(b.getValue())));
    }
    Token f_mul(const std::vector<Token> tokens) {
        Token a = tokens[0];
        Token b = tokens[1];
        if (a.getType() == TokenType::Number && b.getType() == TokenType::Number)
            return Token(TokenType::Number, std::to_string((int)(std::stod(a.getValue()) * std::stod(b.getValue()))));
    }
    Token f_div(const std::vector<Token> tokens) {
        Token a = tokens[0];
        Token b = tokens[1];
        if (a.getType() == TokenType::Number && b.getType() == TokenType::Number)
            return Token(TokenType::Number, std::to_string((int)std::stod(a.getValue()) / std::stod(b.getValue())));
    }
	Token f_mod(const std::vector<Token> tokens) {
		Token a = tokens[0];
		Token b = tokens[1];
		if (a.getType() == TokenType::Number && b.getType() == TokenType::Number)
			return Token(TokenType::Number, std::to_string(std::stoi(a.getValue()) % std::stoi(b.getValue())));
	}
	Token f_not(const std::vector<Token> tokens) {
		Token a = tokens[0];
		if (a.getType() == TokenType::Number) {
			if (a.getValue() == "0") {
				return Token(TokenType::Number, "1");
			}
			else {
				return Token(TokenType::Number, "0");
			}
		}
	}
    Token f_eq(const std::vector<Token> tokens) {
        Token a = tokens[0];
        Token b = tokens[1];
        if (a.getType() == b.getType() && a.getValue() == b.getValue()) {
            return Token(TokenType::Number, "1");
        } else {
            return Token(TokenType::Number, "0");
        }
    }
    Token f_neq(const std::vector<Token> tokens) {
        Token a = tokens[0];
        Token b = tokens[1];
        if (a.getType() == b.getType() && a.getValue() == b.getValue()) {
            return Token(TokenType::Number, "0");
        } else {
            return Token(TokenType::Number, "1");
        }
    }
    Token f_gt(const std::vector<Token> tokens) {
        Token a = tokens[0];
        Token b = tokens[1];
        if (a.getType() == TokenType::Number && b.getType() == TokenType::Number) {
            return Token(TokenType::Number, std::to_string(std::stod(a.getValue()) > std::stod(b.getValue())));
        }
        if (a.getType() == TokenType::String && b.getType() == TokenType::String) {
            return Token(TokenType::Number, std::to_string(a.getValue().size() > b.getValue().size()));
        }
    }
    Token f_ge(const std::vector<Token> tokens) {
        Token a = tokens[0];
        Token b = tokens[1];
		if (a.getType() == TokenType::Number && b.getType() == TokenType::Number) {
			return Token(TokenType::Number, std::to_string(std::stod(a.getValue()) >= std::stod(b.getValue())));
		}
		if (a.getType() == TokenType::String && b.getType() == TokenType::String) {
			return Token(TokenType::Number, std::to_string(a.getValue().size() >= b.getValue().size()));
		}
    }
    Token f_lt(const std::vector<Token> tokens) {
        Token a = tokens[0];
        Token b = tokens[1];
		if (a.getType() == TokenType::Number && b.getType() == TokenType::Number) {
			return Token(TokenType::Number, std::to_string(std::stod(a.getValue()) < std::stod(b.getValue())));
		}
		if (a.getType() == TokenType::String && b.getType() == TokenType::String) {
			return Token(TokenType::Number, std::to_string(a.getValue().size() < b.getValue().size()));
		}
    }
    Token f_le(const std::vector<Token> tokens) {
        Token a = tokens[0];
        Token b = tokens[1];
		if (a.getType() == TokenType::Number && b.getType() == TokenType::Number) {
			return Token(TokenType::Number, std::to_string(std::stod(a.getValue()) <= std::stod(b.getValue())));
		}
		if (a.getType() == TokenType::String && b.getType() == TokenType::String) {
			return Token(TokenType::Number, std::to_string(a.getValue().size() <= b.getValue().size()));
		}
    }
    Token f_print(const std::vector<Token> tokens) {
        Token a = tokens[0];
        std::cout << a.getValue() << std::endl;
        return Token(TokenType::Null);
    }
    Token f_input(const std::vector<Token> tokens) {
        Token a = tokens[0];
        std::cout << a.getValue();
        std::string userInput;
        std::getline(std::cin, userInput);
        return Token(TokenType::String, userInput);
    }
    Token f_int(const std::vector<Token> tokens) {
        Token toInt = tokens[0];
        return Token(TokenType::Number, std::to_string((int) std::round(std::stod(toInt.getValue()))));
    }
    Token f_string(const std::vector<Token> tokens) {
        Token toString = tokens[0];
        return Token(TokenType::String, toString.getValue());
    }
    Token f_random(const std::vector<Token> tokens) {
        std::mt19937_64 rng;
        uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
        rng.seed(ss);
        std::uniform_real_distribution<double> unif(0, 1);
        return Token(TokenType::Number, std::to_string(unif(rng)));
    }
	Token f_split(const std::vector<Token> tokens) {
		Token saveTo = tokens[0];
		Token string = tokens[1];
		Token delimiter = tokens[2];
		Token storeInstruction(TokenType::Instruction);
		Token accessMemory = Token(TokenType::StackAt);
		accessMemory.addParameter(saveTo);
		storeInstruction.addParameter(accessMemory);
		std::vector<std::string> splittedString = split(string.getValue(), delimiter.getValue());
		for (int i = 0; i < splittedString.size(); i++) {
			Token incMemory(TokenType::StackAt);
			incMemory.addParameter(Token(TokenType::Number, std::to_string(i)));
			Token writeMemory(TokenType::StackAccess);
			writeMemory.addParameter(Token(TokenType::String, splittedString[i]));
			storeInstruction.addParameter(incMemory);
			storeInstruction.addParameter(writeMemory);
		}
		return storeInstruction;
	}
    Token B_(std::string funcname, int amount, std::function<Token(const std::vector<Token>)> func, std::vector<Token> parameters) {
        if (amount != parameters.size()) {
            std::cout << "[Error] Number of parameters not correct in function : " << funcname << std::endl;
            return Token(TokenType::Null);
        }
        return func(parameters);
    }
    std::pair<std::string, std::pair<std::function<Token(const std::vector<Token>)>, int>> B__(std::string name, std::function<Token(const std::vector<Token>)> func, int amount) {
        std::pair<std::function<Token(const std::vector<Token>)>, int> rpart = 
        std::pair<std::function<Token(const std::vector<Token>)>, int>(std::bind(B_, name, amount, func, std::placeholders::_1), amount);
        return std::pair<std::string, std::pair<std::function<Token(const std::vector<Token>)>, int>>(name, rpart);
    }
    static std::map<std::string, std::pair<std::function<Token(const std::vector<Token>)>, int>> FunctionsName = {
        B__("add", f_add, 2),
        B__("sub", f_sub, 2),
        B__("mul", f_mul, 2),
        B__("div", f_div, 2),
		B__("mod", f_mod, 2),
		B__("not", f_not, 1),
        B__("eq", f_eq, 2),
        B__("neq", f_neq, 2),
        B__("gt", f_gt, 2),
        B__("ge", f_ge, 2),
        B__("lt", f_lt, 2),
        B__("le", f_le, 2),
        B__("print", f_print, 1),
        B__("input", f_input, 1),
        B__("string",  f_string, 1),
        B__("int", f_int, 1),
        B__("random", f_random, 0),
		B__("split", f_split, 3)
    };
}

class Program
{
    private:
        Token instructions = Token(TokenType::Instruction);
        TokenType::Type pauseCause = TokenType::Null;
        int flagCounter = 0;
        int flagSeeked = -1;
        bool isProgramOver = false;
        bool execution = true;
        std::vector<int> origins;
        std::map<std::string, std::vector<Token>> stack;
        std::vector<std::string> staticStrings;
        std::vector<std::pair<std::string, int>> stackPosition;
        int linePosition;
    public:
        Program();
		bool compDebug = false;
        void parseFile(std::string path);
        void exec();
		void setDepth(int depth);
        void setStackPosition(std::string pos);
        void setStackPosition(int pos);
        Token getStackAt();
        void storeInStack(Token token);
        void stopExecution(TokenType::Type cause);
        void startExecution();
        void stopProgram();
        bool canExecute();
        TokenType::Type getPauseCause();
        void setSeekedFlag(int flag);
        int getSeekedFlag();
        void addOrigin(int flagNumber);
        int getNextOrigin();
        void removeOriginFlag(Token& flag);
        std::pair<std::string, int> getStackPosition();
};