#! /usr/bin/env python
# -*- coding: utf-8 -*-
'''
####Author Łukasz Buśko                
####Contact buskol.waw.pl@gmail.com    
####Licenes GNU/General Public License                        
'''

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
    print "[Menu]->"
    return myClient()

#MAIN
if __name__ == "__main__":
    sys.exit(Main())
