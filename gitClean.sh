#!/bin/bash

val='D	ClientSource_Python/ui/src/AdminPanel.py ClientSource_Python/ui/src/AdminPanel.py_2010-05-30_22:56:23 ClientSource_Python/ui/src/AdminPanel.pyc ClientSource_Python/ui/src/MainFrame.py ClientSource_Python/ui/src/MainFrame.py_2010-05-30_22:56:23 ClientSource_Python/ui/src/MainFrame.pyc ClientSource_Python/ui/src/MsgsLib.py ClientSource_Python/ui/src/MsgsLib.pyc ClientSource_Python/ui/src/__init__.py ClientSource_Python/ui/src/__init__.pyc ClientSource_Python/ui/src/myClient.py ClientSource_Python/ui/src/myClient.pyc ClientSource_Python/ui/src/myComm.py ClientSource_Python/ui/src/myConfigs.py ClientSource_Python/ui/src/myConfigs.pyc ClientSource_Python/ui/src/myTelnet.py ClientSource_Python/ui/src/myTelnet.pyc ClientSource_Python/ui/src/xmlSupport ClientSource_Python/ui/src/xmlSupport.py ClientSource_Python/ui/src/xmlSupport.pyc'

touch $val
if [ $? -eq 0 ]; then
    git pull origin master
    git add *
    git commint -m "fix"
    git push origin master
    git rm -r ClientSource_Python/ui/src/
    git push origin master
fi


