#ifndef WORDITERATOR_H
#define WORDITERATOR_H

#include <string>
#include "Document.h"

class Document;

class WordIterator
{
	public:
		WordIterator(Document* d, int index);

		bool operator==(const WordIterator& other);
		bool operator!=(const WordIterator& other);
		
		WordIterator& operator++();
		WordIterator& operator++(int unused);
		WordIterator& operator--();
		WordIterator& operator--(int unused);
		
		std::string* operator->();
		std::string& operator*();

	private:
		Document* _d;
		int _index;
};

#endif
