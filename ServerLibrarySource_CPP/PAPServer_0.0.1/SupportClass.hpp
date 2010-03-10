#ifndef SUPPORTCLASS_HPP_INCLUDED
#define SUPPORTCLASS_HPP_INCLUDED

///Headers
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <string>
#include <ctime>
#include <sstream>

///Specials
using std::stringstream;
using std::istringstream;
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
void Bandwidth(string *,double*,double*,int64_t *,int64_t =1024*1024);
///Globals Varuabels

///Ta klasa dba tylko o wysyłanie do terminala informacji z określonym opoźnieniem
class debugMsg{
    public:
        debugMsg();
        void MsgOnQuit(string *,int);
        void MsgWithDelay(string*,int);
};

///Ta klasa uzywa operatorów strumieni do konwersji danych do wskazanych typów
class konwertery{
    public:
            //konwertery();
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
            //Zwalnianie pamieci
            //~konwertery();
};

///Ta klasa dba o właściwe obliczanie czasu przetwarzania
class TimeCout: public konwertery{
    public:
        TimeCout();
        double Cout(double,double);
        double GetTime();
        string ExcutionTime(double,double);
};

#endif // SUPPORTCLASS_HPP_INCLUDED
