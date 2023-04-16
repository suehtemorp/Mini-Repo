#ifndef PARSER_HPP
#define PARSER_HPP

// Lexer and token definitions
#include "StreamLexer.hpp"

// Iterable, dynamically allocated container
#include <list>

// Reference-counted resource wrapper (avoid unnecesary copies)
#include <memory>

// Convienent forward declaration
class Parser;

class CompoundToken : public Token
{
    // The Parser may modify a compound token
    friend class Parser;

    private:
        // List of references to children compound tokens
        std::list<std::shared_ptr<CompoundToken>> children;

        /// @brief Add a children instanciated from a copy-able token
        /// @param copy Const-reference to copy-able token
        void copyAsChildren(const Token& copy);

        /// @brief Add a children instanciated from a copy-able compoud token
        /// @remark Since compound tokens keep track of their children with reference,
        /// counting, the copy's children will have their reference count increased
        /// @param copy Const-reference to copy-able compound token
        void copyAsChildren(std::shared_ptr<CompoundToken> copy);

    public:
        CompoundToken();
        CompoundToken(const Token& token);

        ~CompoundToken();

        const std::list<std::shared_ptr<CompoundToken>>& getChildren();
};

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
        std::list<std::shared_ptr<CompoundToken>> roots;

        /// @brief Reference to lexer for token extraction
        StreamLexer* lexer;

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
        const std::list<std::shared_ptr<CompoundToken>>& getRoots();

        /// @brief Bind a lexer to the parser
        /// @remark The lexer will be emptied out after processing tokens. Reference must stay valid until
        /// processing is done.
        /// @param lexerReference Lexer reference
        void bind(StreamLexer& lexerReference);

        /// @brief Process tokens from lexer's queue to create parse tree
        /// @remark It will empty the previously binded lexer's queue. Reference must be valid.
        void processTokens();
};

#endif
