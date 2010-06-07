#ifndef SERVERFILESLOADER_HPP_INCLUDED
#define SERVERFILESLOADER_HPP_INCLUDED

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
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <fstream>
#include "dbase.hpp"
#include "myHash.hpp"
#include "XMLParser.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

///Specials
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
namespace boostfs = boost::filesystem;
///Globals Varuabels

class ServerFilesLoader: public dBase{
    private:
        string strClassName;//!<Nazwa klasy
        string strWorkDir;//!< Sciezka do katalogu roboczego
        string strPathToXMLFile;//!<Sciezka do pliku xml ktory bedzie zawieral informacje o udostepnianych plikach
        string *p_strFileBuf;//!<Buffor do pliku otwieranego w celu hashowania (hashowanie mozna przeprowadzic na raty ale bez makara)
        int intDirScanDepth;//!<Glebokosc skanowania rekurencyjnego (1024)
        int intDirScanCounter;//!<Licznik glebokosci skanowania
        XMLParser *p_xmlFile;//!<wskaznik do struktury xml
        //boost::shared_ptr<boost::thread> m_thread;//!<wskaznik do watku

        void clearFileBuf();
        void clearXML();
        bool LoadFileToBuf(const string*);
        //bool SetWorkDir();
        bool RunCheck();
        void ListDir(const boostfs::path &);
        void InsertTodBase();
        //save
        void SaveList();
    public:
        ServerFilesLoader(int);
        ~ServerFilesLoader();
        void Die();
        void Run(){
            if(RunCheck()){
                ListDir( strWorkDir );
                SaveList();
                cerr<<GetLocalTime()<<strClassName<<"All works has been done"<<endl;
            }
            Die();
        }
        void operator()(){
            Run();
        }

};

#endif // SERVERFILESLOADER_HPP_INCLUDED
