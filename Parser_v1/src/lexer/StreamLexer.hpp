#ifndef STREAM_LEXER_HPP
#define STREAM_LEXER_HPP

// Text buffer capacity for incoming bytes
#define LEXER_QUEUE_DEFAULT_CAPACITY 200
#define LEXER_QUEUE_PULL_SIZE 10

// Default-assigned token ID
#define NULL_TOKEN_ID 0

// LIFO, dynamically allocated container
#include <deque>

// Iterable, dynamically allocated container
#include <list>

// RegEx matching, we'll be using boost's regex library, as an standalone version
// Make sure to compile with (on g++ 11) the flag -Isrc/boost_regex/ on the parent directory
// Alternatively, have the proper Boost version installed
#define BOOST_REGEX_STANDALONE 1
#include "boost/regex.hpp"

// Socket connectivity
#include "Socket.hpp"

// TODO(me): Documentar apropiadamente
/// @brief Token class for produced tokens by lexer
class Token
{
    // The StreamLexer may modify a lexer
    friend class StreamLexer;

    protected:
        /// @brief String of characters admitted as a token
        std::string text;

        /// @brief ID assigned to token
        size_t id;

        /// @brief Construct a token with a raw text and ID assigned to it
        /// @param rawText Raw text to be stored inside token
        /// @param assignedID ID assigned to this token
        Token(const std::string& rawText, size_t assignedID);

        /// @brief Assign a character string to this token
        void setText(const std::string& inputText);

        /// @brief Assign an ID to this token
        void setID(size_t inputID);

    public:
        Token();
        ~Token();

        /// @brief Get copy of character string associated with token
        /// @remark Not the RegEx pattern, but the matching text
        /// @return String associated with this particular token
        std::string getText() const;

        /// @brief Get copy of the ID associated with the Token
        size_t getID() const;

};

/// @brief Pseudo lexer class for tokens on a buffered stream
class StreamLexer
{
    private:
        /// @brief Queue of currently processed tokens
        std::deque<Token> tokenQueue;

        /// @brief Priority list (higher-to-lowest) of regex expressions
        // with corresponding ID to assign to tokens
        std::list<std::pair<boost::regex, size_t>> regexPriorityList;

        /// @brief Input stream to pull characters from
        std::istream* inputStream;

        /// @brief Buffer of read bytes from stream
        char charBuffer[LEXER_QUEUE_DEFAULT_CAPACITY];

        /// @brief Index of the "head" of currently read bytes string
        /// @remark Its equivalent to the amount of currently considered
        size_t bufferHeadIndex;

        /// @brief Extract text from buffer by processing tokens from it
        /// @return Amount of processed tokens
        size_t extractTokensInBuffer();

    public:
        StreamLexer();
        ~StreamLexer();

        /// @brief Bind an input stream to pull characters out of
        /// @remark After binding, the input stream will be modified 
        /// on pulls, and should not be destructed unless replaced by 
        /// another input stream before further pulls
        /// @param inputStream Input stream to pull from (reference)
        void bind(std::istream& inputStream);

        /// @brief Add regex pattern for search. 
        /// @remark Will be top priority and following patterns will go
        // below in priority
        /// @param pattern Character string to use as pattern
        void addRegex(const std::string& pattern, size_t assignedID);

        /// @brief Processes tokens from binded stream until it runs out
        /// @return The amount of processed tokens
        size_t processTokens();

        /// @brief Print list of currently stored tokens. Mentions each token's ID and its value
        void printTokenList();

        /// @brief Get amount of processed tokens
        /// @return Amount of tokens in this parser's processed list
        size_t getTokenListSize();

        /// @brief Pop earliest-parsed token from list of currently stored tokens
        /// @remark Token queue must not be empty
        /// @return Popped token
        Token popToken();
};

#endif
