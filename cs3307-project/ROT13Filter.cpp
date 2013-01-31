#include "ROT13Filter.h"

void ROT13Filter::apply(Document& doc)
{
	for(WordIterator it = doc.begin(); it != doc.end(); ++it)
	{
		for(unsigned int i=0; i < it->length(); ++i)
		{
			if(isalpha(it->at(i)))
			{
				if((tolower(it->at(i)) - 'a') < 13)
				{
					it->at(i) += 13;
				}
				else
				{
					it->at(i) -= 13;
				}
				
			}
		}
	}
}
