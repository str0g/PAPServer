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

///Specials
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;

class dBase{
    public:
        dBase(char*,char*,char*,int );
        virtual ~dBase();
        void Close();
        bool insertFile(const char *,const char *,const char* ,const char *,const char* ,const char*);
        bool insertBanned(char *,char *);
        bool del();
        bool dBaseRun();
        void Rebuild_SharedFilesTable();
        void Rebuild_BannedTable();
        bool CheckForTable(char *cTable);
        bool Search(const char *,bool =true,char * ="FileName");
        bool Search2(char *,char*,bool =true,char* ="UserIP",char* ="UserLocalName");
        bool SearchFiles(XMLParser *,string *,const char *,bool =true,char * ="FileName");
        bool AskIfBanned(char *,char*,bool =true,char* ="UserIP",char* ="UserLocalName");
        bool UpdateBanned(char *,char*,bool =true,char* ="UserIP",char* ="UserLocalName");
    private:
        //Data
        MYSQL *pConnection;//!< Wskaznik inicjalizayjny biblioteki mysql
        MYSQL_RES *pResult; //!< Wskaznik do rezultatow ewentualnego wyszykiwania
        MYSQL_ROW Row; //!< Wskaznik dla wierszow
        char *p_cAdrress; //!< adress bazy
        char *p_cUser; //!< user bazy
        char *p_cPass; //!< haslo usera
        char *p_cBassName; //!< Nazwa bazy
        char *p_cTableName; //!<Nazwa tabeli z udostepnionymi plikami
        char *p_cTableBanned; //!< Nazawa tabeli ze zbanowanymi lub ostrzezonymi uzytkownikami
        int intOwnerPID; //!< Pid procesu
        //metods
        int dBaseTemplate(){}
        bool dbQuery(char *,string);
        bool drop(char *cTable);
        bool createShared();
        bool createBanned();
};


#endif // DBASE_HPP_INCLUDED
