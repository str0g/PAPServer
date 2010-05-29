#ifndef LISTAV2_HPP_INCLUDED
#define LISTAV2_HPP_INCLUDED

///Headers
#include <cstdlib>
#include <iostream>
#include <string>
#include <ctime>
#include "SupportClass.hpp"
///Specials
using namespace std;
///Drzewo jest klasa, ktora inicjuje i obsluguje liste.
template <class T>
class myList:public konwertery{
    protected:
        string *p_strClassName;
        ///Lisc jest struktura danych dla drzewa i dla tego nie posida zdeklarowanego konstruktora, ani destruktora. Jego dane są statyczne w celu nie generowania dodatkowej nie potrzebnej fragmentacji pamieci.
        /*class lisc{
            public:
                string strNazwaklasy;
                string strRecordDanych;
                int index;
                lisc *wskNastepny; //!< Trzyma adres nastepnika
                lisc *wskPoprzedni; //!< Trzyma adres poprzednika
        };
        lisc *wskNastepny;
        lisc *wskPoprzedni;
        lisc *wskPoczatek;//!< Trzyma adres poczatku listy
        lisc *wskSrodek;//!< Trzyma adres srodka listy
        lisc *wskKoniec;//!< Trzyma adres konca listy
        lisc *wskPomocniczy;*/
        void *p_Node;//!< Wskaznik na klase, ktora uzyje jako lisc
        void *p_prev;
        void *p_next;
        void *p_Begin;//!< Trzyma adres poczatku listy
        void *p_Centre;//!< Trzyma adres srodka listy
        void *p_End;//!< Trzyma adres konca listy
        void *p_Helper;//!< Pomocniczy wskaznik
        int intNodeCounter;//!< Liczba elementow, w celu sprawdzenia spojnosc
        int intNodeIndex;//!<Unikalny licznik elementow listy
    public:
        myList();
        void Sterowanie();
        int RysujMenu();
        void WypiszElementy();
        void AutoTest();
        //Zwracanie
        int GetNodeCounter();
        int GetNodeIndex();
        //Operowanie na liscie
        int RecountList();
        void WyswietlSrodkowyelement();
        int WyliczSrodekListy();
        void WskaznikNaSrodkowyListy(T*);
        void ZamienLiscieMiejscami(T *,T*);
        void AddNode(T*,string);
        //Kasowanie
        void DeleteNode(T *, int);
        void DeleteList(T*);
        virtual ~myList();
};

#endif // LISTAV2_HPP_INCLUDED
