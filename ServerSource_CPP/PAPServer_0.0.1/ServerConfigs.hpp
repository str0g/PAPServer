#ifndef SERVERCONFIGS_HPP_INCLUDED
#define SERVERCONFIGS_HPP_INCLUDED


///Headers
#include <cstdlib>
#include <iostream>
#include <string>

///Moje
#include "SupportClass.hpp"
#include "ReadClass.hpp"
#include "XMLParser.hpp"

///Specials
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
///Globals Varuabels

class ServerConfigs: TimeCout{
    protected:
        string *p_strClassName;
        string *p_strConfigPath;
    public:
        static string strServerIP;
        static string strLoggingFileDir;
        static int intStdErr;
        static int intStdOut;
        static int intDemonize;
        static int intLoggingToFile;
        static int intServerPort;
        static int intStdIn;
        string strMyPath;
        ServerConfigs(string,string *);
        ~ServerConfigs();
        void SetVaruables();
        void ShowConfigs();
};

#endif // SERVERCONFIGS_HPP_INCLUDED
