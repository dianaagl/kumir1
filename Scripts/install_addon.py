#!/usr/bin/python

import sys
RESOURCES = []
ADDON = None
PREFIX = ""
KUMIR_DIR = ""

for arg in sys.argv:
    if arg.startswith("--resources="):
        RESOURCES = arg[12:].split(",")
    if arg.startswith("--prefix="):
        PREFIX = arg[9:]
    if arg.startswith("--spec="):
        SPEC = arg[7:]
    if arg.startswith("--addon="):
        ADDON = arg[8:]
    if arg.startswith("--kumir-dir="):
        KUMIR_DIR = arg[12:]

if SPEC=="": SPEC="unix"
if PREFIX=="": PREFIX="/usr/local"
if KUMIR_DIR=="": KUMIR_DIR = PREFIX+"/kumir"

if ADDON is None:
    print "Error: Addon is not set"
    sys.exit(1)

from install_generic import *

if SPEC=="unix":
    lib = "lib"+ADDON+".so"
elif SPEC=="win32":
    lib = ADDON+".dll"
elif SPEC=="macx":
    lib = "lib"+ADDON+".dylib"

install_file("../"+lib, KUMIR_DIR+"/Addons/"+lib,False)
for resource_dir in RESOURCES:
    install_dir(resource_dir,
                KUMIR_DIR+"/Addons/"+ADDON)
