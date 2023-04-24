#include "NonTerminal.hpp"

NonTerminal::NonTerminal()
{}

NonTerminal::NonTerminal(const Terminal& terminal)
: Terminal::Terminal(terminal)
{}

NonTerminal::~NonTerminal()
{}

void NonTerminal::addChildren(const Terminal& copy)
{
    // Host new instance in heap memory and store a reference 
    // The overloaded method will take care of the rest
    this->addChildren
    (
        std::shared_ptr<NonTerminal> // Wrap in reference counter
        (
            new NonTerminal // Allocate in dynamic memory
            (
                copy // Construct by copy
            ) 
        )
    );
}

void NonTerminal::addChildren(std::shared_ptr<NonTerminal> reference)
{
    // Add to the children non-terminal list. 
    // The shared pointer wrapper will keep track of references for us
    // This is useful to avoid children duplication or double-deletion
    this->children.push_back(reference);
}

const std::list<std::shared_ptr<NonTerminal>>& NonTerminal::getChildren()
{return this->children;}

std::list<std::shared_ptr<NonTerminal>> NonTerminal::operator[](size_t assignedID)
{
    std::list<std::shared_ptr<NonTerminal>> matchingChildren;

    for (auto child : this->children)
    {
        if (child->getID() == assignedID)
            matchingChildren.push_front(child);
    }

    return matchingChildren;
}