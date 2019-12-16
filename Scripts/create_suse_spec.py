#!/usr/bin/python

import sys
import os
import os.path
import subprocess
import re


script_path = os.path.normpath(os.getcwd()+"/"+sys.argv[0])
script_basedir = os.path.dirname(script_path)

template_path = os.path.normpath(script_basedir+"/../RPM_Specs/template-kumir-all.spec")

template_file = open(template_path, "r")
template = template_file.read()
template_file.close()

out_file_name = None
out_file = sys.stdout

revision = None
packager = "dummy <dummy@example.com>"

for arg in sys.argv:
    if arg.startswith("--revision="):
        revision = arg[11:]
    if arg.startswith("--packager="):
        packager = arg[11:]
    if arg.startswith("--out="):
        out_file_name = arg[6:]
        out_file = open(out_file_name, "w")
    if arg=="--help":
        sys.stderr.write("""
USAGE:
    create_suse_spec.py [--revision=REV] [--packager=NAME] [--out=FILE]

    --revision=REV    Revision number, script tries to detect it by
                      calling 'svn info' if not set, then if fails,
                      spec generation is aborted.

    --packager=NAME   Packager name and e-mail. If contains spaces, must
                      be doublequoted. Default value is
                      "dummy <dummy@example.com>".

    --out=FILE        Set output file name. If not specified, the output
                      is to be written to standart output (e.g screen in
                      most cases).
""")
        sys.exit(127)
           

if revision is None:
    rev_expr = re.compile(r"Revision:\s(\d+)")
    svn_output = subprocess.Popen(["svn","info"], 
                                  cwd=script_basedir,
                                  stdout=subprocess.PIPE).communicate()[0]
    m = rev_expr.search(svn_output)
    revision = m.group(1).strip()


if revision is None:
    sys.stderr.write("No revision specified!\n")
    sys.exit(1)

requires = "libqt4 >= 4.6.0 libqt4-x11 >= 4.6.0 libQtWebKit4 >= 4.6.0 phonon"
build_requires = "python libqt4-devel >= 4.6.0 libQtWebKit-devel >= 4.6.0 phonon-devel -post-build-checks -rpmlint-Factory"
pre_req = "shared-mime-info"

data = template.replace("--REVISION--", revision). \
    replace("--PACKAGER--", packager). \
    replace("--REQUIRES--", requires). \
    replace("--BUILD_REQUIRES--", build_requires). \
    replace("--PRE_REQ--", pre_req)

out_file.write(data)

if not out_file_name is None:
    out_file.close()
