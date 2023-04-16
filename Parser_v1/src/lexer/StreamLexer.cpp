#include "StreamLexer.hpp"

// Text buffer capacity for incoming bytes
#define STR_LEXER_BUFF_CAPACITY 200
#define STR_LEXER_PULL_CAPACITY 10

StreamLexer::StreamLexer() : bufferHeadIndex(0)
{this->charBuffer = new char[STR_LEXER_BUFF_CAPACITY];}

StreamLexer::~StreamLexer()
{delete[] this->charBuffer;}

void StreamLexer::bind(std::istream& inputStream)
{this->inputStream = &inputStream;}

size_t StreamLexer::extractTerminalsInBuffer()
{
    // Keep track of to-be-lexed terminals, and fully processed terminals so far
    std::list<Terminal> disjointTerminals;
    size_t terminalsProcessed = 0;

    // Kickstart the terminal analysis by introducing a block of to-be-lexed terminal
    // This terminal contains the entire buffer's text, and is null by default
    disjointTerminals.push_back
    (
        Terminal
        (
            std::string(this->charBuffer, this->bufferHeadIndex),
            NULL_TOKEN_ID
        )
    );

    // Time to analyze the terminals in an analysis loop
    // Store RegEx match info, and if there is a pending match, on a buffer
    boost::smatch matchInfoBuffer;
    bool pendingPartialMatch = false;

    // Analysize terminals from left to right (start to end)
    auto segmentIterator = disjointTerminals.begin();
    while (segmentIterator != disjointTerminals.end())
    {
        // If terminal has already been lexed, skim to the next one
        if (segmentIterator->getID() != NULL_TOKEN_ID)
        {
            // Push terminal to queue and update terminal count
            terminalsProcessed += 1;
            this->terminalQueue.push_back(*segmentIterator);

            // Skim to the next one
            ++segmentIterator;
            continue;
        }

        // Otherwise, analyze the current text and keep track if it was analyzed
        // as a full terminal
        bool terminalAnalyzed = false;
        std::string textAnalyzed = segmentIterator->getText();

        // Check for RegEx matches according to the priority list
        for (auto terminalDefinition : this->regexPriorityList)
        {
            // Keep track of terminal information
            const boost::regex& terminalRegex = terminalDefinition.first;
            const size_t& terminalID = terminalDefinition.second;

            // Scan for possible matches for this current RegEx pattern
            bool mayMatch = boost::regex_search
                (textAnalyzed, matchInfoBuffer, terminalRegex, boost::match_partial);

            // Only consider analysis if a match is possible
            if (mayMatch)
            {
                // Fully process this match if a full one was found
                if (matchInfoBuffer[0].matched)
                {
                    // Split text into before and after detected match
                    std::string textBefore = segmentIterator
                        ->getText().substr(0, matchInfoBuffer.position());
                    std::string textAfter = segmentIterator
                        ->getText().substr(matchInfoBuffer.position() + matchInfoBuffer.length()
                        , segmentIterator->getText().length() - (matchInfoBuffer.position() + matchInfoBuffer.length()));
                    
                    // Insert before and after text as terminals to the left and right of this one
                    disjointTerminals.insert(segmentIterator, Terminal(textBefore, NULL_TOKEN_ID));
                    disjointTerminals.insert(std::next(segmentIterator), Terminal(textAfter, NULL_TOKEN_ID));

                    // Set terminal's ID and text to the corresponding matching text
                    // When we inevitably revisit it once, we will add it to the queue                    
                    segmentIterator->setID(terminalID);
                    segmentIterator->setText(matchInfoBuffer.str());

                    // Try lexing left terminal
                    --segmentIterator;
                    terminalAnalyzed = true;
                }

                // Otherwise, if a partial match has been found, abort processing
                // if there are no following terminals
                else if (std::next(segmentIterator) == disjointTerminals.end())
                {
                    // If a partial match has been found with the whole buffer partition,
                    // it means we found a partial match that requires more capacity
                    // than available on the buffer to be resovled. This is fatal
                    if (segmentIterator->getText().size() == STR_LEXER_BUFF_CAPACITY)
                        throw std::runtime_error
                            ("Pending match without possible resolution under buffer capacity");

                    /* TODO(me): 
                        * Decide if we can really throw away those characters
                        * What if a lower-priority complete match lies on those, and the higher
                        * priority partial match never resolves to a complete match?
                        // Preserve only the partially matching part
                        std::string partialMatchString = 
                        std::string(matchInfoBuffer[0].first, matchInfoBuffer[0].second);
                        segmentIterator->setText(partialMatchString);
                        */

                    // Update flags accordingly, and carry on freeing some queue capacity
                    pendingPartialMatch = true;
                    terminalAnalyzed = true;
                }
                // Otherwise, just carry on trying lower priority regex on this terminal
            }
            // No matches, not even partial ones, were found for this regex expression
        }

        // Since there's a pending, unresolved partial match, abort processing
        if (pendingPartialMatch)
            break;

        // Otherwise, if the terminal is wasn't analyzed to resolution, remove it from the list
        // and move on to the next terminal (since there's no pending partial match)
        else if (!terminalAnalyzed)
        {
            auto nextSegment = std::next(segmentIterator);
            disjointTerminals.erase(segmentIterator);
            segmentIterator = nextSegment;
        }
    }

    // If there is a pending partial matches past the analysis, we will need to preserve it's text
    // and discard the leftover
    if (pendingPartialMatch)
    {
        const std::string& pendingTerminalText = disjointTerminals.back().getText();
        size_t pendingTerminalLen = pendingTerminalText.length();

        memcpy(this->charBuffer, pendingTerminalText.c_str(), pendingTerminalLen);
        this->bufferHeadIndex =  pendingTerminalLen;
    }

    // Otherwise, we may free the buffer's capacity completely
    else
        this->bufferHeadIndex = 0;
    
    return terminalsProcessed;
}

size_t StreamLexer::processTerminals()
{
    // If no regular expressions are present, no lexing can
    // be done
    if (this->regexPriorityList.empty()) 
        return 0; // No terminals parsed

    // Keep track of amount of terminals processed so far
    size_t processedTerminals = 0;

    // And the leftover capacity on the buffer
    size_t leftOverCapacity = STR_LEXER_BUFF_CAPACITY;

    // Perfom pull cycle until max terminal processing has been reached
    // or no more characters can be pulled
    bool canPull = true;
    while (canPull)
    {
        // Extract matches in buffer if it isn't emptied out yet
        if (this->bufferHeadIndex > 0)
            processedTerminals += this->extractTerminalsInBuffer();

        // Pull characters if there is leftover capacity for pulling characters
        if (STR_LEXER_BUFF_CAPACITY - this->bufferHeadIndex > 0)
        {
            // Calculate pull amount
            size_t pullAmount = std::min
                (STR_LEXER_BUFF_CAPACITY - this->bufferHeadIndex, (size_t) STR_LEXER_PULL_CAPACITY);

            // Pull characters from stream
            this->inputStream->clear();
            this->inputStream->getline(this->charBuffer + bufferHeadIndex, pullAmount);
            size_t charactersPulled = this->inputStream->gcount();

            // If succesful, update queue content and head marker
            if (charactersPulled > 0)
            {
                // Update queue head index
                this->bufferHeadIndex += charactersPulled;

                // Update queue left over capacity
                leftOverCapacity -= charactersPulled;
            }

            // Otherwise, stop the requests for pulling bytes
            else
                canPull = false;
        }

        // If there is no more capacity left in buffer, panic. This is fatal
        else
            throw std::runtime_error("Unsufficient capacity for processing stream");
    };

    // Indicate the amount of processed terminals
    return processedTerminals;
}
