#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <typeinfo>
#include <fstream>
#include <algorithm>
#include <functional>

std::vector<std::string> split(const std::string &str, const std::string &delimiters = " ");
std::string join(std::vector<std::string>& vector, std::string sep = "", int start = 0, int end = 0);
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

template <class T>
std::string getClassType()
{
    std::vector<std::string> splittedTypeName = split(typeid(T).name(), " ");
    return join(splittedTypeName, "", 1);
}

class Attribute
{
    private:
        std::string name;
        std::string data;
        std::string dtype;
    public:
        BaseAttribute(std::string bname, std::string btype, std::string bdata);
        std::string getName();
        std::string returnData();
        void set(int var);
        void set(double var);
        void set(std::string var);
        void set(bool var);
        std::string getType();
        template <class T> T get() {
            std::string typeOfData = getClassType<T>();
            if (typeOfData == getClassType<int>())
                return std::stoi(data);
            if (typeOfData == getClassType<double>())
                return std::stod(data);
            if (typeOfData == getClassType<bool>())
                return (data == "True") ? true : false;
            if (typeOfData == getClassType<std::string>())
                return data;
        }
};

template <> inline int Attribute::get() {
    if (dtype == "int")
        return std::stoi(data);
    else
        std::cout << "<Error:DataParser:BaseAttribute>[getData] : " << name << " is not a <int> BaseAttribute (" << dtype << ")" << std::endl;
}
template <> inline double Attribute::get() {
    if (dtype == "float")
        return std::stod(data);
    else
        std::cout << "<Error:DataParser:BaseAttribute>[getData] : " << name << " is not a <float> BaseAttribute (" << dtype << ")" << std::endl;
}
template <> inline bool Attribute::get() {
    if (dtype == "bool")
        return (data == "True") ? true : false;
    else
        std::cout << "<Error:DataParser:BaseAttribute>[getData] : " << name << " is not a <bool> BaseAttribute (" << dtype << ")" << std::endl;
}
template <> inline std::string Attribute::get() {
    if (dtype == "str")
        return data;
    else
        std::cout << "<Error:DataParser:BaseAttribute>[getData] : " << name << " is not a <str> BaseAttribute (" << dtype << ")" << std::endl;
}

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
        ContractStack,
        ExpandStack,
        StackAt,
        CurrentStackValue,
        Flag,
        Goto,
        Origin,
        NewInstruction,
        End,
        Null
    };

    static std::map<std::string, std::pair<std::string, Type>> TypeDataK = {
        {"(", {"OpenInstruction", OpenInstruction}},
        {")", {"CloseInstruction", CloseInstruction}},
        {"[", {"OpenStackAccess", OpenStackAccess}},
        {"]", {"CloseStackAccess", CloseStackAccess}},
        {"?", {"Condition", Condition}},
        {"<", {"ContractStack", ContractStack}},
        {">", {"ExpandStack", ExpandStack}},
        {"$", {"StackAt", StackAt}},
        {"@", {"CurrentStackValue", CurrentStackValue}},
        {"#", {"Flag", Flag}},
        {"&", {"Goto", Goto}},
        {"~", {"Origin", Origin}},
        {";", {"NewInstruction", NewInstruction}},
        {"%", {"End", End}}
    };
    static std::map<Type, std::pair<std::string, std::string>> TypeDataR = {
        {OpenInstruction, {"OpenInstruction", "("}},
        {CloseInstruction, {"CloseInstruction", ")"}},
        {OpenStackAccess, {"OpenStackAccess", "["}},
        {CloseStackAccess, {"CloseStackAccess", "]"}},
        {Condition, {"Condition", "?"}},
        {ContractStack, {"ContractStack", "<"}},
        {ExpandStack, {"ExpandStack", ">"}},
        {StackAt, {"StackAt", "$"}},
        {CurrentStackValue, {"CurrentStackValue", "@"}},
        {Flag, {"Flag", "#"}},
        {Goto, {"Goto", "&"}},
        {Origin, {"Origin", "~"}},
        {NewInstruction, {"NewInstruction", ";"}},
        {End, {"End", "%"}},
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
        TokenType::Type type;
        std::string value;
        std::vector<Token*> parameters;
    public:
        Token(TokenType::Type type);
        Token(TokenType::Type type, std::string value);
        TokenType::Type getType();
        std::string getPrintableType();
        void setType(TokenType::Type type);
        std::string getValue();
        void addParameter(Token* token);
        std::vector<Token*>* getParameters();
        void inspect(int depth = 0);
        void execute(Program* prg);
};
void parametersAffectation(std::vector<Token*>* tokens);
std::vector<Token> stringToTokens(int& flagCounter, std::string string);

namespace StdLib
{
    Token* f_add(std::vector<Token*> tokens) {
        Token* a = tokens[0];
        Token* b = tokens[1];
        if (a->getType() == TokenType::Number && b->getType() == TokenType::Number)
            return new Token(TokenType::Number, std::to_string(std::stoi(a->getValue()) + std::stoi(b->getValue())));
        if (a->getType() == TokenType::String && b->getType() == TokenType::String)
            return new Token(TokenType::String, a->getValue() + b->getValue());
    }
    Token* f_sub(std::vector<Token*> tokens) {
        Token* a = tokens[0];
        Token* b = tokens[1];
        if (a->getType() == TokenType::Number && b->getType() == TokenType::Number)
            return new Token(TokenType::Number, std::to_string(std::stoi(a->getValue()) - std::stoi(b->getValue())));
    }
    Token* f_mul(std::vector<Token*> tokens) {
        Token* a = tokens[0];
        Token* b = tokens[1];
        if (a->getType() == TokenType::Number && b->getType() == TokenType::Number)
            return new Token(TokenType::Number, std::to_string(std::stoi(a->getValue()) * std::stoi(b->getValue())));
    }
    Token* f_div(std::vector<Token*> tokens) {
        Token* a = tokens[0];
        Token* b = tokens[1];
        if (a->getType() == TokenType::Number && b->getType() == TokenType::Number)
            return new Token(TokenType::Number, std::to_string(std::stoi(a->getValue()) / std::stoi(b->getValue())));
    }
    Token* f_eq(std::vector<Token*> tokens) {
        Token* a = tokens[0];
        Token* b = tokens[1];
        if (a->getType() == b->getType() && a->getValue() == b->getValue()) {
            return new Token(TokenType::Number, "1");
        } else {
            return new Token(TokenType::Number, "0");
        }
    }
    Token* f_neq(std::vector<Token*> tokens) {
        Token* a = tokens[0];
        Token* b = tokens[1];
        if (a->getType() == b->getType() && a->getValue() == b->getValue()) {
            return new Token(TokenType::Number, "0");
        } else {
            return new Token(TokenType::Number, "1");
        }
    }
    Token* f_gt(std::vector<Token*> tokens) {
        Token* a = tokens[0];
        Token* b = tokens[1];
    }
    Token* f_ge(std::vector<Token*> tokens) {
        Token* a = tokens[0];
        Token* b = tokens[1];
    }
    Token* f_lt(std::vector<Token*> tokens) {
        Token* a = tokens[0];
        Token* b = tokens[1];
    }
    Token* f_le(std::vector<Token*> tokens) {
        Token* a = tokens[0];
        Token* b = tokens[1];
    }
    Token* f_print(std::vector<Token*> tokens) {
        Token* a = tokens[0];
        std::cout << "[Exe::print] " << a->getValue() << std::endl;
    }
    Token* f_input(std::vector<Token*> tokens) {
        Token* a = tokens[0];
        std::cout << "[Exe::input] " << a->getValue() << " : " << std::endl;
        std::string userInput;
        std::getline(std::cin, userInput);
        return new Token(TokenType::String, userInput);
    }
    Token* f_int(std::vector<Token*> tokens) {

    }
    Token* f_random(std::vector<Token*> tokens) {
        
    }
    Token* B_(std::string funcname, int amount, std::function<Token*(std::vector<Token*>)> func, std::vector<Token*> parameters) {
        if (amount != parameters.size()) {
            std::cout << "ERROR ! Number of parameters not correct in function : " << funcname << std::endl;
            return nullptr;
        }
        return func(parameters);
    }
    std::pair<std::string, std::pair<std::function<Token*(std::vector<Token*>)>, int>> B__(std::string name, std::function<Token*(std::vector<Token*>)> func, int amount) {
        std::pair<std::function<Token*(std::vector<Token*>)>, int> rpart = std::pair<std::function<Token*(std::vector<Token*>)>, int>(std::bind(B_, name, amount, func, std::placeholders::_1), amount);
        return std::pair<std::string, std::pair<std::function<Token*(std::vector<Token*>)>, int>>(name, rpart);
    }
    static std::map<std::string, std::pair<std::function<Token*(std::vector<Token*>)>, int>> FunctionsName = {
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
        B__("int", f_int, 1),
        B__("random", f_random, 0)
    };
}

class Program
{
    private:
        Token instructions = Token(TokenType::Instruction);
        std::map<std::string, int> flags;
        std::vector<std::string> origins;
        std::map<std::string, std::vector<Token*>> stack;
        std::vector<std::string> staticStrings;
        std::string stackPosition = "";
        int subStackPosition = 0;
        int linePosition;
    public:
        Program();
        void parseFile(std::string path);
        void exec();
        void setStackPosition(std::string pos);
        void setStackPosition(int pos);
        Token* getStackAt();
};

void execFile(std::string path);
