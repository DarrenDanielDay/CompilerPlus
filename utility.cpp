#include "utility.h"
#include <string>

CompilerPlus::Utility::Iterator<std::string>* CompilerPlus::Utility::StringSequence::GetIterator()
{
	return this;
}

std::string* CompilerPlus::Utility::StringSequence::Next()
{
	return this->strs[this->index++];
}

bool CompilerPlus::Utility::StringSequence::HasNext()
{
	return this->index < this->size;
}
