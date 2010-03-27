/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (www.SoonWillBe.pl)
 * License:   GNU / General Public Licens
 **************************************************************/
///Headers
#include "myServerAPP.hpp"

///Specials
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
///Globals Varuabels

int myServerAPP::intValueOnExit = 1;
myServerAPP::myServerAPP(): p_strClassNameE(NULL), bServerLoop(true),dCreationTime(GetTime()),tt_CreationTime(GetTimeAfter1970AsTime()){
    p_strClassNameE = new string("[myServer]->");
    cerr<<GetLocalTime()<<*p_strClassNameE<<"PID:"<<getppid()<<endl;
    intValueOnExit = 0;
}
myServerAPP::~myServerAPP(){
    delete p_strClassNameE;
}

void myServerAPP::RunServer(){
    ///Tutaj dzieje sie magia czyli, serwer przyjmuje połączenia oraz tworzy procesy potomne
    double Xtime = GetTime();
    string *p_strMethodName = new string (*p_strClassNameE+"[RunServer]->");
    ///Test
    int intIndexZombie = 0 ;
    pid_t pid;

    BannedNode *p_bnNode = NULL;
    list<BannedNode*> BannedList2;
    list<BannedNode*>::iterator iterBN_List;
    list<BannedNode*>::iterator p_iterBN_List;
//    list<BannedNode*>::pointer p_NodeList;
    ///!-------
    try{
        ///Potrzebne do połączenia z klientem
        boost::asio::io_service io_service;
        ///Potrzebne do połączenia z klientem
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), ServerConfigs::intServerPort);
        ///Potrzebne do połączenia z klientem
        boost::asio::ip::tcp::acceptor acceptor(io_service, endpoint);
        ///Watek ktory bedzie usuwal skonczone forki czy dziala nie wiem, pewnie nie :-)
        //boost::thread w1(&KillZombie);
        while (bServerLoop){///Nieskonczona pętla
            ///Wyjście/Wejście socketa
            boost::asio::ip::tcp::socket socket(io_service);
            acceptor.accept(socket);
            boost::asio::ip::tcp::endpoint endpoint = socket.local_endpoint();
            ++intIndexZombie;
            ///Sprawdzam na liscie czy uzytkownik laczyl sie w czesniej.
            if (BannedList2.begin() == BannedList2.end()){
                    p_bnNode = new BannedNode(endpoint.address().to_string(),boost::asio::ip::host_name(),intIndexZombie);
                    BannedList2.push_back(p_bnNode);
            }
            for ( p_iterBN_List = BannedList2.begin(); iterBN_List != BannedList2.end(); ++p_iterBN_List){
                if( (*p_iterBN_List)->SearchForIPandLocalName(endpoint.address().to_string(),boost::asio::ip::host_name()) == true){
                    break;
                }else{
                    p_bnNode = new BannedNode(endpoint.address().to_string(),boost::asio::ip::host_name(),intIndexZombie);
                    BannedList2.push_back(p_bnNode);
                    p_iterBN_List = BannedList2.end();
                    break;
                }
            }
               cout<<"----------"<<endl;
               (*p_iterBN_List)->PrintInfo();
               cout<<"----------"<<endl;

            if ( (*p_iterBN_List)->AskIfBanned() == false){///Jezeli uzytkownik byl zbanowany to nie zostanie podlaczony.
                if ( *ServerConfigs::p_intMultiThreading == 1){
                /**
                *Wielo watkowa oblsuga klientow*/
                }else if ( *ServerConfigs::p_intMultiThreading == 0){
                /**
                *Klient 1 w jednym czasie brak wyciekow pamieci
                *Nie dziala Restart, odlaczanie klientow*/
                    myClientHandler *o = new myClientHandler(socket,
                                  endpoint.address().to_string(),boost::asio::ip::host_name(),"tu bedzie pass",
                                  intIndexZombie,bServerLoop);
                    o->myClientRun(p_iterBN_List);
                    delete o;
                }else{
                /**
                *Fork powoduje wyciek pamieci
                *Tworze client handler w forku.*/
                    pid = fork();
                    if (pid == 0){
                    myClientHandler *o = new myClientHandler(socket,
                                  endpoint.address().to_string(),boost::asio::ip::host_name(),"tu bedzie pass",
                                  intIndexZombie,bServerLoop);
                    o->myClientRun(p_iterBN_List);
                    delete o;
                    waitpid ( pid, NULL,WNOHANG);
                    exit(0);
                    }//if pid
                }//if *ServerConfigs
            }else{
                cerr<<*p_strMethodName<<endpoint.address().to_string()<<" "<<boost::asio::ip::host_name()<<"::has been banned but tried to connect again"<<endl;
                syslog(3, "PAP Server banned user is tried to connect");
                socket.close();
            }// if *p_iterBN_List
        }//while bServerLoop
        if (BannedList2.begin() != BannedList2.end()){
            for(iterBN_List = BannedList2.begin(); iterBN_List != BannedList2.end(); ++iterBN_List){
                delete *iterBN_List;
            }//for zwalnianie pamieci
        }else{
            delete *p_iterBN_List;
        }
        BannedList2.clear();
    }catch (std::exception& e){
        cerr<<*p_strMethodName<<e.what()<<endl;
    }
    cout<<*p_strMethodName<<ExcutionTime(GetTime(),Xtime)<<" "<<AliveTime(GetTimeAfter1970AsTime(),tt_CreationTime)<<endl;
    delete p_strMethodName;
}
