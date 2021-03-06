#ifndef MYSERVERAPP_HPP_INCLUDED
#define MYSERVERAPP_HPP_INCLUDED

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
#include <cstring>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/ptr_container/ptr_list.hpp>
#include <boost/bind.hpp>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <signal.h>

///Moje
#include "ServerConfigs.hpp"
#include "SupportClass.hpp"
#include "myClientHandler.hpp"
#include "myDefines.hpp"
#include "listBanned.hpp"
#include "dbase.hpp"

///Specials
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;

///Główna klasa serwera
class myServerAPP:public TimeCout,public dBase{
    private:
        bool chk4file(string);
    protected:
        string p_strClassNameE;//!<Nazwa klasy
        bool bServerLoop;//!<Petla servera zalezy od tej zmiennej
        double dCreationTime;
        time_t tt_CreationTime;
    public:
        static int intValueOnExit;//!<Wartość błędu
        myServerAPP();
        void RunServer();
        ~myServerAPP();
};

#endif // MYSERVERAPP_HPP_INCLUDED
