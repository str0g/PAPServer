/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (www.SoonWillBe.pl)
 * License:   GNU / General Public Licens
 **************************************************************/
///Headers
#include "XMLParser.hpp"
///Specials
using std::string;
using std::cout;
using std::cerr;
using std::cin;
using std::endl;
///Globals Varuabels

XMLParser::XMLParser():p_strClassName(NULL),p_PointerToXMLTree(NULL){
    p_strClassName = new string("XMLParser");
}
XMLParser::~XMLParser(){
    ///Desctruktor
    vCleanXMLTree();
    delete p_strClassName;
}

void XMLParser::vCleanXMLTree(){
    ///Zwalnia pamiec zajetą przez strukturę XML
    mxmlDelete(p_PointerToXMLTree);
}


int XMLParser::LoadXMLFile(string *p_strConfigPath){
    /**
    *Laduje plik XML do pamiecie, jako struckturę XML.
    *Zwraca czy się udało
    *0 - udalo sie
    *1 - nie udało się otworzyć pliku
    *2 - buffor struktury jest w użyciu i nie został zwolniony.
    */
    if(p_PointerToXMLTree){
        cout<<*p_strClassName<<"Buffer in use"<<endl;
        return 2;
    }
    FILE *fp;
    fp = fopen(p_strConfigPath->c_str(), "rb");
    if(fp){
        p_PointerToXMLTree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
        fclose(fp);
        return 0;
    }
    else{
        cout<<p_strClassName<<" Failed to open: "<<*p_strConfigPath<<endl;
        return 1;
    }
}

//template <typename T>
int XMLParser::LoadXMLFromBuf(const char *p_cBuf){//T *p_cBuf){
    /**
    *Metoda akceptuje tylko char* i const char*
    *Metoda laduje dane z bufora do struktory xml
    *sprawdzajac czy wskaznik struktory nie jest juz w urzyciu, trzeba go zwalniac recznie.
    *Domyslan flaga to OPAQUE, aby mozna bylo pobierac dane ze struktory bez problemow
    *Metoda zwraca kody bledow 0- sukces, 1 - podano pusty buffor, 2 - nie zwolniono bufora struktry.
    */
    if(p_PointerToXMLTree){
        cout<<*p_strClassName<<"Buffer in use"<<endl;
        return 2;
    }
    if(p_cBuf){
        p_PointerToXMLTree = mxmlLoadString(NULL,p_cBuf,MXML_OPAQUE_CALLBACK);
        return 0;
    }else{
        cout<<p_strClassName<<" NULL"<<endl;//*p_strConfigPath<<endl;
        return 1;
    }
}

bool XMLParser::SetWorkNode(string strWordToSearch){
    /**
    *Wyszukiwanie wskazanego elementu
    *Jezeli znaleziony operacyjny wskaznik struktury zostanie na niego ustawiony, true zostanie zwrocone.
    *W Przypadku nie znalezienia lub bledu, wskaznik opreacyjny strukty zostanie ustawiony na pierwszy element
    *Wskaznik dziecka zostanie ustawiony na pierwszy element struktury,
    *Wyswietlony zostaine komunikat o bledzie, oraz zwrocony false.
    */
    p_WorkNode = mxmlFindElement(p_PointerToXMLTree, p_PointerToXMLTree,
                                                strWordToSearch.c_str(),NULL,NULL,
                                                MXML_DESCEND);
    if(!p_WorkNode){
        cerr<<*p_strClassName+"[SetWorkNode]->"<<strWordToSearch<<" have't been found"<<endl;
        p_WorkNode = p_PointerToXMLTree;
        p_childNode = p_PointerToXMLTree;
        return false;
    }
    return true;
}

bool XMLParser::GoDeeper(){
    ///Jezeli istnieje dziecko element wskaznik operacyjny struktury zostanie na niego ustawiony i wartosc true zostanie zworcona
    if(p_WorkNode->child){
        p_WorkNode = p_WorkNode->child;
        return true;
    }else{
        return false;
    }
}

bool XMLParser::GoUpper(){
    ///Jezeli istnieje rodzic elementu wskaznik operacyjny struktury zostanie na niego ustawiony i wartosc true zostanie zworcona
    if(p_WorkNode->parent){
        p_WorkNode = p_WorkNode->parent;
        return true;
    }else{
        return false;
    }
}

bool XMLParser::NextElement(){
    ///Jezeli istnieje nastepny element wskaznik operacyjny struktury zostanie na niego ustawiony i wartosc true zostanie zworcona
    if(p_WorkNode->next){
        p_WorkNode = p_WorkNode->next;
        return true;
    }else{
        return false;
    }
}

bool XMLParser::PrevElement(){
    ///Jezeli istnieje poprzedni element wskaznik operacyjny struktury zostanie na niego ustawiony i wartosc true zostanie zworcona
    if(p_WorkNode->parent){
        p_WorkNode = p_WorkNode->parent;
        return true;
    }else{
        return false;
    }
}

void XMLParser::Print(string tmp){
    /**
    *Rekurencyjny algorytm potrafiacy przepokac sie przez elementy kazdego dokumentu xml.
    *Schodzenie na nizsze elementy powoduje dodanie spacji w celu logicznej interpretacji graficznej
    */
    mxml_node_t *Save_State = p_WorkNode; //!< Przechowuje wartosc wskaznika operacyjnego struktury, poniewaz przeszukiwanie odbywa sie na glownym wskazniku struktory
    string strSpace=" ";
    do{
        cout<<tmp+GetCurrentElement()<<endl;

        if(GoDeeper()){
            tmp.append(strSpace);
            Print(tmp);
            GoUpper();
        }
    }while(NextElement());

    p_WorkNode = Save_State;
}

bool XMLParser::createXMLDoc(string EleName,string standard){
    /**
    *Tworzy document o okreslonym standardzie domyslen xml 1.0 oraz wstawia pierwszy element
    * Zwraca true jezeli sie udalo jezeli nie zwaraca false
    */
    mxml_node_t *data;

    if(p_PointerToXMLTree){
        cerr<<*p_strClassName<<"Buffer in use"<<endl;
        return false;
    }

    p_PointerToXMLTree = mxmlNewXML(standard.c_str());
    if(!p_PointerToXMLTree){
        cerr<<"Creating XML doc in standard "<<EleName<<standard<<" failed"<<endl;
        return false;
    }
        data = mxmlNewElement(p_PointerToXMLTree, EleName.c_str());
        if(!data){
            cerr<<"Creating "<<EleName<<" element faild"<<endl;
            return false;
        }
        p_WorkNode = data;
    return true;
}

string XMLParser::GetCurrentElement(){
    ///Zwraca wartosc elementu,mozna nim pobrac nazwe tego elementu lub przechowywana wartosc
    return p_WorkNode->value.opaque;
}

string XMLParser::GetStringValue(string strWordToSearch,string strValueOnError){
    /**
    *Ta metoda szuka podanego słowa w strukturze, następnie pobiera jego wartość.
    *Jeżeli nie zostało znalezione lub nie ma wartości to użyte zostanie wartość w przypadku błedu
    */
    mxml_node_t *p_mxmlPointer = mxmlFindElement(p_WorkNode, p_PointerToXMLTree,
                                                strWordToSearch.c_str(),NULL,NULL,
                                                MXML_DESCEND);
    if(!p_mxmlPointer){
        cout<<*p_strClassName<<" Didn't found: "<<strWordToSearch<<endl;
        return strValueOnError;
    }
    return p_mxmlPointer->child->value.opaque;
}

string &XMLParser::p_GetStringValue(string strWordToSearch,string strValueOnError){
    /**
    *Ta metoda szuka podanego słowa w strukturze, następnie pobiera jego wartość.
    *Jeżeli nie zostało znalezione lub nie ma wartości to użyte zostanie wartość w przypadku błedu
    *Zwraca wskaznik
    */
    mxml_node_t *p_mxmlPointer = mxmlFindElement(p_WorkNode, p_PointerToXMLTree,
                                                strWordToSearch.c_str(),NULL,NULL,
                                                MXML_DESCEND);
    string *p_strReturn = NULL;
    if(!p_mxmlPointer){
        cout<<*p_strClassName<<" Didn't found: "<<strWordToSearch<<endl;
        p_strReturn = new string(strValueOnError);
        return *p_strReturn;
    }
    p_strReturn = new string (p_mxmlPointer->child->value.opaque);
    return *p_strReturn;
}

bool XMLParser::insertXMLSearch(const char* fp,const char* fn,const char* fs,const char* fht,const char* fh,const char* flm,const char* fo,char* mojaNazwa){
    /**
    *Wklada elemnety do struktury
    */
    string MethodName = (*p_strClassName+"[insertXMLSearch]->Failed to create: ");
    bool bRet =true;
    mxml_node_t *node;
    node = mxmlNewElement(p_WorkNode,mojaNazwa);
        if(!node){
            cerr<<MethodName<<mojaNazwa<<endl;
            bRet = false;
        }
        if(!mxmlNewOpaque(mxmlNewElement(node, "FilePath"),fp)){
            cerr<<MethodName<<fp<<endl;
            bRet = false;
        }
        if(!mxmlNewOpaque(mxmlNewElement(node, "FileName"),fn)){
            cerr<<MethodName<<fn<<endl;
            bRet = false;
        }
        if(!mxmlNewOpaque(mxmlNewElement(node, "FileSize"),fs)){
            cerr<<MethodName<<fs<<endl;
            bRet = false;
        }
        if(!mxmlNewOpaque(mxmlNewElement(node, "FileHashType"),fht)){
            cerr<<MethodName<<fht<<endl;
            bRet = false;
        }
        if(!mxmlNewOpaque(mxmlNewElement(node, "FileHash"),fh)){
            cerr<<MethodName<<fh<<endl;
            bRet = false;
        }
        if(!mxmlNewOpaque(mxmlNewElement(node, "FileLastModification"),flm)){
            cerr<<MethodName<<flm<<endl;
            bRet = false;
        }
        if(!mxmlNewOpaque(mxmlNewElement(node, "FileOwner"),fo)){
            cerr<<MethodName<<fo<<endl;
            bRet = false;
        }
    return bRet;
}

char &XMLParser::GetXMLAsChar(){
    ///Zwraca strukture w formie dynamicznie alkowanej tablicy char, zwalnianie free()
    return *mxmlSaveAllocString(p_PointerToXMLTree, MXML_NO_CALLBACK);
}

string XMLParser::GetXMLAsString(){
    ///Strumieniem operatora dokonuje konwersji z char* na stringa
    char *p_cBuf = &GetXMLAsChar();
    string ret = liczba_na_string(p_cBuf);
    free(p_cBuf);
    return ret;
}

