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
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <syslog.h>
//#include <libxml++/libxml++.h>
//#include <libxml++/parsers/textreader.h>
#include <boost/asio.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/ptr_container/ptr_list.hpp>
///Moje
#include "SupportClass.hpp"
#include "ServerConfigs.hpp"
#include "version.h"
#include "myDefines.hpp"
#include "listBanned.hpp"
///Specials
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;

void KillZombie();

///Ta klasa obsluguje klienta
class myClientHandler:public TimeCout{
    private:
        ///Info
        string *p_strClassName;//!<Nazwa klasy
        ///Connection
        boost::asio::ip::tcp::socket &ClientSocket;//!<Socket, na którym klient jest połączony
        string ClientIP;//!<IP klienta
        string ClientUserLocal;//!<Nazwa localna klienta
        string ClientUser;//!<Login klienta
        string ClientPassword;//!<Haslo klienta
        int id_Session;//!<Czy sesja aktywna
        int intGID;//!<Poziom dostepu uzytkownika
        ///Statistics
        int intIndex4Zombie;//!<Unikalny index
        double dCreationTime;//!<Czas utworzenia procesu od właczenia programu
        ui64 ui64DataSend;//!<Wyslane bajty danych
        ui64 ui64DataRecieved;//!<Odebrane bajty danych
        ui64 ui64SendMsgCouter;//!<Licznik wiadomosc wyslanych
        ui64 ui64RecivedMsgCouter;//!<Licznik wiadomosci odebranych
        ///Server
        bool &bServerLoop;
        ///Data
        //Glib::ustring *p_strClientXMLFile;//!<Wskaźnik do przesłanego przez klienta pliku XML z udostępnionymi zasobami
        ///Methods
        string &GetDataFromSocket();
        string GetDataFromSocket2();
        bool Send(string ="");
        string SendInfoAboutServer();
        void RecivedDataParser(string*);
        void Authorization(boost::ptr_list<BannedNode*>::iterator&);//#
        /// KillServer
        void BreakServerLoop();
        void RestartShutdownServer(string);
        void DisconnectAllClients();//#
    public:
        //Methods
        myClientHandler(boost::asio::ip::tcp::socket &,
                        string,string,string,
                        int,bool&);
        virtual ~myClientHandler();
        void myClientRun(boost::ptr_list<BannedNode*>::iterator&);
        void myclientShutdownServer();
        int GetSessionID();
};


#endif // MYCLIENTHANDLER_HPP_INCLUDED
