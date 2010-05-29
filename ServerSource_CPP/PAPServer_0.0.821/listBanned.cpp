/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (www.SoonWillBe.pl)
 * License:   GNU / General Public Licens
 **************************************************************/
///Headers
#include "listBanned.hpp"

///Specials
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
///Globals Varuabels

/**
*BannedNode
*/

BannedNode::BannedNode(string strMsg1,string strMsg2,int index): strClientIP(strMsg1),strLocalName(strMsg2),dCreationTime(GetTime()),
                                                                intCounterToBeBanned(0),intNodeIndex(index){
}
BannedNode::~BannedNode(){
}

void BannedNode::BanClient(){
    intCounterToBeBanned = 3;
}

bool BannedNode::WarnClient(){
    ++intCounterToBeBanned;
    return AskIfBanned();
}

void BannedNode::PrintInfo(){
    fprintf(stdout,"IP:%s,Name:%s,Time:%F,Counter:%d,Index:%d\n",strClientIP.c_str(),strLocalName.c_str(),dCreationTime,intCounterToBeBanned,intNodeIndex);
}

//Pytania
bool BannedNode::SearchForIP(string str1){
    return strClientIP == str1;
}
bool BannedNode::SearchForLocalName(string str1){
    return strLocalName == str1;
}
bool BannedNode::SearchForIPandLocalName(string str1,string str2){
    return (strClientIP == str1 and strLocalName == str2);
}
bool BannedNode::SearchForBanned(){
    return intCounterToBeBanned > 2;
}
bool BannedNode::SearchFotWarned(){
    return  (0 < intCounterToBeBanned and intCounterToBeBanned <3);
}
bool BannedNode::AskIfBanned(){
    return (intCounterToBeBanned > 3);
}
/**
*BannedList
*/
/*
template <class T>
BannedList<T>::BannedList(){
}
template <class T>
BannedList<T>::~BannedList(){
}
*/
//template <class T>
//void BannedList<T>::AddNode(T *NodeType,string strMsg1,string strMsg2){
/**Dodaje nowe elementy w sposob posortowany odrazu sprawdzajac gdzie powinno byc ich miejsce.
*Wyszukiwanie zaczyna sie od srodka listy dla oszczednosci czasu.
*//*
//    string *p_strMethodName= new string(*p_strClassName+"[AddNode]->");
    double dLimitCzasuOperacji;
    T *p_NodeHelper = new T(strMsg1,strMsg2,myList<T>::intNodeIndex);
    if(myList<T>::p_Helper){
        ++myList<T>::intNodeCounter;
        ++myList<T>::intNodeIndex;

        if(myList<T>::p_Begin){ //tutaj dodaje kazdy kolejny T
                p_NodeHelper->p_prev = static_cast<T>(myList<T>::p_End);
                static_cast<T>(myList<T>::p_End)->p_next = p_NodeHelper;
                if (myList<T>::intNodeCounter < 6){
                    static_cast<T>(myList<T>::p_Centre) = static_cast<T>(myList<T>::p_Centre)->p_next;
                }
        }else{ //pierwszy element jest wyjatkowy dla tego trzeba go wyroznic
            static_cast<T>(myList<T>::p_Begin) = p_NodeHelper;
            static_cast<T>(myList<T>::p_Centre) = p_NodeHelper;
            static_cast<T>(myList<T>::p_End) = p_NodeHelper;
        }
    }
    else{
        //cout<<*p_strMethodName+"Zabrakło pamieci na utworzenie kolejnego Tia!"<<endl;
    }
    //delete p_strMethodName;
}

//Debug
template <class T>
void BannedList<T>::PrintNodes(){
    static_cast<T>(myList<T>::p_Begin)->PrintNode();
}
*/
