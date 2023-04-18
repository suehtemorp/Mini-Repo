#include <iostream>
#include <fstream>

#include "StreamLexer.hpp"
#include "Parser.hpp"

int main()
{
    StreamLexer lexLutor;

    lexLutor.addTerminal("Javier", 1);
    lexLutor.addTerminal("Carlos", 2);
    lexLutor.addTerminal("Jesus", 3);

    std::cout << "Opening file..." << std::endl;

    std::ifstream file; 
    file.open("./parseableFile.txt", std::fstream::in);

    if (!file.is_open())
        throw std::runtime_error("Unable to open file");

    std::cout << "Lexing..." << std::endl;

    lexLutor.bind(file);
    lexLutor.processTerminals();

    std::cout << "Tokens list:" << std::endl;
    
    for (auto terminal : lexLutor.getTerminalQueue())
    {
        std::cout << "ID: " << terminal.getID() << std::endl
        << "-> Text: " << terminal.getText() << std::endl;
    }

    Parser doctorParser;

    doctorParser.addProduction
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

    std::cout << "Parsing..." << std::endl;

    doctorParser.bind(lexLutor);
    doctorParser.processTokens();

    std::cout << "Parse tree roots:" << std::endl;

    for(auto nodes : doctorParser.getRoots()) 
    {
        std::cout << "Root ID: " << nodes->getID() << std::endl
        << "-> Root Text: " << nodes->getText() << std::endl;
    }

    return 0;
}
