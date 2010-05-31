#ifndef LISTBANNED_HPP_INCLUDED
#define LISTBANNED_HPP_INCLUDED

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
#include <string>
///Moje
#include "SupportClass.hpp"

///Specials
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
///Globals Varuabels

class BannedNode: public TimeCout{
    private:
        string strClientIP;
        string strLocalName;
        double dCreationTime;
        int intCounterToBeBanned;
        int intNodeIndex;
    public:
        BannedNode(string,string,int);
        ~BannedNode();
        //
        void BanClient();
        bool WarnClient();
        //
        void PrintInfo();
        bool AskIfBanned();
        bool SearchForIP(string);
        bool SearchForLocalName(string);
        bool SearchForIPandLocalName(string,string);
        bool SearchForBanned();
        bool SearchFotWarned();
};/*
template <class T>
class BannedList: public myList<T>{
    public:
        BannedList();
        ~BannedList();
        void AddNode(T*,string,string);
        void PrintNodes();
};
*/
#endif // LISTBANNED_HPP_INCLUDED
