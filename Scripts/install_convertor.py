#!/usr/bin/python

import sys
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

from install_generic import *


install_dir("src", KUMIR_DIR+"/Addons/convertor")
install_file("../c99.convertor", KUMIR_DIR+"/Addons/c99.convertor", False)
install_file("../c_plus_plus.convertor", KUMIR_DIR+"/Addons/c_plus_plus.convertor", False)
