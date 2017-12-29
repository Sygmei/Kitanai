#include <algorithm>
#include <Token.hpp>

namespace ktn
{
    std::array<Token, 25> Token::Tokens = {
        Token(TokenType::Null, "Null", ""),
        Token(TokenType::OpenInstruction, "OpenInstruction", "("),
        Token(TokenType::CloseInstruction, "CloseInstruction", ")"),
        Token(TokenType::OpenStackAccess, "OpenStackAccess", "["),
        Token(TokenType::CloseStackAccess, "CloseStackAccess", "]"),
        Token(TokenType::Condition, "Condition", "?"),
        Token(TokenType::StackAt, "StackAt", "$"),
        Token(TokenType::CurrentStackValue, "CurrentStackValue", "@"),
        Token(TokenType::Flag, "Flag", "#"),
        Token(TokenType::DynamicFlag, "DynamicFlag", ""),
        Token(TokenType::Goto, "Goto", "&"),
        Token(TokenType::GotoNoOrigin, "GotoNoOrigin", "*"),
        Token(TokenType::Origin, "Origin", "~"),
        Token(TokenType::End, "End", "%"),    
        Token(TokenType::ToggleExecution, "ToggleExecution", "!"),
        Token(TokenType::Function, "Function", ""),
        Token(TokenType::String, "String", ""),
        Token(TokenType::Number, "Number", ""),
        Token(TokenType::Instruction, "Instruction", ""),
        Token(TokenType::StackAccess, "StackAccess", ""),
        Token(TokenType::StackSize, "StackSize", "^"),
        Token(TokenType::StackLeft, "StackLeft", "<"),
        Token(TokenType::StackRight, "StackRight", ">"),
        Token(TokenType::CurrentStackPosition, "CurrentStackPosition", ":"),
        Token(TokenType::CurrentSubStackPosition, "CurrentSubStackPosition", ";")
    };

    Token::Token(TokenType type, const std::string& name, const std::string& symbol)
    {
        this->type = type;
        this->name = name;
        this->symbol = symbol;
    }

    Token& Token::FindByName(const std::string& name)
    {
        auto it = std::find_if(Tokens.begin(), Tokens.end(), [&name](const Token& token) {
            return (token.name == name);
        });
        if (it != Tokens.end()) return *it;
        throw std::exception();
    }

    Token& Token::FindBySymbol(const std::string& symbol)
    {
        auto it = std::find_if(Tokens.begin(), Tokens.end(), [&symbol](const Token& token) {
            return (token.symbol == symbol);
        });
        if (it != Tokens.end()) return *it;
        throw std::exception();
    }

    Token& Token::FindByType(TokenType type)
    {
        auto it = std::find_if(Tokens.begin(), Tokens.end(), [&type](const Token& token) {
            return (token.type == type);
        });
        if (it != Tokens.end()) return *it;
        throw std::exception();
    }
}