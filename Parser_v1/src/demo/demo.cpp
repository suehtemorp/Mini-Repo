#include <iostream>
#include <fstream>

#include "StreamLexer.hpp"
#include "Parser.hpp"

// HTML Grammar configuration

void htmlConfigLexer(Lexer& lexer)
{
    lexer.addTerminal("Javier", 1);
    lexer.addTerminal("Carlos", 2);
    lexer.addTerminal("Jesus", 3);

    return;
}

void htmlConfigParser(Parser& parser)
{
    parser.addProduction
    (
        // NonTerminal ID
        4, 

        // Non Terminal Definition Object
        {
            // Non Terminal Definition
            {
                // List of valid Token Sequences
                {
                    // Valid Token Sequence
                    {
                        // Token Node 
                        {
                            1, // Token ID
                            1, // Minimum of appearances
                            1 // Maximum of appearances
                        },

                        // Token Node 
                        {
                            2, // Token ID
                            1, // Minimum of appearances
                            2 // Maximum of appearances
                        }
                    }
                }
            }
        }
    );

    return;
}

// Data visualization

void printLexerTerminals(Lexer& lexer)
{
    std::cout << "Terminals list:" << std::endl;
    
    for (auto terminal : lexer.getTerminalQueue())
    {
        std::cout << "ID: " << terminal.getID() << std::endl
        << "-> Text: " << terminal.getText() << std::endl;
    }

    return;
}

void printTreeRecursively(const std::shared_ptr<NonTerminal>& relativeRoot, size_t indentLevel = 0)
{
    const std::string indentation(indentLevel, ' ');

    std::cout << indentation 
    <<  "[ID = " << relativeRoot->getID() << " | TEXT = " << relativeRoot->getText() << "]" 
    << std::endl;

    if (relativeRoot->getChildren().size() > 0)
    {
        std::cout << indentation << "{" << std::endl;
        for (auto child : relativeRoot->getChildren())
            printTreeRecursively(child, indentLevel + 4);
        std::cout << indentation << "}" << std::endl;
    }

    return;
}

void printParserTree(Parser& parser)
{
    std::cout << "Parse tree:" << std::endl;

    for(auto root : parser.getRoots())
        printTreeRecursively(root);

    return;
}

// Main routine

int main()
{
    // Open file
    std::cout << "Opening file..." << std::endl;

    std::ifstream file; 
    file.open("./parseableFile.txt", std::fstream::in);

    if (!file.is_open())
    {
        std::cerr << "Unable to open file" << std::endl;
        return -1;
    }

    // Lexing
    std::cout << "Lexing..." << std::endl;

    StreamLexer lexLutor;
    lexLutor.bind(file);
    htmlConfigLexer(lexLutor);

    lexLutor.processTerminals();
    printLexerTerminals(lexLutor);

    // Parsing
    std::cout << "Parsing..." << std::endl;

    Parser doctorParser;
    doctorParser.bind(lexLutor);
    htmlConfigParser(doctorParser);
    
    doctorParser.processTokens();
    printParserTree(doctorParser);

    // Close file
    file.close();

    // All done
    return 0;
}
