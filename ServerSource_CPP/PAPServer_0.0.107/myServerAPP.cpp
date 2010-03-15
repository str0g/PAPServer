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
myServerAPP::myServerAPP():bServerLoop(true){
    p_strClassNameE = new string("[myServer]->");
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
    myClientHandler *p_myClientHandler = NULL;
    int intIndexZombie = 0 ;
    ///!-------
    try{
        ///Potrzebne do połączenia z klientem
        boost::asio::io_service io_service;
        ///Potrzebne do połączenia z klientem
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), ServerConfigs::intServerPort);
        ///Potrzebne do połączenia z klientem
        boost::asio::ip::tcp::acceptor acceptor(io_service, endpoint);
        ///Watek ktory bedzie usuwal skonczone forki czy dziala nie wiem, pewnie nie :-)
        boost::thread w1(&KillZombie);
        while (bServerLoop){///Nieskonczona pętla
            ///Wyjście/Wejście socketa
            boost::asio::ip::tcp::socket socket(io_service);
            cout<<*p_strMethodName<<"Oczekiwanie na polaczenie"<<endl;
            acceptor.accept(socket);
            boost::asio::ip::tcp::endpoint endpoint = socket.local_endpoint();
            //bServerLoop = false; sleep(15);
            ///TEST
            if(p_myClientHandler){cout<<"klient dziala"<<p_myClientHandler->dCreationTime<<endl; delete p_myClientHandler;}
            p_myClientHandler = NULL;
            p_myClientHandler = new myClientHandler(socket,endpoint.address().to_string(),boost::asio::ip::host_name(),"tu bedzie pass",NULL,NULL,intIndexZombie);
            ++intIndexZombie;
            ///!---
            if (fork()==0){
                ///Tworze handler clienta w forku i dodaje go do listy, żeby zachować nad nim kontrolę.
                //myClientHandler o(socket,endpoint.address().to_string(),boost::asio::ip::host_name(),"tu bedzie pass",NULL,NULL,intIndexZombie);
                //o.myClientRun();
                p_myClientHandler = new myClientHandler(socket,endpoint.address().to_string(),boost::asio::ip::host_name(),"tu bedzie pass",NULL,NULL,intIndexZombie);
                p_myClientHandler->myClientRun();
            }
        }
    }catch (std::exception& e){
        cerr<<*p_strMethodName<<e.what()<<endl;
    }


    cout<<*p_strMethodName<<ExcutionTime(GetTime(),Xtime)<<endl;
    delete p_strMethodName;
}
