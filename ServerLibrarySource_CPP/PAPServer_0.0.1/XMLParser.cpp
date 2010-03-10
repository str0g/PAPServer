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

XMLParser::XMLParser():p_PointerToXMLTree(NULL){
    p_strClassName = new string("XMLParser");
}
XMLParser::~XMLParser(){
    vCleanXMLTree();
    delete p_strClassName;
}

void XMLParser::vCleanXMLTree(){
    mxmlDelete(p_PointerToXMLTree);
}


int XMLParser::LoadXMLFile(string *p_strConfigPath){
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
        cout<<p_strClassName<<"Failed to open"<<*p_strConfigPath<<endl;
        return 1;
    }
}

string XMLParser::GetStringValue(string strWordToSearch,string strValueOnError){
    mxml_node_t *p_mxmlPointer = mxmlFindElement(p_PointerToXMLTree, p_PointerToXMLTree,
                                                strWordToSearch.c_str(),NULL,NULL,
                                                MXML_DESCEND);
    if(!p_mxmlPointer){
        cout<<*p_strClassName<<"Didn't found: "<<strWordToSearch<<endl;
        return strValueOnError;
    }
    return p_mxmlPointer->child->value.opaque;
}

/*

    FILE *fp;
    mxml_node_t *tree;
    mxml_node_s *p_mxmlServer;
    const char *buf;
    int z;
    fp = fopen(p_strConfigPath->c_str(), "rb");
    tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
    fclose(fp);
    p_mxmlServer = mxmlFindElement(tree, tree,"Adress",
                                   NULL,NULL,
                                   MXML_DESCEND);
    if (p_mxmlServer){
        fprintf(stderr,"Server: %s\n",p_mxmlServer->child->value.opaque);
    }
    buf = p_mxmlServer->child->value.opaque;
    */
