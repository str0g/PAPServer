/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (www.SoonWillBe.pl)
 * License:   GNU / General Public Licens
 **************************************************************/
///Headers
#include "ServerFilesLoader.hpp"
///Specials
using std::string;
using std::cerr;
using std::cerr;
using std::cin;
using std::endl;
///Globals Varuabels


ServerFilesLoader::ServerFilesLoader(int pid):strClassName("[ServerFilesLoader]->"),
                                        strWorkDir(*ServerConfigs::p_strMyPath+"Pool"),
                                        strPathToXMLFile(*ServerConfigs::p_strMyPath+"configs"+ServerConfigs::g_strSlash+"SharedFiles.xml"),
                                        intDirScanDepth(1024),
                                        intDirScanCounter(0),
                                        p_strFileBuf(NULL),
                                        p_xmlFile(new XMLParser),
                                        dBase(pid){
}
ServerFilesLoader::~ServerFilesLoader(){
    cleanFileBuf();
    cleanXML();
}

void ServerFilesLoader::cleanFileBuf(){
/**
* Czysci buffor pliku, jezeli potrzeba
*/
    if (p_strFileBuf){
        delete p_strFileBuf;
        p_strFileBuf = NULL;
    }
}

bool ServerFilesLoader::LoadFileToBuf(const string *fname){
/**
* Wczytuje caly plik do bufora, nie jest to idealne poniewaz przy duzych plikach moze sie skonczyc pamieci.
*/
    string strMethodName = strClassName+"[LoadFileToBuf]->";
    char *buffer;
    int length;
    FILE *rfile;

    if (fname->find(strWorkDir)==0){
        rfile = fopen(fname->c_str(), "rb");
        if(rfile){
            // get length of file:
            fseek(rfile, 0, SEEK_END);
            length = ftell(rfile);
            fseek(rfile, 0, SEEK_SET);
            // allocate memory:
            buffer=(char *)malloc(sizeof(char)* (length)+1);
            if (buffer){
                buffer[length]='\0';
                // read data as a block:
                fread(buffer, length, 1, rfile);
                p_strFileBuf = new string(myConv::ToString(buffer));
                free(buffer);
            }else{
                cerr<<GetLocalTime()<<strMethodName<<"char* buffer error..."<<endl;
                return false;
            }//buffer
            fclose(rfile);
        }else{
            cerr<<GetLocalTime()<<strMethodName<<"Failed to read: "<<*fname<<endl;
            return false;
        }//rfile
        return true;

    }//found string
    cerr<<GetLocalTime()<<strMethodName<<"Security breach!"<<endl;
    //other actions email to admin or what else you might want :-)
    return false;
}

void ServerFilesLoader::cleanXML(){
/**
*Zwalnia wskaznik do struktory xml
*/
    if(p_xmlFile){
        delete p_xmlFile;
        p_xmlFile = NULL;
    }
}
bool ServerFilesLoader::RunCheck(){
/**
* Inicjuje operacje
*/
    if(!p_xmlFile->LoadXMLFile(&strPathToXMLFile)){
        p_xmlFile->createXMLDoc("SharedFiles");
    }
    return dBaseRun();
}

void ServerFilesLoader::ListDir(const boostfs::path & directory){
/**
* Laduje rekurencyjnie plik z folderu Pool do bazy danych
* Dodac validacje z xml oraz dodwania jezeli trzeba...
*/
    string strMethodName = strClassName+"[ListDir]->";

    if( boostfs::exists( directory ) ){
        boostfs::directory_iterator end ;
        for( boostfs::directory_iterator iter(directory) ; iter != end ; ++iter ){
            if ( boostfs::is_directory( *iter ) ){
                if(intDirScanCounter < intDirScanDepth){
                    //cerr << iter->path().directory_string() << " (directory)\n" ;
                    ++intDirScanCounter;
                    ListDir(*iter) ;
                }//deep scan
            }//if dir
            else{
                //cerr << iter->path().file_string() << " (file)\n" ;
                if (is_regular_file(iter->status())){
                    if(LoadFileToBuf(&iter->path().file_string())){
                        if(!insertFile((iter->path().file_string()).c_str(),
                                (iter->path().filename()).c_str(),
                                myConv::ToString(file_size(iter->path())).c_str(),
                                string("sha512").c_str(),
                                Hash::SHA512(*p_strFileBuf).c_str(),
                                myConv::ToString(last_write_time(iter->path())).c_str()
                                )){
                                    cerr<<GetLocalTime()<<strMethodName<<"Fail to insert file into database ["<<iter->path().file_string()<<"]"<<endl;
                                }
                    }//load file
                    cleanFileBuf();
                }//status
                else{
                    cerr<<GetLocalTime()<<strMethodName<<"File has bad status ["<<iter->path().file_string()<<"]"<<endl;
                }//status
            }//if dir_ file
        }//loop
  }//exists
}
