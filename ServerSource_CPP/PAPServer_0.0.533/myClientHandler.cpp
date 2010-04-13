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
                                 int index ,bool &loop):
                                 p_strClassName(NULL), ClientSocket(Socket),
                                 ClientIP(IP),ClientUserLocal(user),ClientUser("unknow"),ClientPassword(pass),
                                 id_Session(0),intGID(0),
                                 intIndex4Zombie(index), dCreationTime(GetTime()),tt_CreationTime(GetTimeAfter1970AsTime()),
                                 ui64DataSend(0),ui64DataRecieved(0),ui64SendMsgCouter(0),ui64RecivedMsgCouter(0)
                                 ,bServerLoop(loop){//,p_strClientXMLFile(NULL){
    p_strClassName = new string("[myClassHandler]->[Zombie::"+liczba_na_string(intIndex4Zombie)+"]->");
    cerr<<GetLocalTime()<<*p_strClassName<<" Connecting::"<<ClientUser<<"@"<<ClientIP<<"("<<ClientUserLocal<<")"<<", PID:"<<getppid()<<endl;
}

myClientHandler::~myClientHandler(){
    ClientSocket.close();
    cerr<<*p_strClassName<<"Summary:\n"<<ExcutionTime(GetTime(),dCreationTime)<<" "<<AliveTime(GetTimeAfter1970AsTime(),tt_CreationTime)<<", Data in/out::"<<ui64DataRecieved<<"/"<<ui64DataSend<<"bajts, Msg in/out::"<<ui64RecivedMsgCouter<<"/"<<ui64SendMsgCouter<<"\n----------"<<endl;
    delete p_strClassName;
}
/**
*Authorization
*/

void myClientHandler::Authorization(list<BannedNode*>::iterator &myList2){
///Porownuje uzytkownika z dostepnymi uzytkownikami, nasptenie prosi o haslo i ustawia intGID.
    string *p_strSocketBuffer = NULL;
    Send("Login:");
    p_strSocketBuffer = &GetDataFromSocket();
    if (p_strSocketBuffer){
        if (*p_strSocketBuffer == "unknow" ){
            intGID = -1; cout<<"unknow"<<endl;
            if((*myList2)->WarnClient() == 1){cerr<<"BANNED"<<endl;}
        }else if (*p_strSocketBuffer == "user" ){
            intGID = 2;
            ClientUser = "user";
        }else if (*p_strSocketBuffer == "debug"){
            intGID = 1;
            ClientUser = "debug";
        }else if (*p_strSocketBuffer == "root"){
            intGID = 0;
            syslog(3, "PAP Server someone is logged as root");
            cerr<<GetLocalTime()<<*p_strClassName<<" Logged as root::"<<ClientUser<<"@"<<ClientIP<<"("<<ClientUserLocal<<")"<<endl;
            ClientUser = "root";
        }else{
            intGID = -1; cout<<"else"<<endl;
            if((*myList2)->WarnClient() == 1){cerr<<"BANNED"<<endl;}
        }//if *p_strSocketBuffer ==
        delete p_strSocketBuffer;
    }//if p_strSocketBuffer
}
/**
*Main Loop
*/

void myClientHandler::myClientRun(list<BannedNode*>::iterator &myList2){
///Tutaj  przebiega cala interakcja z clietem.; Przerywalna petela dba o poprawnosc komunikacji pod warunkiem pozytywnej autoryzacji.;Tutaj zwracana jest infromacja o zakonczeniu polaczenie.
    string *p_strSocketBuffer = NULL;
    string strSocketBuffer;
    string strError;
    bool bloop = true;

    double XTime = GetTime();
        Authorization(myList2);
        if (intIndex4Zombie > 0 and intGID >= 0){
            Send(SendInfoAboutServer());
            while (bloop == true){
                cout<<*p_strClassName+"Ready for new data"<<endl;
                p_strSocketBuffer = &GetDataFromSocket();
                cout<<*p_strClassName+"Something recived"<<endl;
                if (p_strSocketBuffer){
                    if (*p_strSocketBuffer == "DisconnectMe" or Cout(GetTime(),XTime) >= *ServerConfigs::p_intClientTimeOut){
                        bloop = false;
                        if(*ServerConfigs::p_intClientTimeOut - Cout(GetTime(),XTime)<= 0){strError = "::TimeOut";}
                    }else if (p_strSocketBuffer->length()>1) {
                        RecivedDataParser(p_strSocketBuffer,myList2);
                        XTime = GetTime();
                        if ((*myList2)->AskIfBanned() == true){ bloop = false; }
                    }//p_strSocketBuffer
                    delete p_strSocketBuffer;
                }else{
                    cout<<GetLocalTime()<<*p_strClassName<<"Connection problem::"<<*ServerConfigs::p_intClientTimeOut - Cout(GetTime(),XTime)<<" to time out"<<endl;
                    usleep(100);
                }//p_strSocketBuffer
                if (bloop == true ) { bloop = Send(); }
                if (Cout(GetTime(),XTime)>= *ServerConfigs::p_intClientTimeOut){ bloop = false; }
            }//while bloop
        }else{
            if (intGID == -1){ strError ="::Logging failed";}
        }//if intIndex4Zombie
        Send("Disconnected"+strError);
        cerr<<GetLocalTime()<<*p_strClassName<<" Disconnected::"<<ClientUser<<"@"<<ClientIP<<"("<<ClientUserLocal<<")"<<strError<<endl;
}

string &myClientHandler::GetDataFromSocket(){
///Ta metoda pobiera dane z socketu, w przypadku bledu wysyla polecenie zakonczenia, forka()
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
        p_strReturn = new string("DisconnectMe");
        return *p_strReturn;
    }
    std::istream isBuffer(&basbBuffer);
    p_charBuffer = new char[intRead+1];
    if (!p_charBuffer){ return *p_strReturn; }
    p_charBuffer[intRead] = '\0';

    isBuffer.read(p_charBuffer,intRead);
    p_strReturn = new string (p_charBuffer);

    ui64DataRecieved += p_strReturn->length();
    ++ui64RecivedMsgCouter;

    p_strReturn->erase(p_strReturn->rfind("\r\n\r\n"),p_strReturn->length());
    delete[] p_charBuffer;

    return *p_strReturn;
}

void myClientHandler::RecivedDataParser(string *p_strData,list<BannedNode*>::iterator &myList2){
    string strSearchFor = "SearchFor: ";
    int intCounter = 0 ;
    int intIndexHelperBegin = 0;
    int intIndexHelperEnd = 0;
    cout<<"Recived:"<<*p_strData<<endl;
    if (intGID == 0 or intGID == 1){
        if(*p_strData == "Shutdown"){
            BreakServerLoop();
        }else if (*p_strData == "ShowBannedList"){
        }else if (*p_strData == "ShutdownForced"){
            RestartShutdownServer("stop");
        }else if(*p_strData == "Restart"){
            RestartShutdownServer("restart");
        }else if(*p_strData == "DisconnectEveryOne"){
        }else{}
        /*
        *Generator XML dla listy uzytkownikow
        *Generatro XML dla listy plikow
        *Listy zapytan
        */
    }

    if(*p_strData == "GetServerInfo"){
            Send(SendInfoAboutServer());
    }else if(*p_strData == "GetServerTime"){
            Send(GetTimeAfter1970());
    }else if(p_strData->find("SearchFor: ") != -1){
            strSearchFor = p_strData->substr( p_strData->find(strSearchFor)+strSearchFor.length(),p_strData->rfind("\r\n\r\n"));
            if (strSearchFor.length() > 1){
                /*intCounter = 0;
                do{
                    intIndexHelperBegin = strSearchFor.find(intIndexHelperEnd,",")
                    if
                    //metoda(strSearchFor);
                }while(intIndexHelperEnd != -1 and intIndexHelperBegin != -1)*/
                Send("<SharedFiles><File1><FilePath>hahha/haha</FilePath><FileName>haha</FileName><FileSize>1</FileSize><FileHashType>md5</FileHashType><FileHash>aaa</FileHash><FileLastModification>1.1</FileLastModification></File1></SharedFiles>");
            }else{
                (*myList2)->BanClient();
            }
    }else{}
    /*
    * XML parser
    * status
    * pliki z listy plikow
    */
}


bool myClientHandler::Send(string strData){
///Wysyła do klienta odpowiedz, dodajac jego login i znak zachety, w przypadku bledu zakonczy prace forka()
    try
    {
        strData.append("\n"+ClientUser+"~$ ");
        ui64DataSend += strData.length();
        boost::asio::write(ClientSocket,boost::asio::buffer(strData),boost::asio::transfer_all());
    }
    catch (const std::exception &e)
    {
        fprintf(stderr,"Send]-> %s",e.what());
        return false;

    }
    ++ui64SendMsgCouter;
    return true;
}

string myClientHandler::SendInfoAboutServer(){
    string strCommandlist = "XML=1.0,\nServer_Version=";
    strCommandlist.append(AutoVersion::FULLVERSION_STRING);
    strCommandlist.append(",\nCommands=UploadFile,DisconnectMe,GetServerInfo,GetServerTime,SearchFor: <arg0,arg1,arg2>");
    if( intGID == 0 or intGID == 1){
        strCommandlist.append("Shutdown,ShutdownForced,Restart,DisconnectEveryOne");
    }
    return strCommandlist;
}

int myClientHandler::GetSessionID(){
    return id_Session;
}
/**
*KILLSERVER
*/
void myClientHandler::BreakServerLoop(){
    bServerLoop = false;
    Send("Loop has been breaked");
}

void myClientHandler::RestartShutdownServer(string strOption){
    Send("Server will be "+strOption+" in a moment");
    strOption.insert(0,"cd "+*ServerConfigs::strMyPath+"; ./myServer ");
    cout<<strOption<<endl;
    system(strOption.c_str());
}

void myClientHandler::DisconnectAllClients(){
    Send("Disconnecting clients in progress");
}
