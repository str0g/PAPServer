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
        /*SEND INFO TO LOGGER*/
    }
    delete scale;
    delete sScale;
    delete NazwaFunkcji;
}

TimeCout::TimeCout(){
}

double TimeCout::GetTime(){
    return (double)clock()/CLOCKS_PER_SEC;
}

double TimeCout::Cout(double dLiczbaA, double dLiczbaB){
    return dLiczbaA - dLiczbaB;
}

string TimeCout::ExcutionTime(double A,double B){
    return "Execution: "+liczba_na_string(Cout(A,B))+"s";
}

debugMsg::debugMsg(){
}

void debugMsg::MsgOnQuit(string *strMsg, int intValue =1){
    cout<<*strMsg<<endl;
    sleep(intValue);
    exit(1);
}

void debugMsg::MsgWithDelay(string *strMsg, int intValue =1){
    cout<<*strMsg<<endl;
    sleep(intValue);
}
