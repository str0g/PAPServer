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

int g_intNumerOfCurrentProcess = 0;

void sig_child(int sig)
{
    ///Usuwa skonczone procesy potomne
    signal(SIGCHLD, sig_child);
    int status;
    pid_t pid;
    pid = wait(&status);
    TimeCout *Time = new TimeCout;
    cerr<<Time->GetLocalTime()<<"[sig_child]->Zombie Pid: "<<pid<<" Status: "<<status<<endl;
    delete Time;
    --g_intNumerOfCurrentProcess;
}

int myServerAPP::intValueOnExit = 1;
myServerAPP::myServerAPP(): dBase(getpid()),
                            p_strClassNameE(new string("[myServer]->")),
                            bServerLoop(true),dCreationTime(GetTime()),
                            tt_CreationTime(GetTimeAfter1970AsTime()){
    //p_strClassNameE = new string("[myServer]->");
    cerr<<GetLocalTime()<<*p_strClassNameE<<"PID:"<<getpid()<<endl;
    intValueOnExit = 0;
}
myServerAPP::~myServerAPP(){
    ClearVal();
}

void myServerAPP::ClearVal(){
    delete p_strClassNameE;
    p_strClassNameE = NULL;
}

void myServerAPP::RunServer(){
    /**
    *Tutaj dzieje sie magia czyli, serwer przyjmuje połączenia oraz tworzy procesy potomne
    *Sprawdzane jest tez czy klient dostal bana na podstawie ostrzezen
    *Bezpieczne Wylaczanie i resetowania serwera
    *Decyzja czy serwer jest interacyjny wielowatkowy/procesowy.
    */
    double Xtime = GetTime();
    string *p_strMethodName = new string(*p_strClassNameE+"[RunServer]->");
    string *strShutdown = new string(*ServerConfigs::p_strMyPath+ServerConfigs::ServerConfigs::g_strSlash+"ShutdownServer.pid");
    string *strRestart = new string(*ServerConfigs::p_strMyPath+ServerConfigs::ServerConfigs::g_strSlash+"RestartServer.pid");

    if ( *ServerConfigs::p_intMultiThreading == 2){
        signal(SIGCHLD, sig_child);
    }
    int intIndexZombie = 0 ;
    pid_t pid;
    ///Test

    ServerFilesLoader *SFL = new ServerFilesLoader(0);//getpid());
    SFL->Run();
    delete SFL;


    //ServerFilesLoader SFL(0);//getpid());

    char p_cIPHelp[16];
    char p_cLoclaHelp[1024];

    if (dBaseRun()){
    ///!-------
        try{
            ///Potrzebne do połączenia z klientem
            boost::asio::io_service io_service;
            ///Potrzebne do połączenia z klientem
            boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), ServerConfigs::intServerPort);
            ///Potrzebne do połączenia z klientem
            boost::asio::ip::tcp::acceptor acceptor(io_service, endpoint);
            while (bServerLoop){///Nieskonczona pętla
                ///Wyjście/Wejście socketa
                boost::asio::ip::tcp::socket socket(io_service);
                acceptor.accept(socket);
                boost::asio::ip::tcp::endpoint endpoint = socket.local_endpoint();
                ++intIndexZombie;
                ++g_intNumerOfCurrentProcess;
                if(myIO::chk4file(strShutdown)){
                    bServerLoop = false;
                    if (remove(strShutdown->c_str())==-1){
                        cerr<<GetLocalTime()<<*p_strClassNameE<<"ShutdownServer.pid hasn't been deleted!"<<endl;
                    }
                }else if(myIO::chk4file(strRestart)){
                    bServerLoop = false;
                    if (remove(strRestart->c_str())==-1){
                        cerr<<GetLocalTime()<<*p_strClassNameE<<"RestartServer.pid hasn't been deleted!"<<endl;
                    }else{
                        system("sleep 60 && ./start.sh start&");
                    }
                }

                ///Sprawdzam  czy uzytownik dostala bana przy poprzednich polaczeniach
                strcpy(p_cIPHelp, endpoint.address().to_string().c_str());
                strcpy(p_cLoclaHelp, boost::asio::ip::host_name().c_str());

                if (!AskIfBanned(p_cIPHelp,p_cLoclaHelp) and bServerLoop){
                    if ( *ServerConfigs::p_intMultiThreading == 1){
                    /**
                    *Wielo watkowa oblsuga klientow*/
                    }else if ( *ServerConfigs::p_intMultiThreading == 0){
                    /**
                    *Klient 1 w jednym czasie brak wyciekow pamieci
                    *Nie dziala Odlaczanie klientow*/
                        myClientHandler *o = new myClientHandler(socket,
                                  p_cIPHelp,p_cLoclaHelp,"tu bedzie pass",
                                  intIndexZombie,getppid(),g_intNumerOfCurrentProcess);
                        o->myClientRun();
                        delete o;
                    }else{
                    /**
                    *Fork powoduje wyciek pamieci
                    *Tworze client handler w forku.*/
                        pid = fork();
                        if (pid == 0){
                            mClose(false);
                            myClientHandler *o = new myClientHandler(socket,
                                  p_cIPHelp,p_cLoclaHelp,"tu bedzie pass",
                                  intIndexZombie,getpid(),g_intNumerOfCurrentProcess);
                            o->myClientRun();
                            delete o;
                            //waitpid ( pid, NULL,WNOHANG);
                            ServerConfigs::Clean();
                            ClearVal();
                            delete strShutdown;
                            delete strRestart;
                            delete p_strMethodName;
                            exit(0);
                        }//if pid
                    }//if *ServerConfigs
                }else{
                    cerr<<GetLocalTime()<<*p_strMethodName<<endpoint.address().to_string()<<" "<<boost::asio::ip::host_name()<<"::has been banned but tried to connect again"<<endl;
                    syslog(3, "PAP Server banned user is tried to connect");
                    socket.close();
                }// if ban
            }//while bServerLoop
            Rebuild_SharedFilesTable();
            Rebuild_Ordered();

        }catch (std::exception& e){
            cerr<<GetLocalTime()<<*p_strMethodName<<e.what()<<endl;
        }
    }
    myIO::rmAll(*ServerConfigs::p_strMyPath+"/Pool/tmp/");
    cout<<GetLocalTime()<<*p_strMethodName<<ExcutionTime(GetTime(),Xtime)<<" "<<AliveTime(GetTimeAfter1970AsTime(),tt_CreationTime)<<endl;
    delete strShutdown;
    delete strRestart;
    delete p_strMethodName;
}
