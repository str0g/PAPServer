#ifndef DBASE_HPP_INCLUDED
#define DBASE_HPP_INCLUDED
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
#include <stdio.h>
#include <string>
#include <mysql++.h>
#include "SupportClass.hpp"
#include "XMLParser.hpp"
#include "ServerConfigs.hpp"
#include "myConv.hpp"

///Specials
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;

class dBase:public TimeCout{
    public:
        dBase(int );
        dBase();
        virtual ~dBase();
        void Close();
        void mClose();
        bool insertFile(const char *,const char *,const char* ,const char *,const char* ,const char*);
        bool insertBanned(char *,char *);
        bool del();
        bool dBaseRun();
        bool Rebuild_SharedFilesTable();
        bool Rebuild_BannedTable();
        bool Rebuild_Orderd();
        bool CheckForTable(char *cTable);
        bool Search(const char *,bool =true,char * ="FileName");
        bool Search2(char *,char*,bool =true,char* ="UserIP",char* ="UserLocalName");
        bool Search3(const char*,const char*);
        bool SearchFiles(XMLParser *,string *,string *,bool =true,char * ="FileName");
        bool AskIfBanned(char *,char*,bool =true,char* ="UserIP",char* ="UserLocalName");
        bool UpdateBanned(char *,char*,bool =true,char* ="UserIP",char* ="UserLocalName");
        int newOrder(const char*,const char*);
    private:
        string *p_strClassNameD;//!<Nazwa klasy unikalna dla kazdnego procesu
        //Data
        MYSQL *pConnection;//!< Wskaznik inicjalizayjny biblioteki mysql
        MYSQL_RES *pResult; //!< Wskaznik do rezultatow ewentualnego wyszykiwania
        MYSQL_ROW Row; //!< Wskaznik dla wierszow
        const char *p_cAdrress; //!< adress bazy
        const char *p_cUser; //!< user bazy
        const char *p_cPass; //!< haslo usera
        const char *p_cBassName; //!< Nazwa bazy
        char *p_cTableName; //!<Nazwa tabeli z udostepnionymi plikami
        char *p_cTableBanned; //!< Nazawa tabeli ze zbanowanymi lub ostrzezonymi uzytkownikami
        char *p_cTableOrderd;//!<Nazwa tabeli zamowien
        int intOwnerPID; //!< Pid procesu
        //metods
        int dBaseTemplate(){}
        bool dbQuery(char *,string);
        bool drop(char *cTable);
        bool createShared();
        bool createBanned();
        bool createOrderd();
};


#endif // DBASE_HPP_INCLUDED
