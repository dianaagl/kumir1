#!/usr/bin/python

import sys
import subprocess
import os
sys.path += ["Scripts"]

from find_qmake import *
from find_lrelease import *
from generate_translations import *

MINIMUM_QT_VERSION = "4.6.0"

QMAKE_COMMAND = None
CUSTOM_QMAKE_COMMAND = None
QMAKE_ARGUMENTS = ["-recursive"]
LRELEASE_COMMAND = None
CUSTOM_LRELEASE_COMMAND = None
PREFIX = "/usr/local"
DIR = None

if "--help" in sys.argv:
    print """Usage:
configure [--prefix=PREFIX] [--qmake=QMAKE_COMMAND] [--lrelease-LRELEASE_COMMAND] [--debug] [--spec=QMAKE_SPEC]
    --prefix=PREFIX              set installation root (default "/usr/local")
    --target-dir=DIR             set installation directory (e.g. "/opt/kumir")
                                 Note:
                                   PREFIX and DIR may be different,
                                   then kumir installs in DIR, but
                                   launchers and desktop entries
                                   installs in PREFIX
    --qmake=QMAKE_COMMAND        custom qmake command to test
    --lrelease=LRELEASE_COMMAND  custom lrelease command to test
    --debug                      build with debugging support
    --spec=QMAKE_SPEC            set custom qmake spec (e.g. for cross-compile)
"""
    sys.exit(127)

for arg in sys.argv:
    if arg.startswith("--prefix="):
        PREFIX = arg[9:]
    if arg.startswith("--target-dir="):
        DIR = arg[13:]
    if arg.startswith("--qmake="):
        CUSTOM_QMAKE_COMMAND = arg[8:]
    if arg.startswith("--lrelease"):
        CUSTOM_LRELEASE_COMMAND = arg[10:]
    if arg=="--debug":
        QMAKE_ARGUMENTS += ["CONFIG+=debug"]
    if arg.startswith("--spec="):
        QMAKE_ARGUMENTS += ["-spec",arg[7:]]

if DIR is None:
    DIR = PREFIX + "/kumir"

QMAKE_ARGUMENTS += ["PREFIX="+PREFIX, "KUMIR_DIR="+DIR]

(QMAKE_COMMAND, QT_VERSION) = find_qmake(MINIMUM_QT_VERSION,
                                         CUSTOM_QMAKE_COMMAND)
if QMAKE_COMMAND is None:
    print "Cannot find Qt >= ", MINIMUM_QT_VERSION
    sys.exit(1)
else:
    print "Found Qt version", QT_VERSION, "using", QMAKE_COMMAND

(LRELEASE_COMMAND, QT_VERSION) = find_lrelease(MINIMUM_QT_VERSION,
                                               CUSTOM_LRELEASE_COMMAND)

if LRELEASE_COMMAND is None:
    print "Cannot find lrelease >= ", MINIMUM_QT_VERSION
    sys.exit(1)
else:
    print "Found lrelease version", QT_VERSION, "using", LRELEASE_COMMAND


print "Generating translations..."
if run_lrelease(LRELEASE_COMMAND, ".")>0:
    print "Error while generating translations"

    sys.exit(2)

p = subprocess.Popen([QMAKE_COMMAND]+QMAKE_ARGUMENTS+["all.pro"])
status = os.waitpid(p.pid, 0)[1]
exit(status)

 