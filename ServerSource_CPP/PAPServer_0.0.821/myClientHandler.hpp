#ifndef MYCLIENTHANDLER_HPP_INCLUDED
#define MYCLIENTHANDLER_HPP_INCLUDED

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
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>
#include <mxml.h>
//#include <libxml++/libxml++.h>
//#include <libxml++/parsers/textreader.h>
#include <boost/asio.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/ptr_container/ptr_list.hpp>
///Moje
#include "SupportClass.hpp"
#include "XMLParser.hpp"
#include "ServerConfigs.hpp"
#include "version.h"
#include "myDefines.hpp"
#include "listBanned.hpp"
#include "dbase.hpp"
///Specials
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;

///Ta klasa obsluguje klienta
class myClientHandler:public TimeCout, public dBase{
    private:
        ///Info
        string *p_strClassName;//!<Nazwa klasy
        ///Connection
        boost::asio::ip::tcp::socket &ClientSocket;//!<Socket, na którym klient jest połączony
        char *ClientIP;//!<IP klienta
        char *ClientUserLocal;//!<Nazwa localna klienta
        string ClientUser;//!<Login klienta
        string ClientPassword;//!<Haslo klienta
        int id_Session;//!<Czy sesja aktywna
        int intGID;//!<Poziom dostepu uzytkownika
        int intChunkSizeUP;//!<Predkosc z jaka klient deklaruje wysyl na sekunde
        int intChunkSizeDL;//!<Predkosc z jaka klient deklaruje odbior na sekunde
        int intChunkSize;//!<Predkosc z jaka klient bedzie dostawac dane od innego klienta wysyl na sekunde
        string strLineEnd;
        ///Statistics
        int intIndex4Zombie;//!<Unikalny index
        double dCreationTime;//!<Czas utworzenia procesu od właczenia programu
        time_t tt_CreationTime;//!<Czas utworzenia procesu od 1970
        ui64 ui64DataSend;//!<Wyslane bajty danych
        ui64 ui64DataRecieved;//!<Odebrane bajty danych
        ui64 ui64SendMsgCouter;//!<Licznik wiadomosc wyslanych
        ui64 ui64RecivedMsgCouter;//!<Licznik wiadomosci odebranych
        ///Server
        bool &bServerLoop;
        ///Data
        XMLParser *p_strSharedXmlList;//!<Lista plikow udostepnianych przez klienta
        XMLParser *p_strSearchRezualt;//!<Lista plikow wyszukanych na rzadanie klienta
        string *p_strSearchCtrl;//!< Trzyma unikalne wartosci wyszukiwania, zeby weryfikowac czy klient nie wymusza plikow.
        ///Methods
        //Actions
        string SendInfoAboutServer();
        void Authorization();//#
        bool SetNewSharedListFromClient(string*);
        bool GetSharedListFromClient(bool);
        void RecivedDataParser(string*);
        //comunication
        string &GetDataFromSocket();
        string GetDataFromSocket2();
        bool Send(string ="");
        //dbg
        template <typename T>
        void MsgToOut(T);
        //dbase
        void Serach4Files(string*,string*);
        /// KillServer
        bool createFile(string);
        void RestartShutdownServer(string);
        void DisconnectAllClients();//#
        ///Clean
        void CleanSharedList();
        void CleanSearchRezualt();
    public:
        //Methods
        myClientHandler(boost::asio::ip::tcp::socket &,
                        char*,char*,string,
                        int,bool&);
        virtual ~myClientHandler();
        void myClientRun();
        void myclientShutdownServer();
        int GetSessionID();
};


#endif // MYCLIENTHANDLER_HPP_INCLUDED
