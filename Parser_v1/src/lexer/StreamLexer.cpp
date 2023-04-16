#include "StreamLexer.hpp"

/// TOKEN ///

Token::Token() : text(""), id(NULL_TOKEN_ID)
{}

Token::Token(const std::string& rawText, size_t assignedID)
: text(rawText), id(assignedID)
{}

Token::~Token()
{}

void Token::setText(const std::string& inputText)
{this->text = inputText;}

void Token::setID(size_t inputId)
{this->id = inputId;}

std::string Token::getText() const
{return this->text;}

size_t Token::getID() const
{return this->id;}

/// STREAM LEXER ///

StreamLexer::StreamLexer() : bufferHeadIndex(0)
{}

StreamLexer::~StreamLexer()
{}

void StreamLexer::bind(std::istream& inputStream)
{this->inputStream = &inputStream;}

void StreamLexer::addRegex(const std::string& pattern, size_t assignedID)
{
    // Assigning a token the NULL ID is forbidden. Only the lexer may do it
    if (assignedID == NULL_TOKEN_ID)
        throw std::runtime_error("Assigning NULL ID to token by hand is forbidden");

    this->regexPriorityList.emplace_back(pattern, assignedID);
}

size_t StreamLexer::extractTokensInBuffer()
{
    // Keep track of to-be-lexed tokens, and fully processed tokens so far
    std::list<Token> disjointTokens;
    size_t tokensProcessed = 0;

    // Kickstart the token analysis by introducing a block of to-be-lexed token
    // This token contains the entire buffer's text, and is null by default
    disjointTokens.push_back
    (
        Token
        (
            std::string(this->charBuffer, this->bufferHeadIndex)
            , NULL_TOKEN_ID
        )
    );

    // Time to analyze the tokens in an analysis loop
    // Store RegEx match info, and if there is a pending match, on a buffer
    boost::smatch matchInfoBuffer;
    bool pendingPartialMatch = false;

    // Analysize tokens from left to right (start to end)
    auto segmentIterator = disjointTokens.begin();
    while (segmentIterator != disjointTokens.end())
    {
        // If token has already been lexed, skim to the next one
        if (segmentIterator->id != NULL_TOKEN_ID)
        {
            // Push token to queue and update token count
            tokensProcessed += 1;
            this->tokenQueue.push_back(*segmentIterator);

            // Skim to the next one
            ++segmentIterator;
            continue;
        }

        // Otherwise, analyze the current text and keep track if it was analyzed
        // as a full token
        bool tokenAnalyzed = false;
        std::string textAnalyzed = segmentIterator->text;

        // Check for RegEx matches according to the priority list
        for (auto regexIterator = this->regexPriorityList.cbegin()
            ; (regexIterator != this->regexPriorityList.cend()) && !tokenAnalyzed
            ; ++regexIterator)
        {
            // Scan for possible matches for this current RegEx pattern
            bool mayMatch = boost::regex_search
                (textAnalyzed, matchInfoBuffer, regexIterator->first, boost::match_partial);

            // Only consider analysis if a match is possible
            if (mayMatch)
            {
                // Fully process this match if a full one was found
                if (matchInfoBuffer[0].matched)
                {
                    // Split text into before and after detected match
                    std::string textBefore = segmentIterator
                        ->text.substr(0, matchInfoBuffer.position());
                    std::string textAfter = segmentIterator
                        ->text.substr(matchInfoBuffer.position() + matchInfoBuffer.length()
                        , segmentIterator->text.length() - (matchInfoBuffer.position() + matchInfoBuffer.length()));
                    
                    // Insert before and after text as tokens to the left and right of this one
                    disjointTokens.insert(segmentIterator, Token(textBefore, NULL_TOKEN_ID));
                    disjointTokens.insert(std::next(segmentIterator), Token(textAfter, NULL_TOKEN_ID));

                    // Set token's ID and text to the corresponding matching regex
                    // When we inevitably revisit it once, we will add it to the queue
                    segmentIterator->setID(regexIterator->second);
                    segmentIterator->setText(matchInfoBuffer.str());

                    // Try lexing left token
                    --segmentIterator;
                    tokenAnalyzed = true;
                }

                // Otherwise, if a partial match has been found, abort processing
                // if there are no following tokens
                else if (std::next(segmentIterator) == disjointTokens.end())
                {
                    // If a partial match has been found with the whole buffer partition,
                    // it means we found a partial match that requires more capacity
                    // than available on the buffer to be resovled. This is fatal
                    if (segmentIterator->text.size() == LEXER_QUEUE_DEFAULT_CAPACITY)
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
                    tokenAnalyzed = true;
                }
                // Otherwise, just carry on trying lower priority regex on this token
            }
            // No matches, not even partial ones, were found for this regex expression
        }

        // Since there's a pending, unresolved partial match, abort processing
        if (pendingPartialMatch)
            break;

        // Otherwise, if the token is wasn't analyzed to resolution, remove it from the list
        // and move on to the next token
        else if (!tokenAnalyzed)
        {
            auto nextSegment = std::next(segmentIterator);
                disjointTokens.erase(segmentIterator);
            segmentIterator = nextSegment;
        }
    }

    // If there is a pending partial matches past the analysis, we will need to preserve it's text
    // and discard the leftover
    if (pendingPartialMatch)
    {
        size_t pendingTokenLen = disjointTokens.back().text.length();

        memcpy(this->charBuffer, disjointTokens.back().text.c_str(), pendingTokenLen);
        this->bufferHeadIndex =  pendingTokenLen;
    }

    // Otherwise, we may free the buffer's capacity completely
    else
        this->bufferHeadIndex = 0;
    
    return tokensProcessed;
}

size_t StreamLexer::processTokens()
{
    // If no regular expressions are present, no lexing can
    // be done
    if (this->regexPriorityList.empty()) 
        return 0; // No tokens parsed

    // Keep track of amount of tokens processed so far
    size_t processedTokens = 0;

    // And the leftover capacity on the buffer
    size_t leftOverCapacity = LEXER_QUEUE_DEFAULT_CAPACITY;

    // Perfom pull cycle until max token processing has been reached
    // or no more characters can be pulled
    bool canPull = true;
    while (canPull)
    {
        // Check for matches if buffer isn't emptied out yet
        if (this->bufferHeadIndex > 0)
            processedTokens += this->extractTokensInBuffer();

        // Pull characters if there is leftover capacity for pulling characters
        if (LEXER_QUEUE_DEFAULT_CAPACITY - this->bufferHeadIndex > 0)
        {
            // Calculate pull amount
            size_t pullAmount = std::min
                (LEXER_QUEUE_DEFAULT_CAPACITY - this->bufferHeadIndex, (size_t) LEXER_QUEUE_PULL_SIZE);

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

    // Indicate the amount of processed tokens
    return processedTokens;
}

void StreamLexer::printTokenList()
{
    for (auto it = this->tokenQueue.cbegin(); 
        it != this->tokenQueue.cend(); ++it)
        printf("[ID = %zu] | [VAL = \"%s\"]\n", it->id, it->text.c_str());
}

size_t StreamLexer::getTokenListSize()
{return this->tokenQueue.size();}

Token StreamLexer::popToken()
{
    if (this->tokenQueue.empty())
        throw std::runtime_error("Cannot pop empty token queue");
    
    Token firstToken = this->tokenQueue.front();
    this->tokenQueue.pop_front();
    return firstToken;
}
