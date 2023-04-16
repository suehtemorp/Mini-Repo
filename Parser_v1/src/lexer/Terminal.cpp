#include "Terminal.hpp"

Terminal::Terminal() : text(""), id(NULL_TOKEN_ID)
{}

Terminal::Terminal(const std::string& rawText, size_t assignedID)
: text(rawText), id(assignedID)
{}

Terminal::~Terminal()
{}

void Terminal::setText(const std::string& inputText)
{this->text = inputText;}

void Terminal::setID(size_t inputId)
{this->id = inputId;}

std::string Terminal::getText() const
{return this->text;}

size_t Terminal::getID() const
{return this->id;}
