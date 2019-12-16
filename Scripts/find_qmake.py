import subprocess
import re

QMAKE_VARIANTS = ["qmake", "qmake-qt4"]

def find_qmake(minimum_version, custom_command):
    variants = [custom_command] + QMAKE_VARIANTS
    p = re.compile(r'Qt\sversion\s(.*)\sin')
    for variant in variants:
        try:
            qmake_out = subprocess.Popen([variant,"--version"],
                                         stdout=subprocess.PIPE).communicate()[0]
        except:
            qmake_out = ""
        m = p.search(qmake_out)
        if not m is None:
            try:
                qt_version = m.group(1).strip()
            except:
                qt_version = ""
            if qt_version!="":
                version_match = qt_version>=minimum_version
                if version_match:
                    return (variant, qt_version)                    
    return (None, None)
