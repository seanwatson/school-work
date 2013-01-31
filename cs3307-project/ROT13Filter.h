#ifndef ROT13FILTER_H
#define ROT13FILTER_H

#include "DocumentFilter.h"
#include "WordIterator.h"

class ROT13Filter : public DocumentFilter
{
	public:
		virtual void apply(Document& doc);
};

#endif
