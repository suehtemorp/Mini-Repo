#ifndef STREAM_LEXER_HPP
#define STREAM_LEXER_HPP

// Lexer interface
#include "Lexer.hpp"

// Input stream connectivity
#include <istream>

/// @brief Pseudo lexer class for tokens on a buffered stream
class StreamLexer : Lexer
{
    private:

        /// @brief Input stream to pull characters from
        std::istream* inputStream;

        /// @brief Buffer of read bytes from stream
        char* charBuffer;

        /// @brief Index of the "head" of currently read bytes string
        /// @remark Its equivalent to the amount of currently considered
        size_t bufferHeadIndex;

        /// @brief Extract text from buffer by processing tokens from it
        /// @return Amount of processed tokens
        size_t extractTerminalsInBuffer();

    public:
        StreamLexer();
        ~StreamLexer();

        /// @brief Bind an input stream to pull characters out of
        /// @remark After binding, the input stream will be modified 
        /// on pulls, and should not be destructed unless replaced by 
        /// another input stream before further pulls
        /// @param inputStream Input stream to pull from (reference)
        void bind(std::istream& inputStream);

        /// @brief Processes tokens from binded stream until it runs out
        /// @return The amount of processed tokens
        size_t processTerminals();
};

#endif
