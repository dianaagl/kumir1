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
    install_file("../pictomir",KUMIR_DIR+"/pictomir",True)
    create_shell_link(KUMIR_DIR+"/pictomir",PREFIX+"/bin/pictomir")
elif SPEC=="win32":
    install_file("../pictomir.exe",KUMIR_DIR+"/pictomir.exe",True)
install_dir("Documentation",KUMIR_DIR+"/Pictomir")
install_dir("Environments",KUMIR_DIR+"/Pictomir")
install_dir("Tasks",KUMIR_DIR+"/Pictomir")
install_dir("Stylesheets",KUMIR_DIR+"/Pictomir")
install_dir("HyperText",KUMIR_DIR+"/Pictomir")
install_dir("Images",KUMIR_DIR+"/Pictomir")
install_dir("Languages",KUMIR_DIR+"/Pictomir")
if SPEC=="unix":
    install_file("HyperText/about/logo.png",
                 PREFIX+"/share/pixmaps/pictomir.png",False)
    install_file("X-Desktop/pictomir.desktop",
                 PREFIX+"/share/applications/pictomir.desktop",False)

