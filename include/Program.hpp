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
        int getStackSize();
        Node getStackAt();
        void storeInStack(Node token);
        void stopExecution(TokenType cause);
        void startExecution();
        void stopProgram();
        bool canExecute();
        TokenType getPauseCause();
        void setSeekedFlag(int flag);
        int getSeekedFlag();
        void addOrigin(int flagNumber);
        int getNextOrigin();
        void removeOriginFlag(Node& flag);
        void import(std::string path);
        Node& getInstructions();
        std::pair<std::string, int> getStackPosition();
    };
}