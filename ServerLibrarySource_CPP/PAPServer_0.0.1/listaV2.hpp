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
class drzewo:public konwertery{
    protected:
        string *wskNazwaklasy;
        ///Lisc jest struktura danych dla drzewa i dla tego nie posida zdeklarowanego konstruktora, ani destruktora. Jego dane są statyczne w celu nie generowania dodatkowej nie potrzebnej fragmentacji pamieci.
        class lisc{
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
        lisc *wskPomocniczy;
        int intLiczbaElementow;//!< Liczba elementow, w celu sprawdzenia spojnosc
        int intUnikalnyIndex;
        int intSrodekListy;//!< Mniej wiecej, ile elementow jest do srodka
    public:
        drzewo();
        void Sterowanie();
        int RysujMenu();
        void WypiszElementy();
        void AutoTest();
        void WyswietlSrodkowyelement();
        void DodajLisc(string, string);
        void UsunLisc(string);
        void UsunLisc(int);
        void WyliczSrodekListy();
        void ZamienLiscieMiejscami(lisc *,lisc*);
        void UsunListe();
        ~drzewo();
};

#endif // LISTAV2_HPP_INCLUDED
