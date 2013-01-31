#include "WordIterator.h"

using std::string;

WordIterator::WordIterator(Document* d, int index) : _d(d), _index(index)
{

}

bool WordIterator::operator==(const WordIterator& other)
{
	return (_d == other._d && _index == other._index)?true:false;
}

bool WordIterator::operator!=(const WordIterator& other)
{
	return (_d == other._d && _index == other._index)?false:true;
}

WordIterator& WordIterator::operator++()
{
	++_index;
	return *this;
}

WordIterator& WordIterator::operator++(int unused)
{
	WordIterator tmp(_d, _index);
	++_index;
	return tmp;
}

WordIterator& WordIterator::operator--()
{
	--_index;
	return *this;
}

WordIterator& WordIterator::operator--(int unused)
{
	WordIterator tmp(_d, _index);
	--_index;
	return tmp;
}

string* WordIterator::operator->()
{
	return &(_d->wordAt(_index));
}

string& WordIterator::operator*()
{
	return _d->wordAt(_index);
}
