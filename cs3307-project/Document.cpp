#include "Document.h"
#include <fstream>

using std::string;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::endl;

Document::Document(const std::string filename) : _filename(filename)
{
}

Document::~Document()
{
	for(vector<DocumentFilter*>::iterator it = _filters.begin(); it != _filters.end(); ++it)
	{
		delete *it;
	}
}

bool Document::load()
{
	ifstream in_file;
	string next_word;
	
	in_file.open(_filename.c_str());

	if(in_file.fail())
	{
		return false;
	}
	else
	{
		while(!in_file.eof())
		{
			in_file >> next_word;
			_words.push_back(string(next_word));
		}

		in_file.close();
	}
	return true;
}

bool Document::save(const string& filename)
{
	ofstream out_file;
	int char_count = 0;
	
	for(vector<DocumentFilter*>::iterator it = _filters.begin(); it != _filters.end(); ++it)
	{
		(*it)->apply(*this);
	}

	out_file.open(filename.c_str());

	if(out_file.fail())
	{
		return false;
	}
	else
	{
		for(WordIterator it = this->begin(); it != this->end(); ++it)
		{
			out_file << *it;
			char_count += it->length();


			if(char_count > 80)
			{
				out_file << endl;
				char_count = 0;
			}
			else
			{
				out_file << " ";
				char_count++;
			}
		}

		out_file.seekp((long)out_file.tellp() - 1);	//Overwrite the last space
		out_file << endl;
		out_file.close();
	}
	
	return true;			
}

void Document::addFilter(DocumentFilter* f)
{
	_filters.push_back(f);
	return;
}

WordIterator Document::begin()
{
	return WordIterator(this, 0);
}

WordIterator Document::end()
{
	return WordIterator(this, _words.size() - 1);
}

string& Document::wordAt(int index)
{
	return _words[index];
}
