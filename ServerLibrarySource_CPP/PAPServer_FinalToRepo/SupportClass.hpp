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
#include <fstream>
#include <fcntl.h>
#include <cstdio>
#include <string>
#include <ctime>
#include <sys/stat.h>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
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
        string GetYMD();
        string GetTimeAfter1970();
        string AliveTime(time_t,time_t);
        time_t GetTimeAfter1970AsTime();
        long int CoutTimeAfter1970(time_t,time_t);
        void Set_strTimeFormatter(string);
        void Set_intFormatterLenght(int);
};

namespace myIO{
    inline bool chk4file(const string *strFileName){
    ///Jezeli sie udalo otworzyc zworic true
    struct stat stFileInfo;
    int intStat;

    intStat = stat(strFileName->c_str(),&stFileInfo);
    if(intStat == 0) {
        return true;
    }
    return false;
    }
    inline uint64_t GetFileSize(const string*strFileName){
        return boost::filesystem::file_size(*strFileName);
    }
    inline bool touch(const string *strFileName,int int_chmod =0644){
        std::ofstream o(strFileName->c_str());
        if(o){
            o.close();
            chmod(strFileName->c_str(),int_chmod);
            return true;

        }
        return false;
    }
    inline void rmAll(const boost::filesystem::path & directory,bool bForced = true,int intDirScanDepth =1024){
        int intDirScanCounter = 0;
        if( boost::filesystem::exists( directory ) ){
            boost::filesystem::directory_iterator end ;
            for( boost::filesystem::directory_iterator iter(directory) ; iter != end ; ++iter ){
                if ( boost::filesystem::is_directory( *iter ) ){
                    if(intDirScanCounter < intDirScanDepth){
                        ++intDirScanCounter;
                        myIO::rmAll(*iter,bForced);
                        boost::filesystem::remove(*iter);
                    }//deep scan
                }else{
                    if (bForced){
                        chmod(iter->path().file_string().c_str(),0644);
                    }
                    boost::filesystem::remove(*iter);
                }//if dir
            }//loop
        }//if exist
    }
    inline bool appToFile(const string *strFileName,const string *data,int int_chmod =0644,int int_chmodE = 0444){
        if (chmod(strFileName->c_str(),int_chmod)==0){
            std::ofstream o(strFileName->c_str(),std::ios::app);
            if(o){
                o << *data;
                o.close();
                if (chmod(strFileName->c_str(),int_chmodE)!=0){
                    cerr<<"[appToFile]: "<<*strFileName<<" Premission error(after write)"<<endl;
                }
                return true;
            }
            cerr<<"[appToFile]: "<<*strFileName<<" Failed to open"<<endl;
        }else{
            cerr<<"[appToFile]: "<<*strFileName<<" Premission error (before Open)"<<endl;
        }
        return false;
    }
    inline bool readFile(const string *strFileName,string *p_strData){
        if (!p_strData){
            cerr<<"[readFile] Expected allocated string"<<*strFileName<<endl;
            return false;
        }
        std::ifstream::pos_type size;
        char * memblock = NULL;
        std::ifstream file (strFileName->c_str(), std::ios::in|std::ios::binary);
        if (file.is_open()){
            file.seekg (0, std::ios::end);
            size = file.tellg();
            memblock = new char [size+1];
            memblock[size] = '\0';
            if (memblock){
                file.seekg (0, std::ios::beg);
                file.read (memblock, size);
                file.close();
                p_strData->append(memblock);
                delete[] memblock;
                if (file.gcount() < 1){
                    cout<<"[readFile] Faild to read any data from:"<<*strFileName<<endl;
                    return false;
                }
                return true;
            }
            cerr<<"[readFile] Failed to allocate memory "<<*strFileName<<endl;
            return false;
        }
        cerr<<"[readFile] Failed to open:"<< *strFileName<<endl;
        return false;
    }
    inline bool readFileChunk(const string *strFileName,string *p_strBuf,uint64_t &intPosBegin,uint64_t intChunkSize){
        char *cBuf = NULL;
        if (!p_strBuf){
            cerr<<"[readFileChunk] Expected allocated string "<<*strFileName<<endl;
            return false;
        }

        uint64_t read = intPosBegin;
        uint64_t int64End = 0;
        std::ifstream o(strFileName->c_str(), std::ios::in|std::ios::binary);
            if(o.is_open()){
                o.seekg(0,std::ios::end);
                int64End = (uint64_t)o.tellg();
                if (int64End < intPosBegin + intChunkSize){
                    intChunkSize = (int64End - intPosBegin );
                }
                if (intChunkSize < 1){
                    cerr<<"[readFileChunk] Nothing to be read at possition:"<<intPosBegin<<"/ "<<*strFileName<<endl;
                    o.close();
                    return false;
                }
                cBuf = new char[intChunkSize+1];
                cBuf[intChunkSize] = '\0';
                if(!cBuf){
                    cerr<<"[readFileChunk] Memmory allocation failed"<<"/ "<<*strFileName<<endl;
                    o.close();
                    return false;
                }
                /*uint64_t i;
                for(i=0; cBuf[i]!='\0'; ++i);
                if (i!=intChunkSize){
                    cout<<"Allocated:"<<i<<" exp:"<<intChunkSize<<endl;
                }*/

                o.seekg(read,std::ios::beg);
                o.read(cBuf,intChunkSize);
                read = o.gcount();
                for(uint64_t i=0; i!=read; ++i){
                //p_strBuf->append(cBuf);
                    p_strBuf->push_back(cBuf[i]);
                }
                o.close();
                delete[] cBuf;

                if (read == 0 and int64End > 0){
                    cerr<<"[readFileChunk] Nothing has been read "<<*strFileName<<endl;
                    return false;
                }
                if(read != intChunkSize){
                    cerr<<"read not equal to intChunkSize! "<<"Read:"<<read<<" Exp:"<<intChunkSize<<"/ "<<*strFileName<<endl;
                }
                intPosBegin += read;
                return true;
            }
            cerr<<"[readFileChunk]: "<<*strFileName<<" Failed to open"<<endl;
            return false;
    }
}

#endif // SUPPORTCLASS_HPP_INCLUDED
