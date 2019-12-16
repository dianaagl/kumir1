#!/usr/bin/python

import sys

LIB = None

for arg in sys.argv:
    if arg.startswith("--prefix="):
        PREFIX = arg[9:]
    if arg.startswith("--spec="):
        SPEC = arg[7:]
    if arg.startswith("--kumir-dir="):
        KUMIR_DIR = arg[12:]
    if arg.startswith("--lib="):
        LIB = arg[6:]
    
if SPEC=="": SPEC="unix"
if PREFIX=="": PREFIX="/usr/local"
if KUMIR_DIR=="": KUMIR_DIR = PREFIX+"/kumir"

if LIB is None:
    sys.stderr.write("Library not specified")
    sys.exit(1)

from install_generic import *

if SPEC=="unix":
    install_file("../../Libraries/lib"+LIB+".so",KUMIR_DIR+"/Libraries/lib"+LIB+".so",False)
elif SPEC=="win32":
    install_file("../../Libraries/"+LIB+".dll",KUMIR_DIR+"/Libraries/"+LIB+".dll",False)