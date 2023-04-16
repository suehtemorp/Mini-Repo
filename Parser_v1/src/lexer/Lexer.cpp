#include "Lexer.hpp"

Lexer::Lexer()
{}

Lexer::~Lexer()
{}

void Lexer::addTerminal(const std::string& pattern, size_t assignedID)
{
    if (assignedID == NULL_TOKEN_ID)
        throw std::runtime_error("Assigning NULL ID to terminal by hand is forbidden");

    this->regexPriorityList.emplace_back(pattern, assignedID);
}

Terminal Lexer::popTerminal()
{
    if (this->terminalQueue.empty())
        throw std::runtime_error("Popping an empty terminal queue is forbidden");
    
    Terminal firstTerminal = this->terminalQueue.front();
    this->terminalQueue.pop_front();

    return firstTerminal;
}

const std::list<Terminal>& Lexer::getTerminalQueue()
{return this->terminalQueue;}
