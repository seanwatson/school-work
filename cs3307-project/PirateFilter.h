#ifndef PIRATEFILTER_H
#define PIRATEFILTER_H

#include <map>
#include <string>

#include "DocumentFilter.h"
#include "WordIterator.h"

class PirateFilter : public DocumentFilter
{
	public:
		PirateFilter();
		
		virtual void apply(Document& doc);

	private:
		std::map<std::string, std::string> dictionary;
};

#endif
