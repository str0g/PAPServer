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
//#include <cstring>
#include <unistd.h>

///Moje
#include "SupportClass.hpp"
#include "XMLParser.hpp"
#include "version.h"
#include "myDefines.hpp"
#include "myConv.hpp"

///Specials
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;

///Ta klasa trzyma ustawienia serwera oraz lokalizacje językową.
class ServerConfigs: TimeCout{
    public:
        static string p_cMySqlServerAddress;//!<Adres serwera mysql
        static string p_cMySqlUser;//!<Uzytkownik serwera mysql
        static string p_cMySqlPass;//!<Haslo uzytkownika mysql
        static string p_cMySqldBase;//!<Nazwa bazy mysql
        static string strFormatter;//!<Formatownaie logow
        static string strServerIP;//!<Ustawienia IP naszego serwera
        static string strLoggingFileDir;//!<Scieżka do pliku logowania
        static int *p_intClientTimeOut;//!<Czas, po którym klient zostanie odłaczony w przypadku błedów/zerwania połaczenia.
        static uint64_t *p_intChunkSize;//!<Rozmiar pakietu danych.
        static int intStdErr;//!<Informacja o użyciu cerr
        static int intStdOut;//!<Informacja o użyciu cout
        static int intStdIn;//!<Informacja o użyciu cin
        static int intDemonize;//!<Informacja o demonizacji
        static int *p_intMultiThreading;//!<Informacja o tym czy serwer bedze interacyjny lub wielowatkowy.
        static int *p_intMaxClients;//!<Maksymalna ilosc klientow
        static int *p_intMaxWarnings;//!<Maksymalan ilosc ostrzez po jakich serwer zbanuje klienta.
        static int intLoggingToFile;//!<Informacja o logowaniu
        static int intServerPort;//!<Port na którym serwer będzie nasłuchiwać.
        static string *p_strConfigPath;//!<Scieżka do pliku logów
        static string *p_strMyPath;//!<Sćieżka, z której odpalamy serwer(pobierana z getpwd)
        static string *p_strMyPath2;//!<Pobierana z argv[0];
        static string g_strSlash;//!<Trzyma Slasha, kiedys mozna zrobic porotwalnosc serwera
        ServerConfigs(string*,string *);
        ~ServerConfigs();
        void SetVaruables();
        void ShowConfigs();
        static void Clean();
};
///Ta klasa wyświetla wersje serwera.
class myServerVersion{
    public:
        static void ShowVersion(){
            fprintf(stdout,"myServer\nVersion: %s Build: %ld Date: %s.%s.%s\n",
                    AutoVersion::FULLVERSION_STRING,AutoVersion::BUILDS_COUNT,
                    AutoVersion::YEAR,AutoVersion::MONTH,AutoVersion::DATE);
            cout<<"External libraries:\n -libboost 1.38 http://www.boost.org/"<<endl;
            cout<<"Author: Łukasz Buśko\nContact: buskol.waw.pl@gmail.com\nCopyright 2010\nLicense:GNU/GPL"<<endl;
        }
};
#endif // SERVERCONFIGS_HPP_INCLUDED
