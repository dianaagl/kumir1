#coding=UTF-8

import sys
import configuration
import program
import traceback
import codecs
import base64
import os
import Tix
import tkFileDialog
import os.path
import tkCommonDialog


try:
    import pygtk
    pygtk.require('2.0')
    import gtk
    face = "gtk"
except:
    face = "tkinter"


class TkApplication(Tix.Tk):
    
    def browerTargetDir(self):
        dir = tkFileDialog.askdirectory()
        if dir!=None:
            self.TARGET_DIR.delete(0, 'end')
            self.TARGET_DIR.insert(0, dir)
            if os.name=="nt":
                configdir = os.path.expanduser("~\Application Data\NIISI RAS\Convertor")
            else:
                configdir = os.path.expanduser("~/.config/NIISI RAS/Convertor")
            os.makedirs(configdir)
            f = open(configdir+"/LastTargetDir","w")
            f.write(dir).strip()
            f.close()
          
          
    def convert(self):
        self.package.reset()
        result = self.package.process(self.tree, self.platform)
        out_dir = unicode(self.TARGET_DIR.get())
        out_dir = out_dir.strip()
        if out_dir=="":
            self.STATUS['fg'] = 'red'
            self.STATUS['text'] = u'Не указан целевой каталог'
            return
        if not os.path.exists(out_dir) or not os.path.isdir(out_dir):
            self.STATUS['fg'] = 'red'
            self.STATUS['text'] = u'Указанный каталог не существует'
            return
        out_enc = unicode(self.ENCODING_CHOOSER['value'])
        space_pos = out_enc.index(' ')
        out_enc = out_enc[0:space_pos].strip()
        for filename, unicode_data in result:
            raw_data = unicode_data.encode(out_enc)
            full_path = os.path.normpath(out_dir+os.path.sep+filename)
            f = open(full_path, 'w')
            f.write(raw_data)
            f.close()
        self.STATUS['fg'] = 'black'
        self.STATUS['text']= u'Конвертирование успешно завершено'
    
    def quit(self):
        sys.exit(0)
         
    def createWidgets(self):
        self.LBL_LANGUAGE = Tix.Label(master=self,text=u'Целевой язык')
        self.LBL_LANGUAGE.grid(column=0, row=0)
        self.LANGUAGE_CHOOSER = Tix.ComboBox(master=self)
        self.LANGUAGE_CHOOSER.grid(row=0,column=1,columnspan=2,sticky='ew')
        self.LANGUAGE_MATCH = list()
        self.LANGUAGE_MATCH.append(('c99','C (ISO/IEC 9899:1999)'))
        self.LANGUAGE_MATCH.append(('c_plus_plus','C++ (ISO/IEC 14882:1998)'))
        self.LANGUAGE_CHOOSER.insert(0, 'C (ISO/IEC 9899:1999)')
        self.LANGUAGE_CHOOSER.insert(1, 'C++ (ISO/IEC 14882:1998)')
        self.LBL_ENCODING = Tix.Label(master=self,text=u'Кодировка файлов')
        self.LBL_ENCODING.grid(column=0,row=1)
        self.ENCODING_CHOOSER = Tix.ComboBox(master=self)
        self.ENCODING_CHOOSER.insert(0, u'cp866 (Windows OEM)')
        self.ENCODING_CHOOSER.insert(1, u'cp1251 (Windows ANSI)')
        self.ENCODING_CHOOSER.insert(2, u'koi8-r (FreeBSD)')
        self.ENCODING_CHOOSER.insert(3, u'utf-8 (Linux)')
        self.ENCODING_CHOOSER.grid(column=1,columnspan=2,sticky='ew',row=1)
        if os.name=="nt":
            self.ENCODING_CHOOSER['value'] = u'cp866 (Windows OEM)'
        else:
            self.ENCODING_CHOOSER['value'] = u'utf-8 (Linux)'
        self.LBL_TARGET_DIR = Tix.Label(master=self,text=u'Целевой каталог')
        self.LBL_TARGET_DIR.grid(column=0,row=2)
        self.TARGET_DIR = Tix.Entry(master=self)
        self.TARGET_DIR.grid(column=1,row=2)

        self.BTN_BROWSE = Tix.Button(master=self,text=u'...',command=self.browerTargetDir)
        self.BTN_BROWSE.grid(column=2,row=2)
        
        self.STATUS = Tix.Label(master=self, text='', fg='red')
        self.STATUS.grid(row=3,columnspan=3,sticky='ew')

        self.BOX = Tix.ButtonBox(master=self)
        self.BOX.grid(row=4,columnspan=3,sticky='ew')
        
        self.BTN_GO = Tix.Button(master=self.BOX,text=u'Конвертировать',bg="green",command=self.convert)
        self.BTN_GO.grid(column=0,row=0,sticky='ew')

        self.BTN_CLOSE = Tix.Button(master=self.BOX,text=u'Закрыть',bg="red",command=self.quit)
        self.BTN_CLOSE.grid(column=1,row=0,sticky='ew')


    def loadArguments(self):
        has_source = False
        self.target_lanuage = None
        for arg in sys.argv:
            if arg=="-i":
                has_source = True
            elif arg.startswith("-lang="):
                self.target_language = arg[6:]
                self.setTargetLanguage(self.target_language)
        if not has_source:
            fileTypes = [(u'XML-сериализованная программа','*.kum.xml')]
            file = tkFileDialog.askopenfile(parent=self,
                                            filetypes=fileTypes,
                                            title=u'Укажите файл с данными')
            if file!=None :
                self.data = file.read()
                file.close()
        else:
            has_more_data = True
            self.data = ''
            while has_more_data:
                buff = sys.stdin.read(1024)
                has_more_data = len(buff)>0
                self.data += buff
                has_more_data = len(buff)>0
        self.tree = program.parse(self.data)
            
    def setTargetLanguage(self, lang):
        self.target_language = lang;
        __import__(lang)
        self.package = sys.modules[lang]
        print self.package
        for pkg, title in self.LANGUAGE_MATCH:
            if pkg==lang:
                self.LANGUAGE_CHOOSER['value'] = title
                
    def __init__(self):
        Tix.Tk.__init__(self)
        self.createWidgets()
        self.title(u"Конвертор")
        self.geometry('300x150+50+50')
        if os.name=="posix":
            oss = "linux"
        elif os.name=="nt":
            oss = "windows"
        arch = "i386"
        self.platform = (arch,oss)
        self.loadArguments()
        if os.name=="nt":
            configdir = os.path.expanduser("~\Application Data\NIISI RAS\Convertor")
        else:
            configdir = os.path.expanduser("~/.config/NIISI RAS/Convertor")
        if os.path.exists(configdir+"/LastTargetDir"):
            f = open(configdir+"/LastTargetDir","r")
            lastDir = f.read().strip()
            self.TARGET_DIR.delete(0, 'end')
            self.TARGET_DIR.insert(0, lastDir)
            f.close()


class GtkApplication:

    def destroy(self, widget, data=None):        
        gtk.main_quit()
        
    def delete_event(self, widget, event, data=None):
        if os.name=="nt":
            configdir = os.path.expanduser("~\Application Data\NIISI RAS\Convertor")
        else:
            configdir = os.path.expanduser("~/.config/NIISI RAS/Convertor")
        if not os.path.exists(configdir):
            os.makedirs(configdir)
        f = open(configdir+"/LastWindowPosition","w")
        x, y = self.window.get_position()
        f.write(str(x)+"\n"+str(y))
        f.close()
        return False
        
    def browse_target(self,widget):
        selector = gtk.FileChooserDialog(title=u"Выберете целевой каталог",
                                         action=gtk.FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                         buttons=(gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                                  gtk.STOCK_OK, gtk.RESPONSE_OK))
        selector.set_current_folder(self.target.get_text())
        response = selector.run()
        if response==gtk.RESPONSE_OK:
            dir = selector.get_filename()
            self.target.set_text(dir)
            if os.name=="nt":
                configdir = os.path.expanduser("~\Application Data\NIISI RAS\Convertor")
            else:
                configdir = os.path.expanduser("~/.config/NIISI RAS/Convertor")
            os.makedirs(configdir)
            f = open(configdir+"/LastTargetDir","w")
            f.write(dir).strip()
            f.close()
        selector.destroy()
        
    def convert(self,widget):
        self.package.reset()
        result = self.package.process(self.tree, self.platform)
        out_dir = unicode(self.target.get_text())
        out_dir = out_dir.strip()
        if out_dir=="":
            self.status.set_text(u'Не указан целевой каталог')
            return
        if not os.path.exists(out_dir) or not os.path.isdir(out_dir):
            self.status.set_text(u'Указанный каталог не существует')
            return
        enc_model = self.encoding_chooser.get_model()
        actice_enc = self.encoding_chooser.get_active()
        out_enc = unicode(enc_model[actice_enc][0])
        space_pos = out_enc.index(' ')
        out_enc = out_enc[0:space_pos].strip()
        for filename, unicode_data in result:
            raw_data = unicode_data.encode(out_enc)
            full_path = os.path.normpath(out_dir+os.path.sep+filename)
            f = open(full_path, 'w')
            f.write(raw_data)
            f.close()
        self.status.set_text(u'Конвертирование успешно завершено')
        
    def handle_language_choosed(self, entry):
        model = entry.get_model()
        active = entry.get_active()
        title = model[active][0]
        for pkg, t in self.language_match:
            if t==title:
                self.setTargetLanguage(pkg)
        
        
    def createWidgets(self): 
        lang_box = gtk.HBox()
        self.lbl_language = gtk.Label(u"Целевой язык")
        lang_box.add(self.lbl_language)
        self.language_chooser = gtk.combo_box_new_text()
        self.language_chooser.connect('changed', self.handle_language_choosed)
        lang_box.add(self.language_chooser)
        for module, title in self.language_match:
            self.language_chooser.append_text(title)
        self.vbox.add(lang_box)
        enc_box = gtk.HBox()
        self.lbl_encoding = gtk.Label(u"Кодировка")
        self.encoding_chooser = gtk.combo_box_new_text()
        encodings = ["cp866 (Windows OEM)", 
                     "cp1251 (Windows ANSI)",
                     "koi8-r (FreeBSD)",
                     "utf-8 (Linux)" ]
        for enc in encodings:
            self.encoding_chooser.append_text(enc)
        if os.name=="nt":
            self.encoding_chooser.set_active(0)
        else:
            self.encoding_chooser.set_active(3)
        enc_box.add(self.lbl_encoding)
        enc_box.add(self.encoding_chooser)
        self.vbox.add(enc_box)
        target_box = gtk.HBox()
        self.lbl_target = gtk.Label(u"Целевой каталог")
        self.target = gtk.Entry()
        self.btn_browse_target = gtk.Button("...")
        self.btn_browse_target.connect("pressed", self.browse_target)
        target_box.add(self.lbl_target)
        target_box.add(self.target)
        target_box.add(self.btn_browse_target)
        self.vbox.add(target_box)
        self.status = gtk.Label();
        self.vbox.add(self.status)
        button_box = gtk.HBox()
        self.btn_convert = gtk.Button(u"Конвертировать")
        self.btn_convert.connect("pressed", self.convert)
        button_box.add(self.btn_convert)
        self.btn_close = gtk.Button(u"Закрыть")
        self.btn_close.connect("pressed", self.destroy)
        button_box.add(self.btn_close)
        self.vbox.add(button_box)
        
        

    def setTargetLanguage(self, lang):
        self.target_language = lang;
        __import__(lang)
        self.package = sys.modules[lang]
        print self.package
        i = 0
        for pkg, title in self.language_match:
            if pkg==lang:
                self.language_chooser.set_active(i)
            i += 1
    
    def loadArguments(self):
        has_source = False
        self.target_lanuage = None
        for arg in sys.argv:
            if arg=="-i":
                has_source = True
            elif arg.startswith("-lang="):
                self.target_language = arg[6:]
                self.setTargetLanguage(self.target_language)
        if not has_source:
            selector = gtk.FileChooserDialog(title=u"Укажите файл с данными",
                                             action=gtk.FILE_CHOOSER_ACTION_OPEN,
                                             buttons=(gtk.STOCK_CANCEL, 
                                                      gtk.RESPONSE_CANCEL,
                                                      gtk.STOCK_OK, 
                                                      gtk.RESPONSE_OK))
            filter = gtk.FileFilter()
            filter.set_name(u"XML-сериализованная программа")
            filter.add_pattern("*.kum.xml")
            selector.set_filter(filter)
            response = selector.run()
            if response==gtk.RESPONSE_OK:
                filename = selector.get_filename()
                f = open(filename,"r")
                self.data = f.read()
                f.close()
            selector.destroy()
        else:
            has_more_data = True
            self.data = ''
            while has_more_data:
                buff = sys.stdin.read(1024)
                has_more_data = len(buff)>0
                self.data += buff
                has_more_data = len(buff)>0
        self.tree = program.parse(self.data)
    
        
    def __init__(self):
        if os.name=="posix":
            oss = "linux"
        elif os.name=="nt":
            oss = "windows"
        arch = "i386"
        self.platform = (arch,oss)
        self.language_match = list()
        self.language_match.append(('c99','C (ISO/IEC 9899:1999)'))
        self.language_match.append(('c_plus_plus','C++ (ISO/IEC 14882:1998)'))
        self.window = gtk.Window(gtk.WINDOW_TOPLEVEL)
        self.window.connect("destroy", self.destroy)
        self.window.connect("delete_event", self.delete_event)
        self.vbox = gtk.VBox()
        self.window.add(self.vbox)
        self.createWidgets()
        self.window.set_title(u"Конвертор")
        if os.name=="nt":
            configdir = os.path.expanduser("~\Application Data\NIISI RAS\Convertor")
        else:
            configdir = os.path.expanduser("~/.config/NIISI RAS/Convertor")
        if os.path.exists(configdir+"/LastTargetDir"):
            f = open(configdir+"/LastTargetDir","r")
            lastDir = f.read().strip()
            self.target.set_text(lastDir)
            f.close()
        if os.path.exists(configdir+"/LastWindowPosition"):
            f = open(configdir+"/LastWindowPosition","r")
            try:
                x = int(f.readline().strip())
                y = int(f.readline().strip())                
                self.window.move(x,y)
            except:
                self.window.set_position(gtk.WIN_POS_CENTER_ON_PARENT)
            f.close()
        
    
        
    def main(self):
        self.window.show_all()
        self.loadArguments()
        gtk.main()

if __name__ == '__main__':
    print "Starting convertor..."
    if face=="tkinter":
        app = TkApplication()
        app.mainloop()
    elif face=="gtk":
        app = GtkApplication()
        app.main()
    print "Exiting convertor..."

