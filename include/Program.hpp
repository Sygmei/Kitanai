#pragma once

#include <map>

#include <Node.hpp>

namespace ktn
{
    class Program
    {
    private:
        Node instructions = Node(TokenType::Instruction);
        TokenType pauseCause = TokenType::Null;
        int flagCounter = 0;
        int flagSeeked = -1;
        bool isProgramOver = false;
        bool execution = true;
        std::vector<int> origins;
        std::map<std::string, std::vector<Node>> stack;
        std::vector<std::pair<std::string, int>> stackPosition;
    public:
        Program();
        bool compDebug = false;
        void parseFile(const std::string& path);
        void exec();
        void setDepth(int depth);
        void setStackPosition(const std::string& pos);
        void setStackPosition(int pos);
        int getStackSize() const;
        Node getStackAt() const;
        void storeInStack(Node token);
        void stopExecution(TokenType cause);
        void startExecution();
        void stopProgram();
        bool canExecute() const;
        TokenType getPauseCause() const;
        void setSeekedFlag(int flag);
        int getSeekedFlag() const;
        void addOrigin(int flagNumber);
        int getNextOrigin() const;
        void removeOriginFlag(Node& flag);
        void import(const std::string& path);
        Node& getInstructions();
        std::pair<std::string, int> getStackPosition() const;
    };
}