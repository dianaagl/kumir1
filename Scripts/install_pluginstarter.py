#!/usr/bin/python

import sys

for arg in sys.argv:
    if arg.startswith("--prefix="):
        PREFIX = arg[9:]
    if arg.startswith("--spec="):
        SPEC = arg[7:]
    if arg.startswith("--kumir-dir="):
        KUMIR_DIR = arg[12:]
    
if SPEC=="": SPEC="unix"
if PREFIX=="": PREFIX="/usr/local"
if KUMIR_DIR=="": KUMIR_DIR = PREFIX+"/kumir"

from install_generic import *

if SPEC=="unix":
    install_file("../pluginstarter",KUMIR_DIR+"/pluginstarter",True)
    create_shell_link(KUMIR_DIR+"/pluginstarter",PREFIX+"/bin/kumpluginstarter")
elif SPEC=="win32":
    install_file("../pluginstarter.exe",KUMIR_DIR+"/pluginstarter.exe",True)
