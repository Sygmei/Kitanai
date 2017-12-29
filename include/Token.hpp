#pragma once

#include <array>
#include <string>
#include <vector>

namespace ktn
{
    enum class TokenType
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
        End,
        ToggleExecution,
		StackSize,
		StackLeft,
		StackRight,
		CurrentStackPosition,
		CurrentSubStackPosition,
        Null
    };

    class Token
    {
    public:
        std::string name;
        TokenType type;
        std::string symbol;
        Token(TokenType type, const std::string& name, const std::string& symbol);

        static std::array<Token, 25> Tokens;

        static Token& FindByName(const std::string& name);
        static Token& FindByType(TokenType type);
        static Token& FindBySymbol(const std::string& symbol);
    };

    static std::vector<std::vector<TokenType>> TypeParameters = {
        {TokenType::Goto, TokenType::Origin},
        {TokenType::Goto, TokenType::Number},
        {TokenType::GotoNoOrigin, TokenType::Number},
        {TokenType::StackAt, TokenType::String},
        {TokenType::StackAt, TokenType::Number},
		{TokenType::StackAt, TokenType::Instruction},
        {TokenType::Condition, TokenType::Instruction, TokenType::Instruction, TokenType::Instruction},
        {TokenType::Condition, TokenType::Instruction, TokenType::Instruction}
    };
    static std::vector<std::vector<TokenType>> TypeCompression = {
        {TokenType::OpenInstruction, TokenType::CloseInstruction, TokenType::Instruction},
        {TokenType::OpenStackAccess, TokenType::CloseStackAccess, TokenType::StackAccess},
    };
}