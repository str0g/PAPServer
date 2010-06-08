#! /usr/bin/env python
# -*- coding: utf-8 -*-
'''
####Author Łukasz Buśko                
####Contact buskol.waw.pl@gmail.com    
####Licenes GNU/General Public License                    
'''
try:
    import sys
    import os
    import time
    import string
    import hashlib
    from StringIO import StringIO
    from lxml import etree
    #from PyQt4 import QtCore
    #Moje
except ImportError, error:
    print "[xmlSupport]->", error
    time.sleep(2)
    sys.exit(1)

class XMLFileGenerator():
    def __init__(self, FilePath = ""):
        '''
        Poza inicjalizacja clasy sprawdza czy plik profilu udostepnianych plikow istnieje i jest poprawny.
        Jezeli nie podjemuje akcje typu tworzenie pliku, nadpisywanie pliku.
        '''
        XMLFileGenerator.ClassName = "[XMLFileGenerator]->"
        self.Templates()
    def CreateXMLSharedFilesList(self):
        '''
        Tworze nowa liste udostepnionych plikow
        '''
        return etree.Element("SharedFiles")
    def CreateXMLServerFilesList(self, strData):
        '''
        Tworze nowa liste udostepnionych plikow
        '''
        return etree.fromstring(strData)
    def OpenXMLSharedFileList(self, strPathToFile):
        '''
        Wczytuje juz stworzona wczesniej liste udostepnionych plikow
        '''
        try:
            fd = open(strPathToFile, 'ro')
            XMLBuffer = etree.fromstring(fd.read())
            fd.close()
        except IOError, error:
            print error
            XMLBuffer = None
        return XMLBuffer
    def AddNodeToXMLFilesList(self,XMLBuffer, Offset,  ElementList, HashType, isCountSum):
        '''
        Generuje nowy subelement nastepnie do niego generuje subelementu zawierajaca dane oraz zwiekszam licznik
        '''
        intRetCode = 0
        intInvalidData = 0
        intTmpCountSum  = Offset
        strNode = "FilePath"
        for index in range(len(ElementList)):
            intFound = 0
            if len(XMLBuffer) > 0: #kazdy kolejny element
                for indeX in range(len(XMLBuffer)):
                    for indeY in range(len(XMLBuffer[indeX])):
                        if XMLBuffer[indeX][indeY].tag == strNode and XMLBuffer[indeX][indeY].text == ElementList[index] :
                            intFound = 1
                            intInvalidData += 1
                            break
                    if intFound == 1:
                        break
                if intFound == 0:
                    print "Count", intTmpCountSum
                    self.AddNodeToXMLFilesListByOne(XMLBuffer, intTmpCountSum, str(ElementList[index]), str(HashType), isCountSum )
                    intTmpCountSum +=1
            elif len(XMLBuffer.tag)  == 11: #pierwszy element
                self.AddNodeToXMLFilesListByOne(XMLBuffer, intTmpCountSum, str(ElementList[index]), str(HashType),  isCountSum)
                intTmpCountSum +=1
            else:#Nieznany blad
                intRetCode = 2
                break
        print "SUPERWAZNE", intTmpCountSum
        if intInvalidData == len(ElementList):
            intRetCode = 1
        return intRetCode, intInvalidData
    def AddNodeToXMLFilesListByOne(self, XMLBuffer, Offset, ElementList, HashType,  isCountSum):
        '''
        Dodaj Elementy do listy
        Limitacja biblioteki lxml 2.2.6 ELEMENTY MUSZA byc rozroznialne
        '''
        newFile = etree.SubElement(XMLBuffer, "File"+str(Offset))                              # Limitacja biblioteki lxml 2.2.6 ELEMENTY MUSZA byc rozroznialne
        etree.SubElement(newFile, "FilePath").text = ElementList                                                            #0
        etree.SubElement(newFile, "FileName").text = os.path.basename(ElementList)                              #1
        etree.SubElement(newFile, "FileSize").text = str(os.path.getsize(ElementList))                             #2
        etree.SubElement(newFile, "FileHashType").text = HashType                                                           #3 +:
        if HashType == "md5" and isCountSum == 1:
            etree.SubElement(newFile, "FileHash").text = hashlib.md5(open(ElementList, "ro").read()).hexdigest()   
        elif HashType == "sha512" and isCountSum == 1:
            etree.SubElement(newFile, "FileHash").text = hashlib.sha512(open(ElementList, "ro").read()).hexdigest()   
        else:
            etree.SubElement(newFile, "FileHash").text = "None"                                                                        #3
        etree.SubElement(newFile, "FileLastModification").text = str(os.path.getctime(ElementList))             #4
    def DeleteNodeToXMLFilesList(self,XMLBuffor,  strToDelete , NodeToDelete ="FilePath"):
        '''
        Kasuje Element z listy udostepnianych plikow, pozwala wybrac po jakim elemencie szukamy wskazane wartosci (wymaga lxml 2.2.6)
        Limitacja biblioteki lxml 2.2.6 ELEMENTY MUSZA byc rozroznialne
        '''
        #'''
        for index in range(len(strToDelete)):
            for indexX in range(len(XMLBuffor)):
                breakLoop = 0
                for indexY in range(len(XMLBuffor[indexX])):
                    if XMLBuffor[indexX][indexY].tag == NodeToDelete :
                       if XMLBuffor[indexX][indexY].text == strToDelete[index] :
                        etree.strip_elements(XMLBuffor, XMLBuffor[indexX].tag)
                        breakLoop = 1
                    break
                if breakLoop == 1:
                    break
        #'''
        #Wersja ponizej to jeszcze nie dokonczone rozwiazanie problemu...
        '''
        for index in range(len(strToDelete)):
            for indexX in range(len(XMLBuffor)):
                breakLoop = 0
                for indexY in range(len(XMLBuffor[indexX])):
                    if XMLBuffor[indexX][indexY].tag == NodeToDelete :
                        if XMLBuffor[indexX][indexY].text == strToDelete[index] :
                            etree.strip_elements(XMLBuffor[indexX], '*')
                            #XMLBuffor = XMLBuffor[indexX-1:index+1] #bezsens...
                            breakLoop = 1
                        break
                if breakLoop == 1:
                    break
        '''   
    def ValidateXMLDocument(self, PastDoc, intOption = 0):
        '''
        Sprawdza poprawnosc dokumentu zwraca True/False
        '''
        MethodName = XMLFileGenerator.ClassName+"[ValidateXMLDocument]->"
        if intOption == 0:
            return etree.RelaxNG(etree.parse(self.TemplateSharedFiles)).validate(etree.parse(PastDoc))
        elif intOption == 1:
            pass
        elif intOption == 2:
            pass
        else:
            print MethodName,"Due to limitation of lxml 2.2.6 library, validation will be aborder and True will be returned"
            return 0
    def GenerateResponsForServer(self, strStatus ="aboard", intChunk = None, bufData = None):
        '''
        Generowania szablonu wyslania odpowiedz lub danych
        '''
        ResponseXML = etree.Element("StreamedFile")
        subEle_status = etree.SubElement(ResponseXML, "status").text = strStatus
        if intChunk:
            subEle_chunk = etree.SubElement(ResponseXML, "chunk").text = str(intChunk)
        if bufData:
            subEle_data = etree.SubElement(ResponseXML, "data").text = bufData
        return etree.tostring( ResponseXML, pretty_print=True )
    def WriteXMLToFile(self, strXMLFile, XmlData):
        '''
        Wpisuje do pliku w sposob czytelny dla czlowieka
        '''
        #self.PrintXML(XmlData)
        try:
            fd = open(strXMLFile, 'w')
            if fd:
                fd.write(etree.tostring( XmlData, pretty_print=True))#Pretty_print nie dziala z jakiejs przczyny
                fd.close()
                return 0
        except IOError,  error:
            print error
            return 1
    def PrintXML(self, XMLToBePrinted):
        '''
        Wypisuje drzewo xml na stdout (wymaga lxml 2.2.6)
        '''
        etree.dump(XMLToBePrinted, pretty_print=True, with_tail=True)
        #print(etree.tostring(self.buf_XMLSharedFilesList, pretty_print=True,xml_declaration=True,  encoding='utf-8'))
    def PurgeXMLList(self, objToBePurge):
        etree.strip_elements(objToBePurge, '*')
    def Templates(self):
        self.TemplateSharedFiles = StringIO('''\
<element name="SharedFiles" xmlns="http://relaxng.org/ns/structure/1.0" datatypeLibrary="http://www.w3.org/2001/XMLSchema-datatypes">
 <zeroOrMore>
  <element name="File">
   <element name="FilePath">
     <data type="string"/>
   </element>
   <element name="FileName">
     <data type="string"/>
   </element>
   <element name="FileSize">
     <data type="integer"/>
   </element>
   <element name="FileHashType">
     <data type="string"/>
   </element>
   <element name="FileHash">
     <data type="string"/>
   </element>
   <element name="FileLastModification">
     <data type="double"/>
   </element>
  </element>
 </zeroOrMore>
</element>
''')
        self.TemplateSendSharedToServer = StringIO('''\
<Unknown/>
''')
        self.TemplateStatus = StringIO('''\
<status></status>
<chunk></chunk>
<data></data>
''')

