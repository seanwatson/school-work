#ifndef UPPERCASEFILTER_H
#define UPPERCASEFILTER_H

#include "DocumentFilter.h"
#include "WordIterator.h"

class UppercaseFilter : public DocumentFilter
{
	public:
		virtual void apply(Document& doc);
};

#endif
