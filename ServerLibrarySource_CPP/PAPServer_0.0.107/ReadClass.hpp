#ifndef READCLASS_HPP_INCLUDED
#define READCLASS_HPP_INCLUDED

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

///Moje
#include "SupportClass.hpp"

///Specials
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;

class FileRead: public debugMsg{
    protected:
        string *p_strClassName;
    public:
        FileRead();
        ~FileRead();
        /**
        *Error Codes for Read:
        * 1 File not found
        * 2 Buffer haven't been created
        * 3 Other errors if there will be.
        */
        template <typename T>
        T &ReadByLine(string *p_strFileName, int16_t &ErrorCode, T ValueOnError){
        /**
        *Odczyt Strumieniowy ze wszelkimi znakami
        */
            string *p_strMethodName = new string (*p_strClassName+"[ReadByLine]->");
            char *buffer;
            int64_t *length;
            double *start,*end;
            FILE *readfile;
            T *wskReturn = NULL;

            start = new double ((double)clock()/CLOCKS_PER_SEC);

            readfile = fopen(p_strFileName->c_str(), "rb");
            if(readfile){
                // get length of file:
                fseek(readfile, 0, SEEK_END);
                length = new int64_t (ftell(readfile));
                fseek(readfile, 0, SEEK_SET);
                // allocate memory:
                buffer=(char *)malloc(sizeof(char)* (*length)+1);
                if (buffer){
                    buffer[*length]='\0';
                    // read data as a block:
                    fread(buffer, *length, 1, readfile);
                    wskReturn = new T(buffer);
                    free(buffer);
                }else{
                    ErrorCode = 2;
                    /*SEND INFO TO LOGGER*/
                }
                fclose(readfile);
                //cout.write (buffer,*length); // wypisanie na ekran
                cout<<*p_strMethodName+"File Size: "<<*length<<" B"<<endl;
            }else{
                ErrorCode = 1;
                /*SEND INFO TO LOGGER*/
                cout<<*p_strMethodName+"Failed to read: "+ *p_strFileName<<endl;
            }
            end = new double((double)clock()/CLOCKS_PER_SEC);
            if ( ErrorCode == 0){
                Bandwidth(p_strMethodName,end,start,length);
            }else{
                cout<<*p_strMethodName+"ErrorCode: "<<ErrorCode<<endl;
                delete wskReturn;
                wskReturn = new T (ValueOnError);
            }
            cout<<*p_strMethodName+"Excecution time: "<<*end-*start<<"s"<<endl;
            //
            if ( ErrorCode != 1){
                delete length;
            }
            delete p_strMethodName;
            delete start;
            delete end;

            return *wskReturn;
            }
};

#endif // READCLASS_HPP_INCLUDED
