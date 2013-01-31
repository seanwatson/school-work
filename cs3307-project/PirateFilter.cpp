#include "PirateFilter.h"
#include <fstream>
#include <iostream>

using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::map;
using std::pair;

PirateFilter::PirateFilter()
{
	ifstream in_file;
	in_file.open("pirate.dict");
	string english, pirate;

	if(in_file.fail())
	{

	}
	else
	{
		while(!in_file.eof())
		{
			in_file >> english;
			in_file.ignore();
			getline(in_file, pirate);
			dictionary.insert(pair<string,string>(string(english),string(pirate)));
		}
		in_file.close();
	}

	for(map<string,string>::iterator it = dictionary.begin(); it != dictionary.end(); ++it)
	{
		cout << it->first << " " << it->second << endl;
	}
}

void PirateFilter::apply(Document& doc)
{
	map<string,string>::iterator dict_it;
	
	for(WordIterator it = doc.begin(); it != doc.end(); ++it)
	{
		dict_it = dictionary.find(*it);
		if(dict_it != dictionary.end())
		{
			*it = dict_it->second;
		}
	}
}
