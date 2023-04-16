#ifndef LEXER_HPP
#define LEXER_HPP

#include "Terminal.hpp"

// Iterable, dynamically allocated container
#include <list>

/// @brief Pseudo lexer class for terminal tokens on a buffered stream
class Lexer
{
    protected:
        /// @brief Queue of currently processed terminal tokens
        std::list<Terminal> terminalQueue;

        /// @brief Priority list (higher-to-lowest) of regex expressions
        // with corresponding ID to assign to terminals
        std::list<std::pair<boost::regex, size_t>> regexPriorityList;

    public:
        Lexer();
        ~Lexer();

        /// @brief Add regex pattern for search. 
        /// @remark Will be top priority and following patterns will go
        // below in priority
        /// @param pattern Character string to use as pattern
        void addTerminal(const std::string& pattern, size_t assignedID);

        /// @brief Pop earliest-parsed token from list of currently stored terminal tokens
        /// @remark Terminal token queue must not be empty
        /// @return Popped terminal token
        Terminal popTerminal();

        /// @brief Get amount of processed terminal tokens
        /// @return Amount of terminal tokens in this parser's processed list
        const std::list<Terminal>& getTerminalQueue();

        /// @brief Processes terminal tokens from binded stream until it runs out
        /// @return The amount of processed terminal tokens
        virtual size_t processTerminals() = 0;

};

#endif
