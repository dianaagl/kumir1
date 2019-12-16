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
    install_file("../kumir",KUMIR_DIR+"/kumir",True)
    create_shell_link(KUMIR_DIR+"/kumir",PREFIX+"/bin/kumir")
elif SPEC=="win32":
    install_file("../kumir.exe",KUMIR_DIR+"/kumir.exe",True)
install_dir("Config",KUMIR_DIR+"/Kumir")
install_dir("Environments",KUMIR_DIR+"/Kumir")
install_dir("Examples",KUMIR_DIR+"/Kumir")
install_dir("Help",KUMIR_DIR+"/Kumir")
install_dir("HyperText",KUMIR_DIR+"/Kumir")
install_dir("RobotPult",KUMIR_DIR+"/Kumir")
install_dir("Macro",KUMIR_DIR+"/Kumir")
install_dir("PDScripts",KUMIR_DIR+"/Kumir")
install_dir("Languages",KUMIR_DIR+"/Kumir")
install_dir("Images/customwindow_pixmaps",KUMIR_DIR+"/Kumir")
if SPEC=="unix":
    install_file("Images/kumir.png",
                 PREFIX+"/share/pixmaps/kumir.png",False)
    install_file("X-Desktop/kumir.desktop",
                 PREFIX+"/share/applications/kumir.desktop",False)

