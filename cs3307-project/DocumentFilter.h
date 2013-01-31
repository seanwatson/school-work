#ifndef DOCUMENT_FILTER_H
#define DOCUMENT_FILTER_H

class Document;

class DocumentFilter
{
  public:
    virtual ~DocumentFilter() { };
    virtual void apply(Document& doc) = 0;
};

#endif
