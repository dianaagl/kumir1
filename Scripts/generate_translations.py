import os.path
import subprocess
import os

def run_lrelease(lrelease, dirname):
    for dirname, dirnames, filenames in os.walk(dirname):
        for filename in filenames:
            if filename.endswith(".ts"):
                full_path = os.path.join(dirname, filename)
                print "\t", full_path
                p = subprocess.Popen([lrelease, full_path])
                status = os.waitpid(p.pid, 0)[1]
                if status>0:
                    return status
    return 0
