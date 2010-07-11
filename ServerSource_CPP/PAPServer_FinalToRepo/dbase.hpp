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
//ZAMIEN SPRAWDZANIE TABLI NA ENUMERATORY
class dBase:public TimeCout{
    public:
        dBase(int );
        dBase();
        virtual ~dBase();
        void Close();
        void mClose(bool = true);
        bool insertFile(const char *,const char *,const char* ,const char *,const char* ,const char*);
        bool insertBanned(char *,char *);
        void del(int =0);
        bool dBaseRun();
        bool Rebuild_SharedFilesTable();
        bool Rebuild_BannedTable();
        bool Rebuild_Ordered();
        bool CheckForTable(char *cTable);
        bool SearchByMatched(const char*,char* ="FileOwner",char* ="OrderTable");
        bool SearchByMatched2(const char*,const char*,char* ="FileOwner",char* ="FilePath",char* ="SharedFiles");
        bool SearchFiles(XMLParser *,string *,string *,bool =true,char * ="FileName");
        bool AskIfBanned(char *,char*,bool =true,char* ="UserIP",char* ="UserLocalName");
        bool UpdateBanned(char *,char*,bool =true,char* ="UserIP",char* ="UserLocalName");
        bool UpdateOrder(const char*,const char*,int,string);
        int newOrder(const char*,const char*,string &);
        string checkIfOrdered(bool);
        string GetHashInfo(const char*, int);
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
        char *p_cTableOrdered;//!<Nazwa tabeli zamowien
        int intOwnerPID; //!< Pid procesu
        //metods
        int dBaseTemplate(){}
        bool dbQuery(char *,string);
        bool drop(char *cTable);
        bool createShared();
        bool createBanned();
        bool createOrdered();
        bool Search(const char *,bool =true,char * ="FileName");
        bool Search2(char *,char*,bool =true,char* ="UserIP",char* ="UserLocalName");
        bool SearchOrder(const char*,const char*);
};


#endif // DBASE_HPP_INCLUDED
