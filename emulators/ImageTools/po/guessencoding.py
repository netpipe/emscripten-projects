#!/usr/bin/python

import locale

FILENAME = "pt.po"
ENCODING = "UTF-8"

if __name__ == "__main__":
    
    data = open(FILENAME, 'rb').read()
    
    try:
        unicode(data, ENCODING)
    except:
        print "wrong encoding (" + ENCODING + ") or something"
    else:
        print "encoding " + ENCODING + " seemed to work on " + FILENAME
