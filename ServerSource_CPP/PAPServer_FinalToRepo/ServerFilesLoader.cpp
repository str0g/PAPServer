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
                                        p_xmlFile(new XMLParser),//m_thread(boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&ServerFilesLoader::Run, this)))),
                                        dBase(pid){

}
ServerFilesLoader::~ServerFilesLoader(){
    Die();
}

void ServerFilesLoader::Die(){
    clearFileBuf();
    clearXML();
    //mClose();
    //m_thread->join();
}

void ServerFilesLoader::clearFileBuf(){
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

void ServerFilesLoader::clearXML(){
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

void ServerFilesLoader::SaveList(){
    std::ofstream out(strPathToXMLFile.c_str());
    string *p_strBuf = new string(p_xmlFile->GetXMLAsString());
    out << *p_strBuf;
    out.close();
    cerr<<GetLocalTime()<<strClassName<<"SharedList.xml Size ["<<p_strBuf->size()<<"]"<<endl;
    cerr<<GetLocalTime()<<strClassName<<"SharedList.xml Hash ["<<Hash::SHA512(*p_strBuf)<<"]"<<endl;
    delete p_strBuf;
}

void ServerFilesLoader::ListDir(const boostfs::path & directory){
/**
* Laduje rekurencyjnie plik z folderu Pool do bazy danych
* Dodac validacje z xml oraz dodwania jezeli trzeba...
* Jezeli plik ma zly status, czyli jest uszkodzony w sposob uniemozliwiajacy odczytanie jego atrybutow jest usuwany listy xml jezeli byl dodany.
*/
    string strMethodName = strClassName+"[ListDir]->";
    bool bAdd=false;

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
                p_xmlFile->ResetWorkPointer();
                if (is_regular_file(iter->status())){
                    if(p_xmlFile->Seatch4Value(&(iter->path().file_string()))){
                        p_xmlFile->GoUpper();
                        for(int i=0; i<5; ++i){
                            p_xmlFile->NextElement();
                        }
                        p_xmlFile->GoDeeper();
                        if(myConv::FromString<int>(p_xmlFile->GetCurrentElement()) != last_write_time(iter->path())){
                            cerr<<GetLocalTime()<<strMethodName<<"Warning File ["<<iter->path().file_string()<<"] has change since last time"<<endl;
                            p_xmlFile->setCurrentElementValue(myConv::ToString(last_write_time(iter->path())).c_str());
                            p_xmlFile->GoUpper();

                            p_xmlFile->PrevElement();
                            p_xmlFile->GoDeeper();
                            if(LoadFileToBuf(&iter->path().file_string())){
                                p_xmlFile->setCurrentElementValue(Hash::SHA512(*p_strFileBuf).c_str());
                            }
                            clearFileBuf();
                            p_xmlFile->GoUpper();
                            p_xmlFile->PrevElement();
                            p_xmlFile->PrevElement();
                            p_xmlFile->GoDeeper();
                            p_xmlFile->setCurrentElementValue(myConv::ToString(file_size(iter->path())).c_str());
                            //other actions
                        }
                    }else{
                        if(LoadFileToBuf(&iter->path().file_string())){
                            p_xmlFile->SetWorkNode("SharedFiles");
                            p_xmlFile->insertXMLSearch((iter->path().file_string()).c_str(),
                                                    (iter->path().filename()).c_str(),
                                                    (myConv::ToString(file_size(iter->path()))).c_str(),
                                                    string("sha512").c_str(),
                                                    (Hash::SHA512(*p_strFileBuf)).c_str(),
                                                    (myConv::ToString(last_write_time(iter->path()))).c_str(),
                                                    string("0").c_str()
                                                    );
                        }
                        clearFileBuf();
                    }
                    p_xmlFile->ResetWorkPointer();
                    if(p_xmlFile->Seatch4Value(&(iter->path().file_string()))){
                        p_xmlFile->GoUpper();
                        for(int i=0; i<4; ++i){
                            p_xmlFile->NextElement();
                        }
                        p_xmlFile->GoDeeper();
                        if(!insertFile((iter->path().file_string()).c_str(),
                                (iter->path().filename()).c_str(),
                                myConv::ToString(file_size(iter->path())).c_str(),
                                string("sha512").c_str(),
                                p_xmlFile->GetCurrentElement().c_str(),
                                myConv::ToString(last_write_time(iter->path())).c_str()
                                )){
                                    cerr<<GetLocalTime()<<strMethodName<<"Fail to insert file into database ["<<iter->path().file_string()<<"]"<<endl;
                            }
                        }//load file
                        else{ cerr<<GetLocalTime()<<strMethodName<<"Nothing to be shared?"<<endl;}
                        clearFileBuf();
                    //}//is regular
                    //}
                }//status
                else{
                    cerr<<GetLocalTime()<<strMethodName<<"File has bad status ["<<iter->path().file_string()<<"]"<<endl;
                    //dodac usuwanie z XML
                    if(p_xmlFile->Seatch4Value(&(iter->path().file_string()))){
                        p_xmlFile->GoUpper();
                        p_xmlFile->GoUpper();
                        p_xmlFile->delCurrentElement();
                    }
                }//status
            }//if dir_ file
        }//loop
  }//exists
}
