/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (www.SoonWillBe.pl)
 * License:   GNU / General Public Licens
 **************************************************************/
///Headers
#include "listaV2.hpp"
extern int intZapis;//!< Zmienna globalna "importowana z main"
///Specials
using namespace std;
///Globals Varuabels
/*SEKCJA DRZEWA*/
drzewo::drzewo():wskPoczatek(NULL),wskSrodek(NULL),wskKoniec(NULL),wskNastepny(NULL),wskPoprzedni(NULL),wskPomocniczy(NULL),intLiczbaElementow(0),intSrodekListy(0),intUnikalnyIndex(0){
///Inicujej wartosci w sposob maksymalnie oszczedzajacy pamiec
    string strNazwaklasy = "[Drzewo]->";
    wskNazwaklasy = new string(strNazwaklasy);
    if(!wskNazwaklasy){ cerr<<strNazwaklasy+"Blad alokacji pamieci wskNazwaKlasy"<<endl; exit(1); } //Ze wzgledu, ze jest to lista bledy pamieci musza konczyc program
    else{
       // Sterowanie();
    }
}
int drzewo::RysujMenu(){
    /**
    * Wypisuje textowe Menu i czekam na wpisanie znaku, ktory zostanie zwrocony
    */
    string strNazwaModulu="[NarysujMenu]->";
    string *wskNazwaModulu = new string (*wskNazwaklasy+strNazwaModulu);
    if(!wskNazwaModulu){ cerr<<*wskNazwaklasy+strNazwaModulu+"Blad alkokacji pamieci"<<endl; }

    int zwroc;
    cout<<"Dostepne opcje to:"<<endl;
    cout<<"1. Wypisz liste"<<endl;
    cout<<"2. Dodaj do listy"<<endl;
    cout<<"3. Usun z listy po indexie"<<endl;
    cout<<"4. Usun z listy po nazwie"<<endl;
    cout<<"5. Testuj danymi testowymi"<<endl;
    cout<<"6. Wczytaj (nie za implementowane narazie)"<<endl;
    cout<<"8. zapisz i wyjdz"<<endl;
    cout<<"9. Koniec"<<endl;
    cout<<"Podaj wyborany numer i zatwierdz Enterem: ";
    cin>>zwroc;
    return zwroc;
}
void drzewo::Sterowanie(){
    bool koniec=false;
    string strNazwa;
    string strDane;
    int intIndex;
    while(!koniec){
        switch(RysujMenu()){
            case 1:
                WypiszElementy();
                cout<<"\n\n\n"<<endl;
                break;
            case 2:
                cout<<"Podaj Nazwe: ";
                cin>>strNazwa;
                cout<<"Podaj Dane: ";
                cin>>strDane;
                DodajLisc(strNazwa,strDane);
                cout<<"\n\n\n"<<endl;
                break;
            case 3:
                cout<<"Podaj index do usuniecia:";
                cin>>intIndex;
                UsunLisc(intIndex);
                cout<<"\n\n\n"<<endl;
                break;
            case 4:
                cout<<"Podaj nazwe do usuniecia:";
                cin>>strNazwa;
                UsunLisc(strNazwa);
                cout<<"\n\n\n"<<endl;
                break;
            case 5:
                AutoTest();
                cout<<"\n\n\n"<<endl;
                break;
            case 6:
                break;
            case 7:
                break;
            case 8:
//                intZapis=1;
                koniec=true;
                break;
            case 9:
                koniec=true;
                break;
            default:
                cout<<"\nNie wlasciwy argument\n"<<endl;
                break;
        }
    }
}
void drzewo::AutoTest(){
/** Ta metoda testuje czy lista dziala poprawnie dzieki
*
*/
    cout<<"Wypisuje elementy: (powinno byc 0)"<<endl;
    WypiszElementy();
    cout<<"Dodaje elementy: nazwa,nazw,nazwb,na,nazwdas \n(powinno byc 5)"<<endl;
    DodajLisc("nazwa","text");
    WyswietlSrodkowyelement();
    DodajLisc("nazw","text");
    WyswietlSrodkowyelement();
    DodajLisc("nazwb","text");
    WyswietlSrodkowyelement();
    DodajLisc("na","text");
    WyswietlSrodkowyelement();
    DodajLisc("nazwdas","text");
    WyswietlSrodkowyelement();
    WypiszElementy();
    cout<<"Usuwam elementy po indexie 1,5 po nazwie na[4]:"<<endl;
    UsunLisc(1);
    WyswietlSrodkowyelement();
    UsunLisc(5);
    WyswietlSrodkowyelement();
    UsunLisc("na[4]");
    WyswietlSrodkowyelement();
    WypiszElementy();
}

void drzewo::WyswietlSrodkowyelement(){
    cout<<"Srodkowy element to ["+wskSrodek->strNazwaklasy+"]"<<endl;
}

void drzewo::DodajLisc(string Nazwa,string text){
/**Dodaje nowe elementy w sposob posortowany odrazu sprawdzajac gdzie powinno byc ich miejsce.
*Wyszukiwanie zaczyna sie od srodka listy dla oszczednosci czasu.
*/
    string strNazwaklasy="[DodajLisc]->";
    string *wskNazwaMetody = new string(*wskNazwaklasy+strNazwaklasy);
    if(!wskNazwaMetody){ cerr<<*wskNazwaklasy+strNazwaklasy+"Blad alokacji pamieci wskNazwaKlasy"<<endl; exit(1); }
    lisc *wskPomocniczy;
    double dLimitCzasuOperacji;
    this->wskPomocniczy = new lisc; //wskaznik this "wskazuje na ta metode" dzieki niemu moge sie odwolac do zmiennej lokalnej o takie samej nazwie co zmienna zadeklarowana dla klasy
    if(this->wskPomocniczy){
        this->wskPomocniczy->index=intUnikalnyIndex+1;
        this->wskPomocniczy->strNazwaklasy=Nazwa+"["+liczba_na_string(intUnikalnyIndex+1)+"]"; //numeracja licznika celwo zaczyna sie od 1
        this->wskPomocniczy->strRecordDanych=text;

        ++intLiczbaElementow;
        ++intUnikalnyIndex;

        if(wskPoczatek==NULL){ //pierwszy element jest wyjatkowy dla tego trzeba go wyroznic
            this->wskPomocniczy->wskPoprzedni=NULL;
            this->wskPomocniczy->wskNastepny=NULL;

            wskPoczatek=this->wskPomocniczy;
            wskSrodek=this->wskPomocniczy;
            wskKoniec=this->wskPomocniczy;
            intSrodekListy = 1;

        }
        else if(wskPoczatek!=NULL){ //tutaj dodaje kazdy kolejny lisc

            if(wskSrodek->strNazwaklasy == this->wskPomocniczy->strNazwaklasy){//takie zdazenie nie moze miec miejsca ale lepiej byc przygotowanym
                cerr<<*wskNazwaklasy+"Obiekt o identycznej nazwie"<<endl;
                this->wskPomocniczy->wskPoprzedni=wskSrodek;
                this->wskPomocniczy->wskNastepny=wskSrodek->wskNastepny;
                wskSrodek->wskNastepny=this->wskPomocniczy;
            }
            else if(wskSrodek->strNazwaklasy > this->wskPomocniczy->strNazwaklasy){//Jezeli jest mniejszy
                dLimitCzasuOperacji = (double)clock()/CLOCKS_PER_SEC;
                for(wskPomocniczy=wskSrodek;  wskPomocniczy!=NULL; wskPomocniczy=wskPomocniczy->wskPoprzedni){
                    if((double)clock()/CLOCKS_PER_SEC - dLimitCzasuOperacji > 5){///Jezeli cos pojdzie nie tak petla zostanie przerwana po 5sekundach, a element trafi na koniec. Dzieki temu nie bedzie wycieku pameci.
                        cerr<<*wskNazwaMetody+"Przekroczono limit czasu operacji < , obiekt "+this->wskPoprzedni->strNazwaklasy+"trafia na koniec listy"<<endl;
                        this->wskPomocniczy->wskNastepny=NULL;
                        this->wskPomocniczy->wskPoprzedni=wskKoniec;
                        wskKoniec->wskNastepny=this->wskPomocniczy;
                        wskSrodek=wskSrodek->wskNastepny;
                        break;
                    }else if(wskPomocniczy->wskPoprzedni==NULL || (wskPomocniczy->strNazwaklasy <= this->wskPomocniczy->strNazwaklasy)){//jezeli jest mniejszy rowny albo nasz jest najmniejszy
                        this->wskPomocniczy->wskNastepny=wskPomocniczy;
                        this->wskPomocniczy->wskPoprzedni=wskPomocniczy->wskPoprzedni;
                        wskPomocniczy->wskPoprzedni=this->wskPomocniczy;
                        wskSrodek=wskSrodek->wskPoprzedni;//korekcja pozycji srodkowego elementu
                        if(this->wskPomocniczy->wskPoprzedni==NULL){ wskPoczatek = this->wskPomocniczy; }//korekcja polozenia pierwszego elementu blad tutaj rozklada cala liste
                        break;
                    }else{}
                }
            }
            else if(wskSrodek->strNazwaklasy < this->wskPomocniczy->strNazwaklasy){
                dLimitCzasuOperacji = (double)clock()/CLOCKS_PER_SEC;
                for(wskPomocniczy=wskSrodek;  wskPomocniczy!=NULL; wskPomocniczy=wskPomocniczy->wskNastepny){
                    if((double)clock()/CLOCKS_PER_SEC - dLimitCzasuOperacji > 5){
                        cerr<<*wskNazwaMetody+"Przekroczono limit czasu operacji < , obiekt "+this->wskPoprzedni->strNazwaklasy+"trafia na koniec listy"<<endl;
                        this->wskPomocniczy->wskNastepny=NULL;
                        this->wskPomocniczy->wskPoprzedni=wskKoniec;
                        wskKoniec->wskNastepny=this->wskPomocniczy;
                        wskSrodek=wskSrodek->wskNastepny;
                        break;
                    }else if(wskPomocniczy->wskNastepny==NULL || (wskPomocniczy->strNazwaklasy >= this->wskPomocniczy->strNazwaklasy)){//wiekszy rowny nasz najwiekszy
                        this->wskPomocniczy->wskNastepny=wskPomocniczy->wskNastepny;
                        this->wskPomocniczy->wskPoprzedni=wskPomocniczy;
                        wskPomocniczy->wskNastepny=this->wskPomocniczy;
                        wskSrodek=wskSrodek->wskNastepny;
                        if(this->wskPomocniczy->wskNastepny==NULL){ wskKoniec = this->wskPomocniczy; }
                        break;
                    }else{}
                }
            }
            else{//Tak na wszelki wypadek nie zaszkodzi
                cerr<<*wskNazwaMetody+"Niespelniono warunkow < > ==,"+this->wskPomocniczy->strNazwaklasy+" obiekt trafia na koniec listy"<<endl;
                this->wskPomocniczy->wskNastepny=NULL;
                this->wskPomocniczy->wskPoprzedni=wskKoniec;
                wskKoniec->wskNastepny=this->wskPomocniczy;
                wskSrodek=wskSrodek->wskNastepny;
            }

            intSrodekListy = intLiczbaElementow/2;
        }
        else{
            cerr<<*wskNazwaMetody+"Blad pamieci prawdopodbnie lista ulegla rozsypaniu"<<endl;
            delete wskNazwaMetody;
            exit(1);
        }
    }
    else{
        cout<<*wskNazwaMetody+"Zabrakło pamieci na utworzenie kolejnego liscia!"<<endl;
    }
    delete wskNazwaMetody;
}

void drzewo::UsunLisc(int intNrIndexu){
/**
*Usuwa element o danym unikalnym indexie.
*Przesuwa srodek na aktualna pozycje
*/
    lisc *wskPomocniczy;

    for(this->wskPomocniczy=wskKoniec; this->wskPomocniczy; this->wskPomocniczy=this->wskPomocniczy->wskPoprzedni){//uzywam this->wskPomocnicza poniewaz zmienne lokalne sa szybsze
        if(this->wskPomocniczy->index==intNrIndexu){
            wskPomocniczy=this->wskPomocniczy;

            if(wskSrodek->strNazwaklasy < this->wskPomocniczy->strNazwaklasy){//Przesuniecie srodka
                wskSrodek=wskSrodek->wskPoprzedni;
            }else{
                wskSrodek=wskSrodek->wskNastepny;
            }

            if(!this->wskPomocniczy->wskNastepny && this->wskPomocniczy->wskPoprzedni){//jezeli ostatni element to wycinamy polaczenie
                wskKoniec=wskKoniec->wskPoprzedni;
                wskKoniec->wskNastepny=NULL;
            }
            else if(!this->wskPomocniczy->wskPoprzedni){//jezeli pierwszy element to wycinamy polaczenie
                wskPoczatek=wskPoczatek->wskNastepny;
                wskPoczatek->wskPoprzedni=NULL;
            }
            else{
                this->wskPomocniczy->wskNastepny->wskPoprzedni=wskPomocniczy->wskPoprzedni; //zamieniam wskazniki tak zeby do obecnego elementu nie pozostalo zadne polaczenie
                this->wskPomocniczy->wskPoprzedni->wskNastepny=wskPomocniczy->wskNastepny;
            }

            delete wskPomocniczy; //kasuje odlaczony element
            --intLiczbaElementow;
            break;
            }
    }
}

void drzewo::UsunLisc(string intNrIndexu){
/**
*Usuwa element o danej nazwie.
*Przesuwa srodek na aktualna pozycje
*/
    lisc *wskPomocniczy;
    if(wskSrodek->strNazwaklasy > intNrIndexu){
        wskPomocniczy=wskKoniec;
    }else{
        wskPomocniczy=wskSrodek;
    }
    for(this->wskPomocniczy=wskPomocniczy; this->wskPomocniczy; this->wskPomocniczy=this->wskPomocniczy->wskPoprzedni){//uzywam this->wskPomocnicza poniewaz zmienne lokalne sa szybsze
        if(this->wskPomocniczy->strNazwaklasy==intNrIndexu){
            wskPomocniczy=this->wskPomocniczy;

            if(wskSrodek->strNazwaklasy < this->wskPomocniczy->strNazwaklasy){//Przesuniecie srodka
                wskSrodek=wskSrodek->wskPoprzedni;
            }else{
                wskSrodek=wskSrodek->wskNastepny;
            }

            if(!this->wskPomocniczy->wskNastepny && this->wskPomocniczy->wskPoprzedni){//jezeli ostatni element to wycinamy polaczenie
                wskKoniec=wskKoniec->wskPoprzedni;
                wskKoniec->wskNastepny=NULL;
            }
            else if(!this->wskPomocniczy->wskPoprzedni){//jezeli pierwszy element to wycinamy polaczenie
                wskPoczatek=wskPoczatek->wskNastepny;
                wskPoczatek->wskPoprzedni=NULL;
            }
            else{
                this->wskPomocniczy->wskNastepny->wskPoprzedni=wskPomocniczy->wskPoprzedni; //zamieniam wskazniki tak zeby do obecnego elementu nie pozostalo zadne polaczenie
                this->wskPomocniczy->wskPoprzedni->wskNastepny=wskPomocniczy->wskNastepny;
            }

            delete wskPomocniczy; //kasuje odlaczony element
            --intLiczbaElementow;
            break;
            }
    }
}

void drzewo::WypiszElementy(){
/**Wypisuje elementy od pierwszego do ostatniego
*Sprawdza zadeklarowna ilosc elementow ze stanem faktycznym
*/
    int intLicznik=0;
    string strNazwaModulu="[WypiszElementy]->";
    string *wskNazwaModulu= new string(strNazwaModulu);

    cout<<"[---------------------------------------------------------]"<<endl;
    for(wskPomocniczy=wskPoczatek; wskPomocniczy!=NULL; wskPomocniczy=wskPomocniczy->wskNastepny){//wypisuje elementy od pierwszego do ostatniego
        cout<<wskPomocniczy->index<<". "+wskPomocniczy->strNazwaklasy<<" Dane: "+wskPomocniczy->strRecordDanych<<endl;
        ++intLicznik;
    }
    if(wskNazwaModulu){
        if(intLicznik!=intLiczbaElementow){ cerr<<*wskNazwaModulu+"Mozliwa nie spojnosc tablica!\n"<<"Wyswietlono: "<<intLicznik<<"\nOczekiwana ilosc elementow: "<<intLiczbaElementow<<endl; }
        else{
            cout<<"Ilosc elementow: "<<intLicznik<<endl;
        }
        delete wskNazwaModulu;
    }else{
        cerr<<*wskNazwaklasy+strNazwaModulu+"Blad alokacji pamieci"<<endl;
    }
}
void drzewo::UsunListe(){
///Kasuje elementy od ostatniego do 2 w petli element 1 jest kasowany osobno, poniewaz nie skasowanie elemntu, na ktorym jestemy uniemozliwi odczytanie poprzednika.
    if(wskKoniec){//inaczej bedzie blad segmentacji spowodowany odwolaniem sie od elementu, ktorego nie ma (tylko w przypadku braku danych)
        for(wskPomocniczy=wskKoniec->wskPoprzedni; wskPomocniczy!=NULL; wskPomocniczy=wskPomocniczy->wskPoprzedni){
            delete wskPomocniczy->wskNastepny;
        }
    }
    delete wskPoczatek;
}
drzewo::~drzewo(){
///Ostateczne sprzatanie
    UsunListe();
    delete wskNazwaklasy;
}
