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
#include <cstdio>
#include <syslog.h>
#include "ServerConfigs.hpp"

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
    string *strArgv;
    string *p_strConfigPath = NULL;
    openlog("PAP Server",0,0);
    if (argc > 1){
        cout<<"[Lista argumentow]"<<endl;
        for(int i=1; argv[i]!=NULL; ++i){
            cout<<argv[i]<<endl;
            strArgv = new string (argv[i]);
            if (*strArgv == "-c" or *strArgv == "--config"){
                p_strConfigPath = new string(argv[i]);
            }else if (*strArgv == "-h" or *strArgv == "--help"){
                cout<<"myServer version 0.0.0 Use [Option] [Argument]"<<endl;
                cout<<"-c or --config to use diffrent config file"<<endl;
                cout<<"-h or --help print this message and quit"<<endl;
                cout<<"restart to restart working server"<<endl;
                cout<<"start to start server if it crashed"<<endl;
                cout<<"stop to stop working server if its running"<<endl;
                delete strArgv;
                exit(0);
            }else if (*strArgv == "start"){
                cout<<"PAP Server is going on-line"<<endl;
            }else if (*strArgv == "stop"){
                cout<<"PAP Server is going to be shuted down in a moment"<<endl;
                syslog(3, "PAP Server is going to be shutdown in a moment", 0);
            }else if (*strArgv == "restart"){
                cout<<"PAP Server is going to be restarted"<<endl;
                syslog(3, "PAP Server is going to be restarted", 0);
            }else if (strArgv->length() == 0){
            }else{
                printf("Unknow option [%s] please use -h to see help.\n",argv[i]);
                delete strArgv;
                exit(0);
            }
            delete strArgv;
        }
        cout<<"[-------------------]"<<endl;
    }
    syslog(3, "PAP Server is going on-line", 0);

    ServerConfigs *pConfigs = new ServerConfigs(argv[0],p_strConfigPath);
    pConfigs->SetVaruables();
    pConfigs->ShowConfigs();

    delete pConfigs;
    delete p_strConfigPath;
    syslog(3, "PAP Server is going to be shutdown", 0);
    closelog();
    return 0;
}
