/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (www.SoonWillBe.pl)
 * License:   GNU / General Public Licens
 **************************************************************/
///Headers
#include "myClientHandler.hpp"
///Specials
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;

void KillZombie(){
/*    while (true)
    {
        if (wait(NULL)==-1)
        {
            sleep(1);
        }
    }*/
    waitpid ( -1, NULL,WNOHANG);
}

myClientHandler::myClientHandler(boost::asio::ip::tcp::socket &Socket,
                                 string IP,string user, string pass,
                                 myClientHandler *prev = NULL,myClientHandler *next = NULL,
                                 int index = -1):
                                 ClientSocket(Socket),
                                 ClientIP(IP),ClientUserLocal(user),ClientPassword(pass),ClientUser("Unknow"),
                                 p_prevZombie(prev),p_nextZombie(next),intIndex4Zombie(index),
                                 id_Session(0), dCreationTime(GetTime()){//,p_strClientXMLFile(NULL){
    p_strClassName = new string("[myClassHandler]->[Zombie::"+liczba_na_string(intIndex4Zombie)+"]->");
    cerr<<GetLocalTime()<<*p_strClassName<<" Connecting::"<<ClientUser<<"@"<<ClientIP<<"("<<ClientUserLocal<<")"<<endl;
}

myClientHandler::~myClientHandler(){
//    delete p_strClientXMLFile;
    cerr<<*p_strClassName<<ExcutionTime(GetTime(),dCreationTime);
    delete p_strClassName;
}

void myClientHandler::myClientRun(){
    string *p_strSocketBuffer = NULL;
    string strError;
    bool bloop = true;
    int intTimeOut = ServerConfigs::intClientTimeOut;
    double XTime = GetTime();
        while (bloop == true){
            p_strSocketBuffer = &GetDataFromSocket();
            if (p_strSocketBuffer){
                if (*p_strSocketBuffer == "KILLME" or Cout(GetTime(),XTime) > intTimeOut){
                    bloop = false;
                    if(intTimeOut - Cout(GetTime(),XTime)<= 0){strError = "::TimeOut";}
                    cerr<<GetLocalTime()<<*p_strClassName<<" Disconnected::"<<ClientUser<<"@"<<ClientIP<<"("<<ClientUserLocal<<") "<<strError<<endl;
                }else if (p_strSocketBuffer->length()>1) {
                    cout<<*p_strClassName<<"Was send::"<<*p_strSocketBuffer<<endl;
                    XTime = GetTime();
                }
                delete p_strSocketBuffer;
            }else{
                cout<<GetLocalTime()<<*p_strClassName<<"Connection problem::"<<intTimeOut - Cout(GetTime(),XTime)<<" to time out"<<endl;
            }
        }

    ClientSocket.close();
    exit(0);
}

string &myClientHandler::GetDataFromSocket(){
    boost::asio::streambuf basbBuffer;
    int intRead;
    string *p_strReturn = NULL;
    char *p_charBuffer = NULL;
    try
    {
        intRead = boost::asio::read_until(ClientSocket, basbBuffer, "\r\n\r\n");
    }
    catch (const std::exception &e)
    {
        cerr<<GetLocalTime()<<*p_strClassName<<e.what()<<endl;
        p_strReturn = new string("KILLME");
        return *p_strReturn;
    }///Nie wytestowane
    std::istream isBuffer(&basbBuffer);
    p_charBuffer=new char[intRead+1];
    if (!p_charBuffer){ return *p_strReturn; }
    p_charBuffer[intRead]=0;

    isBuffer.read(p_charBuffer,intRead);
    p_strReturn = new string (p_charBuffer);
    delete p_charBuffer;

    return *p_strReturn;///!-----
}
void myClientHandler::myclientShutdownServer(bool &ServerLoop){
    ServerLoop = false;
}

int myClientHandler::GetSessionID(){
    return id_Session;
}
