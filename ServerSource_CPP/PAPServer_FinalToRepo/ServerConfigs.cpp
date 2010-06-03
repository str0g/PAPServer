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

string ServerConfigs::g_strSlash = "/";
string ServerConfigs::p_cMySqlServerAddress;//="localhost";
string ServerConfigs::p_cMySqlUser;//="root";
string ServerConfigs::p_cMySqlPass;//="qwerty71";
string ServerConfigs::p_cMySqldBase;//="SharedBase";
string ServerConfigs::strLoggingFileDir;
string ServerConfigs::strServerIP;
string ServerConfigs::strFormatter;
string *ServerConfigs::p_strConfigPath = NULL;
string *ServerConfigs::strMyPath = NULL;
string ServerConfigs::strMyPath2;
int ServerConfigs::intStdErr = 0;
int ServerConfigs::intStdOut = 0;
int ServerConfigs::intDemonize = 0;
int *ServerConfigs::p_intMultiThreading = NULL;
int ServerConfigs::intLoggingToFile = 0;
int ServerConfigs::intStdIn = 0;
int ServerConfigs::intServerPort = 0;
int *ServerConfigs::p_intClientTimeOut = NULL;
int *ServerConfigs::p_intChunkSize = NULL;


void ServerConfigs::SetVaruables(){
    double Xtime = GetTime();
    string p_strMethodName = (p_strClassName + "[SetVarubales]->");

    XMLParser *X = new XMLParser;
    X->LoadXMLFile(p_strConfigPath);
    X->SetWorkNode("configs");
    ///Logger
    X->SetWorkNode("logger");
    strLoggingFileDir = X->GetStringValue("LoggingFileDir","/dev/NULL");
    intLoggingToFile = X->GetNumericValue<int>("LoggingToFile","0");
    strFormatter = X->GetStringValue("Formatter","[%Y/%m/%d %H:%M:%S]::");
    ///Server
    X->SetWorkNode("server");
    intStdErr = X->GetNumericValue<int>("StdErr","1");
    intStdOut = X->GetNumericValue<int>("StdOut","1");
    intStdIn = X->GetNumericValue<int>("StdIn","0");
    intDemonize = X->GetNumericValue<int>("Demonize","1");
    p_intMultiThreading = &X->p_GetNumericValue<int>("MultiThreading","1");

    X->SetWorkNode("sqlconn");
    ///MySQL
    p_cMySqlServerAddress = X->GetStringValue("Address","localhost");
    p_cMySqlUser = X->GetStringValue("User","root");
    p_cMySqlPass = X->GetStringValue("Password","qwerty71");
    p_cMySqldBase = X->GetStringValue("dBaseName","SharedBase");

    ///connection
    X->SetWorkNode("connection");
    strServerIP = X->GetStringValue("Address","127.0.0.1");
    intServerPort = X->GetNumericValue<int>("Port","6666");
    p_intClientTimeOut = &X->p_GetNumericValue<int>("ClientTimeOut","60");
    p_intChunkSize = &X->p_GetNumericValue<int>("ChunkSize","1024");
    *p_intChunkSize *= 1024;

    ///Setting up static values
    Set_strTimeFormatter(strFormatter);

    delete X;
    cout<<p_strMethodName<<ExcutionTime(GetTime(),Xtime)<<endl;
}


void ServerConfigs::ShowConfigs(){
    double Xtime = GetTime();
    string p_strMethodName = (p_strClassName + "[ShowConfigs]->");

    cout<<"[Logger]"<<endl;
    cout<<"LoggingToFile: "<<intLoggingToFile<<endl;
    cout<<"LoggingFileDir: "<<strLoggingFileDir<<endl;
    cout<<"Formatter: "<<strFormatter<<endl;
    cout<<"Formatter output: "<<GetLocalTime()<<endl;
    cout<<"Lenght of formatter outputs: "<<GetLocalTime().length()<<endl;

    cout<<"[Server]"<<endl;
    cout<<"MyPath: "<<*strMyPath<<endl;
    cout<<"MyPath2: "<<strMyPath2<<endl;
    cout<<"ConfigPath: "<<*p_strConfigPath<<endl;
    cout<<"StdIn: "<<intStdIn<<endl;
    cout<<"stdOut: "<<intStdOut<<endl;
    cout<<"stdErr: "<<intStdErr<<endl;
    cout<<"Demonize: "<<intDemonize<<endl;
    cout<<"MultiThreading: "<<*p_intMultiThreading<<endl;

    cout<<"[MySQL]"<<endl;
    cout<<"SQL address: "<<p_cMySqlServerAddress<<endl;
    cout<<"SQL user: "<<p_cMySqlUser<<endl;
    cout<<"SQL password: "<<p_cMySqlPass<<endl;
    cout<<"SQL password: "<<p_cMySqldBase<<endl;

    cout<<"[Server Connection]"<<endl;
    cout<<"Server Adress: "<<strServerIP<<endl;
    cout<<"Server Port: "<<intServerPort<<endl;
    cout<<"Client Time out:"<<*p_intClientTimeOut<<endl;
    cout<<"Chunk Size:"<<*p_intChunkSize<<endl;

    cout<<p_strMethodName<<ExcutionTime(GetTime(),Xtime)<<endl;
}

ServerConfigs::ServerConfigs(char *cMsg1,string *p_strMsg2):p_strClassName("[ServerConfigs]->"){
    //p_strClassName = new string ("[ServerConfigs]->");
    p_strConfigPath = p_strMsg2;
    char *path=NULL;
    path=getcwd(path,BUFFER);
    strMyPath2 = liczba_na_string(cMsg1);
    string strPath = liczba_na_string(path);
    free(path);

    if (!p_strConfigPath){ p_strConfigPath = new string (strPath+g_strSlash+"configs"+g_strSlash+"config.xml"); }
    if (p_strConfigPath->length()<2){ p_strConfigPath = new string (strPath+g_strSlash+"configs"+g_strSlash+"config.xml"); }
    strMyPath = new string(p_strConfigPath->substr(0,p_strConfigPath->rfind(g_strSlash+"configs")));
}

ServerConfigs::~ServerConfigs(){
    Clean();
//    delete p_strClassName;
}

void ServerConfigs::Clean(){
    delete p_strConfigPath;
    p_strConfigPath = NULL;
    delete strMyPath;
    strMyPath = NULL;
    delete p_intClientTimeOut;
    p_intClientTimeOut =NULL;
    delete p_intMultiThreading;
    p_intMultiThreading = NULL;
}

