#ifndef PARSER_HPP
#define PARSER_HPP

// Iterable, dynamically allocated container
#include <list>

// Reference-counted resource wrapper (avoid unnecesary copies)
#include <memory>

// Lexer and terminal definitions
#include "Lexer.hpp"

// Non-terminal definitions
#include "NonTerminal.hpp"

/// @brief Shorthand for two-level nested lists (token with multiple definitions)
typedef std::list<std::list<size_t>> TokenIDMatrix;

/// @brief Token parser for compound tokens
class Parser
{
    private:
        ///@brief List of compounded token definitions
        /// @remark Token are defined by a list of sequences of token ID's
        std::list<std::pair<std::size_t, TokenIDMatrix>> 
        definitionsMatrix;

        /// @brief Map of indexes for recursive definitions on the definitions matrix
        /// @remark It returns iterators, allowing read-write access. 
        /// Be careful not to invalidate the map / render it useless.
        std::map<size_t, std::list<std::pair<std::size_t, TokenIDMatrix>>::iterator> 
        definitionsMap;

        /// @brief Highest-level tokens so far
        std::list<std::shared_ptr<NonTerminal>> roots;

        /// @brief Reference to lexer for token extraction
        Lexer* lexer;

        /// @brief Attempts to compound one token at a time from the roots
        /// @remarks It uses the defined productions to do so
        /// @return True if one compounding was performed. False otherwise.
        bool compoundOneToken();

    public:
        Parser();
        ~Parser();

        /// @brief Add a (recursive) production for a token, using sequence of token IDs
        /// @remark Multiple productions for the same token ID are valid.
        /// Productions added first take precedence and priority over later ones.
        /// @param assignedID ID for newly to-be-assigned token
        /// @param tokenIDList List of tokens ID to define this current token
        /// @return Reference to Parser, for method chanining
        Parser& addProduction(size_t assignedID, const std::list<size_t>& tokenIDList);

        /// @brief Get a reference to the production list
        /// @return Read-only reference of token definitions
        const TokenIDMatrix& getProductions(size_t compoundID);

        /// @brief Get a reference to the top-level compound tokens (and subsequently, the whole tree)
        /// @return Read-only reference to list of top-lvel compound tokens
        const std::list<std::shared_ptr<NonTerminal>>& getRoots();

        /// @brief Bind a lexer to the parser
        /// @remark The lexer will be emptied out after processing tokens. Reference must stay valid until
        /// processing is done.
        /// @param lexerReference Lexer reference
        void bind(Lexer& lexerReference);

        /// @brief Process tokens from lexer's queue to create parse tree
        /// @remark It will empty the previously binded lexer's queue. Reference must be valid.
        void processTokens();
};

#endif
