/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (www.SoonWillBe.pl)
 * License:   GNU / General Public Licens
 **************************************************************/

/*! \mainpage
//
*\f[ Moja Serwera p2p\f]
*\f[ Łukasz Busko\f]
*\f[ Licencja GNU/GPL General Public License\f]
*/


///Headers
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdio>
#include <syslog.h>
#include <unistd.h>

///Moje
#include "ServerConfigs.hpp"
#include "myServerAPP.hpp"
#include "myLibraryVersion.hpp"

///Specials
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;

void openlog(const char *ident, int option, int facility);
void syslog(int priority, const char *format, ...);
void closelog(void);

///Globals Varuabels
extern string g_strSlash;
#ifdef WIN64 or WIN32
g_strSlash ="\\";
#endif


int main(int argc,char **argv){
    string *strArgv;//!<Wskaźnik, do którego zostanie skopiowana wartość argv[index].
    string *p_strConfigPath = NULL; //!<Wskaźnik, trzymający scieżkę do pliku z konfiguracją serwera.
    int intValueOnExit = 1; //!< Wartość zwrócona na wyłączenie programu.
    openlog("PAP Server",0,0);
    if (argc > 1){
        cout<<"[Lista argumentow]::"<<argc<<endl;
        for(int i=1; argv[i]!=NULL; ++i){
            cout<<argv[i]<<endl;
            strArgv = new string (argv[i]);
            if (*strArgv == "-c" or *strArgv == "--config"){
                p_strConfigPath = new string(argv[i+1]);
                ++i;
            }else if (*strArgv == "-h" or *strArgv == "--help"){
                cout<<"myServer version 0.0.0 Use [Option] [Argument]"<<endl;
                cout<<"-c or --config to use diffrent config file"<<endl;
                cout<<"-h or --help print this message and quit"<<endl;
                cout<<"-v or --version print server and library version then quit"<<endl;
                cout<<"restart to restart working server"<<endl;
                cout<<"start to start server if it crashed"<<endl;
                cout<<"stop to stop working server if its running"<<endl;
                delete strArgv;
                exit(0);
            }else if (*strArgv == "-v" or *strArgv == "--version"){
                myServerVersion::ShowVersion();
                myLibraryVersion::ShowVersion();
                return 0;
            }else if (*strArgv == "start"){
                cout<<"PAP Server is going on-line"<<endl;
            }else if (*strArgv == "stop"){
                cout<<"PAP Server is going to be shuted down in a moment"<<endl;
                syslog(3, "PAP Server is going to be shutdown in a moment");
                exit(0);
            }else if (*strArgv == "restart"){
                cout<<"PAP Server is going to be restarted"<<endl;
                syslog(3, "PAP Server is going to be restarted");
            }else if (strArgv->length() == 0){
            }else{
                fprintf(stdout,"Unknow option [%s] please use -h to see help.\n",argv[i]);
                delete strArgv;
                exit(0);
            }
            delete strArgv;
        }
        cout<<"[-------------------]"<<endl;
    }

    ServerConfigs pConfigs (argv[0],p_strConfigPath);

        std::ofstream StreamToDevNUll;
        std::ofstream StreamToLogFile;
        std::streambuf *CoutCopy = cout.rdbuf();//!< kopia domyślnego ustawienia wyjścia dla cout
        std::streambuf *CerrCopy = cerr.rdbuf();//!< kopia domyślnego ustawienia wyjścia dla cerr
        std::streambuf *CinCopy = cin.rdbuf();//!< kopia domyślnego ustawienia wyjścia dla cin
        std::streambuf *Redirected;
        StreamToDevNUll.open("/dev/null");
        pConfigs.SetVaruables();
        if (pConfigs.intLoggingToFile == 1){ ///Ustawienie pliku do ktorego std zostanie przekierowany
            StreamToLogFile.open(pConfigs.strLoggingFileDir.c_str(),std::ios::app);
            Redirected = StreamToLogFile.rdbuf();
        }

        if (pConfigs.intDemonize == 1){ ///Ustawienie pliku do ktorego std zostanie przekierowany
            daemon(0,1);
        }
        if (pConfigs.intStdOut == 0){ /// Zamkniecie StdOut lub przekiereowanie do /dev/null
            cout.rdbuf(StreamToDevNUll.rdbuf());
        }else if (pConfigs.intStdOut == 1){
            cout.rdbuf(Redirected);
        }else{
            pConfigs.ShowConfigs();
        }


        if (pConfigs.intStdErr == 0){ /// Zamkniecie StdErr lub przekiereowanie do /dev/null
            cerr.rdbuf(StreamToDevNUll.rdbuf());
        }else if (pConfigs.intStdErr == 1){
            cerr.rdbuf(Redirected);
        }else{}

        if (pConfigs.intStdIn == 0){ /// Zamkniecie StdIn lub przekiereowanie do /dev/null
            cin.rdbuf(StreamToDevNUll.rdbuf());
        }

        myServerAPP pServer;
            syslog(3, "PAP Server is going on-line");
            pServer.RunServer();
            intValueOnExit = pServer.intValueOnExit;

        syslog(3, "PAP Server is going to be shutdown");
        cout.rdbuf(CoutCopy);
        cerr.rdbuf(CerrCopy);
        cin.rdbuf(CinCopy);
        if (pConfigs.intLoggingToFile == 1){ StreamToLogFile.close(); }
        StreamToDevNUll.close();

    closelog();
    return intValueOnExit;
}
