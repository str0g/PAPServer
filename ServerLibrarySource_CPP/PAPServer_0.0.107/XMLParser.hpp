#ifndef XMLPARSER_HPP_INCLUDED
#define XMLPARSER_HPP_INCLUDED
/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (www.SoonWillBe.pl)
 * License:   GNU / General Public Licens
 **************************************************************/
///Headers
#include <cstdlib>
#include <iostream>
#include <string>
#include <mxml.h>

///Moje
#include "SupportClass.hpp"

///Specials
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;

/// Ta klasa przetwarza plik XML w celu pobrania z niego wyszukiwanej wartości. Zwaraca dane jako string lub dowolną okresloną.
class XMLParser: public konwertery{
    protected:
        string *p_strClassName; //!<Nazwa klasy
        mxml_node_t *p_PointerToXMLTree; //!< Wskaznik, w który bedzie wskazywał na struktura XML
    public:
        XMLParser();
        ~XMLParser();
        int LoadXMLFile(string *);
        template <typename T>
        T GetNumericValue(string strWordToSearch,string strValueOnError){
            ///Zwraca rezlutat wyszukania stringa po konwersji do określonego typu.
            return string_naliczba<T> (GetStringValue(strWordToSearch,strValueOnError));
        }
        string GetStringValue(string,string);
        void vCleanXMLTree();
};

#endif // XMLPARSER_HPP_INCLUDED
