#! /usr/bin/env python
# -*- coding: utf-8 -*-

__author__ =  'Lukasz Busko'                
__email__ = 'buskol.waw.pl@gmail.com'
__licenes__= 'Licenes GPL'                      


try:
    import sys
    import time
    #Moje
    from myClient import myClient
except ImportError, error:
    print "[Menu]->", error
    time.sleep(2)
    sys.exit(1)

def Main():
    '''
    Main starter, zwraca to co zwraca Qt4 na wyjsciu
    '''
    return myClient()

#MAIN
if __name__ == "__main__":
    sys.exit(Main())
