import subprocess
import re

VARIANTS = ["lrelease", "lrelease-qt4"]

def find_lrelease(minimum_version, custom_command):
    variants = [custom_command] + VARIANTS
    p = re.compile(r'version\s(.*)')
    for variant in variants:
        try:
            out = subprocess.Popen([variant,"-version"],
                                         stdout=subprocess.PIPE).communicate()[0]
        except:
            out = ""
        m = p.search(out)
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
