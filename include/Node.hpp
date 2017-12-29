#pragma once

#include <Token.hpp>

namespace ktn
{
    class Program;
    class Node
    {
    private:
        static int NodeAmount;
        int id = 0;
        int parentID = -1;
        Node* parent = nullptr;
        TokenType type;
        std::string value;
        std::vector<Node> parameters;
    public:
        static bool _execDebug;
        static bool _compDebug;
        static bool _dispMemAd;
        Node(TokenType type);
        Node(TokenType type, const std::string& value);
        void setParent(Node* parent);
        int getID();
        int getParentID();
        Node* getAtID(int id);
        std::string getSummary();
        TokenType getType();
        std::string getPrintableType();
        void setType(TokenType type);
        std::string getValue();
        void addParameter(Node node);
        std::vector<Node>* getParameters();
        void eraseNodeAtId(int id);
        void inspect(int depth = 0);
        void execute(Program* prg);
        Node getInstructionContent(std::vector<Node>& nodes, int index = 0);
        Node* getParent();
        void insertAfter(int id, Node node);
        int getDepth();
        void doAST();
        void doParametersAffectation();
        void doDynamicFlags(int& flagCounter);
        void doChainAdoption();
    };
}