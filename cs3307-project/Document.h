#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <string>
#include <vector>

#include "WordIterator.h"
#include "DocumentFilter.h"

class WordIterator;

class Document
{
	public:
		Document(const std::string filename);
		~Document();

		bool load();
		bool save(const std::string& filename);

		void addFilter(DocumentFilter* f);
		
		WordIterator begin();
		WordIterator end();
		
		std::string& wordAt(int index);

	private:
		std::string _filename;
		std::vector<std::string> _words;
		std::vector<DocumentFilter*> _filters;
};

#endif
