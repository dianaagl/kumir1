import os
import os.path

def install_file(filename, target, execuable):
    if target.endswith("/"):
        target = target+filename
    dirname = os.path.normpath(target+"/../")
    if not os.path.exists(dirname):
        os.makedirs(dirname)
        
    print "Installing ", target
    f = open(filename,"rb")
    d = f.read()
    f.close()
    f = open(target,"wb")
    f.write(d)
    f.close()
    if execuable and os.name=="posix":
        os.chmod(target,0755)
    elif os.name=="posix":
        os.chmod(target,0666)

def install_dir(dirname, target):
    if not os.path.exists(os.path.join(target, dirname)):
        os.makedirs(os.path.join(target, dirname))
    entries = os.listdir(dirname)
    for entry in entries:        
        if entry!=".svn":
            if os.path.isdir(os.path.join(dirname,entry)):
                install_dir(os.path.join(dirname,entry),
                            os.path.join(target))
            else:
                install_file(os.path.join(dirname,entry),
                             os.path.join(target,dirname,entry),
                             False)

def create_shell_link(source, target):
    env = os.environ
    if env.has_key('RPM_BUILD_ROOT'):
        rpm_build_root = os.environ['RPM_BUILD_ROOT']
        if source.startswith(rpm_build_root):
            source = source[len(rpm_build_root):]
    if os.path.exists(target):
        os.unlink(target)
    dirname = os.path.normpath(target+"/../")
    if not os.path.exists(dirname):
        os.makedirs(dirname)
    f = open(target, "w")
    f.write("#!/bin/sh\n")
    f.write("exec "+source+" %@\n")
    f.close()
    os.chmod(target, 0755)
    
