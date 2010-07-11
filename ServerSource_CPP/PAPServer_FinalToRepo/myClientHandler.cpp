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
                                 int index,int pid,int ulimit):
                                 dBase(pid),
                                 p_strClassNameX(new string("[myClassHandler]->[Zombie::"+myConv::ToString(intIndex4Zombie)+"]->")), ClientSocket(Socket),
                                 ClientIP(IP),ClientUserLocal(user),ClientUser("unknow"),ClientPassword(pass),
                                 intPID(pid),id_Session(0),intGID(0),intUserLimit(ulimit),
                                 intChunkSizeUP(BUFFER_1024),intChunkSizeDL(BUFFER_1024),intChunkSize(BUFFER_1024),
                                 strLineEnd("\r\n\r\n"),bLoop(true),
                                 intIndex4Zombie(index), dCreationTime(GetTime()),tt_CreationTime(GetTimeAfter1970AsTime()),
                                 ui64DataSend(0),ui64DataRecieved(0),ui64SendMsgCouter(0),ui64RecivedMsgCouter(0),
                                 p_strSharedXmlList(NULL),p_strSearchRezualt(NULL),p_xmlOrder(NULL),p_xmlIN(NULL),p_xmlOUT(NULL),
                                 p_strSearchCtrl(new string),p_strOtherClientPidFile(NULL),p_strWhoOrder(NULL),p_strMyPidFile(new string(*ServerConfigs::p_strMyPath+"Pool/tmp/"+myConv::ToString(pid)+"/"+myConv::ToString(pid)+".pid")),
                                 p_strTodayWorkFolder(new string(*ServerConfigs::p_strMyPath+"Pool/")),
                                 bOrderFileStatus(false),intFilePossition(0)
                                 {
    //p_strClassNameX = ;
    cerr<<GetLocalTime()<<*p_strClassNameX<<" Connecting::"<<ClientUser<<"@"<<ClientIP<<"("<<ClientUserLocal<<")"<<", PID:"<<intPID<<endl;
    p_strSharedXmlList = new XMLParser;
    p_strSearchRezualt = new XMLParser;
    p_xmlOrder = new XMLParser;
    p_xmlIN = new XMLParser;
    p_xmlOUT = new XMLParser;
    //p_strSearchCtrl = new string;
    p_strTodayWorkFolder->append(GetYMD());
}

myClientHandler::~myClientHandler(){
    ///Zwalnianie nie zwolnionej wczesniej pamieci oraz wyswietlnie/wyslanie podsumowania.
    ClientSocket.close();
    CleanSharedList();
    CleanSearchRezualt();
    del(1);
    //Kasuje Ordery, ktore zelecilem
    if (myIO::chk4file(p_strMyPidFile)){
        remove(p_strMyPidFile->c_str());
    }
    delete p_xmlOrder;
    delete p_xmlIN;
    delete p_xmlOUT;
    delete p_strSharedXmlList;
    delete p_strSearchRezualt;
    delete p_strSearchCtrl;
    delete p_strOtherClientPidFile;
    delete p_strTodayWorkFolder;
    delete p_strWhoOrder;
    delete p_strMyPidFile;
    cerr<<GetLocalTime()<<*p_strClassNameX<<"Summary:\n"<<ExcutionTime(GetTime(),dCreationTime)<<" "<<AliveTime(GetTimeAfter1970AsTime(),tt_CreationTime)<<", Data in/out::"<<ui64DataRecieved<<"/"<<ui64DataSend<<"bajts, Msg in/out::"<<ui64RecivedMsgCouter<<"/"<<ui64SendMsgCouter<<"\n----------"<<endl;
    delete p_strClassNameX;
}
template <typename T>
void myClientHandler::MsgToOut(T strMsg){
    ///Wiadomosci
    cerr<<GetLocalTime()<<*p_strClassNameX<<ClientUser<<"@"<<ClientIP<<"("<<ClientUserLocal<<")::"<<strMsg<<endl;
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
            if(intUserLimit > *ServerConfigs::p_intMaxClients){
                Send("221");
                bLoop = false;
            }
        }else if (*p_strSocketBuffer == "root"){
            intGID = 0;
            syslog(3, "PAP Server someone is logged as root");
            MsgToOut("Logged as root");
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
    string strTMP = "";
    double XTime = GetTimeAfter1970AsTime();
        if (dBaseRun()){
            Authorization();
            if (intIndex4Zombie > 0 and intGID >= 0){
                bLoop = Send(SendInfoAboutServer());
                while (bLoop){
                    cout<<*p_strClassNameX+"Ready for new data"<<endl;
                    p_strSocketBuffer = &GetDataFromSocket();
                    cout<<*p_strClassNameX+"Something recived"<<endl;
                    if (p_strSocketBuffer){
                        if (*p_strSocketBuffer == "DisconnectMe" or CoutTimeAfter1970(GetTimeAfter1970AsTime(),XTime) > *ServerConfigs::p_intClientTimeOut){
                            bLoop = false;
                            if(*ServerConfigs::p_intClientTimeOut < CoutTimeAfter1970(GetTimeAfter1970AsTime(),XTime)){strError = "::TimeOut";}
                        }else if (p_strSocketBuffer->length()>1) {
                            XTime = GetTimeAfter1970AsTime();
                            AskIfBanned(ClientIP,ClientUserLocal) == true ? bLoop = false : bLoop;
                            if(bLoop){
                                usleep(10);
                                RecivedDataParser(p_strSocketBuffer);
                            }
                        }//p_strSocketBuffer
                        delete p_strSocketBuffer;
                    }else{
                        cout<<GetLocalTime()<<*p_strClassNameX<<"Connection problem::"<<*ServerConfigs::p_intClientTimeOut - Cout(GetTime(),XTime)<<" to time out"<<endl;
                        Send();
                        usleep(100);
                    }//p_strSocketBuffer
                    //if (bLoop == true ) { bLoop = Send(); }
                    if (CoutTimeAfter1970(GetTimeAfter1970AsTime(),XTime)> *ServerConfigs::p_intClientTimeOut){ bLoop = false; }
                    if (myIO::chk4file(p_strMyPidFile) and !bOrderFileStatus){// Tutaj bedzie sprawdzanie pliku.pid i wpisanie identyfikatora zadajacego procesu.
                        strTMP = checkIfOrdered(bOrderFileStatus);
                        if(strTMP!=""){
                            if(true){// tu zostanie sprawdzony pid zamawiajacego i porownany z baza
                                bOrderFileStatus = true;
                                //zmiana status z get na trying
                                Send(strTMP);
                            }else{
                                Send("Failed Setting pid file");
                            }
                        }
                    }/*
                    if(bOrderFileStatus){
                        //Metoda wysylu pliku
                        //Tu bedzie uzupelnianie licznikow
                        //wpis do pidfile z trying na done [sciezka_do_pliku] lub failed
                        bOrderFileStatus = false;
                    }*/
                }//while bLoop
            }else{
                if (intGID == -1){ strError ="::Logging failed";}
            }//if intIndex4Zombie
        }else{
            strError="::dBase Logging failed";
        }//if dBase
        Send("Disconnected"+strError);
        MsgToOut("Disconnected"+strError);
        //cerr<<GetLocalTime()<<*p_strClassNameX<<" Disconnected::"<<ClientUser<<"@"<<ClientIP<<"("<<ClientUserLocal<<")"<<strError<<endl;
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
        cerr<<GetLocalTime()<<*p_strClassNameX<<e.what()<<endl;
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

    if(p_strData->length()<100){
        cout<<"Recived(below100):"<<*p_strData<<"("<<p_strData->length()<<")"<<endl;
    }else{
        cout<<"Recived data above 100chars"<<"("<<p_strData->length()<<")"<<endl;
    }

    //Commands for user
    if(*p_strData == "GetServerInfo"){
            Send(SendInfoAboutServer());
    }else if(*p_strData == "GetServerTime"){
            Send("Time "+GetTimeAfter1970());
    }else if(*p_strData == "Alive"){
            Send("YES");
    }else if((int)p_strData->find(strSearchFor) != -1 and p_strData->length() > (strSearchFor.length()+1) ){ //Pytanie sie o plik z dBase
            Serach4Files(p_strData,&strSearchFor);
    }else
    //XML
    if((int)p_strData->find("<") != -1 and (int)p_strData->find("</") != -1){
            if((int)p_strData->find("<SharedFiles>") != -1 and (int)p_strData->rfind("</SharedFiles>") != -1 ){
                SetNewFileList(p_strData);
            }else if((int)p_strData->find("<Order>") != -1 and (int)p_strData->rfind("</Order>") != -1 ){
                OrderFiles(p_strData);
            }else if((int)p_strData->find("<Upload>") != -1 and (int)p_strData->rfind("</Upload>") != -1 ){
                p_xmlIN->vCleanXMLTree();
                p_xmlIN->LoadXMLFromBuf(p_strData->c_str());
                GetDoneFileFromOtherClient();
                p_xmlIN->vCleanXMLTree();
            }else if((int)p_strData->find("<Download>") != -1 and (int)p_strData->rfind("</Download>") != -1 ){//Tu jest blad?
                p_xmlOUT->vCleanXMLTree();
                p_xmlOUT->LoadXMLFromBuf(p_strData->c_str());
                p_xmlOUT->GoDeeper();
                p_xmlOUT->NextElement();
                cout<<"Wyjscie:"<<p_xmlOUT->GetCurrentElement();
                if (p_xmlOUT->GetCurrentElement() != "ABORT" or p_xmlOUT->GetCurrentElement() != "DONE"){
                    p_xmlOUT->NextElement();
                    intFilePossition = myConv::FromString<uint64_t>(p_xmlOUT->GetCurrentElement());
                }else{
                    intFilePossition = 0;
                    MsgToOut("FileTransfer complited->"+p_xmlOUT->GetCurrentElement());
                }
                p_xmlOUT->vCleanXMLTree();
            }else{
                Send("900");
            }
    }else{
        //Commandes for super user
        if (intGID == 0 or intGID == 1){
            if(*p_strData == "Shutdown"){
                if (myIO::touch(&(*ServerConfigs::p_strMyPath+ServerConfigs::g_strSlash+"ShutdownServer.pid"))){
                    Send("102");
                }else{
                    Send("f");
                }
            }else if(*p_strData == "Restart"){
                if( myIO::touch(&(*ServerConfigs::p_strMyPath+ServerConfigs::g_strSlash+"RestartServer.pid"))){
                    Send("100");
                }else{
                    Send("f2");
                }
            }else if (*p_strData == "ShowBannedList"){
                Send("900");
            }else if (*p_strData == "ResetBanned"){
                if (Rebuild_BannedTable()){
                    Send("OK");
                }else{
                    Send("Faild");
                }
            }else if (*p_strData == "ResetShared"){
                if(Rebuild_SharedFilesTable()){
                    ServerFilesLoader *SFL = new ServerFilesLoader(0);//getpid());
                    if(SFL){
                        SFL->Run();
                        delete SFL;
                        Send("OK");
                    }else{
                        Send("Faild2");
                    }
                }else{
                    Send("Faild");
                }
            }else if (*p_strData == "ResetOrdered"){
                if(Rebuild_Ordered()){
                    Send("OK");
                }else{
                    Send("Faild");
                }
            }else if (*p_strData == "ShutdownForced"){
                Send("103");
                RestartShutdownServer("stop");
            }else if(*p_strData == "RestartForced"){
                Send("101");
                RestartShutdownServer("restart");
            }else if(*p_strData == "DisconnectEveryOne"){
                cout<<"Nie zaimplementowane"<<endl;
                Send("Nie zaimplementowane");
            }else{
                Send("900");
            }
        }else{
            cout<<"Nie znane polecenie["<<*p_strData<<"]"<<endl;
            Send("900");
        }
    }
}

void myClientHandler::Serach4Files(string *p_strData,string *strSearchFor){
    ///Wyszukiwanie plikow oraz wysylanie rezultatow wyszukiwania do klienta
    int intB = 0;
    int intEnd = 0;
    int intLenght = p_strData->length();
    string strSpacja = " ";
    int intSpacja = strSpacja.length();

        if(p_strSearchRezualt->createXMLDoc("SharedFiles")){
            intB = p_strData->find(strSearchFor->c_str(),intB)+strSearchFor->length();
            if (intB != -1){
                do{
                    intEnd = p_strData->find(strSpacja,intB);
                    intEnd == -1 ? intEnd = intLenght : intEnd;
                    if(!SearchFiles(p_strSearchRezualt,
                                        p_strSearchCtrl,
                                        &p_strData->substr(intB,intEnd-intB)
                                        )
                    ){
                        cerr<<GetLocalTime()<<*p_strClassNameX<<"SearchForFiles:"<<endl;
                        intEnd = intLenght;
                        //Send("303");
                    }
                    intB = intEnd+intSpacja;
                }while(intEnd!=intLenght);
            Send(p_strSearchRezualt->GetXMLAsString());
            usleep(50);
            CleanSearchRezualt();
            }else{
                Send("944");
            }//intB
        }else{//createXML
            Send("602");
        }
}

void myClientHandler::OrderFiles(string *p_strData){
    int intTMP=0;
    string strTMP="";
    p_xmlOrder->vCleanXMLTree();
    p_xmlOrder->LoadXMLFromBuf(p_strData->c_str());
    if(p_xmlOrder->SetWorkNode("Order") or p_xmlOrder->GetCurrentElement()=="Order"){
        if(p_xmlOrder->GoDeeper()){
            do{
                intTMP = newOrder(p_xmlOrder->GetStringValue("FilePath","-1").c_str(),
                            p_xmlOrder->GetStringValue("FileOwner","-1").c_str(),strTMP
                            );
                switch (intTMP){
                    case 0:
                        intFilePossition = 0;
                        SendFile();
                        break;
                    case 1:
                        if (p_strOtherClientPidFile){
                            delete p_strOtherClientPidFile;
                            p_strOtherClientPidFile = NULL;
                        }
                        strTMP = p_xmlOrder->GetStringValue("FileOwner","-1");
                        p_strOtherClientPidFile = new string(*ServerConfigs::p_strMyPath+"Pool/tmp/"+strTMP);
                        if(p_strOtherClientPidFile){
                            if(!myIO::chk4file(p_strOtherClientPidFile)){
                                if(mkdir(p_strOtherClientPidFile->c_str(),0744)!=0){
                                    MsgToOut("PidFolderCreationFailed");
                                    Send("212");
                                }
                            }
                            p_strOtherClientPidFile->append("/"+strTMP+".pid");
                            GetFileFromOtherClient(&p_xmlOrder->GetStringValue("FilePath","-1"));
                            cout<<"p_strToday:"<<*p_strTodayWorkFolder<<endl;
                            if(p_strTodayWorkFolder->rfind("/"+myConv::ToString(intPID)+"/") > p_strTodayWorkFolder->length()){
                                if(!myIO::chk4file(p_strTodayWorkFolder)){
                                    if(mkdir(p_strTodayWorkFolder->c_str(),0744)!=0){
                                        MsgToOut("TodayFolderCreationFailed");
                                        Send("212");
                                    }else{
                                        p_strTodayWorkFolder->append("/"+myConv::ToString(intPID));
                                        if(mkdir(p_strTodayWorkFolder->c_str(),0744)!=0){
                                            MsgToOut("PIDinTodayFolderCreationFailed");
                                            Send("212");
                                        }else{
                                            p_strTodayWorkFolder->append("/");
                                        }
                                    }
                                }
                            }
                        }else{
                            MsgToOut("OutOfMemmory");
                            Send("200");
                        }
                        break;
                    case 3:
                        Send("333");//search failed
                        break;
                    case 4:
                        Send("334");//file n/a
                        break;
                    case 5:
                        Send("335");//faild to add order
                        break;
                    default:
                        Send("366");//unknown error
                        break;
                }
            }while(p_xmlOrder->NextElement());//while
        }else{
            Send("614");
        }//if deeper
    }else{
        Send("613");
    }
    //Send("900");
}

void myClientHandler::SendFile(){
    string *p_strBuf = new string;
    uint64_t intTmp = intFilePossition;
    p_xmlOUT->createXMLDoc("Download");
    p_xmlOUT->addElement("FilePath",p_xmlOrder->GetStringValue("FilePath","-1").c_str());
    p_xmlOUT->addElement("Status","ABORT");
    p_xmlOUT->addElement("Chunk",myConv::ToString(intTmp).c_str());
    p_xmlOUT->addElement("Data","");
    if (p_xmlOUT->GoDeeper() or p_strBuf){
        if (myIO::readFileChunk(&p_xmlOrder->GetStringValue("FilePath","-1"),
                           p_strBuf,
                           intFilePossition,
                           *ServerConfigs::p_intChunkSize)){
            p_xmlOUT->NextElement();
            p_xmlOUT->GoDeeper();
            p_xmlOUT->setCurrentElementValue("OK");
            p_xmlOUT->GoUpper();
            p_xmlOUT->NextElement();
            p_xmlOUT->NextElement();
            p_xmlOUT->GoDeeper();
            /*char *cBuf = new char[p_strBuf->length()+1];
            cBuf[p_strBuf->length()] = '\0';
            p_strBuf->copy(cBuf,p_strBuf->length(),0);*/
            p_xmlOUT->setCurrentElementValue(p_strBuf->c_str());//Tu jest blad z powodu EOF'ow... przy downloadzie po stronie klienta
            intFilePossition += p_strBuf->length();
//            delete cBuf;
        }
        Send(p_xmlOUT->GetXMLAsString());
    }else{
        Send("Blad w alokacji");//blad alkoacji pamieci
    }
    delete p_strBuf;
    p_xmlOUT->vCleanXMLTree();
}

void myClientHandler::GetFileFromOtherClient(const string *strData){
    //sprawdz czy plik istnieje jezeli tak ustaw go do zapisu
    bool bGO = true;
    if (myIO::chk4file(p_strOtherClientPidFile)){ chmod(p_strOtherClientPidFile->c_str(),0644)==0 ? bGO : bGO = false; }
    if (bGO){
        std::ofstream pidFile(p_strOtherClientPidFile->c_str(),std::ios::app);
        if(pidFile.is_open()){
            //pidFile << *strData +" get 0\n";// mapowanie pliku
            pidFile.close();
            Send("WaitFor [SCIEZKA PLIKU]");//success but file have to be downloaded from other client
            chmod(p_strOtherClientPidFile->c_str(),0444);
        }else{
            delete p_strOtherClientPidFile;
            p_strOtherClientPidFile = NULL;
            MsgToOut("Failed to Open pidFile["+(*p_strClassNameX)+"] myPid["+myConv::ToString(intPID)+"]");
            Send("211");
        }
    }else{
        MsgToOut("File Premssion error::"+(*p_strOtherClientPidFile)+" Val: "+myConv::ToString(chmod(p_strOtherClientPidFile->c_str(),0644)));
        Send("210");
    }
}

void myClientHandler::GetDoneFileFromOtherClient(){//get zostanie zastapione statusem a 0 liczba [0-99], done sciezka do pool
    uint8_t intFlag = 0;
    bool bValid = false;
    string strTMP;
    string strTMPHASH;
    if(p_xmlIN->SetWorkNode("Upload") or p_xmlIN->GetCurrentElement()=="Upload"){
        if(p_xmlIN->GoDeeper()){
            strTMP = p_xmlIN->GetCurrentElementValue().substr(p_xmlIN->GetCurrentElementValue().rfind("/"));//sciezka zdalna
            cout<<"strTMP:"<<strTMP<<endl;
            cout<<"Sciezka:"<<*p_strTodayWorkFolder+strTMP<<endl;
            p_xmlIN->NextElement();
            if (p_xmlIN->GetCurrentElementValue() == "DONE" ){
                intFlag = 1;
            }else if(p_xmlIN->GetCurrentElementValue() == "ABORT" ){
                intFlag = 2;
            }//status

            if(!myIO::chk4file(&(p_strMyPidFile->substr(0,p_strMyPidFile->rfind("/"))+strTMP))){
                myIO::touch(&(p_strMyPidFile->substr(0,p_strMyPidFile->rfind("/"))+strTMP));
            }

            switch (intFlag){
                case 0:
                    p_xmlIN->NextElement();
                    p_xmlIN->GetCurrentElementValue();//chunk
                    p_xmlIN->NextElement();
                    myIO::appToFile(&(p_strMyPidFile->substr(0,p_strMyPidFile->rfind("/"))+strTMP),&p_xmlIN->GetCurrentElementValue());//dane
                    p_xmlIN->PrevElement();
                    p_xmlIN->setCurrentElementValue(myConv::ToString(myIO::GetFileSize(&(*p_strTodayWorkFolder+strTMP))).c_str());
                    p_xmlIN->delCurrentElement();
                    //Odeslij potwierdzenie
                    break;
                case 1:
                    p_xmlIN->PrevElement();
                    strTMPHASH = p_xmlIN->GetCurrentElementValue();
                    p_xmlIN->NextElement();
                    p_xmlIN->NextElement();
                    p_xmlIN->NextElement();
                    myIO::appToFile(&(p_strMyPidFile->substr(0,p_strMyPidFile->rfind("/"))+strTMP),&p_xmlIN->GetCurrentElementValue());//dane
                    strTMPHASH = GetHashInfo(strTMPHASH.c_str(),intPID);
                    if(!strTMPHASH.find("None:")){
                        string *p_strHash = new string;
                        myIO::readFile(&(p_strMyPidFile->substr(0,p_strMyPidFile->rfind("/"))+strTMP),p_strHash);
                        if(strTMPHASH.find("sha512:")){
                            strTMPHASH.erase(0,string("sha512:").length());
                            p_strHash->assign(Hash::SHA512(*p_strHash),0,p_strHash->length());
                            if (*p_strHash == strTMPHASH){
                                bValid = true;
                                cout<<"VALID!"<<endl;
                            }else{
                                MsgToOut("File Invalid: "+strTMP+"\nHash: "+*p_strHash+"\nExpected:"+strTMPHASH);
                            }
                            //jezeli sie nie zgadza kasuje plik i ustawiam status w dbase
                        }
                        else if(strTMPHASH.find("md5:")){
                            strTMPHASH.erase(0,string("md5:").length());
                            strTMPHASH.erase(0,string("sha512:").length());
                            p_strHash->assign(Hash::SHA512(*p_strHash),0,p_strHash->length());
                            if(*p_strHash == strTMPHASH){
                                bValid = true;
                                cout<<"VALID!"<<endl;
                            }else{
                                MsgToOut("File Invalid: "+strTMP+"\nHash: "+*p_strHash+"\nExpected:"+strTMPHASH);
                            }
                        }
                        delete p_strHash;
                    }else{
                        bValid = true;
                    }
                    break;
                case 2:
                    p_xmlIN->NextElement();
                    p_xmlIN->NextElement();
                    MsgToOut("File Upload has been aborded reason:"+p_xmlIN->GetCurrentElementValue());
                    //kasuje plik tmp
                default:
                    MsgToOut("Flag switch["+myConv::ToString(intFlag)+"], Error in GetDoneFileFromOtherClient------");
                    MsgToOut(p_xmlIN->GetXMLAsString());
                    MsgToOut("---------------------------");
                    break;
            }
        if (bValid){
        //Tutaj bedzie
        //liczy syme kontrolna jezeli sie zgadza przenosze plik do folderu roboczego
        //dodaje dodaje mapowanie do ordertable i dodaje do bazy danych
        }
        }
    }
}

void myClientHandler::SetNewFileList(string *p_data){
    ///Laduje przeslane dane xml do bazy danych
    int intCounterSharedFiles = 0;//!<Licznik ile plikow udalo sie dodac do bazy
    p_strSharedXmlList->LoadXMLFromBuf(p_data->c_str());
    if(p_strSharedXmlList->SetWorkNode("SharedFiles") or p_strSharedXmlList->GetCurrentElement()=="SharedFiles"){
        if(p_strSharedXmlList->GoDeeper()){
            do{
                if(insertFile(p_strSharedXmlList->GetStringValue("FilePath","-1").c_str(),
                            p_strSharedXmlList->GetStringValue("FileName","-1").c_str(),
                            p_strSharedXmlList->GetStringValue("FileSize","-1").c_str(),
                            p_strSharedXmlList->GetStringValue("FileHashType","-1").c_str(),
                            p_strSharedXmlList->GetStringValue("FileHash","-1").c_str(),
                            p_strSharedXmlList->GetStringValue("FileLastModification","-1").c_str()
                            )){ ++intCounterSharedFiles;}
            }while(p_strSharedXmlList->NextElement());//while
            MsgToOut("Client Shared: "+myConv::ToString(intCounterSharedFiles)+" files");
        }else{
            Send("614");
        }//if deeper
    }else{
        Send("613");
    }//sharedfiles
    CleanSharedList();
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
        fprintf(stderr,"%s[Send]-> %s\n",p_strClassNameX->c_str(),e.what());
        //exit(0);
        bLoop = false;
        //return false;

    }
    ++ui64SendMsgCouter;
    return true;
}

string myClientHandler::SendInfoAboutServer(){
///Zwraca infromacje o serwerze
    string strCommandlist = "XML=1.0,\nServer_Version=";
    strCommandlist.append(AutoVersion::FULLVERSION_STRING);
    strCommandlist.append(",\nCommands=UploadFile,DisconnectMe,GetServerInfo,GetServerTime,SearchFor: <arg0 arg1 arg2>,RenewSharedList,Alive");
    if( intGID == 0 or intGID == 1){
        strCommandlist.append(",ResetBanned,ResetShared,ResetOrdered,Shutdown,ShutdownForced,Restart,DisconnectEveryOne");
    }
    strCommandlist.append("\nLoggedAs "+ClientUser+"~$");
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
        cerr<<p_strClassNameX<<"Czy bedzie suma?:";
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
                if(XMLBuf->GetNumericValue<int>("chunk",myConv::ToString(intIndex))> intIndex){
                    p_strBuf->append(XMLBuf->GetStringValue("data",""));
                    if ( (int)XMLBuf->GetStringValue("data","").length() + intIndex == intSizeOfList ){
                        intIndex = intSizeOfList;
                    }else{
                        intIndex += intChunkSizeUP;
                    }
                }else{
                    if(XMLBuf->GetNumericValue<int>("chunk",myConv::ToString(intIndex))-intIndex < intIndex){
                        //WYSLIJ SEND_PREVOUSE_CHUNK
                    }else{
                        //WYSLIJ SEND_NEXT_CHUNK
                    }
                }
            }else{//WYSLIJ RESEND
            }
            if (intIndex != XMLBuf->GetNumericValue<int>("chunk",myConv::ToString(intSizeOfList))){
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

void myClientHandler::RestartShutdownServer(string strOption){
///Wymuszenie wylaczenia lub restartu servera, ktory jest "zarzadzany przez usluge systemowej demonizacji"
    Send("Server will be "+strOption+" in a moment");
    strOption.insert(0,"cd "+*ServerConfigs::p_strMyPath+"; ./myServer ");
    cout<<strOption<<endl;
    system(strOption.c_str());
}

void myClientHandler::DisconnectAllClients(){
///Nie zaimplementowane
    Send("Disconnecting clients in progress");
}
