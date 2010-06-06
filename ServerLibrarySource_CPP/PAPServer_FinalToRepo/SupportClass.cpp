/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (www.SoonWillBe.pl)
 * License:   GNU / General Public Licens
 **************************************************************/
///Headers
#include "SupportClass.hpp"

///Specials
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
///Globals Varuabels

void Bandwidth(string *nazwa, double *end,double *start,int64_t *length, int64_t g_scale){
    /**
    * Ta metdoa sluży do obliczania przepustowości.
    * dzieki zastosowaniu wskażników jest 5% szybsza.
    */

    string *NazwaFunkcji = new string (*nazwa+"Bandwidth: ");
    int64_t *scale = new int64_t(g_scale);
    string *sScale;
    if (*scale == 1024){
        sScale = new string (" KB");
    }else if ( *scale == 1024*1024){
        sScale = new string (" MB");
    }else if ( *scale == 1024*1024*1024){
        sScale = new string (" MB");
    }
    else{
        sScale = new string (" B");
    }
    if (*length){
        if (*length > *end && *end >= 0.01){
            cout<<*NazwaFunkcji<<int64_t(*length/ (*end - *start))/ *scale<<*sScale<<endl;
        }
        else{
            cout<<*NazwaFunkcji<<(double)*length/ *scale<<*sScale<<endl;
        }
    }else{
        cout<<cout<<*NazwaFunkcji+"NULL extention!"<<endl;
    }
    delete scale;
    delete sScale;
    delete NazwaFunkcji;
}




string TimeCout::strTimeFormatter = "[%Y/%m/%d %H:%M:%S]::";
int TimeCout::intFormatterLenght = 80;

TimeCout::TimeCout(){
}
TimeCout::~TimeCout(){
}

double TimeCout::GetTime(){
    ///Zwraca własciwie obliczony czas od właczenia programu
    return (double)clock()/CLOCKS_PER_SEC;
}

double TimeCout::Cout(double dLiczbaA, double dLiczbaB){
    ///Zwraca różnice czasów
    return dLiczbaA - dLiczbaB;
}

string TimeCout::ExcutionTime(double A,double B){
    ///Zwraca stringa składający się z opis + rożnice czasu
    return "Execution: "+myConv::ToString(Cout(A,B))+"s";
}

string TimeCout::GetLocalTime(){
    ///Pobiera czas wedlug zdefinowanego formatowania
    time_t rawtime; //!< pobranie czasu
    struct tm * timeinfo; //!< struktura czasu, dzieki ktorej dziala formatowanie
    char buffer2 [BUFFER]; //!< buffor do ktorego zostanie wpisany string

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    strftime (buffer2,BUFFER,strTimeFormatter.c_str(),timeinfo);

    return buffer2;
}

string TimeCout::GetTimeAfter1970(){
    char *buffer = new char[BUFFER];

    sprintf(buffer,"%ld",GetTimeAfter1970AsTime());
    string strReturn = buffer;
    delete[] buffer;

    return strReturn;
}

time_t TimeCout::GetTimeAfter1970AsTime(){
    return time(NULL);
}

long int TimeCout::CoutTimeAfter1970(time_t newTime,time_t oldTime){
    return newTime - oldTime;
}

string TimeCout::AliveTime(time_t newTime, time_t oldTime){
    char *buffer = new char[BUFFER];

    sprintf(buffer,"Alive for: %ld",CoutTimeAfter1970(newTime,oldTime));
    string strReturn = buffer;
    delete[] buffer;

    return strReturn+"s";
}

void TimeCout::Set_strTimeFormatter(string strFormatter){
    strTimeFormatter = strFormatter;
}

void TimeCout::Set_intFormatterLenght(int lenght){
    intFormatterLenght = lenght;
}

debugMsg::debugMsg(){
}
debugMsg::~debugMsg(){
}

void debugMsg::MsgOnQuit(string *strMsg, int intValue =1){
    ///Wypisuje wiadomość, następnie usypia watęk w którym działa i wyłacza go
    MsgWithDelay(strMsg,intValue);
    exit(1);
}

void debugMsg::MsgWithDelay(string *strMsg, int intValue =1){
    ///Wiadmość, z opóźnieniem
    cout<<*strMsg<<endl;
    sleep(intValue);
}
