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
    for (auto matrixIt = this->definitionsPriorityList.cbegin(); 
        matrixIt != this->definitionsPriorityList.cend() && !compoundingDone; ++matrixIt)
    {
        // Then, iterate over each row (production) of a given token's matrix
        const size_t& currentCompoundID = matrixIt->first;
        const TokenDefinition& currentMatrix = matrixIt->second;

        for (auto prodIt = currentMatrix.getValidSequences().cbegin(); 
            prodIt != currentMatrix.getValidSequences().cend() && !compoundingDone; ++prodIt)
        {
            // Keep track of the current production and its' length
            const TokenSequence& currentProd = *prodIt;
            size_t currentProdLength = currentProd.getRequiredNodes().size();

            // Finally, try matches starting at each point of the roots list
            // Iterate over the roots list's starting points
            for (auto rootIt = this->roots.begin(); 
                rootIt != this->roots.end() && !compoundingDone
                // Reject analysis if the production overflows on the list
                && currentProdLength <= std::distance(rootIt, this->roots.end());
                // Precendence is done left-to-right (start to end)
                ++rootIt
            )
            {
                // Try walking from start point to end point on the roots
                auto rootItWalk = rootIt;
                auto prodItWalk = currentProd.getRequiredNodes().cbegin();

                // Abort walk if there's a token sequence mismatch
                bool mismatchFound = false;

                // Walk at most an amount of steps the size of the production length
                for (size_t walkedSteps = 0; walkedSteps < currentProdLength 
                    && !mismatchFound; ++walkedSteps)
                {
                    // If the iterators mismatch on token ID, the sequences diverge
                    // Therefore, there would be a mismatch
                    if (rootItWalk->get()->getID() != prodItWalk->getID())
                        mismatchFound = true;
                    
                    // Otherwise, keep walking
                    else
                    {
                        ++rootItWalk; 
                        ++prodItWalk;
                    }
                }

                // If there was no mismatch, well... There's a match xD
                // We can safely bundle the root tokens in the match into a compound token itself!
                if (!mismatchFound)
                {
                    // First, craft the compound token with its properly assigned ID
                    std::shared_ptr<NonTerminal> newRoot(new NonTerminal());
                    newRoot->setID(currentCompoundID);
                    newRoot->setText(std::to_string(currentCompoundID)); // Useful for identifying

                    // Then, steal the contents from the roots sublist and assign them to
                    // this compound token as its children
                    newRoot->children.splice
                    (
                        newRoot->children.cbegin(), // Insert at beginning
                        this->roots, // Steal from the root list
                        rootIt, // Starting from the beggining of the sublist match
                        rootItWalk // And stopping before the end (one position past the end)
                    );

                    // Insert compound token to the roots
                    this->roots.insert(rootItWalk, newRoot);

                    // All done, we may now stop the entire process (phew)
                    compoundingDone = true;
                }
            }
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
