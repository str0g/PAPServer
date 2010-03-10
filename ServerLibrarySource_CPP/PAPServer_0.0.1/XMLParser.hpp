#ifndef XMLPARSER_HPP_INCLUDED
#define XMLPARSER_HPP_INCLUDED
///Headers
#include <cstdlib>
#include <iostream>
#include <string>
#include <mxml.h>
#include "SupportClass.hpp"
///Specials
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
///Globals Varuabels

class XMLParser: public konwertery{
    protected:
        string *p_strClassName;
        mxml_node_t *p_PointerToXMLTree;
    public:
        XMLParser();
        ~XMLParser();
        int LoadXMLFile(string *);
        template <typename T>
        T GetNumericValue(string strWordToSearch,string strValueOnError){
            return string_naliczba<T> (GetStringValue(strWordToSearch,strValueOnError));
        }
        string GetStringValue(string,string);
        void vCleanXMLTree();
};

#endif // XMLPARSER_HPP_INCLUDED
