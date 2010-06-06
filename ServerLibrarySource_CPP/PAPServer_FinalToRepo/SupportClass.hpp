#ifndef SUPPORTCLASS_HPP_INCLUDED
#define SUPPORTCLASS_HPP_INCLUDED
/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (www.SoonWillBe.pl)
 * License:   GNU / General Public Licens
 **************************************************************/

///Headers
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <string>
#include <ctime>
#include "myConv.hpp"

///Specials
#define BUFFER 1024
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
void Bandwidth(string *,double*,double*,int64_t *,int64_t =1024*1024);

///Ta klasa dba tylko o wysyłanie do terminala informacji z określonym opoźnieniem
class debugMsg{
    public:
        debugMsg();
        virtual ~debugMsg();
        void MsgOnQuit(string *,int);
        void MsgWithDelay(string*,int);
};

///Ta klasa dba o właściwe obliczanie czasu przetwarzania
class TimeCout{
    private:
        static string strTimeFormatter;//!<Formatowanie daty wpisywanej do pliku logow.
        static int intFormatterLenght;//!<Rozmiar buffora czasu
    public:
        TimeCout();
        virtual ~TimeCout();
        double Cout(double,double);
        double GetTime();
        string ExcutionTime(double,double);
        string GetLocalTime();
        string GetTimeAfter1970();
        string AliveTime(time_t,time_t);
        time_t GetTimeAfter1970AsTime();
        long int CoutTimeAfter1970(time_t,time_t);
        void Set_strTimeFormatter(string);
        void Set_intFormatterLenght(int);
};

#endif // SUPPORTCLASS_HPP_INCLUDED
