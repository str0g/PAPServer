/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (www.SoonWillBe.pl)
 * License:   GNU / General Public Licens
 **************************************************************/
///Headers
#include "ServerConfigs.hpp"
///Specials
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
///Globals Varuabels

string g_strSlash = "/";
string ServerConfigs::strLoggingFileDir;
string ServerConfigs::strServerIP;
int ServerConfigs::intStdErr = 0;
int ServerConfigs::intStdOut = 0;
int ServerConfigs::intDemonize = 0;
int ServerConfigs::intLoggingToFile = 0;
int ServerConfigs::intStdIn = 0;
int ServerConfigs::intServerPort = 0;


void ServerConfigs::SetVaruables(){
    double Xtime = GetTime();
    string *p_strMethodName = new string (*p_strClassName + "[SetVarubales]->");

    XMLParser *X = new XMLParser;
    X->LoadXMLFile(p_strConfigPath);

    ///Logger
    strLoggingFileDir = X->GetStringValue("LoggingFileDir","/dev/NULL");
    intLoggingToFile = X->GetNumericValue<int>("LoggingToFile","0");
    ///Server
    intStdErr = X->GetNumericValue<int>("StdErr","1");
    intStdOut = X->GetNumericValue<int>("StdOut","1");
    intStdIn = X->GetNumericValue<int>("StdIn","0");
    intDemonize = X->GetNumericValue<int>("Demonize","1");
    ///connection
    strServerIP = X->GetStringValue("Adress","127.0.0.1");
    intServerPort = X->GetNumericValue<int>("Port","6666");

    delete X;
    cout<<*p_strMethodName<<ExcutionTime(GetTime(),Xtime)<<endl;
    delete p_strMethodName;
}


void ServerConfigs::ShowConfigs(){
    double Xtime = GetTime();
    string *p_strMethodName = new string (*p_strClassName + "[ShowConfigs]->");

    cout<<"[Logger]"<<endl;
    cout<<"LoggingToFile: "<<intLoggingToFile<<endl;
    cout<<"LoggingFileDir: "<<strLoggingFileDir<<endl;

    cout<<"[Server]"<<endl;
    cout<<"MyPath: "<<strMyPath<<endl;
    cout<<"ConfigPath: "<<*p_strConfigPath<<endl;
    cout<<"StdIn: "<<intStdIn<<endl;
    cout<<"stdOut: "<<intStdOut<<endl;
    cout<<"stdErr: "<<intStdErr<<endl;
    cout<<"Demonize: "<<intDemonize<<endl;
    cout<<"MultiThreading: "<<endl;

    cout<<"[Server Connection]"<<endl;
    cout<<"Server Adress: "<<strServerIP<<endl;
    cout<<"Server Port: "<<intServerPort<<endl;

    cout<<*p_strMethodName<<ExcutionTime(GetTime(),Xtime)<<endl;
    delete p_strMethodName;
}

ServerConfigs::ServerConfigs(string strMsg1,string *p_strMsg2):strMyPath(strMsg1),p_strConfigPath(p_strMsg2){
    p_strClassName = new string ("[ServerConfigs]->");
    if (!p_strConfigPath){ p_strConfigPath = new string ("configs"+g_strSlash+"config.xml"); }
    if (p_strConfigPath->length()<2){ p_strConfigPath = new string ("configs"+g_strSlash+"config.xml"); }
}

ServerConfigs::~ServerConfigs(){
    delete p_strConfigPath;
    delete p_strClassName;
}
