#ifndef TERMINAL_HPP
#define TERMINAL_HPP

// Default-assigned token ID
#define NULL_TOKEN_ID 0

// RegEx matching, we'll be using boost's regex library, as an standalone version
// Alternatively, have the proper Boost version installed
#include <boost/regex.hpp>

// Convenient forward declaration
class Lexer;

/// @brief Terminal token class for produced tokens by lexer
class Terminal
{
    // A lexer may modify a terminal token
    friend class Lexer;

    protected:
        /// @brief String of characters admitted as a terminal token
        std::string text;

        /// @brief ID assigned to token
        size_t id;

    public:
        Terminal();
        ~Terminal();

        /// @brief Construct a terminal token with a raw text and ID assigned to it
        /// @param rawText Raw text to be stored inside token
        /// @param assignedID ID assigned to this token
        Terminal(const std::string& rawText, size_t assignedID);

        /// @brief Assign a character string to this terminal token
        void setText(const std::string& inputText);

        /// @brief Assign an ID to this terminal token
        void setID(size_t inputID);

        /// @brief Get copy of character string associated with terminal token
        /// @remark Not the RegEx pattern, but the matching text
        /// @return String associated with this particular token
        std::string getText() const;

        /// @brief Get copy of the ID associated with the terminal token
        size_t getID() const;
};

#endif
