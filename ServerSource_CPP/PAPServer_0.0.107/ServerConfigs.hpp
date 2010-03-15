#ifndef SERVERCONFIGS_HPP_INCLUDED
#define SERVERCONFIGS_HPP_INCLUDED

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

///Moje
#include "SupportClass.hpp"
#include "ReadClass.hpp"
#include "XMLParser.hpp"
#include "version.h"

///Specials
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;

///Ta klasa trzyma ustawienia serwera oraz lokalizacje językową.
class ServerConfigs: TimeCout{
    protected:
        string *p_strClassName;//!<Trzyma nazwę klasy
        string *p_strConfigPath;//!<Scieżka do pliku logów
    public:
        static string strFormatter;//!<Formatownaie logow
        static string strServerIP;//!<Ustawienia IP naszego serwera
        static string strLoggingFileDir;//!<Scieżka do pliku logowania
        static int intClientTimeOut;//!<Czas, po którym klient zostanie odłaczony w przypadku błedów/zerwania połaczenia.
        static int intStdErr;//!<Informacja o użyciu cerr
        static int intStdOut;//!<Informacja o użyciu cout
        static int intStdIn;//!<Informacja o użyciu cin
        static int intDemonize;//!<Informacja o demonizacji
        static int intLoggingToFile;//!<Informacja o logowaniu
        static int intServerPort;//!<Port na którym serwer będzie nasłuchiwać.
        static int intSizeOfFormatter;//!Rozmiar buffora formatera.
        string strMyPath;//!<Sćieżka, z której odpalamy serwer
        ServerConfigs(string,string *);
        ~ServerConfigs();
        void SetVaruables();
        void ShowConfigs();
};
///Ta klasa wyświetla wersje serwera.
class myServerVersion{
    public:
        static void ShowVersion(){
            fprintf(stdout,"myServer\nVersion: %s Build: %i Date: %s.%s.%s\n",
                    AutoVersion::FULLVERSION_STRING,AutoVersion::BUILDS_COUNT,
                    AutoVersion::YEAR,AutoVersion::MONTH,AutoVersion::DATE);
        }
};
#endif // SERVERCONFIGS_HPP_INCLUDED
