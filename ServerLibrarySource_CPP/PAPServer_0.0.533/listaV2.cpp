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
///Specials
using namespace std;
///Globals Varuabels
/*SEKCJA DRZEWA*/
template <class T>
myList<T>::myList():p_strClassName(NULL),
                 p_Node(NULL),p_prev(NULL),p_next(NULL),p_Begin(NULL),p_Centre(NULL),p_End(NULL),p_Helper(NULL),
                 intNodeCounter(0),intNodeIndex(1){
    p_strClassName = new string("[Drzewo]->");
}
template <class T>
int myList<T>::RysujMenu(){
    /**
    * Wypisuje textowe Menu i czekam na wpisanie znaku, ktory zostanie zwrocony
    */
    string *p_strMethodName = new string (*p_strClassName+"[NarysujMenu]->");

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

    delete p_strMethodName;

    return zwroc;
}
template <class T>
void myList<T>::Sterowanie(){/*
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
                AddNode(strDane);
                cout<<"\n\n\n"<<endl;
                break;
            case 3:
                cout<<"Podaj index do usuniecia:";
                cin>>intIndex;
                DeleteNode(intIndex);
                cout<<"\n\n\n"<<endl;
                break;
            case 4:
                cout<<"Podaj nazwe do usuniecia:";
                cin>>strNazwa;
                DeleteNode(strNazwa);
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
    }*/
}
template <class T>
void myList<T>::AutoTest(){
/** Ta metoda testuje czy lista dziala poprawnie dzieki
*
*//*
    cout<<"Wypisuje elementy: (powinno byc 0)"<<endl;
    WypiszElementy();
    cout<<"Dodaje elementy: nazwa,nazw,nazwb,na,nazwdas \n(powinno byc 5)"<<endl;
    AddNode("text");
    WyswietlSrodkowyelement();
    AddNode("text");
    WyswietlSrodkowyelement();
    AddNode("text");
    WyswietlSrodkowyelement();
    AddNode("text");
    WyswietlSrodkowyelement();
    AddNode("text");
    WyswietlSrodkowyelement();
    WypiszElementy();
    cout<<"Usuwam elementy po indexie 1,5 po nazwie na[4]:"<<endl;
    DeleteNode(1);
    WyswietlSrodkowyelement();
    DeleteNode(5);
    WyswietlSrodkowyelement();
    DeleteNode("na[4]");
    WyswietlSrodkowyelement();
    WypiszElementy();*/
}
template <class T>
void myList<T>::AddNode(T *NodeType,string text){
/**Dodaje nowe elementy w sposob posortowany odrazu sprawdzajac gdzie powinno byc ich miejsce.
*Wyszukiwanie zaczyna sie od srodka listy dla oszczednosci czasu.
*/
    string *p_strMethodName= new string(*p_strClassName+"[AddNode]->");
    double dLimitCzasuOperacji;
    T *p_NodeHelper = new T;
    if(p_Helper){
        p_NodeHelper->index=intNodeIndex;
        p_NodeHelper->strNazwaklasy = "Node Numer["+liczba_na_string(intNodeIndex)+"]";
        p_NodeHelper->strRecordDanych=text;

        ++intNodeCounter;
        ++intNodeIndex;

        if(p_Begin){ //tutaj dodaje kazdy kolejny T
                p_NodeHelper->p_prev = static_cast<T>(p_End);
                static_cast<T>(p_End)->p_next = p_NodeHelper;
                if (intNodeCounter < 6){
                    static_cast<T>(p_Centre) = static_cast<T>(p_Centre)->p_next;
                }
        }else{ //pierwszy element jest wyjatkowy dla tego trzeba go wyroznic
            static_cast<T>(p_Begin) = p_NodeHelper;
            static_cast<T>(p_Centre) = p_NodeHelper;
            static_cast<T>(p_End) = p_NodeHelper;
        }
    }
    else{
        cout<<*p_strMethodName+"Zabrakło pamieci na utworzenie kolejnego Tia!"<<endl;
    }
    delete p_strMethodName;
}

template <class T>
void myList<T>::DeleteNode(T *NodeType, int intNrIndexu){
/**
*Usuwa element o danym unikalnym indexie.
*Przesuwa srodek na aktualna pozycje
*/
    T *p_TypeHelper;
    for(NodeType=dynamic_cast<T>(p_End); NodeType; NodeType=NodeType->p_prev){
        if(NodeType->index == intNrIndexu){

            if(!NodeType->p_next && NodeType->p_prev){//jezeli ostatni element to wycinamy polaczenie
                *p_TypeHelper = dynamic_cast<T>(p_End);
                p_TypeHelper = p_TypeHelper->p_prev;
                p_TypeHelper->p_next=NULL;
                dynamic_cast<T>(p_End) = p_TypeHelper;
            }else if(!NodeType->p_prev){//jezeli pierwszy element to wycinamy polaczenie
                *p_TypeHelper = dynamic_cast<T>(p_Begin);
                p_TypeHelper = p_TypeHelper->p_next;
                p_TypeHelper->p_prev = NULL;
                dynamic_cast<T>(p_End) = p_TypeHelper;
            }else{
                NodeType->p_next->p_prev=NodeType->p_prev; //zamieniam wskazniki tak zeby do obecnego elementu nie pozostalo zadne polaczenie
                NodeType->p_prev->p_next=NodeType->p_next;
            }

            delete NodeType; //kasuje odlaczony element
            --intNodeCounter;
            //WyliczSrodekListy();
            break;
        }//if NodeType
    }//for
}

template <class T>
void myList<T>::WypiszElementy(){
/**Wypisuje elementy od pierwszego do ostatniego
*Sprawdza zadeklarowna ilosc elementow ze stanem faktycznym
*//*
    int intLicznik=0;
    string strNazwaModulu="[WypiszElementy]->";
    string *p_strMethodName= new string(strNazwaModulu);

    cout<<"[---------------------------------------------------------]"<<endl;
    for(p_Helper=wskPoczatek; p_Helper!=NULL; p_Helper=p_Helper->p_next){//wypisuje elementy od pierwszego do ostatniego
        cout<<p_Helper->index<<". "+p_Helper->strNazwaklasy<<" Dane: "+p_Helper->strRecordDanych<<endl;
        ++intLicznik;
    }
    if(p_strMethodName){
        if(intLicznik!=intNodeCounter){ cerr<<*p_strMethodName+"Mozliwa nie spojnosc tablica!\n"<<"Wyswietlono: "<<intLicznik<<"\nOczekiwana ilosc elementow: "<<intNodeCounter<<endl; }
        else{
            cout<<"Ilosc elementow: "<<intLicznik<<endl;
        }
        delete p_strMethodName;
    }else{
        cerr<<*p_strClassName+strNazwaModulu+"Blad alokacji pamieci"<<endl;
    }*/
}

template <class T>
void myList<T>::DeleteList(T *NodeType){
///Kasuje elementy liscie od pierwszego do ostatniego , 1 jest kasowany osobno, poniewaz nie skasowanie elemntu, na ktorym jestemy uniemozliwi odczytanie nastepnika.

    if(p_Begin){//inaczej bedzie blad segmentacji spowodowany odwolaniem sie od elementu, ktorego nie ma (tylko w przypadku braku danych)
        NodeType = dynamic_cast<T>(p_Begin);
        if (NodeType->p_next){
            for(NodeType=NodeType->p_next; NodeType!=NULL; NodeType=NodeType->p_next){
                delete NodeType->p_prev;
            }//for
        }else{
            delete p_Begin;
        }//if NodeType
    }//if begin
    delete p_End;
}
template <class T>
myList<T>::~myList(){
///Ostateczne sprzatanie
    delete p_strClassName;
}
