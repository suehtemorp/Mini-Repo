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

    std::cout << "Tokens processing..." << std::endl;

    lexLutor.bind(file);
    lexLutor.processTerminals();

    std::cout << "Tokens list:" << std::endl;
    
    for (auto terminal : lexLutor.getTerminalQueue())
    {
        std::cout << "ID : " << terminal.getID() << std::endl
        << "-> Text : " << terminal.getText() << std::endl;
    }

    return 0;
}
