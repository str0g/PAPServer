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

myClientHandler::myClientHandler(boost::asio::ip::tcp::socket &Socket,
                                 char *IP,char *user, string pass,
                                 int index ,bool &loop):
                                 p_strClassName(NULL), ClientSocket(Socket),
                                 ClientIP(IP),ClientUserLocal(user),ClientUser("unknow"),ClientPassword(pass),
                                 id_Session(0),intGID(0),
                                 intChunkSizeUP(BUFFER_1024),intChunkSizeDL(BUFFER_1024),intChunkSize(BUFFER_1024),
                                 strLineEnd("\r\n\r\n"),
                                 intIndex4Zombie(index), dCreationTime(GetTime()),tt_CreationTime(GetTimeAfter1970AsTime()),
                                 ui64DataSend(0),ui64DataRecieved(0),ui64SendMsgCouter(0),ui64RecivedMsgCouter(0),p_strSearchCtrl(NULL),
                                 bServerLoop(loop),p_strSharedXmlList(NULL),p_strSearchRezualt(NULL),dBase("localhost","root","qwerty71",getppid()){
    p_strClassName = new string("[myClassHandler]->[Zombie::"+liczba_na_string(intIndex4Zombie)+"]->");
    cerr<<GetLocalTime()<<*p_strClassName<<" Connecting::"<<ClientUser<<"@"<<ClientIP<<"("<<ClientUserLocal<<")"<<", PID:"<<getppid()<<endl;
    p_strSharedXmlList = new XMLParser;
    p_strSearchRezualt = new XMLParser;
    p_strSearchCtrl = new string;
}

myClientHandler::~myClientHandler(){
    ///Zwalnianie nie zwolnionej wczesniej pamieci oraz wyswietlnie/wyslanie podsumowania.
    ClientSocket.close();
    CleanSharedList();
    CleanSearchRezualt();
    delete p_strSharedXmlList;
    delete p_strSearchRezualt;
    delete p_strSearchCtrl;
    cerr<<*p_strClassName<<"Summary:\n"<<ExcutionTime(GetTime(),dCreationTime)<<" "<<AliveTime(GetTimeAfter1970AsTime(),tt_CreationTime)<<", Data in/out::"<<ui64DataRecieved<<"/"<<ui64DataSend<<"bajts, Msg in/out::"<<ui64RecivedMsgCouter<<"/"<<ui64SendMsgCouter<<"\n----------"<<endl;
    delete p_strClassName;
}
template <typename T>
void myClientHandler::MsgToOut(T strMsg){
    ///Wiadomosci
    cerr<<GetLocalTime()<<*p_strClassName<<ClientUser<<"@"<<ClientIP<<"("<<ClientUserLocal<<")::"<<strMsg<<endl;
}
/**
*Authorization
*/

void myClientHandler::Authorization(){
///Porownuje uzytkownika z dostepnymi uzytkownikami, nasptenie prosi o haslo i ustawia intGID.
    string *p_strSocketBuffer = NULL;
    Send("Login:");
    p_strSocketBuffer = &GetDataFromSocket();
    if (p_strSocketBuffer){
        if (*p_strSocketBuffer == "unknow" ){
            intGID = -1; cout<<"unknow"<<endl;
            UpdateBanned(ClientIP,ClientUserLocal);
        }else if (*p_strSocketBuffer == "user" ){
            intGID = 2;
            ClientUser = "user";
        }else if (*p_strSocketBuffer == "debug"){
            intGID = 1;
            ClientUser = "debug";
        }else if (*p_strSocketBuffer == "root"){
            intGID = 0;
            syslog(3, "PAP Server someone is logged as root");
            MsgToOut("Logged as root");
            //cerr<<GetLocalTime()<<*p_strClassName<<" Logged as root::"<<ClientUser<<"@"<<ClientIP<<"("<<ClientUserLocal<<")"<<endl;
            ClientUser = "root";
        }else{
            intGID = -1; cout<<"else"<<endl;
            UpdateBanned(ClientIP,ClientUserLocal);
        }//if *p_strSocketBuffer ==
        delete p_strSocketBuffer;
    }//if p_strSocketBuffer
}
/**
*Main Loop
*/

void myClientHandler::myClientRun(){
///Tutaj  przebiega cala interakcja z clietem.; Przerywalna petela dba o poprawnosc komunikacji pod warunkiem pozytywnej autoryzacji.;Tutaj zwracana jest infromacja o zakonczeniu polaczenie.
    string *p_strSocketBuffer = NULL;
    string strSocketBuffer;
    string strError;
    bool bloop = true;

    double XTime = GetTime();
        if (dBaseRun()){
            Authorization();
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
                            RecivedDataParser(p_strSocketBuffer);
                            XTime = GetTime();
                            AskIfBanned(ClientIP,ClientUserLocal) == true ? bloop = false : bloop;
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
        }else{
            strError="::dBase Logging failed";
        }//if dBase
        Send("Disconnected"+strError);
        MsgToOut("Disconnected"+strError);
        //cerr<<GetLocalTime()<<*p_strClassName<<" Disconnected::"<<ClientUser<<"@"<<ClientIP<<"("<<ClientUserLocal<<")"<<strError<<endl;
}

string &myClientHandler::GetDataFromSocket(){
///Ta metoda pobiera dane z socketu, w przypadku bledu wysyla polecenie zakonczenia, forka()
    boost::asio::streambuf basbBuffer;
    int intRead;
    string *p_strReturn = NULL;
    char *p_charBuffer = NULL;
    try
    {
        intRead = boost::asio::read_until(ClientSocket, basbBuffer, strLineEnd.c_str());
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

    p_strReturn->erase(p_strReturn->rfind(strLineEnd),p_strReturn->length());
    delete[] p_charBuffer;

    return *p_strReturn;
}

void myClientHandler::RecivedDataParser(string *p_strData){
    ///Pareser decyduje o wiekszosci akcji podjetych przez serwer na podstawie polecen od klienta
    string strSearchFor = "SearchFor: ";
    string strHMM = "<SharedFiles>\
		<File0>\
			<FilePath>siezka/do/pliku</FilePath>\
			<FileName>pliku</FileName>\
			<FileSize>33</FileSize>\
		</File0>\
		<File1>\
			<FilePath>siezka/do/2pliku</FilePath>\
			<FileName>2pliku</FileName>\
			<FileSize>66</FileSize>\
		</File1>\
	</SharedFiles>";
    int intCounter = 0 ;
    int intIndexHelperBegin = 0;
    int intIndexHelperEnd = 0;
    if(p_strData->length()<100){
        cout<<"Recived(below100):"<<*p_strData<<endl;
    }
    if (intGID == 0 or intGID == 1){
        if(*p_strData == "Shutdown"){
            createFile(*ServerConfigs::strMyPath+ServerConfigs::g_strSlash+"ShutdownServer.pid");
        }else if (*p_strData == "ShowBannedList"){
        }else if (*p_strData == "ResetBanned"){
            Rebuild_BannedTable();
        }else if (*p_strData == "ResetShared"){
            Rebuild_SharedFilesTable();
        }else if (*p_strData == "ShutdownForced"){
            RestartShutdownServer("stop");
        }else if(*p_strData == "Restart"){
            RestartShutdownServer("restart");
        }else if(*p_strData == "DisconnectEveryOne"){
            cout<<"Nie zaimplementowane"<<endl;
            Send("Nie zaimplementowane");
        }else if((int)p_strData->find("<") != -1 and (int)p_strData->find("</") != -1){
            cout<<"Prawdo podobnie plik szablon XML"<<endl;
            if((int)p_strData->find("<SharedFiles>") != -1 and (int)p_strData->find("</SharedFiles>") != -1 ){
                p_strSharedXmlList->LoadXMLFromBuf(p_strData->c_str());
                if(p_strSharedXmlList->SetWorkNode("SharedFiles") or p_strSharedXmlList->GetCurrentElement()=="SharedFiles"){
                    if(p_strSharedXmlList->GoDeeper()){
                        while(p_strSharedXmlList->NextElement()){
                            if(!insertFile(p_strSharedXmlList->GetStringValue("FilePath","-1").c_str(),
                                       p_strSharedXmlList->GetStringValue("FileName","-1").c_str(),
                                       p_strSharedXmlList->GetStringValue("FileSize","-1").c_str(),
                                       p_strSharedXmlList->GetStringValue("FileHashType","-1").c_str(),
                                       p_strSharedXmlList->GetStringValue("FileHash","-1").c_str(),
                                       p_strSharedXmlList->GetStringValue("FileLastModification","-1").c_str()
                                       )){cerr<<"Insertion faild"<<endl;}
                        }//while
                        CleanSharedList();
                    }//if deeper
                }//sharedfiles
            }else if(true){
            }else{
                cout<<"Tu sypnie sie ban0..."<<*p_strData<<endl;
            }
        //}
        }else{
                cout<<"Tu sypnie sie ban1..."<<*p_strData<<endl;
        }
    }
        /*
        *Generator XML dla listy uzytkownikow
        *Generatro XML dla listy plikow
        *Listy zapytan
        */
    //}

    if(*p_strData == "GetServerInfo"){
            Send(SendInfoAboutServer());
    }else if(*p_strData == "GetServerTime"){
            Send(GetTimeAfter1970());
    }else if((int)p_strData->find(strSearchFor) != -1 and p_strData->length() > (strSearchFor.length()+1) ){ //Pytanie sie o plik z dBase
            Serach4Files(p_strData,&strSearchFor);
    }else{}
    /*
    * XML parser
    * status
    * pliki z listy plikow
    */
}

void myClientHandler::Serach4Files(string *p_strData,string *strSearchFor){
    int intIndexHelperBegin = 0;
    int intIndexHelperEnd = 0;
    p_strData->erase(
                     0,//p_strData->find(strSearchFor),
                     strSearchFor->length()
                     );
    if (p_strData->length() > 1){
        if((int)p_strData->find(",")!= -1){
            intIndexHelperBegin = 0;
            do{
                intIndexHelperEnd = p_strData->find(",",intIndexHelperBegin);
                    if(intIndexHelperEnd != -1){
                        if(!SearchFiles(p_strSearchRezualt,
                                        p_strSearchCtrl,
                                        p_strData->substr(intIndexHelperBegin,intIndexHelperEnd - intIndexHelperBegin).c_str()
                                        )
                           ){
                        Send("666");
                        }else{
                            Send(p_strSearchRezualt->GetXMLAsString());
                        }//search fail
                    }//not found
                    intIndexHelperBegin = intIndexHelperEnd;
            }while(intIndexHelperEnd != -1);
        }else{
            if(!SearchFiles(p_strSearchRezualt,p_strSearchCtrl,
                            p_strData->c_str())){
                Send("666");
            }else{
                Send(p_strSearchRezualt->GetXMLAsString());
            }
        }//search if ,
                //Send(p_strSearchRezualt-);
                /*Send("<SharedFiles>\
                            <File1>
                                <FilePath>hahha/haha</FilePath>\
                                <FileName>haha</FileName>\
                                <FileSize>1</FileSize>\
                                <FileHashType>md5</FileHashType><\
                                FileHash>aaa</FileHash>\
                                <FileLastModification>1.1</FileLastModification>\
                            </File1>\
                        </SharedFiles>");
        */CleanSearchRezualt();
    }else{
        UpdateBanned(ClientIP,ClientUserLocal);
    }//length check
}

bool myClientHandler::Send(string strData){
///Wysyła do klienta odpowiedz, dodajac jego login i znak zachety, w przypadku bledu zakonczy prace forka()
    try
    {
        strData.append("\n"+ClientUser+"~$ ");
        ui64DataSend += strData.length();
        boost::asio::write(ClientSocket,boost::asio::buffer(strData),boost::asio::transfer_all());
        cerr<<"WAS SEND:\n ***["<<strData<<"]******************"<<endl;
    }
    catch (const std::exception &e)
    {
        fprintf(stderr,"%s[Send]-> %s",p_strClassName->c_str(),e.what());
        return false;

    }
    ++ui64SendMsgCouter;
    return true;
}

string myClientHandler::SendInfoAboutServer(){
///Zwraca infromacje o serwerze
    string strCommandlist = "XML=1.0,\nServer_Version=";
    strCommandlist.append(AutoVersion::FULLVERSION_STRING);
    strCommandlist.append(",\nCommands=UploadFile,DisconnectMe,GetServerInfo,GetServerTime,SearchFor: <arg0,arg1,arg2>,RenewSharedList");
    if( intGID == 0 or intGID == 1){
        strCommandlist.append("ResetBanned,ResetShared,Shutdown,ShutdownForced,Restart,DisconnectEveryOne");
    }
    return strCommandlist;
}

int myClientHandler::GetSessionID(){
///Zwraca czy sesja jest aktywna
    return id_Session;
}

bool myClientHandler::SetNewSharedListFromClient(string *p_strData){
///Przyjmuje wskaznik do danych, z ktorych zostanie utworzona lista xml, zwarca czy sie udalo
    CleanSharedList();
    p_strSharedXmlList->LoadXMLFromBuf(p_strData->c_str());
    return p_strSharedXmlList!=NULL;
}
bool myClientHandler::GetSharedListFromClient(bool bError = false){
///Server prosi klienta o przeslanie listy udostepnionych plikow
    string *p_strData = NULL;
    string *p_strBuf = new string;
    string *p_strCheckSum = NULL;
    char *p_cBuf = NULL;
    XMLParser *XMLBuf = NULL;
    int intSizeOfList = 0;
    int intIndex = 0;
    bool bFail = false;
    Send("RenewSharedList");
    p_strData = &GetDataFromSocket();

    if (p_strData){
        XMLParser *XMLBuf = new XMLParser;
        //p_cBuf = new char (p_strData->c_str());
        XMLBuf->LoadXMLFromBuf(const_cast<char*> (p_strData->c_str()));
        //delete p_cBuf;
        p_cBuf = NULL;
        intSizeOfList = XMLBuf->GetNumericValue<int>("size","-1");
        p_strCheckSum = &XMLBuf->p_GetStringValue("checksum","");
        XMLBuf->vCleanXMLTree();
        cerr<<p_strClassName<<"Czy bedzie suma?:";
        cerr<<p_strCheckSum<<endl;
    }

    while (true){
        if (XMLBuf){
            //mxmlDelete(*XMLBuf);
            delete XMLBuf;
            XMLBuf = NULL;
        }
        p_strData = &GetDataFromSocket();
        //XML PARSER tutaj
        if (p_strData){
            XMLBuf->LoadXMLFromBuf(const_cast<char*> (p_strData->c_str()));
            if (XMLBuf->GetStringValue("data","")!= ""){
                if(XMLBuf->GetNumericValue<int>("chunk",liczba_na_string(intIndex))> intIndex){
                    p_strBuf->append(XMLBuf->GetStringValue("data",""));
                    if ( (int)XMLBuf->GetStringValue("data","").length() + intIndex == intSizeOfList ){
                        intIndex = intSizeOfList;
                    }else{
                        intIndex += intChunkSizeUP;
                    }
                }else{
                    if(XMLBuf->GetNumericValue<int>("chunk",liczba_na_string(intIndex))-intIndex < intIndex){
                        //WYSLIJ SEND_PREVOUSE_CHUNK
                    }else{
                        //WYSLIJ SEND_NEXT_CHUNK
                    }
                }
            }else{//WYSLIJ RESEND
            }
            if (intIndex != XMLBuf->GetNumericValue<int>("chunk",liczba_na_string(intSizeOfList))){
                GetSharedListFromClient(true);
                break;
            }
            XMLBuf->vCleanXMLTree();
        }
        else{
            //WYSLIJ STATUS RESEND
            bFail = true;
        }

        if(intSizeOfList == intIndex and intSizeOfList > 0){///Sprawdzam czy nie wystapil wyjatek lub czy dostalem cala liste
            MsgToOut("List recived with out errors");
            bError = false;
            break;
        }else if(intSizeOfList < intIndex){
            MsgToOut("List recived but its bigger then expected");
            if (!bError){
                bError = GetSharedListFromClient(true);
            }
            break;
        }else if(intSizeOfList == 0 or !p_strCheckSum){
            MsgToOut("Have not recived size of list");
            if (!bError){
                bError = GetSharedListFromClient(true);
            }
            break;
        }else if(*p_strCheckSum == ""){
            MsgToOut("Have not recived list checksum");
            if (!bError){
                bError = GetSharedListFromClient(true);
            }
            break;
        //Policz rsa
        /*}else if( != p_strCheckSum){
            if(!Error){
                bError = GetSharedListFromClient(true);
            }
            break;
        */
        }else{
            MsgToOut("List reciving Faild from unknow error");
            if (!bError){
                bError = GetSharedListFromClient(true);
            }
            break;
        }
        bFail = false;
    }

    if (XMLBuf){
        delete XMLBuf;
        XMLBuf = NULL;
    }

    if (!bError){
        CleanSharedList();
        if (SetNewSharedListFromClient(p_strBuf) == true){
            MsgToOut("List receved and allocated successfuly");
            delete p_strSharedXmlList;
        }else{
            MsgToOut("List receved but alocation failed ");
            delete p_strBuf;
        }
    }
    delete p_strCheckSum;
    delete p_strData;

    return bError;
}

void myClientHandler::CleanSharedList(){
///Czysci liste plikow udostepnionych przez klienta jezeli juz jest zaalokowana
    if(p_strSharedXmlList!=NULL){
        //delete p_strSharedXmlList;
        p_strSharedXmlList->vCleanXMLTree();
    }else{
        MsgToOut("SearchRezualt does not exist!");
    }
}
void myClientHandler::CleanSearchRezualt(){
///Czysci liste plikow wyszukanych przez klienta jezeli juz jest zaalokowana
    if(p_strSearchRezualt!=NULL){
        //delete
        p_strSearchRezualt->vCleanXMLTree();
        //p_strSearchRezualt = NULL;
    }else{
        MsgToOut("SearchRezualt does not exist!");
    }
}
/**
*KILLSERVER
*/
bool myClientHandler::createFile(string FileName){
///Miekkie wyłaczanie serwera poprzez przerwanie glownej petli serwera
    ofstream o(FileName.c_str());
    if(o){
        Send("Server is being shutdown in a moment");
        return true;
    }
    return false;
}

void myClientHandler::RestartShutdownServer(string strOption){
///Wymuszenie wylaczenia lub restartu servera, ktory jest "zarzadzany przez usluge systemowej demonizacji"
    Send("Server will be "+strOption+" in a moment");
    strOption.insert(0,"cd "+*ServerConfigs::strMyPath+"; ./myServer ");
    cout<<strOption<<endl;
    system(strOption.c_str());
}

void myClientHandler::DisconnectAllClients(){
///Nie zaimplementowane
    Send("Disconnecting clients in progress");
}
