#!/usr/bin/python 
# -*- coding: utf-8 -*-

import re

def extract_codes(filename):
    "Извлечение кодов ошибок и соответствующих им переменных из .h-файла"
    f = open(filename, "r")
    lines = f.read().split("\n")
    f.close()
    result = []
    for line in lines:
        define = re.compile(r'#define\s+(\S+)\s+(\d+)')
        m = define.search(line)
        if not m is None and len(m.groups())==2:
            var = m.groups()[0]
            code = m.groups()[1]
            result.append( (var, code) )
    return result
    
def extract_texts(filename):
    "Извлечение текстов ошибок из .msg-файла"
    f = open(filename, "r")
    lines = f.read().split("\n")
    f.close()
    result = dict()
    for line in lines:
        msg = re.compile(r'(\d+)\s+(.+)')
        m = msg.search(line)
        if not m is None and len(m.groups())==2:
            code = m.groups()[0]
            text = m.groups()[1]
            result[code] = text
    return result

H = ["error.h", "error_internal.h"]

CODES = []

import sys
import os.path

script_path = os.path.normpath(os.getcwd()+"/"+sys.argv[0])
script_basedir = os.path.dirname(script_path)

KUMIR_ROOT = script_basedir+"/../Kumir/"

LANG = "russian"

if len(sys.argv)>1:
    LANG = sys.argv[1]

for h in H: 
    CODES.append( (h, extract_codes(KUMIR_ROOT+"Headers/"+h) ) )

MSG_FILE = KUMIR_ROOT+"Languages/"+LANG+".msg"

TEXTS = extract_texts(MSG_FILE)

for h, codes in CODES:
    missing = []
    for var, code in codes:
        if not TEXTS.has_key(code):
            missing.append( (var, code) )
    if len(missing)>0:
        print "================"
        print "File: ", h
        for var, code in missing:
            print "\t%s\t%s" % ( var, code )
            
            



    
                
        
        
