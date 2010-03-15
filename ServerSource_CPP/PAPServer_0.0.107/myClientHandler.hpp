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
//#include <libxml++/libxml++.h>
//#include <libxml++/parsers/textreader.h>
#include <boost/asio.hpp>
#include <boost/filesystem/operations.hpp>
///Moje
#include "SupportClass.hpp"
#include "ServerConfigs.hpp"
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
        int id_Session;//!<Czy sesja aktywna
        string ClientIP;//!<IP klienta
        string ClientUser;//!<Login klienta
        string ClientPassword;//!<Haslo klienta
        string ClientUserLocal;//!<Nazwa localna klienta
        boost::asio::ip::tcp::socket &ClientSocket;//!<Socket, na którym klient jest połączony
        ///Data
        //Glib::ustring *p_strClientXMLFile;//!<Wskaźnik do przesłanego przez klienta pliku XML z udostępnionymi zasobami
        ///Methods
        string &GetDataFromSocket();
        /// KillServer
    public:
        ///Statistics
        int intIndex4Zombie;//!<Unikalny index
        double dCreationTime;//!<Czas utworzenia procesu od właczenia programu
        ///List
        myClientHandler *p_prevZombie;//!<Wskanik na poprzedniego Zombie
        myClientHandler *p_nextZombie;//!<Wskanik na następnego Zombie
        //Methods
        myClientHandler(boost::asio::ip::tcp::socket &,
                        string,string,string,
                        myClientHandler*,myClientHandler*,int);
        ~myClientHandler();
        void myClientRun();
        void myclientShutdownServer(bool&);
        int GetSessionID();
};


#endif // MYCLIENTHANDLER_HPP_INCLUDED
