#ifndef TOKEN_SEQUENCE_HPP
#define TOKEN_SEQUENCE_HPP

#include "Terminal.hpp"

class TokenSequenceNode
{
    private:
        size_t requiredID, requiredMin, requiredMax;
    
    public:
        TokenSequenceNode(size_t id = NULL_TOKEN_ID, size_t min = 1, size_t max = 1);
        ~TokenSequenceNode();

        size_t getMin() const;
        size_t getMax() const;
        size_t getID() const;
};

class TokenSequence
{
    private:
        std::list<TokenSequenceNode> requiredNodes;
    
    public:
        TokenSequence(const std::list<TokenSequenceNode>& nodeSequence);
        ~TokenSequence();

        const std::list<TokenSequenceNode>& getRequiredNodes() const;
};

class TokenDefinition
{
    private:
        std::list<TokenSequence> validSequences;
    
    public:
        TokenDefinition(const std::list<TokenSequence>& nodeSequence);
        ~TokenDefinition();

        const std::list<TokenSequence>& getValidSequences() const;
};

#endif
