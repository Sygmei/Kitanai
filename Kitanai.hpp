#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <typeinfo>
#include <fstream>
#include <algorithm>

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
        StaticString,
        StaticNumber,
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
        LeftParenthesis,
        RightParenthesis,
        Parenthesis
    };

    static std::map<std::string, std::pair<std::string, Type>> TypeDataK = {
        {"{", {"OpenInstruction", OpenInstruction}},
        {"}", {"CloseInstruction", CloseInstruction}},
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
        {"(", {"LeftParenthesis", LeftParenthesis}},
        {")", {"RightParenthesis", RightParenthesis}}
    };
    static std::map<Type, std::pair<std::string, std::string>> TypeDataR = {
        {OpenInstruction, {"OpenInstruction", "{"}},
        {CloseInstruction, {"CloseInstruction", "}"}},
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
        {LeftParenthesis, {"LeftParenthesis", "("}},
        {RightParenthesis, {"RightParenthesis", ")"}},
        {Function, {"Function", ""}},
        {StaticString, {"StaticString", ""}},
        {StaticNumber, {"StaticNumber", ""}},
        {Instruction, {"Instruction", ""}},
        {Parenthesis, {"Parenthesis", ""}},
        {StackAccess, {"StackAccess", ""}},
    };
    static std::vector<std::vector<Type>> TypeParameters = {
        {Goto, Origin},
        {Goto, StaticNumber},
        {StackAt, StaticNumber},
        {Condition, Instruction, Instruction, Instruction},
        {Condition, Instruction, Instruction}
    };
    static std::vector<std::vector<Type>> TypeCompression = {
        {OpenInstruction, CloseInstruction, Instruction},
        {OpenStackAccess, CloseStackAccess, StackAccess},
        {LeftParenthesis, RightParenthesis, Parenthesis}
    };
    static std::map<std::string, std::pair<std::string, Type>, cmpByStringLength> TypeDataF = {
        {"print", {"print", Condition}}
    };
}

class Token
{
    private:
        TokenType::Type type;
        std::string value;
        std::vector<Token> parameters;
    public:
        Token(TokenType::Type type);
        Token(TokenType::Type type, std::string value);
        TokenType::Type getType();
        void setType(TokenType::Type type);
        std::string getValue();
        void addParameter(Token token);
        std::vector<Token> getParameters();
        void inspect(int depth = 0);
};
std::vector<Token> stringToTokens(std::string string);

class InstructionBloc
{
    private:
        std::vector<Token> instructions;
    public:
        InstructionBloc();
        void addToken(Token token);
        std::vector<Token>* getAllTokens();
        void inspect(int depth = 0);
};
void inspectToken(Token& token);
void inspectBloc(InstructionBloc& bloc);

class Program
{
    private:
        std::vector<InstructionBloc> instructions;
        std::map<std::string, int> flags;
        std::vector<std::string> origins;
        std::vector<Attribute*> stack;
        std::vector<std::string> staticStrings;
        int stackPosition;
        int linePosition;
    public:
        Program();
        void parseFile(std::string path);
};

void execFile(std::string path);
