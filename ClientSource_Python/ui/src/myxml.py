#! /usr/bin/env python
# -*- coding: utf-8 -*-
"""Ten plik zawiera klasy zapewniajace wielo-watkowa obsluge procesow zwiazanych z przetwarzaniem dokumentu xml
Ustawianiem praw dostepu, oraz flagi zapisu do pliku
"""
__author__ =  'Lukasz Busko'                
__email__ = 'buskol.waw.pl@gmail.com'
__version__= '0.2.2'
__licenes__= 'Licenes GPL'

try:
    import time,os,sys
    import hashlib
    import codecs
    import StringIO
    from xml.dom import minidom
    import xml.parsers.expat
    from PyQt4 import QtCore
    import threading
except ImportError,error:
    print "[XML]->",error
    sys.exit(1)

class FileLoaderThread(threading.Thread):
    """Zadaniem tej klasy jest odczytac atrybuty plik oraz policzyc jego sume kontrolna
        Testy wydjajnosc wykazaly,ze:
        [Dla plikow znajdujacych sie na jednym dysku i w obrebie tej samej partycji]
        - dla plikow o podobnej wielkosc nie ma znaczenia czy uzyjemy 1, 2  czy 6 watkow
        - dla plikow o wielkosc losowej 2 lub wiecej watkow podnosi wydajnosc do 20%
        [Dla plikow znajdujacych sie na 2 roznych dyskach]
        - w zaleznosci od teog czy pliki zostaly podzielone po rowno wzorst wydjanosci o 100%
        - przy zlym podziale wydajnosc moze wzorsnac od -50% do 99%
    """
    def __init__(self,data,parent):
        threading.Thread.__init__(self)
        self.Data = data
        self.Parent = parent
        self.strFileName = ""
        self.strSize = 0
        self.HashType = parent.HashType
        self.strHashSum = ""
        self.strLastModData = 0
    def run(self):
        """Przed z updatowaniem zasobo xml, poprzez metode rodzica nastepuje zablokowania specjalnym semaforem
        """
        for i in self.Data:
            if os.path.isfile(i):
                self.strFileName = os.path.basename(i)
                self.strSize = str(os.path.getsize(i))
                self.strLastModData = str(os.path.getctime(i))
                if self.HashType == "md5" and iself.Parent.sCountSum == 1:
                    self.strHashSum = hashlib.md5(open(i, "ro").read()).hexdigest() 
                elif self.HashType == "sha512" and self.Parent.isCountSum == 1:
                    self.strHashSum = hashlib.sha512(open(i, "ro").read()).hexdigest() 
                else:
                    self.strHashSum = "None"
                self.Parent.conditionFileLoaders.acquire()
                self.Parent.addFileNode(i, self.strFileName, self.strSize, self.HashType, self.strHashSum, self.strLastModData)
                self.Parent.conditionFileLoaders.notify()
                self.Parent.conditionFileLoaders.release()
                
class FileIfInsertThread(threading.Thread):
    """Klasa porownuje w watkach czy zawrtosc listy znajduje sie juz na liscie mxl
    Testow wydajnosc nie przeprowadzielm
    Jednak biorac pod uwage fakt, ze limitami sa ilosc rdzeni i przepustowosc podsystemu pamieci
    Wzorst wydajnosc powinien wyniesc okolo 99% dla maszyny wyposarzonej w 2 rdzenie oraz 350% dla 4 z dosc szybkim podsystemem pamieci
    """
    def __init__(self, lista, node, start, end):
        threading.Thread.__init__(self)
        self.Lista = lista
        self.Node = node
        self.startp = start
        self.endp = end
        self.VectorList = []
        self.intCounter = 0
    def run(self):
        """Po zakonczeniu operacji trzeba z pamieci pobrac licznik wystapien oraz Wektor do ktorego trafialy indexy zawierajace powtarzajace sie dane
        """
        for i in range(self.startp, self.endp):
            listTmp = self.Node.item(i)
            if listTmp != None:
                listTmp = listTmp.childNodes
                if listTmp.length > 0:
                    if (listTmp.item(0).childNodes.item(0).nodeValue in self.Lista):
                        self.VectorList.append(self.Lista.index(listTmp.item(0).childNodes.item(0).nodeValue))
                        self.intCounter+=1
                else:
                    break
            else:
                break
        #self.Queue.task_done()
        

class myXmlDoc():
    """Obsluguje zawiarajacy w swoich zasobach obiekt XML
    """
    def __init__(self, obj_Config):
        self.ClassName ="[myXML]->"
        self.intDivderValid = 2 #obj_Config.
        self.intDivderInsertion = 2 #obj_Config.
        self.HashType ="sha512" #obj_Config.
        self.isCountSum = 1
        self.xmlDiskFile = obj_Config.SharedFiles
        self.HumanReadAbleOutPut = obj_Config.HumanReadAbleOutPut
        self.Threadlist = []
        self.WriteToFileFlag = 0
        self.XMLDoc = None
        self.xmlWorkNode = None
        self.conditionFileLoaders = threading.Condition()
        self.SetSignals()
        
    def SetSignals(self):
        self.QSignalObjFileLoader = QtCore.QObject(None)
        
    def CreateDocFromFile(self, file =None):
        """Metoda tworzy drzewo xml z bufora lub pliku ponad to prymitywnie obsluguje te bledy
        czyli zwraca, ze sie nie udalo i przy czesci flag co poszlo nie tak
        """
        intRetCode = 1
        if file == None:
            file = self.xmlDiskFile
        if self.XMLDoc == None:
            if not os.path.isfile(file):
                file = StringIO.StringIO(str(file))
            try :
                self.XMLDoc = minidom.parse(file)
                self.xmlWorkNode = self.XMLDoc.documentElement
                intRetCode = 0
            except xml.parsers.expat.XML_ERROR_INCORRECT_ENCODING, error:
                #singal
                print self.ClassName,"error"
            except xml.parsers.expat.XML_ERROR_INVALID_TOKEN, error:
                #singal
                print self.ClassName,"error"
            except xml.parsers.expat.XML_ERROR_BAD_CHAR_REF, error:
                #singal
                print self.ClassName,"error"
            except xml.parsers.expat.XML_ERROR_JUNK_AFTER_DOC_ELEMENT, error:
                #singal
                print self.ClassName,"error"
            except xml.parsers.expat.XML_ERROR_NO_MEMORY, error:
                #singal
                print self.ClassName,"error"
            except xml.parsers.expat.XML_ERROR_UNCLOSED_TOKEN, error:
                #singal
                print self.ClassName,"error"
            except xml.parsers.expat.XML_ERROR_NO_MEMORY, error:
                #singal
                print self.ClassName,"error"
            except xml.parsers.expat.XML_ERROR_FINISHED, error:
                #singal
                print self.ClassName,"error"
            finally:
                return intRetCode
            
        else:
            print self.ClassName+"XMLDocInUse (Buf/File)"
            return intRetCode
            
    def CreateDoc(self, root =None, attr =None):
        """Tworzy nowy dokument z element root o podanej nazwie"""
        if self.XMLDoc == None:
            self.XMLDoc = minidom.Document()
            self.xmlWorkNode = self.XMLDoc
            if root != None:
                self.addElement(root, None, 1, attr)
            self.xmlWorkNode = self.XMLDoc.documentElement
        else:
            print self.ClassName+"XMLDocInUse (CreateDoc)"
            
    def ValidateFiles(self):
        """Przeprowadza walidacje elementow jezeli jakis element zostal zmodyfikowany jego dane zostana uaktualnione
        Ta metoda nie dzial na watkach poniewaz nie przewiduje zmienienia tak duzej ilosci elementow naraz.
        """
        MethodName = self.ClassName +"[ValidateFiles]->"
        if self.XMLDoc != None:
            try:
                for i in range(self.XMLDoc.childNodes[0].childNodes.length):
                    tmp = self.XMLDoc.childNodes[0].childNodes[i].childNodes[0].childNodes[0].nodeValue
                    if(os.path.isfile(tmp)):
                        if str(os.path.getctime(tmp)) != self.XMLDoc.childNodes[0].childNodes[i].childNodes[5].childNodes[0].nodeValue:
                            print self.ClassName+"Updating information about:", tmp
                            self.XMLDoc.childNodes[0].childNodes[i].childNodes[5].childNodes[0].nodeValue = str(os.path.getctime(tmp))
                            self.XMLDoc.childNodes[0].childNodes[i].childNodes[2].childNodes[0].nodeValue = str(os.path.getsize(tmp))
                            if self.XMLDoc.childNodes[0].childNodes[i].childNodes[3].childNodes[0].nodeValue == "sha512":
                                self.XMLDoc.childNodes[0].childNodes[i].childNodes[4].childNodes[0].nodeValue = hashlib.sha512(open(tmp, "ro").read()).hexdigest() 
                            elif self.XMLDoc.childNodes[0].childNodes[i].childNodes[3].childNodes[0].nodeValue == "md5":
                                self.XMLDoc.childNodes[0].childNodes[i].childNodes[4].childNodes[0].nodeValue = hashlib.md5(open(tmp, "ro").read()).hexdigest() 
                            else:
                                self.XMLDoc.childNodes[0].childNodes[i].childNodes[4].childNodes[0].nodeValue = "None"
                            self.WriteToFileFlag= 1
                    else:
                        print self.ClassName+"File has been removed from list:", tmp
                        self.XMLDoc.childNodes[0].removeChild(self.XMLDoc.childNodes[0].childNodes[i])
                        self.WriteToFileFlag= 1
            except IndexError, error:
                print MethodName, error
                #sygnal
                print self.ClassName+"Please share files"
                
    def ValidateIfFilesAreAlreadyThere(self , FileList):
        """Metoda tworzy watki, czeka az skoncza prace, pobierach z nich wyniki nastepnie jezeli potrzeba zmienia liste ktora dostal na wejsciu
        zwraca ilosc elementow kotre zostna updatowane oraz ile elementow podano
        """
        intElementNumber = len(FileList)
        intCounter = 0
        intTMP = 0
        intLen = 0
        intPossition = 0
        bLoop = bool(True)
        
        if self.XMLDoc != None:
            xmlList = self.XMLDoc.childNodes
            if (xmlList.length > 0):
                listChild = xmlList.item(0)
                if listChild != None:
                    listChild = listChild.childNodes
                    intLen = listChild.length
                    if intLen > 1:
                        intTMP = intLen /self.intDivderValid
                    elif intLen == 1:
                        intTMP = 1
                    else:
                        bLoop = bool(False)
                        
                    while(bLoop):
                        v_th = FileIfInsertThread(FileList,  listChild, intPossition, intTMP)
                        self.Threadlist.append(v_th)
                        v_th.start()
                        intPossition = intTMP
                        intTMP +=intTMP
                        
                        if 2*intLen == intTMP:
                            bLoop = bool(False)
                        elif intLen < intTMP:
                            intTMP = intLen
                        elif 2*intTMP > intLen:
                            intTMP = intLen
                        elif intTMP == 0:
                            bLoop = bool(False)
                            print self.ClassName+"This error can Not happed!"
                            
                    print self.ClassName+"Validing in", len(self.Threadlist), "threads"
                    VectorList= []
                    for i in self.Threadlist:
                        i.join()
                        print i.VectorList ,"<->",  i.intCounter
                        if i.intCounter != 0:
                            intCounter+=i.intCounter
                            VectorList.extend(i.VectorList)
                                        
                    self.CleanThreadList()

                    VectorList.sort()
                    VectorList.reverse()
                    for i in VectorList:
                        FileList.pop(i)
#                    intV = len(VectorList)-1
#                    for i in range(len(VectorList)):
#                        FileList.pop(VectorList[intV - i])

        if intElementNumber - intCounter != 0:
            self.WriteToFileFlag = 1
        return intElementNumber - intCounter, intElementNumber
            
    def  addElement(self, Ename="", value=None, intdeeper = 0, attr = None):
        """Metoda  sluzy do budowania elementow drzewa oraz przypisywania im okreslonej warosci
        """
        if self.XMLDoc!= None and Ename!="":
            tmp = self.XMLDoc.createElement(Ename)
            if value != None:
                tmp.appendChild(self.XMLDoc.createTextNode(value))
            self.xmlWorkNode.appendChild(tmp)
            if intdeeper == 1:
                self.xmlWorkNode = tmp
                
    def addFileNode(self, fp, fn, fz, fht, fh, flmd):
        """Metoda tworzy elementy wedlug wzorca
        """
        tmpFile = self.XMLDoc.createElement("File")
        tmpFile.appendChild(self.XMLDoc.createElement("FilePath")).appendChild(self.XMLDoc.createTextNode(fp))
        tmpFile.appendChild(self.XMLDoc.createElement("FileName")).appendChild(self.XMLDoc.createTextNode(fn))
        tmpFile.appendChild(self.XMLDoc.createElement("FileSize")).appendChild(self.XMLDoc.createTextNode(fz))
        tmpFile.appendChild(self.XMLDoc.createElement("FileHashType")).appendChild(self.XMLDoc.createTextNode(fht))
        tmpFile.appendChild(self.XMLDoc.createElement("FileHash")).appendChild(self.XMLDoc.createTextNode(fh))
        tmpFile.appendChild(self.XMLDoc.createElement("FileLastModificationData")).appendChild(self.XMLDoc.createTextNode(flmd))
        self.xmlWorkNode.appendChild(tmpFile)
    def addNode(self, node):
        """Dopisuje glaz/lisc do struktury xml"""
        self.xmlWorkNode.appendChild(node)

    def FreeXMLDoc(self):
        """Teoretycznie unlink powinien usunac cala struktora w zeczywistosci nie dziala prawidlow ale mniejsza o to
        del konczy dzielo zniszczenia
        """
        self.XMLDoc.unlink()
        del self.XMLDoc
        self.XMLDoc = None
    def CleanThreadList(self):
        """Czysci liste z watkow, odwijajac od tylu w celu unikniecia bledow"""
        tc = len(self.Threadlist) -1
        for i in range(tc+1):
            del self.Threadlist[tc - i]
        
        if len(self.Threadlist) >0:
            print self.ClassName+"Threads haven't been remove join or whatever... fatal error"
            print "*"*20
            print self.Threadlist
            print "*"*20
    def PurgeNodes(self):
        try:
            self.XMLDoc.removeChild(self.XMLDoc.childNodes[0])
            self.xmlWorkNode = self.XMLDoc
            self.WriteToFileFlag= 1
        except IndexError, error:
            print self.ClassName, error
        
    def RemoveChildren(self, VectorList):
        MethodName = self.ClassName+"[Remove]->"
        intLen = len(VectorList)
        Vector=[]
        try:
            for i in range(self.GetNumberOfChildren()):
                if  self.XMLDoc.childNodes[0].childNodes[i].childNodes[0].childNodes[0].nodeValue in VectorList:
                    Vector.append(i)
                    intLen -= 1
                elif intLen == 0:
                    break
            Vector.sort()
            Vector.reverse()
            for i in Vector:
                self.XMLDoc.childNodes[0].removeChild(self.XMLDoc.childNodes[0].childNodes[i])
                
            self.WriteToFileFlag= 1
        except IndexError, error:
            print MethodName, error
    def PrintXMLDoc(self):
        """Wypisuje na ekran document w standardzie UTF-8"""
        if self.XMLDoc != None:
            print self.XMLDoc.toprettyxml(encoding="UTF-8")
        else:
            print self.ClassName+"First create document then try to print it :-)"
            
    def GetXMLAsText(self, encoding = "ASCII"):
        """Zwraca drzewo w formie textu bez formatowania czytelnego dla ludzi
        """
        return self.XMLDoc.toxml(encoding)
        
    def WriteXMLDocToFile(self, pretty =0):
        """Po zapisie do pliku ustawia mu flage read-only, ze ciekawscy niczego przypadkiem nie zmodyfikowali"""
        ind= ""
        newl=""
        if pretty == 1 or self.HumanReadAbleOutPut == 1:
            print "ustawione"
            ind ="\t"
            newl="\n"
        if self.XMLDoc != None and self.WriteToFileFlag == 1:
            try:
                if os.path.isfile(self.xmlDiskFile):
                    os.chmod(self.xmlDiskFile, 0644)
                fp = open(self.xmlDiskFile,"w")
                writer = codecs.lookup('utf-8')[3](fp)
                self.XMLDoc.writexml(writer,ind, ind, newl ,encoding="UTF-8")
                fp.close()
                os.chmod(self.xmlDiskFile, 0444)
            except IOError, error:
                print self.ClassName,  error
                #signal
        elif self.WriteToFileFlag!= 1:
            print self.ClassName, "File need't to be updated"
        else:
            print self.ClassName, "First create document then try write it to file :-)"
            
    def FileListInsertion(self,filelist):
        """Metoda tworzy watki potrzebne do dodania informacji o plikach do struktury xml"""
        intPossition = 0
        intLen = len(filelist)
        bLoop= bool(True)
        
        if intLen > 1:
            intTMP = intLen/self.intDivderInsertion
        elif intLen == 1:
            intTMP = 1
        else:
            bLoop=bool(False)
            
        while(bLoop):
            l_th = FileLoaderThread(filelist[intPossition:intTMP],self)
            self.Threadlist.append(l_th)
            l_th.start()
            intPossition = intTMP
            intTMP +=intTMP
            
            if 2*intLen == intTMP:
                bLoop = bool(False)
            elif intLen < intTMP:
                intTMP = intLen
            elif 2*intTMP > intLen:
                intTMP = intLen
            else:
                pass

        print self.ClassName+"Adding in", len(self.Threadlist), "threads"
        for i in self.Threadlist:
            i.join()
        self.CleanThreadList()
        #sygnal
    def GetNumberOfChildren(self):
        """Zwraca ilosc dzieci po elementem roota (<file>)"""
        return self.XMLDoc.childNodes[0].childNodes.length
        
    def GetNodeChildrenAsList(self, intIndex, rend, rstart=0):
        """Pobiera dane z dokumetu i zwraca je jako liste w celu latwiejszego przetwarzania
        """
        retList=[]
        try:
            for i in range(rstart, rend):
                retList.append(self.XMLDoc.childNodes[0].childNodes[intIndex].childNodes[i].childNodes[0].nodeValue)
        except IndexError, error:
            print self.ClassName, error
        finally:
            return retList
    def GetNodeChildrenAsNode(self, intIndex):
        """Kopiuje Lisci/Galaz i  zwraca ja"""
        MethodName = self.ClassName+"[GetNodeChildrenAsNode]->"
        retNode = None
        try:
            retNode = self.XMLDoc.childNodes[0].childNodes[intIndex].cloneNode(True)
        except IndexError, error:
            print Method, error
        finally:
            return retNode       
    def GenerateFileRequest(self, externalXMLstruct, VectorIndex):
        """Przetwarza liste zawierajaca wektory interesujacych nas dzieci w drzwie z ktorego pobierzemy dane"""
        self.CreateDoc("FileRequest")
        for index in VectorIndex:
            node = externalXMLstruct.GetNodeChildrenAsNode(index)
            if node != None:
                self.addNode(node)
