#ifndef NONTERMINAL_HPP
#define NONTERMINAL_HPP

// Iterable, dynamically allocated container
#include <list>

// Reference-counted resource wrapper (avoid unnecesary copies)
#include <memory>

// Terminal implementation
#include "Terminal.hpp"

// Convienent forward declaration
class Parser;

class NonTerminal : public Terminal
{
    // The Parser may modify a compound token
    friend class Parser;

    private:
        // List of references to children compound tokens
        std::list<std::shared_ptr<NonTerminal>> children;

        /// @brief Add a children instanciated from a copy-able token
        /// @param copy Const-reference to copy-able token
        void addChildren(const Terminal& copy);

        /// @brief Add a children instanciated from a copy-able compoud token
        /// @remark Since compound tokens keep track of their children with reference,
        /// counting, the copy's children will have their reference count increased
        /// @param copy Const-reference to copy-able compound token
        void addChildren(std::shared_ptr<NonTerminal> copy);

    public:
        NonTerminal();
        NonTerminal(const Terminal& terminal);

        ~NonTerminal();

        const std::list<std::shared_ptr<NonTerminal>>& getChildren();

        std::list<std::shared_ptr<NonTerminal>> operator[](size_t assignedID);
};

#endif
