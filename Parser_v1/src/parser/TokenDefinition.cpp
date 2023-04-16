#include "TokenDefinition.hpp"

// TOKEN SEQUENCE NODE

TokenSequenceNode::TokenSequenceNode(size_t id, size_t min, size_t max)
: requiredID(id), requiredMin(min), requiredMax(max)
{}

TokenSequenceNode::~TokenSequenceNode()
{}

size_t TokenSequenceNode::getMin() const
{return this->requiredMin;}

size_t TokenSequenceNode::getMax() const
{return this->requiredMax;}

size_t TokenSequenceNode::getID() const
{return this->requiredID;}

// TOKEN SEQUENCE

TokenSequence::TokenSequence(const std::list<TokenSequenceNode>& nodes)
: requiredNodes(nodes)
{}

TokenSequence::~TokenSequence()
{}

const std::list<TokenSequenceNode>& TokenSequence::getRequiredNodes() const
{return this->requiredNodes;}

// Token definition

TokenDefinition::TokenDefinition(const std::list<TokenSequence>& nodeSequences)
: validSequences(nodeSequences)
{}

TokenDefinition::~TokenDefinition()
{}

const std::list<TokenSequence>& TokenDefinition::getValidSequences() const
{return this->validSequences;}
