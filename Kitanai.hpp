#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <typeinfo>
#include <fstream>
#include <algorithm>
#include <functional>
#include <random>
#include <chrono>
#include <sstream>

std::vector<std::string> split(const std::string &str, const std::string &delimiters = " ");
std::string replaceString(std::string subject, const std::string& search, const std::string &replace);
typedef std::tuple<std::vector<std::string>, std::vector<std::string>, std::vector<std::pair<int, int>>> ExtractionResult;
ExtractionResult extractStrings(std::string string);
void removeCharFromString(std::string &str, std::string charToRemove);
int occurencesInString(std::string str, std::string occur) ;
bool isStringNumeric(std::string str);
bool isStringInt(std::string str);
bool isStringFloat(std::string str);

struct cmpByStringLength {
    bool operator()(const std::string& a, const std::string& b) const {
        return a.length() > b.length();
    }
};

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
        Token* parent;
        TokenType::Type type;
        std::string value;
        std::vector<Token> parameters;
        void explore(std::vector<Token*>& list, int depth = -1);
        friend class TokenCrawler;
    public:
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
        bool operator==(Token& other);
        int getDepth();
        void doSemantics();
        void doParametersAffectation();
        void doDynamicFlags(int& flagCounter);
};

class TokenCrawler 
{
    private:
        std::vector<Token*> tokenVector;
        Token* iterator = nullptr;
        int index = 0;
    public:
        TokenCrawler(Token& tokens, Token* iterator);
        void refresh(Token& tokens, int depth = -1);
        void reset();
        bool next();
        bool hasNext();
        Token* get();
        Token* getAtOffset(int offset);
        Token* find(int id);
        std::vector<Token*> getList();
};
void parametersAffectation(std::vector<Token>* tokens);
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
            return Token(TokenType::Number, std::to_string(std::stod(a.getValue()) * std::stod(b.getValue())));
    }
    Token f_div(const std::vector<Token> tokens) {
        Token a = tokens[0];
        Token b = tokens[1];
        if (a.getType() == TokenType::Number && b.getType() == TokenType::Number)
            return Token(TokenType::Number, std::to_string(std::stoi(a.getValue()) / std::stoi(b.getValue())));
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
    }
    Token f_lt(const std::vector<Token> tokens) {
        Token a = tokens[0];
        Token b = tokens[1];
    }
    Token f_le(const std::vector<Token> tokens) {
        Token a = tokens[0];
        Token b = tokens[1];
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
    Token B_(std::string funcname, int amount, std::function<Token(const std::vector<Token>)> func, std::vector<Token> parameters) {
        if (amount != parameters.size()) {
            std::cout << "ERROR ! Number of parameters not correct in function : " << funcname << std::endl;
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
        B__("random", f_random, 0)
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
        void parseFile(std::string path);
        void exec();
        void setStackPosition(int depth, std::string pos);
        void setStackPosition(int depth, int pos);
        Token getStackAt(int depth);
        void storeInStack(int depth, Token token);
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
        std::string getStackIndex();
};

void execFile(std::string path);
