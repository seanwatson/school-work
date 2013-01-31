#include "UppercaseFilter.h"

void UppercaseFilter::apply(Document& doc)
{
	for(WordIterator it = doc.begin(); it != doc.end(); ++it)
	{
		for(int i=0; i < it->length(); ++i)
		{
			if(it->at(i) >= 'a' && it->at(i) <= 'z')
			{
				it->at(i) += 'A' - 'a';
			}
		}
	}
	return;
}
