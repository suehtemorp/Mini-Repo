#include "Parser.hpp"

Parser::Parser() 
: lexer(nullptr)
{}

Parser::~Parser()
{}

Parser& Parser::addProduction(size_t assignedID, const TokenDefinition& tokenDefinition)
{
    // Forbid adding empty definitions (no token sequences)
    if (tokenDefinition.getValidSequences().empty())
        throw std::runtime_error("Adding token definition without any sequence is forbidden");

    // Forbid adding empty sequences as definitions
    for (auto sequence : tokenDefinition.getValidSequences())
    {
        if (sequence.getRequiredNodes().size() == 0)
            throw std::runtime_error("Adding token definition with any empty sequence is forbidden");
    }

    // Check if a definition already exists on the definition list, and where.
    auto itDefinition = this->definitionsMap.find(assignedID);

    // If it doesn't exist, add a new definition altogether to the end of the list
    if (itDefinition == this->definitionsMap.end())
    {
        // Add it to the list of definitions' matrices
        this->definitionsPriorityList.push_back
        (
            // Add an entry with the assigned ID, and definition matrix associated with it
            std::pair<size_t, TokenDefinition>(assignedID, tokenDefinition)
        );

        // Finally, add a record of the iterator to the definition map
        this->definitionsMap[assignedID] =
            // Element before the element past the end of the list
            // Yes, this is an awkward work-around to get the end of the list
            std::prev(this->definitionsPriorityList.end()); 
    }

    // If it does exist, simply substitute the old definition with the new one
    else 
        itDefinition // Access token interator on map
            ->second // Access token iterator on list
                ->second = tokenDefinition; // Access token definition (productions)

    // Return self to chain method
    return *this;
}

const TokenDefinition& Parser::getProduction(size_t assignedID)
{
    // Attempt to find the current definition matrix for the provided token ID
    auto whereProduction = this->definitionsMap.find(assignedID);

    // If it doesn't exist, panic. This is fatal
    if (whereProduction == this->definitionsMap.end())
        throw std::runtime_error("Cannot access definition for undefined production");

    // Otherwise, return a read-only (implicitly casted) reference to the definitions
    return 
        whereProduction // De-reference map iterator to obtain list iterator
            ->second // De-reference list iterator to obtain definition entry
                ->second; // De-reference definition entry to obtain definition matrix
}

const std::list<std::shared_ptr<NonTerminal>>& Parser::getRoots()
{return this->roots;}

void Parser::bind(Lexer& lexerReference)
{this->lexer = &lexerReference;}

bool Parser::compoundOneToken()
{
    // If there are no roots, no compounding can take place
    if (this->roots.empty())
        return false;
    
    // Attempt matches until one is a hit, or all definitions have been exhausted
    bool compoundingDone = false;

    // First, iterate over all definition matrices (group of productions for the same token)
    for (auto nonTerminalEntry : this->definitionsPriorityList)
    {
        // If compounding is done, don't continue
        if (compoundingDone)
            break;

        // Iterate over each row (production) of a given token's matrix
        const size_t& currentNonTerminalID = nonTerminalEntry.first;
        const TokenDefinition& currentNonTerminalDef = nonTerminalEntry.second;

        for (auto currentProd : currentNonTerminalDef.getValidSequences())
        {
            // If compounding is done, don't continue
            if (compoundingDone)
                break;

            // Keep track of the current production's definition sequence
            const auto& currentDef = currentProd.getRequiredNodes();

            // Keep track of the definition's length, and a count of appearances of each token
            const size_t defTokenLength = currentProd.getRequiredNodes().size();
            size_t* tokenCountTracker = (size_t*) calloc(defTokenLength, sizeof(size_t));

            // Walk the production's definition, and the tokens on the roots (top-level expression)
            auto rootIt = this->roots.cbegin();
            auto defIt = currentDef.cbegin();

            // Keep track of the last accepted token's position in the expression
            decltype(rootIt) lastMatch;

            // Repeat the following while iterators for definition and expression are both bounded
            while (rootIt != this->roots.cend() && defIt != currentDef.cend())
            {
                // Take hold of the definition's currently examined token's appearance count
                size_t& appearancesSoFar = 
                    tokenCountTracker[std::distance(currentDef.cbegin(), defIt)];

                // If there's a match...
                if (rootIt->get()->getID() == defIt->getID())
                {
                    // If the definition's currently examined token's maximum would be surpassed when incrementing
                    // its token count, examine the next token in the definition
                    if (appearancesSoFar + 1 > defIt->getMax())
                        ++defIt;
                    
                    // Otherwise (if it wouldn't), increment its token count and examine the next token in the 
                    // expression
                    else
                    {
                        ++appearancesSoFar;
                        ++rootIt;
                    }

                    // Update the position of the last match
                    lastMatch = rootIt;
                }

                // Else (if there's a mismatch)
                else
                {
                    // If the the definition's currently examined token minimum has been reached, examine the next
                    // token in the definition
                    if (appearancesSoFar >= defIt->getMin())
                        ++defIt;

                    // Otherwise (if it hasn't), examine the next token in the expression
                    else
                        ++rootIt;
                }
            }

            // If the definition's token iterator is past the end, or if the definition's token 
            // iterator is at the end, and its minimum token count is satisfied, there's also match
            if 
            (
                // Past the end
                (defIt == currentDef.cend())
                |
                (
                    // At the end, not past it
                    defIt == std::prev(currentDef.cend())
                    // Yet, the token count is greater or equal than its minimum
                    && tokenCountTracker[std::distance(currentDef.cbegin(), defIt)] >= defIt->getMin() 
                )
            )
            {
                // The matching token sequence encompasses all matched tokens behind the
                // last match's position
                size_t matchedTokens = 0;
                for (size_t i = 0; i < defTokenLength; ++i) matchedTokens += tokenCountTracker[i];

                auto firstMatch = lastMatch;
                for (size_t i = 0; i < matchedTokens; ++i) --firstMatch;

                // Create a child compound token to hold the tokens
                std::shared_ptr<NonTerminal> newRoot(new NonTerminal());
                newRoot->setID(currentNonTerminalID); // Useful for future parsing
                newRoot->setText(std::to_string(currentNonTerminalID)); // Useful for identifying

                // Then, steal the contents from the roots sublist and assign them to
                // this compound token as its children
                newRoot->children.splice
                (
                    newRoot->children.cbegin(), // Insert at beginning
                    this->roots, // Steal from the root list
                    firstMatch, // Starting from the beggining of the sublist match
                    lastMatch // And stopping before the end (end as in "one position past the end")
                );

                // Finally, reinsert the commpound token onto the root list behind the last
                // match's position
                this->roots.insert(lastMatch, newRoot);

                // Finally, indicate the compounding was done
                compoundingDone = true;
            }

            // Free the token count tracker
            free(tokenCountTracker);
        }
    }

    // Indicate if compounding done
    return compoundingDone;
}

void Parser::processTokens()
{
    // Keep track of the total amount of terminal tokens
    size_t totalTerminalTokens = lexer->getTerminalQueue().size();

    // Repeat the parse cycle until no new terminal tokens can be
    // extracted
    for (size_t terminalCounter = 0; terminalCounter < totalTerminalTokens; 
    ++terminalCounter)
    {
        // Since there's a new terminal token avalaible, add it as a new root
        this->roots.push_back
        (
            // Wrap the compound token as a reference
            std::shared_ptr<NonTerminal>
            (
                // Allocate and construct compound token from terminal token
                new NonTerminal(this->lexer->popTerminal())
            )
        );

        // Try to compound the roots until exhaustion
        // The brackets after the while are unnecesary but help avoid accidental 
        // syntax erros
        while (this->compoundOneToken()){;}
    }

    // All done! Tree should be created. Let's pray it works as expected :')
}
