/***************************************************************
 * Name:      <Input name>.cpp/h
 * Purpose:   Code for Application <Name>
 * Author:    Łukasz Buśko (buskol.waw.pl@gmail.com)
 * Created:   <Input date>
 * Copyright: Łukasz Buśko (www.SoonWillBe.pl)
 * License:   GNU / General Public Licens
 **************************************************************/
///Headers
#include "dbase.hpp"

dBase::dBase(int opid):
            p_strClassName(new string("[dBase]["+liczba_na_string(opid)+"]->")),
            pConnection(mysql_init(NULL)),
            pResult(NULL),Row(NULL),
            p_cAdrress(ServerConfigs::p_cMySqlServerAddress.c_str()),
            p_cUser(ServerConfigs::p_cMySqlUser.c_str()),
            p_cPass(ServerConfigs::p_cMySqlPass.c_str()),
            p_cBassName(ServerConfigs::p_cMySqldBase.c_str()),
            p_cTableName("SharedFiles"),
            p_cTableBanned("BannedList"),
            bQuaryFaild(false),intOwnerPID(opid){
    ///Sprawdza czy udalo sie zainicjalizowac elementy biblioteki mysql jezeli nie to konczy forka
    if(!pConnection){
        cerr<<*p_strClassName<<"pConnection: Error"<<endl;
        delete p_strClassName;
        mysql_library_end();
        exit(EXIT_FAILURE);//zawsze 0 czy przy bledzie 1?
    }
    //p_strClassName = ;
}

dBase::~dBase(){
    ///Zwalnia pamieci w tym o czym czesto ludzie nie wiedza zwalnia zaladowane elementy biblioteki mysql
    if(pConnection){
        del();
        Close();
    }
    delete p_strClassName;
    mysql_library_end();
}
void dBase::Close(){
    ///Zamyka polaczenie z baza
    cerr<<*p_strClassName<<"dbase connection closed"<<endl;
    mysql_close(pConnection);
    }
bool dBase::insertFile(const char *fp,const char *fn,const char* fs,const char *fht,const char* fh,const char* flmd){
    ///Dodaje do bazy udostepniony plik uzytkownika
    char Query[8192];
    sprintf(Query, "INSERT INTO %s\
            (ID,FilePath,FileName,FileSize,FileHashType,FileHash,FileLastModification,FileOwner)\
            VALUES(NULL,'%s','%s','%s','%s','%s','%s','%d')"\
            ,p_cTableName,fp,fn, fs,fht,fh,flmd,intOwnerPID);
    return dbQuery(Query,"Data has been insert into database");
}
bool dBase::insertBanned(char *ip,char* lname){
    ///Dodaje do bazy infromacje o uzytkownku ktory dostal 1 ostrzezenie
    char Query[1280];
    sprintf(Query, "INSERT INTO %s\
            (ID,UserIP,UserLocalName,Tries,Banned)\
            VALUES(NULL,'%s','%s',%d,%d)"\
            ,p_cTableBanned,ip,lname, 1,0);
    return dbQuery(Query,"Missbehaving user has been add to banedlist");
}
bool dBase::del(){
    ///Kasuje wskazana wpisy po pid uzytkownika
    char Query[128];
    sprintf(Query, "DELETE FROM %s WHERE FileOwner = %d\
            ",p_cTableName,intOwnerPID);
    return dbQuery(Query,"Data has been deleted from database");
}
bool dBase::dBaseRun(){
    /**
    *Laczy sie baza
    *Sprawdza czy istnieja tablice
    */
    if(!mysql_real_connect(pConnection,p_cAdrress,p_cUser,p_cPass,p_cBassName,0,NULL,0)){
        cerr<<"Connection to dbase faild"<<endl;
        return false;
    }
    cout<<"Connected to database"<<endl;
    if (!CheckForTable(p_cTableBanned)){
        return false;
    }
    if (!CheckForTable(p_cTableName)){
        return false;
    }
    return true;
}
bool dBase::Rebuild_SharedFilesTable(){
    ///Tworzy tablice Shared
    if(drop("SharedFiles")){
        return createShared();
    }
    return false;
}
bool dBase::Rebuild_BannedTable(){
    ///Tworzy tablice Banned
    if(drop("BannedList")){
        return createBanned();
    }
    return false;
}
bool dBase::CheckForTable(char *cTable){
    ///Sprawdza czy tabela istnieje, jezeli nie to ja tworzy
    char Query[128];
    bool bRet = true;
    cout<<"Checking if table "<<cTable<<" exists"<<endl;
    sprintf(Query, "select * from %s limit 0,1\
            ",cTable);
    if(!dbQuery(Query,"Table found nothing to be done")){
        if(cTable == p_cTableBanned){
            bRet = createBanned();
        }else if(cTable == p_cTableName){
            bRet = createShared();
        }else{
            cerr<<"Wyszukiwanie nie obslugiwanej tablicy"<<endl;
            bRet = false;
        }
    }else{
        pResult = mysql_store_result(pConnection);
        mysql_free_result(pResult);
    }
    return bRet;
}
bool dBase::Search(const char *cSearch,bool Coma,char *cToSearch){
    ///wyszykuje pliki uzytkownikow o innym pidzie niz moj wlasny
    char Query[128];
    char *cSpecial = "%";
    cout<<"Searching for "<<cSearch<<" in "<<cToSearch<<endl;
    if (Coma){
        sprintf(Query, "select * from %s where %s like '%s%s%s' and FileOwner != '%d'\
                ",p_cTableName,cToSearch,cSpecial,cSearch,cSpecial,intOwnerPID);
    }else{
        sprintf(Query, "select * from %s where %s like %s%s%s and FileOwner != '%d'\
                ",p_cTableName,cToSearch,cSpecial,cSearch,cSpecial,intOwnerPID);
    }
    return dbQuery(Query,"Check results");
}
bool dBase::Search2(char *cSearch1,char *cSearch2,bool Coma,char *cToSearch1,char *cToSearch2){
    ///wyszukje banaowanych
    char Query[128];
    cout<<"Searching for "<<cSearch1<<" in "<<cToSearch1<<endl;
    if (Coma){
        sprintf(Query, "select * from %s where %s = '%s' and %s = '%s'\
                ",p_cTableBanned,cToSearch1,cSearch1,cToSearch2,cSearch2);
    }else{
        sprintf(Query, "select * from %s where %s = %s and %s = %s\
                ",p_cTableBanned,cToSearch1,cSearch1,cToSearch2,cSearch2);
    }
    return dbQuery(Query,"Check results2");
}

bool dBase::SearchFiles(XMLParser *p_xmlStructPointer,string *strctrl,string *strSearch,bool Coma,char *cToSearch){
    /**
    *Szukana wskazanego pliku wysyalajac zapytanie do bazy
    *Jezeli wyszukanie bylo bez bledow tworzy dokument xml
    *Do dokumentu trafiaja wyniki wyszukiwania jezeli byly jakie kolwiek
    *strctrl jest lista utworzona ze stringa w celu "zmagazynowania unikalnych wartosci plikow" jakie uzytkownik wyszukiwal (ochrona przed wymuszeniami)
    */
    if(*strSearch=="" or *strSearch==" "){ return false; }
    if(Search(strSearch->c_str(),Coma,cToSearch)){
        pResult = mysql_store_result(pConnection);
        while(Row = mysql_fetch_row(pResult)){
            if(p_xmlStructPointer->insertXMLSearch(Row[1],Row[2],Row[3],Row[4],Row[5],Row[6],Row[7])){
                strctrl->append(Row[1]);
                strctrl->append(" ");
                strctrl->append(Row[7]);
                strctrl->append("\n");
            }else{
                fprintf(stderr,"faild to insert |%s|%s|%s|%s|%s|%s|%s|\n",Row[1],Row[2],Row[3],Row[4],Row[5],Row[6],Row[7]);
            }
        }
        mysql_free_result(pResult);
        return true;
    }
    return false;
}
bool dBase::AskIfBanned(char *cSearch1,char *cSearch2,bool Coma,char *cToSearch1,char *cToSearch2){
    /**
    *Pytam sie bazy czy uzytkownik o takiej nazwie lokalnej i ip zostal dodany do bazy jezeli tak sprawdzam czy juz dostal bana.
    */
    bool bBanned = false;
    //konwertery k;
    if(Search2(cSearch1,cSearch2,Coma,cToSearch1,cToSearch2)){
        pResult = mysql_store_result(pConnection);
        while(Row = mysql_fetch_row(pResult)){
            string_naliczba<int>(Row[4]) == 1 ? bBanned = true : bBanned;
        }
        mysql_free_result(pResult);
    }
    return bBanned;
}
bool dBase::UpdateBanned(char *cSearch1,char *cSearch2,bool Coma,char *cToSearch1,char *cToSearch2){
    /**
    *Pytam sie bazy czy uzytkownik o takiej nazwie lokalnej i ip zostal dodany do bazy.
    * jezeli tak inkrementuje mu wartosc ostrzezen
    * jezeli nie utwarzam record
    */
    char Query[256];
    int intValue=1;
    int intBanned=0;
    konwertery k;
    if(Search2(cSearch1,cSearch2,Coma,cToSearch1,cToSearch2)){
        pResult = mysql_store_result(pConnection);
        if(mysql_num_rows(pResult)>0){
            while(Row = mysql_fetch_row(pResult)){
                intValue = (k.string_naliczba<int>(Row[3]))+1;
                intValue > 3 ? intBanned = 1 : intBanned;
            }
            sprintf(Query,"update %s set Tries = '%d',Banned ='%d' where %s ='%s' and %s ='%s'"
                    ,p_cTableBanned,intValue,intBanned,cToSearch1,cSearch1,cToSearch2,cSearch2);
            if(!dbQuery(Query,"Table BannedList has been updated")){ return false;}
        }else{
            insertBanned(cSearch1,cSearch2);
        }//if rows num
        mysql_free_result(pResult);
    }//search
    return AskIfBanned(cSearch1,cSearch2,Coma,cToSearch1,cToSearch2);
}
bool dBase::dbQuery(char *buf,string MsgOnSuccess){
    ///slozu do komunikacju z baza zwraca czy sie udalo czy nie oraz wyswietla komunikaty
    if(mysql_query(pConnection,buf) != 0){ //execute the query
        cerr<<buf<<endl;
        cerr<<GetLocalTime()<<*p_strClassName<<"Error Executing the query: "<<mysql_error(pConnection)<<endl;
        bQuaryFaild = true;
        return false;
    }
    cout<<GetLocalTime()<<*p_strClassName<<MsgOnSuccess<<endl;
    return true;
}
bool dBase::drop(char *cTable){
    ///Usuwam wskazana tabele, ktora musi sie znajdowac w okreslonym zakresie
    string strMsg="Table";
    strMsg.append(cTable);
    strMsg.append("has been droped");
    char Query[128];
    if(cTable == p_cTableBanned or cTable == p_cTableName){
        sprintf(Query, "DROP table %s\
                ",cTable);
        return dbQuery(Query,strMsg);
    }
    return false;
}
bool dBase::createShared(){
    ///Tworzy table SharedFiles
    char Query[2048];
    sprintf(Query,"create table %s\ (\
                           \
                           ID int unsigned not null auto_increment primary key,\
                           \
                           FilePath varchar(2048) not null,\
                           FileName varchar(1024) not null,\
                           FileSize int unsigned not null,\
                           FileHashType varchar(16),\
                           FileHash varchar(1024),\
                           FileLastModification float not null,\
                           FileOwner int unsigned not null,\
                           index(ID),\
                           unique(ID)\
                           )",p_cTableName);
    return dbQuery(Query,"Table SharedFiles has been created");
}
bool dBase::createBanned(){
    ///Tworzy tabele BannedList
    char Query[512];
    sprintf(Query,"create table %s\ (\
                    \
                    ID int unsigned not null auto_increment primary key,\
                    \
                    UserIP varchar(16) not null,\
                    UserLocalName varchar(1024) not null,\
                    Tries int unsigned not null,\
                    Banned bool not null,\
                    \
                    index(ID),\
                    unique(ID)\
                    )",p_cTableBanned);
    return dbQuery(Query,"Table Banned has been created");
}

