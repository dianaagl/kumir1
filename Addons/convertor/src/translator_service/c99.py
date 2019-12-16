#coding=UTF-8

"Модуль конвертирования программы в ISO C99"

GENERATE_MAKEFILE = True
INDENT_STRING = "  "

import program
import util

import configuration 
from config_c_plus_plus import *

"Список зарезервированных слов, которые не могут встречаться в именах"
__reserved_words = ["void","int","uint","long","unsigned",
                    "float","double","char",
                    "_Bool","for","if","while",
                    "do","case","switch","namespace",
                    "main","struct",
                    "union","enum","typedef",
                    "static","global","virtual",
                    "mutable","const",
                    "export","operator","explicit",
                    "sizeof","kumir"]

"Список подключаемых библиотек (для генерации Makefile'ов)"
__libraries_to_link = { "math.h" : "-lm", "kumir" : "-lkumirrt" }
                       

"Имена временных переменных цикла (по аналогии с языком Фортран)"
__temporary_variables = ["i","j","k","l","m","p","q","a","b","c"]

"""Таблица преобразования встроенных функций Кумир ->C++.
Имеет формат: ИСХОДНОЕ_ИМЯ -> ( ШАБЛОН, ( СПИСОК_INCLUDE, СТРОКИ_ИНИЦИАЛИЗАЦИИ ) )"""
__builtin_replacement = { u"div" : ("( ($1)/($2) )",None), # div($1,$2) -> $1 / $2
                         u"mod" : ("( ($1)%($2) )",None), # mod($1,$2) -> $1 % $2
                         u"юникод" : (None,None), # NOT APPLICABLE
                         u"код" : ("(int)($1)",None), # код($1) -> (int)($1)
                         u"символ" : ("(char)($1)",None), # символ($1) -> (char)($1)
                         u"символ2" : (None,None), # NOT APPLICABLE
                         u"вещ_в_лит" : ("kum_string_of_double($1)",(["kumir.h"],None)),
                         u"цел_в_лит" : ("kum_string_of_integer($1)", (["kumir.h"],None)),
                         u"sqrt" : ("sqrt($1)",(["math.h"],None)),
                         u"abs" : ("fabs($1)",(["math.h"],None)),
                         u"iabs" : ("abs($1)",(["stdlib.h"],None)),
                         u"sign" : ("kumir::sign($1)",(["kumir.h"],None)),
                         u"min" : ("fmin($1,$2)",(["math.h"],None)),
                         u"max" : ("fmax($1,$2)",(["math.h"],None)),
                         u"rnd" : ("($1*(double)(rand())/(double)(RAND_MAX))",(["stdlib.h","ctime"],"srand(time(NULL));\n")),
                         u"sin" : ("sin($1)", (["math.h"],None)),
                         u"cos" : ("cos($1)", (["math.h"],None)),
                         u"tg" : ("tan($1)", (["math.h"],None)),
                         u"ctg" : ("(1.0/tan($1))", (["math.h"],None)),
                         u"arcsin" : ("asin($1)", (["math.h"],None)),
                         u"arccos" : ("acos($1)", (["math.h"],None)),
                         u"arctg" : ("atan($1)", (["math.h"],None)),
                         u"arcctg" : ("atan(1.0/$1)", (["math.h"],None)),
                         u"ln" : ("log($1)", (["math.h"],None)),
                         u"lg" : ("log10($1)", (["math.h"],None)),
                         u"exp" : ("exp($1)", (["math.h"],None)),
                         u"int" : ("(int)(floor($1))", (["math.h"],None)),
                         u"длин" : ("strlen($1)", (["string.h"],None)),
                         u"время" : ("kum_time()", (["kumir.h"],None)),
                         u"лит_в_цел" : ("kum_integer_of_string($1,&$2)", (["kumir.h"],None)),
                         u"лит_в_вещ" : ("kum_double_of_string($1,&$2)", (["kumir.h"],None)),
                         u"пауза" : ("kum_pause()", (["kumir.h"],None)),
                         u"sin" : ("sin($1)", (["math.h"],None)),
                         u"sin" : ("sin($1)", (["math.h"],None)),
                         u"sin" : ("sin($1)", (["math.h"],None)),
                         u"стоп" : ("exit(0)", (["stdlib.h"],None)),
                         u"sin" : ("sin($1)", (["math.h"],None)), }

                       
__used_names = dict()
__used_header_includes = dict()
__used_source_includes = dict()
__main = ""
__program = None
__entry_point = None
__initialized_strings = set() 

def reset():
    global __used_names
    __used_names = dict()
    global __used_header_includes
    __used_header_includes = dict()
    global __used_source_includes
    __used_source_includes = dict()
    global __program
    __program = None
    global __main
    __main = ""
    global __entry_point
    __entry_point = None
    global __initialized_strings
    __initialized_strings = set()

def __make_name(name,capitalize_first):
    new_name = ""
    capitalize_next = capitalize_first
    for c in name:
        if c!="_":
            if capitalize_next:
                new_name += c.upper()
                capitalize_next = False
            else:
                new_name += c
        else:
            capitalize_next = True
            new_name += c
    return new_name


def __get_name(namespace,name,capitalize_first):
    latin_name = util.transliterate(name)
    suggest = __make_name(latin_name,capitalize_first)
    if __used_names.has_key(namespace):
        ns = __used_names[namespace]
    else:
        ns = set()
        __used_names[namespace] = ns
    suffix = ""
    counter = 0
    while suggest+suffix in ns:
        counter += 1
        suffix = "_"+unicode(counter)
    ns.add(suggest+suffix)
    return suggest+suffix

def __make_temporary_variable(namespace):
    if __used_names.has_key(namespace):
        ns = __used_names[namespace]
    else:
        ns = set()
        __used_names[namespace] = ns
    suffix = ""
    counter = 1
    found = False
    result = None
    global __temporary_variables
    for i in __temporary_variables:
        if not i in ns:
            ns.add(i)
            found = True
            result = unicode(i)
            break
    if not found:
        while "i_"+unicode(counter) in  ns:
            counter = counter + 1
        ns.add("i_"+unicode(counter))
        result = "i_"+unicode(counter)
    return result
    
    

import string
import resources.kumir_c
import resources.kumir_h

def process(program_tree, client_platform):
    global __program
    use_kumir = False
    __program = program_tree
    files = []
    main_cpp = ""
    main_initializer = ""
    for module in program_tree.modules:
        module.cleanup = ""
        __process_names_and_declarations(module)
    main_finalizer = ""
    for module in program_tree.modules:
        algorhitm_bodies = []
        if module.name!="":
            init_body = ("void "+module.translated_name+"::"+module.translated_name+"()"),__get_body(module,None,module.initializer_instructions) 
            algorhitm_bodies.append(init_body)
        else:
            main_initializer = __get_body(module,None,module.initializer_instructions)
            for var in module.global_variables:
                main_finalizer += var.free
            
        for alg in module.algorhitms:
            alg_body = (alg.declaration_cpp,__generate_algorhitm_implementation(module,alg))
            
            algorhitm_bodies.append(alg_body)
            
        cpp = ""
        for mod in program_tree.modules:
            if mod.translated_name!="":
                cpp += "#include \""+mod.translated_name.lower()+".h\"\n"
            else:
                cpp += "#include \"main.h\"\n"
        cpp += "\n"
        global __used_source_includes
        using_namespace_std = False
        if __used_source_includes.has_key(module.translated_name):
            sis = __used_source_includes[module.translated_name]
            for inc in sis:
                cpp += "#include <"+inc+">\n"
            cpp += "\n"

        if module.translated_name=="":
            for var in module.global_variables:
                cpp += var.kum
                cpp += var.declaration+";\n"
                cpp += var.extra;
            cpp += "\n"
            
        for alg in algorhitm_bodies:
            
            (decl, body) = alg
            cpp += decl + "\n"
            if APPEND_KUMIR_CODE:
                cpp += u"/* нач */\n"
            cpp += "{\n"
            cpp += body
            if APPEND_KUMIR_CODE:
                cpp += u"/* кон */\n"
            cpp += "}\n\n"
        
        if module.translated_name!="":
            fcpp = (module.translated_name.lower()+".c", format_code(cpp))
            files.append(fcpp)
        else:
            main_cpp = cpp
        
        if module.translated_name!="":
            NAME_H = "_"+module.translated_name.upper()+"_H"
            file_base_name = module.translated_name.lower()+".h"
        else:
            NAME_H = "_MAIN_H"
            file_base_name = "main.h"
        h = "#ifndef "+NAME_H+"\n#define "+NAME_H+"\n\n"
        global __used_header_includes
        
        if __used_header_includes.has_key(module.translated_name):
            his = __used_header_includes[module.translated_name]
            for inc in his:
                h += "#include <"+inc+">\n"
                
            h += "\n"
        
        if module.translated_name!="":
            if APPEND_KUMIR_CODE:
                h += u"/* исп "+module.name+" */\n"
            h += "namespace "+module.translated_name+" {\n";
#        for var in module.global_variables:
#            h += "extern "+var.declaration+";\n"
#        h += "\n"
        if module.translated_name!="":
            h += "void "+module.translated_name+"();\n"
        for alg in module.algorhitms:
            h += alg.declaration_h+";\n"
        if module.translated_name!="":
            if APPEND_KUMIR_CODE:
                h += u"/* кон исп */\n"
            h += "\n}\n"
        else:
            h += "\n"
        h += "#endif\n"
        fh = (file_base_name, format_code(h))
        files.append(fh)
    
    main = "void main()\n{\n"
    global __main
    if __main!="":
        main += __main + "\n"
    
    init_modules = ""
    
    for module in program_tree.modules:
        if module.translated_name!="":
            init_modules += module.translated_name+"::"+module.translated_name+"();\n"
    
    if init_modules!="":
        main += init_modules
    
    if main_initializer!="":
        main += main_initializer + "\n"
    
    global __entry_point
    if not __entry_point is None:
        main += __entry_point.translated_name+"();\n"
    main += main_finalizer
    if __used_source_includes.has_key(""):
        usi = __used_source_includes[""]
        if "kumir.h" in usi:
            main += "kum_cleanup();\n"
    main += "}\n"
    main_cpp += "\n"+main
    fmain = ("main.c", format_code(main_cpp))
    files.append(fmain)
    
    if not client_platform is None:
        (arch,os) = client_platform
    else:
        arch = None
        os = None
    
    makefile = __generate_makefile(program_tree,files,os)
    
    fmake = ("Makefile", makefile)
    
    files.append(fmake)
    
    for m in __used_source_includes.values():
        if "kumir.h" in m:
            use_kumir = True
    
    if use_kumir:
        f_kumir_cpp = ("kumir.c", resources.kumir_c.D)
        f_kumir_h = ("kumir.h", resources.kumir_h.D)
        files.append(f_kumir_cpp)
        files.append(f_kumir_h)

    return files
    
def __line_rank(s):
    start = 0
    end = 0
    in_lit = False
    in_char = False
    in_comment = False
    in_small_comment = False
    processing_start = True
    p = None
    for c in s:
        if in_lit:
            if not p is None and p!="\\" and c=="\"":
                in_lit = False
        elif in_char:
            if not p is None and p!="\\" and c=="'":
                in_char = False
        elif in_comment:
            if not p is None and p=="*" and c=="/":
                in_comment = False
        elif in_small_comment:
            if c=="\n":
                in_small_comment = False
        else:
            if c=="\"":
                in_lit = True
            elif c=="'":
                in_char = True
            elif c=="*" and p=="/":
                in_comment = True
            elif c=="{":
                processing_start = False
                end += 1
            elif c=="}":
                if processing_start:
                    start -= 1
                else:
                    end -= 1
            elif not p is None and p=="/" and c=="/":
                in_small_comment = True
        p = c
    return start, end
    
def format_code(s):
    level = 0
    result = ""
    lines = s.split("\n")
    for line in lines:
        start, end = __line_rank(line)
        level += start
        cur_ind = level*INDENT_STRING
        l = cur_ind + line + "\n"
        result += l
        level += end
    return result
        

def __process_names_and_declarations(module):
    if module.name!="":
        module.translated_name = __get_name("",module.name,True)
    else:
        module.translated_name = ""
    # Подготовка имен глобальных переменных и алгоритмов исполнителя
    for var in module.global_variables:
        var.translated_name = __get_name(module.translated_name,var.name,False)
    i = 0
    for alg in module.algorhitms:
        if module.name=="" and i==0:
            global __entry_point
            __entry_point = alg 
        alg.translated_name = __get_name(module.translated_name,alg.name,False)
        for var in alg.arguments:
            var.translated_name = __get_name(module.translated_name+"::"+alg.translated_name,var.name,False)
        for var in alg.local_variables:
            var.translated_name = __get_name(module.translated_name+"::"+alg.translated_name,var.name,False)
        i = i+1
            
    # Создание C++-заголовков глобальных переменных и алгоритмов
    uses = set()
    global __program
    for var in module.global_variables:
        var.declaration, this_uses = __generate_variable_declaration(var, __program, module, None)
        uses |= this_uses
        
    for alg in module.algorhitms:
        alg.declaration_h, alg.declaration_cpp, this_uses = __generate_algorhitm_declaration(module, alg)
        uses |= this_uses
        
    global __used_header_includes
    if __used_header_includes.has_key(module.translated_name):
        his = __used_header_includes[module.translated_name]
    else:
        his = set()
        __used_header_includes[module.translated_name] = his
    his |= uses
                
def __generate_variable_declaration(var, P, M, A):
    uses = set()
    type = ""
    kum = ""
    is_global = A is None
    is_array = var.type.endswith("[]")
    is_argument = var.is_argument
    is_reference = var.is_argument and var.is_output_argument
    base_type = var.type.replace("string","char*").replace("bool","_Bool").replace("[","").replace("]","")
    kum_type = var.type.replace("string",u"лит").replace("char",u"сим").replace("double",u"вещ").replace("int",u"цел").replace("bool",u"лог").replace("[]",u"таб")
    name = var.translated_name
    if is_reference and not is_array:
        decl = "*"+name
    else:
        decl = name
    if is_argument and not is_reference:
        mod = "const "
    else:
        mod = ""
    
    extra = ""
    init = ""
    free = ""
    
    if is_global and is_array:
        decl = "*"+decl;
        sizes = []
        for i in range(0,var.dimension):
            l = __get_expression_body(M,A,var.bounds[i][0])
            r = __get_expression_body(M,A,var.bounds[i][1])
            extra += "int _"+var.translated_name+"_left_bound_"+str(i+1)+";\n"
            extra += "int _"+var.translated_name+"_size_"+str(i+1)+";\n"
            init += "_"+var.translated_name+"_left_bound_"+str(i+1)+" = "+l+";\n"
            init += "_"+var.translated_name+"_size_"+str(i+1)+" = "+r+"-"+l+"+1;\n"
            sizes.append("_"+var.translated_name+"_size_"+str(i+1))
        init += var.translated_name+" = ("+base_type+"*) calloc( "+string.join(sizes, "*")+", sizeof("+base_type+") );\n"
        free = "free("+var.translated_name+");\n"
    elif is_array:
        for i in range(0,var.dimension):
            l = __get_expression_body(M,A,var.bounds[i][0])
            r = __get_expression_body(M,A,var.bounds[i][1])
            decl += "["+r+"-"+l+"+1]"
            extra += "int _"+var.translated_name+"_left_bound_"+str(i+1)+" = "+l+";\n"
            extra += "int _"+var.translated_name+"_size_"+str(i+1)+" = "+r+"-"+l+"+1;\n"
    
    kum = kum_type+" "+var.name;
    var.kum = "/* "+kum+" */\n"
    var.extra = extra
    var.initialization = init
    var.free = free        
    var.is_global = is_global
    result = mod+base_type+" "+decl 
    return result, uses
      
      
#def __generate_variable_declaration(var, P, M, A):
#    uses = set()
#    type = ""
#    kum = ""
#    if var.type.endswith("[]"):
#        base_type = var.type[0:-2]
#        if base_type=="string":
#            base_type = "char*"
#        if base_type=="bool":
#            base_type="_Bool"
##        if var.is_argument:
#        if False:
#            type = base_type
#        else:
#            if var.dimension==1:
#                type += base_type+" *"
#            elif var.dimension==2:
#                type += base_type+" **"
#            elif var.dimension==3:
#                type += base_type+" ***"
#    else:
#        type += var.type
#        if type=="string":
#            type = "char*"
#        if type=="bool":
#            type = "_Bool"
#    
#    if var.type=="string":
#        kum = u'лит'
#    elif var.type=="string[]":
#        kum = u'литтаб'
#    elif var.type=="char":
#        kum = u'сим'
#    elif var.type=="char[]":
#        kum = u'симтаб'
#    elif var.type=="bool":
#        kum = u'лог'
#    elif var.type=="bool[]":
#        kum = u'логтаб'
#    elif var.type=="int":
#        kum = u'цел'
#    elif var.type=="int[]":
#        kum = u'целтаб'
#    elif var.type=="double":
#        kum = u'вещ'
#    elif var.type=="double[]":
#        kum = u'вещтаб'
#    kum += ' '+var.name
#    kum = "/* "+kum+" */\n"
#    declaration_name = var.translated_name
#    
#    if var.is_argument:
##        if var.is_output_argument and not var.type.endswith("[]") and var.type!="string":
#        if var.is_output_argument:
#            declaration_name = "*"+var.translated_name
#    
#    if var.is_argument and var.is_input_argument and not var.is_output_argument:
#        type = "const "+type
#    if False:#var.is_argument:
#        result = type + " " + declaration_name
#        for i in range(0,var.dimension):
#            l = __get_expression_body(M,A,var.bounds[i][0])
#            r = __get_expression_body(M,A,var.bounds[i][1])
#            result += "["+r+"-"+l+"+1]"
#            A.prepend += "int _"+var.translated_name+"_left_bound_"+str(i+1)+" = "+l+";\n"
#    else:
#        result = type
#        if not var.type.endswith("[]"):
#            result += " "
#        result += declaration_name
#        if var.dimension>0:
#            extra = ""
#            sizes = []
#            for i in range(0,var.dimension):
#                l = __get_expression_body(M,A,var.bounds[i][0])
#                r = __get_expression_body(M,A,var.bounds[i][1])
#                extra += "int _"+var.translated_name+"_left_bound_"+str(i+1)+" = "+l+";\n"
#                size = r+"-_"+var.translated_name+"_left_bound_+"+str(i+1)+"+1"
#                sizes.append(size)
#            if var.is_argument:
#                A.prepend += extra
#            else:
#                calloc = "("+type+")kum_calloc_"+str(var.dimension)+"( "+string.join(sizes, ", ")+", sizeof("+base_type+") );\n"
#                result = extra +result
#            
#        
#    
#    if APPEND_KUMIR_CODE and not var.is_argument:
#        result = kum+result 
#    return result, uses
#            
def __generate_algorhitm_declaration(module, alg):
    uses = set()
    ret_type = alg.return_type;
    if ret_type=="bool":
        ret_type = "_Bool"
    if ret_type=="string":
        ret_type = "char*"
    arg_decls = []
    global __program
    alg.prepend = ""
    for arg in alg.arguments:
        arg_decl, arg_uses = __generate_variable_declaration(arg,__program,module,alg)
        arg_decls.append(arg_decl)
        uses |= arg_uses
    alg_name = alg.translated_name
    mod_name = module.translated_name
    if mod_name=="":
        ns = ""
    else:
        ns = mod_name+"::"
    kum_declaration = "/* "+alg.source_string.split("\n")[0]+" */\n"
    h = ret_type+" "+alg_name+"("+string.join(arg_decls, ", ")+")"
    cpp = ret_type+" "+ns+alg_name+"("+string.join(arg_decls, ", ")+")"
    if APPEND_KUMIR_CODE:
        h = kum_declaration + h
        cpp = kum_declaration + cpp
#    if __used_header_includes.has_key(module.translated_name):
#        ns = __used_header_includes[module.translated_name]
#    else:
#        ns = set()
#        __used_header_includes[module.translated_name] = ns
#    ns |= uses
    return h, cpp, uses 

def __generate_algorhitm_implementation(module, alg):
    result = ""
    result += alg.prepend;
    if alg.return_type=="bool":
        result += "_Bool _result;\n"
    elif alg.return_type=="string":
        result += "char* _result;\n"
    elif alg.return_type!="void":
        result += alg.return_type + " _result;\n"
    
    for a in alg.arguments:
        result += a.extra;
    
    for a in alg.input_assertions:
#        if APPEND_KUMIR_CODE:
#            result += u"/* дано "+unicode(a)+" */\n"
        result += __get_assert(module, alg, a, u"дано ")
    result += __get_body(module, alg, alg.instructions)
    for a in alg.output_assertions:
#        if APPEND_KUMIR_CODE:
#            result += u"/* надо "+unicode(a)+" */\n"
        result += __get_assert(module, alg, a, u"надо ")
    if alg.return_type!="void":
        result += "return _result;\n"
    return result
    
def __get_body(module, algorhitm, instructions):
    body = ""
    for ins in instructions:
        assert ins!=None
        c = unicode(ins.__class__)
        clazz = c.split(".")[-1]
        m = globals()["__get_"+clazz.lower()+"_body"]
        
        body += m(module, algorhitm, ins)
    return body


def __get_assertion_body(M,A,I):
    result = ""
    if APPEND_KUMIR_CODE:
        result = u"/* утв "+str(I)+" */\n"
    expr = __get_expression_body(M,A,I.condition)
    result += "assert("+expr+");\n"
    global __used_source_includes
    if __used_source_includes.has_key(M.translated_name):
        sis = __used_source_includes[M.translated_name]
    else:
        sis = set()
        __used_source_includes[M.translated_name] = sis
    sis |= set(["assert.h"])
    return result

def __get_break_body(module, algorhitm, instructions):
    result = ""
    if APPEND_KUMIR_CODE:
        result = u"/* выход */\n"
    result += "break;\n"
    return result

def __get_exit_body(module, algorhitm, instructions):
    result = ""
    if APPEND_KUMIR_CODE:
        result = u"/* выход */\n"
    result += "return"
    if algorhitm.return_type!="void":
        result += " _result"
    result += ";\n"
    return result

def __get_variable_by_id(scope, id):
    result = None
    for var in scope:
        if var.id == id:
            result = var
            break
    assert result!=None
    return result

def __get_variable_declaration_body(module, algorhitm, instruction):
    body = ""
    vars = instruction.variables
    is_argument = False
    global __used_source_includes
    global __program
    if __used_source_includes.has_key(module.translated_name):
        ns = __used_source_includes[module.translated_name]
    else:
        ns = set()
        __used_source_includes[module.translated_name] = ns
    for (id, bounds) in vars:
        if algorhitm is None:
            var = __get_variable_by_id(module.global_variables,id)
            (code, uses) = __generate_variable_declaration(var, __program, module, algorhitm)
#            body += code+";\n"
#            body += var.extra;
            body += var.initialization
            ns |= set(["stdlib.h"])
        else:
            var = __get_variable_by_id(algorhitm.local_variables,id)
            (code, uses) = __generate_variable_declaration(var, __program, module, algorhitm)
            body += code+";\n"
            body += var.extra;
        
                   
    if APPEND_KUMIR_CODE and body!="" and not is_argument:
        return "/* "+unicode(instruction)+" */\n"+body
    else:
        return body



#def __get_variable_declaration_body(module, algorhitm, instruction):
#    body = ""
#    vars = instruction.variables
#    is_argument = False
#    global __used_source_includes
#    if __used_source_includes.has_key(module.translated_name):
#        ns = __used_source_includes[module.translated_name]
#    else:
#        ns = set()
#        __used_source_includes[module.translated_name] = ns
#    for (id, bounds) in vars:
#        if algorhitm is None:
#            # Глобальная переменная, описываем только 
#            # границы массива, т.к. переменная уже объявлена
#            var = __get_variable_by_id(module.global_variables,id)
#            if var.type.endswith("[]"):
#                n = var.translated_name
#                base_type = unicode(var.type)[0:-2]
#                base_type = base_type.replace("string","char")
#                base_type = base_type.replace("bool","_Bool")
#                var.bound_shifts = []
#                cleanup = "free( "+n+" );\n"
#                module.cleanup += cleanup              
#                bounds = var.bounds
#                if var.dimension==1:
#                    [left, right] = bounds[0]
#                    left_decl = "_"+var.translated_name+"_left_bound_1";
#                    var.bound_shifts.append(left_decl)
#                    body += left_decl + " = "
#                    body += __get_expression_body(module,algorhitm,left) + ";\n"
#                    size = __get_expression_body(module,algorhitm,right)
#                    body += n + " = ("+base_type+"*) kum_calloc_1( "+size+"-"+left_decl+"+1 , sizeof("+base_type+") );\n"
#                    ns |= set(["kumir.h"])
#                elif var.dimension==2:
#                    [left1, right1] = bounds[0]
#                    [left2, right2] = bounds[1]
#                    left1_decl = "_"+var.translated_name+"_left_bound_1";
#                    left2_decl = "_"+var.translated_name+"_left_bound_2";
#                    var.bound_shifts.append(left1_decl)
#                    var.bound_shifts.append(left2_decl)
#                    body += left1_decl + " = "
#                    body += __get_expression_body(module,algorhitm,left1) + ";\n"
#                    body += left2_decl + " = "
#                    body += __get_expression_body(module,algorhitm,left2) + ";\n"
#                    size1 = __get_expression_body(module,algorhitm,right1)
#                    size2 = __get_expression_body(module,algorhitm,right2)
#                    body += n + " = ("+base_type+"**) kum_calloc_2( "+size1+"-"+left1_decl+"+1, "+size2+"-"+left2_decl+"+1, sizeof("+base_type+") );\n"
#                    ns |= set(["kumir.h"])
#                elif var.dimension==3:
#                    [left1, right1] = bounds[0]
#                    [left2, right2] = bounds[1]
#                    [left3, right3] = bounds[2]
#                    left1_decl = "_"+var.translated_name+"_left_bound_1";
#                    left2_decl = "_"+var.translated_name+"_left_bound_2";
#                    left3_decl = "_"+var.translated_name+"_left_bound_3";
#                    var.bound_shifts.append(left1_decl)
#                    var.bound_shifts.append(left2_decl)
#                    body += left1_decl + " = "
#                    body += __get_expression_body(module,algorhitm,left1) + ";\n"
#                    body += left2_decl + " = "
#                    body += __get_expression_body(module,algorhitm,left2) + ";\n"
#                    body += left3_decl + " = "
#                    body += __get_expression_body(module,algorhitm,left3) + ";\n"
#                    size1 = __get_expression_body(module,algorhitm,right1)
#                    size2 = __get_expression_body(module,algorhitm,right2)
#                    size3 = __get_expression_body(module,algorhitm,right3)
#                    body += n + " = ("+base_type+"***) kum_calloc_3( "+size1+"-"+left1_decl+"+1, "+size2+"-"+left2_decl+"+1, "+size3+"-"+left3_decl+"+1 , sizeof("+base_type+") );\n"
#                    ns |= set(["kumir.h"]) 
#        else:
#            # Локальная переменная
#            var = __get_variable_by_id(algorhitm.local_variables,id)
#            is_argument = var.is_argument
#            if var.type.endswith("[]"):
#                base_type = unicode(var.type)[0:-2]
#                base_type = base_type.replace("string","char")
#                base_type = base_type.replace("bool","_Bool")
#                bounds = var.bounds
#                var.bound_shifts = []
#                if var.dimension==1:
#                    bound = bounds[0]
#                    left = bound[0]
#                    right = bound[1]
#                    left_decl = "_"+var.translated_name+"_left_bound_1";
#                    var.bound_shifts.append(left_decl)
#                    body += "int "+left_decl + " = "
#                    body += __get_expression_body(module,algorhitm,left) + ";\n"
#                    size = __get_expression_body(module,algorhitm,right)
##                    body += base_type+" "+var.translated_name+"["+size+"-"+left_decl+"+1]"
#                    body += base_type+"* "+var.translated_name + " = ("+base_type+"*) kum_calloc_1( "+size+"-"+left_decl+"+1 , sizeof("+base_type+") )"
#                    ns |= set(["kumir.h"])
#                elif var.dimension==2:
#                    bound1 = bounds[0]
#                    bound2 = bounds[1]
#                    left1 = bound1[0]
#                    right1 = bound1[1]
#                    left2 = bound2[0]
#                    right2 = bound2[1]
#                    left1_decl = "_"+var.translated_name+"_left_bound_1";
#                    left2_decl = "_"+var.translated_name+"_left_bound_2";
#                    var.bound_shifts.append(left1_decl)
#                    var.bound_shifts.append(left2_decl)
#                    body += "int "+left1_decl + " = "
#                    body += __get_expression_body(module,algorhitm,left1) + ";\n"
#                    body += "int "+left2_decl + " = "
#                    body += __get_expression_body(module,algorhitm,left2) + ";\n"
#                    size1 = __get_expression_body(module,algorhitm,right1)
#                    size2 = __get_expression_body(module,algorhitm,right2)
##                    body += base_type+" "+var.translated_name+"[+"+size1+"-"+left1_decl+"+1]["+size2+"-"+left2_decl+"+1]"
#                    body += base_type+"** "+var.translated_name + " = ("+base_type+"**) kum_calloc_2( "+size1+"-"+left1_decl+"+1, "+size2+"-"+left2_decl+"+1, sizeof("+base_type+") )"
#                    ns |= set(["kumir.h"])
#                elif var.dimension==3:
#                    [left1, right1] = bounds[0]
#                    [left2, right2] = bounds[1]
#                    [left3, right3] = bounds[2]
#                    left1_decl = "_"+var.translated_name+"_left_bound_1";
#                    left2_decl = "_"+var.translated_name+"_left_bound_2";
#                    left3_decl = "_"+var.translated_name+"_left_bound_3";
#                    var.bound_shifts.append(left1_decl)
#                    var.bound_shifts.append(left2_decl)
#                    body += "int "+left1_decl + " = "
#                    body += __get_expression_body(module,algorhitm,left1) + ";\n"
#                    body += "int "+left2_decl + " = "
#                    body += __get_expression_body(module,algorhitm,left2) + ";\n"
#                    body += "int "+left3_decl + " = "
#                    body += __get_expression_body(module,algorhitm,left3) + ";\n"
#                    size1 = __get_expression_body(module,algorhitm,right1)
#                    size2 = __get_expression_body(module,algorhitm,right2)
#                    size3 = __get_expression_body(module,algorhitm,right3)
##                    body += base_type+" "+var.translated_name+"[+"+size1+"-"+left1_decl+"+1]["+size2+"-"+left2_decl+"+1]["+size3+"-"+left3_decl+"+1]"
#                    body += base_type+"*** "+var.translated_name + " = ("+base_type+"***) kum_calloc_3( "+size1+"-"+left1_decl+"+1, "+size2+"-"+left2_decl+"+1, "+size3+"-"+left3_decl+"+1 , sizeof("+base_type+") )"
#                    ns |= set(["kumir.h"])
#                    
#                if var.type=="string":
#                    body += "[255]"
#                body += ";\n"
#                
#            else:
#                # локальная переменная -- не массив
#                if var.type=="string":
#                    body += "char* "+var.translated_name+";\n"
#                elif var.type=="bool":
#                    body += "_Bool "+var.translated_name + ";\n"
#                else:
#                    body += var.type + " " + var.translated_name + ";\n"         
#    if APPEND_KUMIR_CODE and body!="" and not is_argument:
#        return "/* "+unicode(instruction)+" */\n"+body
#    else:
#        return body

def __get_assignment_instruction_body(module,algorhitm,instr):
    left = __get_expression_body(module, algorhitm, instr.left_part)
    left_type = instr.left_part.type
    right_type = instr.right_part.type
    right = __get_expression_body(module, algorhitm, instr.right_part)
    if left_type=="string" and right_type=="char":
        result = left + " = (char*) calloc( 2, sizeof(char) );\n"
        result += left+"[0] = "+right+";\n"
        result += left+"[1] = '\0';\n"
    else:
        result = left + " = " + right + ";\n"
    if APPEND_KUMIR_CODE:
        return "/* "+unicode(instr)+" */\n"+result
    else:
        return result


def __get_call_instruction_body(module,algorhitm,instr):
    qualifed_name = ""
    global __program
    if instr.module_name=="internal" or instr.module_name=="system":
        instr.algorhitm = instr.algorhitm_name
        res = ""
        if APPEND_KUMIR_CODE:
            res += "/* "+unicode(instr)+" */\n"
        res += __get_internal_function_call_body(module,algorhitm,instr)
        res += ";\n"
        return res
    if instr.module_name==module.name or instr.module_name=="main" and module.name=="":
        alg = util.find_algorhitm(module,instr.algorhitm_name)
        qualifed_name = alg.translated_name
    else:
        mod = util.find_module(__program, instr.module_name)
        alg = util.find_algorhitm(mod,instr.algorhitm_name)
        qualifed_name = mod.translated_name+"::"+alg.translated_name
    fact_arguments = []
    
    for i in range(0,len(instr.arguments)):
        fact_arg = instr.arguments[i]
        form_arg = alg.arguments[i]
        arg = __get_expression_body(module, algorhitm, fact_arg)
#        if form_arg.is_output_argument and form_arg.type!="string" and not form_arg.type.endswith("[]"):
        if form_arg.is_output_argument and not form_arg.type.endswith("[]"):
            arg = "&"+arg
        if form_arg.type=="string" and fact_arg.type=="char":
            if isinstance(fact_arg, program.Constant):
                arg = char_const_to_string_const(arg)
            else:
                arg = "kum_string_of_char("+arg+")"
        fact_arguments.append(arg)
    result = qualifed_name+"("+string.join(fact_arguments, ", ")+");\n"
    
    if APPEND_KUMIR_CODE:
        return "/* "+unicode(instr)+" */\n"+result
    else:
        return result

def __get_if_then_else_statement_body(module,algorhitm,instr):
    cond = __get_expression_body(module, algorhitm, instr.condition)
    result = ""
    if APPEND_KUMIR_CODE:
        result += "/* "+unicode(instr).split("\n")[0]+u" то */\n"
    result += "if ( "+cond+" ) {\n"
    result += __get_body(module, algorhitm, instr.then_body)
    if len(instr.else_body)>0:
        if APPEND_KUMIR_CODE:
            result += u"/* иначе */\n" 
        result += "} else {\n"
        result += __get_body(module, algorhitm, instr.else_body)
    if APPEND_KUMIR_CODE:
        result += u"/* все */\n"
    result += "}\n"
    
    return result



def __get_input_body(module, algorhitm, instr):
    
    result = ""
    if APPEND_KUMIR_CODE:
        result += "/* "+unicode(instr)+" */\n"
    fstring = ""
    argstrings = []
    strings_to_initialize = []
    global __initialized_strings
    global __used_source_includes
    if __initialized_strings is None:
        __initialized_strings = set()
    for term in instr.terms:
        if term.type=="string":
            strings_to_initialize.append([__get_expression_body(module,algorhitm,term), algorhitm.translated_name, module.translated_name])
    for s in strings_to_initialize:
        hash = s[2]+"::"+s[1]+"::"+s[0]
        if not hash in __initialized_strings:
            __initialized_strings |= set([hash])
            result += s[0]+" = (char*) calloc( 255, sizeof(char) );\n"
            if __used_source_includes.has_key(module.translated_name):
                sis = __used_source_includes[module.translated_name]
            else:
                sis = set()
                __used_source_includes[module.translated_name] = sis
            sis |= set(["stdlib.h"])
            
            
    for term in instr.terms:
        expr = __get_expression_body(module,algorhitm,term)
        
        
        if term.type!="string":
            if term.type=="int":
                fstring += "%d"
            elif term.type=="double":
                fstring += "%f"
            elif term.type=="char":
                fstring += "%c"
            elif term.type=="string":
                fstring += "%s"
            elif term.type=="bool":
                fstring += "%d"
            argstrings.append("&"+expr)
        else:
            if len(argstrings)>0:
                'scanf("'+fstring+'", '+string.join(argstrings,", ")+");\n"
                fstring = ""
                argstrings = []
            result += "gets("+expr+");\n"
        
    
    if __used_source_includes.has_key(module.translated_name):
        ns = __used_source_includes[module.translated_name]
    else:
        ns = set()
        __used_source_includes[module.translated_name] = ns
    ns |= set(["stdio.h"])
    if len(argstrings)>0:
        result += 'scanf("'+fstring+'", '+string.join(argstrings,", ")+");\n"
    return result

def screen_string(src):
    return src.replace("\n","\\n").replace("%","%%").replace('"',"\\\"")

def __get_output_body(module, algorhitm, instr):
    result = ""
    if APPEND_KUMIR_CODE:
        result += "/* "+unicode(instr)+" */\n"
    
    fstring = ""
    argstrings = []
    for term in instr.terms:
        expr = __get_expression_body(module,algorhitm,term)
        if isinstance(term, program.Constant) and not isinstance(term, program.New_Line):
            if term.type=="string" or term.type=="char":
                fstring += expr[1:-1]
            else:
                fstring += expr
        elif isinstance(term, program.New_Line):
            fstring += "\\n"
        else:
            if term.type=="int":
                fstring += "%d"
            elif term.type=="double":
                fstring += "%f"
            elif term.type=="char":
                fstring += "%c"
            elif term.type=="string":
                fstring += "%s"
            elif term.type=="bool":
                fstring += "%d"
            argstrings.append(expr)
    
    global __used_source_includes
    if __used_source_includes.has_key(module.translated_name):
        ns = __used_source_includes[module.translated_name]
    else:
        ns = set()
        __used_source_includes[module.translated_name] = ns
    ns |= set(["stdio.h"])
    if len(argstrings)>0:
        result += 'printf("'+fstring+'", '+string.join(argstrings,", ")+");\n"
    else:
        result += 'printf("'+fstring+'");\n'
    return result

def __get_switch_case_else_statement_body(module,algorhitm,term):
    result = ""
    if APPEND_KUMIR_CODE:
        result += u"/* выбор */\n" 
    for i in range(0,len(term.case_conditions)):
        cond = __get_expression_body(module, algorhitm, term.case_conditions[i])
        body = __get_body(module, algorhitm, term.case_bodies[i])
        if APPEND_KUMIR_CODE:
            result += u"/* при "+unicode(term.case_conditions[i])+": */\n"
        if i==0:
            result += "if ( "+cond+" ) {\n"
        else:
            result += "} else if ( "+cond+") {\n"
        result += body
    if not term.else_body is None:
        if APPEND_KUMIR_CODE:
            result += "/* иначе */\n"
        result += "} else {\n"
        result += __get_body(module, algorhitm, term.else_body)
        
    if APPEND_KUMIR_CODE:
        result += "/* все */\n"
    result += "}\n"
    return result 

def __get_loop_statement_body(module, algorhitm, instr):
    m = globals()["__get_"+instr.type+"_loop_body"]
    return m(module,algorhitm,instr)

def __get_free_loop_body(module, algorhitm, instr):
    result = ""
    if APPEND_KUMIR_CODE:
        result += "/* "+unicode(instr).split("\n")[0]+" */\n"
    result += "do {\n"
    result += __get_body(module,algorhitm,instr.loop_body)
    if APPEND_KUMIR_CODE:
        result += "/* "+unicode(instr).split("\n")[-1]+" */\n"
    result += "} while ( "
    if instr.break_condition is None:
        result += "1 );\n"
    else:
        result += "! ("+__get_expression_body(module, algorhitm, instr.break_condition)+") );\n"
    return result


def __get_times_loop_body(module,algorhitm,instr):
    result = ""
    if APPEND_KUMIR_CODE:
        result += "/* "+unicode(instr).split("\n")[0]+" */\n"
    temp_var = __make_temporary_variable(module.translated_name+"::"+algorhitm.translated_name)
    times = __get_expression_body(module, algorhitm, instr.times_term)
    result += "int "+temp_var+";\n"
    result += "for ( "+temp_var+"=0; "+temp_var+"<"+times+"; "+temp_var+"++ ) {\n"
    result += __get_body(module,algorhitm,instr.loop_body)
    if APPEND_KUMIR_CODE:
        result += "/* "+unicode(instr).split("\n")[-1]+" */\n"
    if instr.break_condition is None:
        result += "}\n"
    else:
        result += "if ( "+__get_expression_body(module, algorhitm, instr.break_condition)+" ) break;\n"
        result += "}\n"
    
    return result

def __get_while_loop_body(module,algorhitm,instr):
    result = ""
    if APPEND_KUMIR_CODE:
        result += "/* "+unicode(instr).split("\n")[0]+" */\n"
    result += "while ( "+__get_expression_body(module, algorhitm, instr.while_condition)+" ) {\n"
    result += __get_body(module,algorhitm,instr.loop_body)
    if APPEND_KUMIR_CODE:
        result += "/* "+unicode(instr).split("\n")[-1]+" */\n"
    if instr.break_condition is None:
        result += "}\n"
    else:
        result += "if ( "+__get_expression_body(module, algorhitm, instr.break_condition)+" ) break;\n"
        result += "}\n"
    
    return result

def __get_for_loop_body(module,algorhitm,instr):
    result = ""
    if APPEND_KUMIR_CODE:
        result += "/* "+unicode(instr).split("\n")[0]+" */\n"
    result += "for ( "
    scope = module.global_variables+algorhitm.local_variables+algorhitm.arguments
    val = __get_variable_by_id(scope, instr.for_reference.id)
    result += val.translated_name+"="
    result += __get_expression_body(module, algorhitm, instr.from_term)
    result += "; "
    result += val.translated_name
    if instr.step_term is None:
        op = "<="
    else:
        step = __get_expression_body(module, algorhitm, instr.step_term)
        op = "!="
        try:
            step_val = int(step)
            if step_val<0:
                op = ">="
            if step_val>=0:
                op = "<="
        except:
            pass
    result += op
    result += __get_expression_body(module, algorhitm, instr.to_term)
    if op=="!=":
        result+="+"+step
    result += "; "
    if instr.step_term is None:
        result += val.translated_name+"++ "
    else:
        result += val.translated_name+"+="
        result += step +" "
    result += ") {\n"
    result += __get_body(module,algorhitm,instr.loop_body)
    if APPEND_KUMIR_CODE:
        result += "/* "+unicode(instr).split("\n")[-1]+" */\n"
    if instr.break_condition is None:
        result += "}\n"
    else:
        result += "if ( "+__get_expression_body(module, algorhitm, instr.break_condition)+" ) break;\n"
        result += "}\n"
    
    return result

def __get_assert_body(M,A,I):
    return __get_assert(M,A,I,u"утв ")

def __get_assert(module,algorhitm,instr, kum_instr):
    result = ""
    if APPEND_KUMIR_CODE:
        result += "/* "+kum_instr+unicode(instr.condition)+" */\n"
    result += "assert ( "
    result += __get_expression_body(module, algorhitm, instr.condition)
    result += " );\n"
    return result

def __get_expression_body(module,algorhitm,expression):
    assert isinstance(expression,program.Term)
    clazz = unicode(expression.__class__).split(".")[-1]
    m = globals()["__get_"+clazz.lower()+"_term_body"]
    return m(module, algorhitm, expression)
    
def __get_expression_term_body(M,A,T):
    assert isinstance(T,program.Expression)
    assert len(T.terms)==1
    return "("+__get_expression_body(M,A,T.terms[0])+")"

def __get_multicompare_term_body(M,A,T):
    assert len(T.operators)+1 == len(T.terms)
    assert isinstance(T,program.Multicompare)
    pairs = list()
    for i in range(0,len(T.operators)):
        operand1 = __get_expression_body(M,A,T.terms[i])
        operand2 = __get_expression_body(M,A,T.terms[i+1])
        pair = (operand1, operand2)
        pairs.append(pair)
    assert len(pairs)==len(T.operators)
    conditions = []
    for i in range(0,len(pairs)):
        l, r = pairs[i]
        op = T.operators[i]
        condition = "("+l+op+r+")"
        conditions.append(condition)
    return string.join(conditions, "&&") 
    
def __get_constant_term_body(module,algorhitm,term):
    if term.type=="bool":
        if term.value:
            return "1"
        else:
            return "0"
        
    elif term.type=="string":
        s = term.value
        scr = ""
        for c in s:
            if c=="\\": scr += "\\\\"
            elif c=="\"": scr += "\\\""
            else: scr += c
        return "\""+scr+"\""
    elif term.type=="char":
        s = term.value
        scr = ""
        for c in s:
            if c=="\\": scr += "\\\\"
            elif c=="\'": scr += "\\\'"
            else: scr += c
        return "\'"+scr+"\'"
    else:
        return unicode(term.value)

def __get_value_term_body(module,algorhitm,term):
    v = util.find_variable(module, algorhitm, term)
    name = v.translated_name
    if v.is_output_argument and not v.type.endswith("[]"): #and v.type!="string" and not v.type.endswith("[]"):
        name = "(*"+name+")"
    return name

def __get_return_value_term_body(module,algorhitm,term):
    return "_result"
    
def __get_array_element_term_body(module,algorhitm,term):
    v = util.find_variable(module, algorhitm, term)
    result = v.translated_name
#    if v.is_output_argument:
#        result = "(*"+result+")"
    indeces = []
    i = 1
    for index in term.indeces:
        if not index is None:
            c_index = __get_expression_body(module,algorhitm,index)
            c_index += "-_"+v.translated_name+"_left_bound_"+unicode(i)
#            result += "["+c_index+"]"
            indeces.append(c_index)
            i = i+1
    if not v.is_global:
        for i in indeces:
            result += "["+i+"]"
    else:
        index = ""
        for i in range(0,len(indeces)):
            if i>0:
                index = "("+index+")*_"+v.translated_name+"_size_"+str(i+1)
                index += "+"+indeces[i]
            else:
                index += indeces[i]
        result += "["+index+"]"
    return result

def __get_string_element_term_body(module,algorhitm,term):
    if term.id==-1:
        var_name = "_result"
    else:
        v = util.find_variable(module, algorhitm, term)
        var_name = v.translated_name
        if v.is_output_argument:
            var_name = "(*"+var_name+")"
    result = var_name
    c_index = __get_expression_body(module, algorhitm, term.index)+"-1"
    result += "["+c_index+"]"
    return result

def __get_string_slice_term_body(module,algorhitm,term):
    if term.id==-1:
        var_name = "_result"
    else:
        v = util.find_variable(module, algorhitm, term)
        var_name = v.translated_name
        if v.is_output_argument:
            var_name = "(*"+var_name+")"
    result = var_name
    c_start_index = __get_expression_body(module, algorhitm, term.start_index)
    c_end_index = __get_expression_body(module, algorhitm, term.end_index)
    result = "kum_substring("+v.translated_name+", "+c_start_index+"-1, "+c_end_index+"-1)"
    global __used_source_includes
    if __used_source_includes.has_key(module.translated_name):
        sis = __used_source_includes[module.translated_name]
    else:
        sis = set()
        __used_source_includes[module.translated_name] = sis
    sis |= set(["kumir.h"])
    return result

def __get_new_line_term_body(module,algorhitm,term):
    return "\\n";

def __get_operator_term_body(module,algorhitm,term):
    result = ""
    global __used_source_includes
    if term.operator=="**":
        # заменяем оператор возведения в степень на соответствующую функцию
        assert len(term.terms)==2        
        pow_func = "pow("
        arg1 = __get_expression_body(module, algorhitm, term.terms[0])
        arg2 = __get_expression_body(module, algorhitm, term.terms[1])
        if term.terms[0].type=="int":
            pow_func += "(double)("+arg1+"), "
        else:
            pow_func += arg1+", "
        if term.terms[1].type=="double":
            pow_func += "(double)("+arg2+"))"
        else:
            pow_func += arg2 + ")"
        if term.type=="double":
            result += pow_func
        else:
            result += "(int)"+pow_func
        
        if __used_source_includes.has_key(module.translated_name):
            sis = __used_source_includes[module.translated_name]
        else:
            sis = set()
            __used_source_includes[module.translated_name] = sis
        sis |= set(["math.h"])
    elif term.operator=="!" or term.operator=="++" or term.operator=="--":
        # унарный оператор
        assert len(term.terms)==1
        if term.operator=="!":
            result += "!"
        elif term.operator=="++":
            pass
        elif term.operator=="--":
            result += "-"
        result += __get_expression_body(module, algorhitm, term.terms[0])
    elif term.operator=="+" and (term.terms[0].type=="string" or term.terms[0].type=="char") and (term.terms[1].type=="string" or term.terms[1].type=="char"):
        # конкатенация
        left = __get_expression_body(module, algorhitm, term.terms[0])
        right = __get_expression_body(module, algorhitm, term.terms[1])
        result += "kum_concatenate_"+term.terms[0].type[0]+term.terms[1].type[0]+"("+left+", "+right+")"
        if __used_source_includes.has_key(module.translated_name):
            sis = __used_source_includes[module.translated_name]
        else:
            sis = set()
            __used_source_includes[module.translated_name] = sis
        sis |= set(["kumir.h"])
    elif (term.operator=="==" or term.operator=="!=" or term.operator=="<=" or term.operator==">=" or term.operator=="<" or term.operator=="<")  and (term.terms[0].type=="string" or term.terms[0].type=="char") and (term.terms[1].type=="string" or term.terms[1].type=="char"):
        # литеральное сравнение
        left = __get_expression_body(module, algorhitm, term.terms[0])
        right = __get_expression_body(module, algorhitm, term.terms[1])
        use_kumir_h = False
        use_string_h = False
        if term.terms[0].type==term.terms[1].type=="char":
            result += left + term.operator + right
        elif term.terms[0].type==term.terms[1].type=="string":
            result += "(strcmp("+left+", "+right+")"+term.operator+"0)"
            use_string_h = True
        else:
            if term.terms[0].type=="char":
                if isinstance(term.terms[0], program.Constant):
                    left = char_const_to_string_const(left)
                else:
                    left = "kum_string_of_char("+left+")"
                    use_string_h = True
            if term.terms[1].type=="char":
                if isinstance(term.terms[1], program.Constant):
                    right = char_const_to_string_const(right)
                else:
                    right = "kum_string_of_char("+right+")"
                    use_string_h = True
            result += "(strcmp("+left+", "+right+")"+term.operator+"0)"
            use_kumir_h = True
        if __used_source_includes.has_key(module.translated_name):
            sis = __used_source_includes[module.translated_name]
        else:
            sis = set()
            __used_source_includes[module.translated_name] = sis
        if use_kumir_h:
            sis |= set(["kumir.h"])
        if use_string_h:
            sis |= set(["string.h"])
    else:
        # бинарные операторы
        assert len(term.terms)==2
        left = __get_expression_body(module, algorhitm, term.terms[0])
        right = __get_expression_body(module, algorhitm, term.terms[1])
        result += left + term.operator + right
    return result
            

def __get_internal_function_call_body(module,algorhitm,term):
#    print term.algorhitm
    assert __builtin_replacement.has_key(term.algorhitm)
    (rule, extra) = __builtin_replacement[term.algorhitm]
    args = []
    for a in term.arguments:
        args.append(__get_expression_body(module, algorhitm, a))
    if term.algorhitm=="mod" or term.algorhitm=="div":
        if term.algorhitm=="mod":
            op = "%"
        else:
            op = "/"
        assert len(args)==2
        result = ""
        if isinstance(term.arguments[0], program.Operator):
            args[0] = "("+args[0]+")"
        if isinstance(term.arguments[1], program.Operator):
            args[0] = "("+args[1]+")"    
#        return "/* "+term.algorhitm+" */("+args[0]+op+args[1]+")"
        return "("+args[0]+op+args[1]+")"
    for i in range(0,len(args)):
        s = "$"+unicode(i+1)
        rule = rule.replace(s,args[i])
    if not extra is None:
        (includes, init) = extra
        if not includes is None:
            global __used_source_includes
            if __used_source_includes.has_key(module.translated_name):
                sis = __used_source_includes[module.translated_name]
            else:
                sis = set()
                __used_source_includes[module.translated_name] = sis
            for inc in includes:
                if inc not in sis:
                    sis.add(inc)
        if not init is None:
            global __main
            __main += init
    
    return rule

def char_const_to_string_const(cc):
    value = cc[1:-1]
    value = value.replace("\"","\\\"")
    value = value.replace("\\'","'")
    return "\"" + value + "\""

def __get_function_call_term_body(module,algorhitm,term):
    if term.module=="internal" or term.module=="system":
        return __get_internal_function_call_body(module,algorhitm,term)
    qualifed_name = ""
    global __program
    if term.module==module.name:
        alg = util.find_algorhitm(module,term.algorhitm)
        qualifed_name = alg.translated_name
    else:
        mod = util.find_module(__program, term.module)
        alg = util.find_algorhitm(mod,term.algorhitm)
        if mod.translated_name=="" or mod.translated_name==module.translated_name:
            qualifed_name = alg.translated_name
        else:
            qualifed_name = mod.translated_name+"::"+alg.translated_name
    fact_arguments = []
    for i in range(0,len(term.arguments)):
        fact_arg = term.arguments[i]
        form_arg = alg.arguments[i]
        arg = __get_expression_body(module, algorhitm, fact_arg)
#        if form_arg.is_output_argument and form_arg.type!="string" and not form_arg.type.endswith("[]"):
        if form_arg.is_output_argument and not form_arg.type.endswith("[]"):
            arg = "&"+arg
        if form_arg.type=="string" and fact_arg.type=="char":
            if isinstance(fact_arg, program.Constant):
                arg = char_const_to_string_const(arg)
            else:
                arg = "kum_string_of_char("+arg+")"
        fact_arguments.append(arg)
    return qualifed_name+"("+string.join(fact_arguments, ", ")+")"

def __process_module(module):
    if module.type=="implemented":
        for algorhitm in module.algorhitms: 
            __process_algorhitm(module, algorhitm)
        return []
    else:
        return []
    
    
def __process_algorhitm(module, algorhitm):
    pass
    
def __make_module_header(module):
    if module.name!="":
        file_name = module.translated_name.lower()+".h";
        ifdef_name = "_"+module.translated_name.upper()+"_H";
    else:
        file_name = "main_module.h"
        ifdef_name = "_MAIN_MODULE_H"
    data = u""
    data += "#ifndef "+ifdef_name+"\n#define "+ifdef_name+"\n\n"
    data += module.header_includes
    if module.name!="":
        data += "namespace "+module.translated_name+" {\n"
#    for var in module.global_variables:
#        data += "static "+var.global_declaration;
#        
    if len(module.initializer_instructions)>0:
        data += "void "+module.translated_name+"();\n";
    
    for alg in module.algorhitms:
        data += alg.generated_header+"\n";
        
    if module.name!="":
        data += "}\n "
    
    data += "#endif\n"
    return file_name, data

LINUX_CXX = "gcc"
LINUX_CXX_FLAGS = "-c -std=c99 -I."
LINUX_LINK = "gcc"
LINUX_LINK_FLAGS = ""
LINUX_OUTPUT_FLAG = "-o "
LINUX_TARGET_FILENAME_EXTENSION = ".bin"
LINUX_OBJECT_FILENAME_EXTENSION = ".o"
LINUX_LIBRARY_FILENAME_EXTENSION = ".a"
LINUX_AR = "ar rcs"
LINUX_LIB_PREFIX = ""

MACOSX_CXX = "gcc"
MACOSX_CXX_FLAGS = "-c -std=c99 -I."
MACOSX_LINK = "gcc"
MACOSX_LINK_FLAGS = ""
MACOSX_OUTPUT_FLAG = "-o "
MACOSX_TARGET_FILENAME_EXTENSION = ".app"
MACOSX_OBJECT_FILENAME_EXTENSION = ".o"
MACOSX_LIBRARY_FILENAME_EXTENSION = ".a"
MACOSX_AR = "ar rcs"
MACOSX_LIB_PREFIX = ""

SOLARIS_CXX = "cc"
SOLARIS_CXX_FLAGS = "-c -std=c99 -I."
SOLARIS_LINK = "cc"
SOLARIS_LINK_FLAGS = ""
SOLARIS_OUTPUT_FLAG = "-o "
SOLARIS_TARGET_FILENAME_EXTENSION = ".bin"
SOLARIS_OBJECT_FILENAME_EXTENSION = ".o"
SOLARIS_LIBRARY_FILENAME_EXTENSION = ".a"
SOLARIS_AR = "ar rcs"
SOLARIS_LIB_PREFIX = ""

WINDOWS_CXX = "mingw32-gcc"
WINDOWS_CXX_FLAGS = "-c -std=c99 -I."
WINDOWS_LINK = "mingw32-gcc"
WINDOWS_LINK_FLAGS = ""
WINDOWS_OUTPUT_FLAG = "-o "
WINDOWS_TARGET_FILENAME_EXTENSION = ".exe"
WINDOWS_OBJECT_FILENAME_EXTENSION = ".o"
WINDOWS_LIBRARY_FILENAME_EXTENSION = ".a"
WINDOWS_AR = "ar rcs"
WINDOWS_LIB_PREFIX = ""

def __generate_makefile(program, files, os="linux"):
    result = ""
    cxx = globals()[os.upper()+"_CXX"]
    cxx_flags = globals()[os.upper()+"_CXX_FLAGS"]
    link = globals()[os.upper()+"_LINK"]
    link_flags = globals()[os.upper()+"_LINK_FLAGS"]
    output_flag = globals()[os.upper()+"_OUTPUT_FLAG"]
    target_filename_extension = globals()[os.upper()+"_TARGET_FILENAME_EXTENSION"]
    object_filename_extension = globals()[os.upper()+"_OBJECT_FILENAME_EXTENSION"]
    library_filename_extension = globals()[os.upper()+"_LIBRARY_FILENAME_EXTENSION"]
    lib_prefix = globals()[os.upper()+"_LIB_PREFIX"]
    ar = globals()[os.upper()+"_AR"]
    global __used_source_includes
    use_math = False
    use_kumir = False
    kumir_a = lib_prefix+"kumir"+library_filename_extension
    for m in __used_source_includes.values():
        if "math.h" in m:
            use_math = True
        if "kumir.h" in m:
            use_kumir = True
    if use_math:
        link_flags += " -lm"
    objects = []
    target_name = ""
    for mod in program.modules:
        if mod.name=="":
            if len(mod.algorhitms)==0 or mod.algorhitms[0].name=="":
                target_name = "output"
            else:
                target_name = mod.algorhitms[0].translated_name
            break
    target_name += target_filename_extension
    result += "all: "+target_name+"\n\n"
    for (filename, body) in files:
        if filename.endswith(".c"):
            basename = filename[:-2]
            objectname = basename + object_filename_extension
            objects.append(objectname)
            result += objectname+": "+filename+"\n\t"
            result += cxx+" "+cxx_flags+" "+output_flag+objectname+" "+filename+"\n\n"
    
    result += target_name+": "
    for o in objects:
        result += o+" "
    if use_kumir:
        result += kumir_a
    result += "\n\t"
    result += link+" "+link_flags+" "+output_flag+target_name+" "
    for o in objects:
        result += o+" "
    if use_kumir:
        result += kumir_a
    result += "\n\n"
    
    if use_kumir:
        kumir_o = "kumir"+object_filename_extension
        result += kumir_o+": kumir.c kumir.h\n\t"
        result += cxx+" "+cxx_flags+" "+output_flag+kumir_o+" kumir.c\n\n"
        
        
        result += kumir_a+": "+kumir_o+"\n\t"
        result += ar+" "+kumir_a+" "+kumir_o +"\n\n"
    
    result += "clean:\n"
    for o in objects:
        result += "\trm -f "+o+"\n"
    if use_kumir:
        result += "\trm -f "+kumir_o+"\n"
#        result += "\trm -f "+kumir_a+"\n"
    result += "\n"
    return result
    
            
def convert(source):
    reset()
    tree = program.parse(source)
    platform = util.get_platform()
    result = process(tree, platform)
    return result
