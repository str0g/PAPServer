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
#include <sstream>

///Specials
#define BUFFER 1024
using std::stringstream;
using std::istringstream;
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
void Bandwidth(string *,double*,double*,int64_t *,int64_t =1024*1024);
/*
template <typename T>
extern string liczba_na_string(T);
template <typename T>
extern T string_naliczba(string);*/


/**
*Konwertuje dowolna liczbę na string
*/
template <typename T>
string liczba_na_string(const T& liczba){

    stringstream ss;
    ss << liczba;
    return ss.str();
}
template <typename T>
/*!
*Metoda przyjmoje na wejsciu dowolnych łańcuch textowy i przerabia go na liczbę w zdeklarowanym typie
*/
T string_naliczba(string s_liczba){
    T liczba;

    istringstream myStream(s_liczba);
    myStream>>liczba;
    return liczba;
}

///Ta klasa dba tylko o wysyłanie do terminala informacji z określonym opoźnieniem
class debugMsg{
    public:
        debugMsg();
        virtual ~debugMsg();
        void MsgOnQuit(string *,int);
        void MsgWithDelay(string*,int);
};



///Ta klasa uzywa operatorów strumieni do konwersji danych do wskazanych typów
class konwertery{
    public:
            konwertery();
            virtual ~konwertery();
            //Z dowolnej liczby robi stringa
            template <typename T>
            /**
            *Konwertuje dowolna liczbę na string
            */
            string liczba_na_string(const T& liczba){
                                stringstream ss;
                                ss << liczba;
                                return ss.str();
                                }
            //Z dowolnego stringa robi liczbe
            template <typename T>
            /*! Metoda przyjmoje na wejsciu dowolnych łańcuch textowy i przerabia go na liczbę w zdeklarowanym typie
            */
            T string_naliczba(string s_liczba){
                                    T liczba;
                                    istringstream myStream(s_liczba);
                                    myStream>>liczba;
                                    return liczba;
                                                }
};

///Ta klasa dba o właściwe obliczanie czasu przetwarzania
class TimeCout: public konwertery{
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
