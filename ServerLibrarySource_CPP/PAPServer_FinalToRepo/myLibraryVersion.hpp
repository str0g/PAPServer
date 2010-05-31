#ifndef MYLIBRARY4SERVER_HPP_INCLUDED
#define MYLIBRARY4SERVER_HPP_INCLUDED

/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (www.SoonWillBe.pl)
 * License:   GNU / General Public Licens
 **************************************************************/

#include "versionl.h"
///Ta klasa wypisuje informacje o wersji biblioteki.
class myLibraryVersion{
    public:
        static void ShowVersion(){
            fprintf(stdout,"myLibrary4Server\nVersion: %s Build: %ld Date: %s.%s.%s\n",
                    AutoVersionL::FULLVERSION_STRING,AutoVersionL::BUILDS_COUNT,
                    AutoVersionL::YEAR,AutoVersionL::MONTH,AutoVersionL::DATE);
            cout<<"External libraries:\n -minixml 2.6 www.minixml.com"<<endl;
            cout<<"Author: Łukasz Buśko\nContact: buskol.waw.pl@gmail.com\nCopyright 2010\nLicense:GNU/GPL"<<endl;
        }
};


/*! \mainpage
//
*\f[ Moja Biblioteka dla Serwera\f]
*\f[ Łukasz Busko\f]
*/
#endif // MYLIBRARY4SERVER_HPP_INCLUDED
