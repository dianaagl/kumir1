#coding=UTF-8

ALLOWED_SYMBOLS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_"

def transliterate(name):
    new_name = ""
    for c in name:
        if c==u'а': new_name += 'a'
        if c==u'б': new_name += 'b' 
        if c==u'в': new_name += 'v'
        if c==u'г': new_name += 'g'
        if c==u'д': new_name += 'd'
        if c==u'е': new_name += 'e'
        if c==u'ё': new_name += 'yo'
        if c==u'ж': new_name += 'zh'
        if c==u'з': new_name += 'z'
        if c==u'и': new_name += 'i'
        if c==u'й': new_name += 'j'
        if c==u'к': new_name += 'k'
        if c==u'л': new_name += 'l'
        if c==u'м': new_name += 'm'
        if c==u'н': new_name += 'n'
        if c==u'о': new_name += 'o'
        if c==u'п': new_name += 'p'
        if c==u'р': new_name += 'r'
        if c==u'с': new_name += 's'
        if c==u'т': new_name += 't'
        if c==u'у': new_name += 'u'
        if c==u'ф': new_name += 'f'
        if c==u'х': new_name += 'h'
        if c==u'ц': new_name += 'ts'
        if c==u'ч': new_name += 'ch'
        if c==u'ш': new_name += 'sh'
        if c==u'щ': new_name += 'sch'
        if c==u'ь': new_name += ''
        if c==u'ы': new_name += 'y'
        if c==u'ъ': new_name += ''
        if c==u'э': new_name += 'e'
        if c==u'ю': new_name += 'ju'
        if c==u'я': new_name += 'ja'
        if c==u'А': new_name += 'A'
        if c==u'Б': new_name += 'B'
        if c==u'В': new_name += 'V'
        if c==u'Г': new_name += 'G'
        if c==u'Д': new_name += 'D'
        if c==u'Е': new_name += 'E'
        if c==u'Ё': new_name += 'JO'
        if c==u'Ж': new_name += 'ZH'
        if c==u'З': new_name += 'Z'
        if c==u'И': new_name += 'I'
        if c==u'Й': new_name += 'J'
        if c==u'К': new_name += 'K'
        if c==u'Л': new_name += 'L'
        if c==u'М': new_name += 'M'
        if c==u'Н': new_name += 'N'
        if c==u'О': new_name += 'O'
        if c==u'П': new_name += 'P'
        if c==u'Р': new_name += 'R'
        if c==u'С': new_name += 'S'
        if c==u'Т': new_name += 'T'
        if c==u'У': new_name += 'U'
        if c==u'Ф': new_name += 'F'
        if c==u'Х': new_name += 'H'
        if c==u'Ц': new_name += 'TS'
        if c==u'Ч': new_name += 'CH'
        if c==u'Ш': new_name += 'SH'
        if c==u'Щ': new_name += 'SCH'
        if c==u'Ь': new_name += ''
        if c==u'Ы': new_name += 'Y'
        if c==u'Ъ': new_name += ''
        if c==u'Э': new_name += 'E'
        if c==u'Ю': new_name += 'ju'
        if c==u'Я': new_name += 'ja'
        if c==u' ': new_name += '_'
        if ALLOWED_SYMBOLS.count(c)>0: new_name += c
    if len(new_name)==0:
        new_name = "unnamed"
    return new_name

import program
import os

def find_variable(module,algorhitm,term):
    if algorhitm is None:
        scope = module.global_variables
    else:
        scope = module.global_variables+algorhitm.local_variables+algorhitm.arguments
    result = None
    for var in scope:
        if var.id==term.id:
            result = var
            break
    assert not result is None
    return result

def find_algorhitm(module,name):
    result = None
    for alg in module.algorhitms:
        if alg.name==name:
            result = alg
            break
    assert not result is None
    return result

def find_module(P, name):
    result = None
    for module in P.modules:
        if module.name==name or (module.name=="" and name=="main"):
            result = module
            break
    assert not result is None
    return result

def __check_variable_for_possible_changes_in_expression(P,M,A,var,term):
    clazz = unicode(term.__class__).split(".")[-1].lower()
    m_name = "__check_for_changes_in_"+clazz
    if m.globals().has_key(m_name):
        m = globals()[m_name]
        return m(P,M,A,var,term)
    else:
        return False
    
def __check_for_changes_in_expression(P,M,A,var,expr):
    result = False
    for term in expr.terms:
        result = result or __check_variable_for_possible_changes_in_expression(P, M, A, var, term)
    return result

def __check_for_changes_in_operator(P,M,A,var,op):
    return __check_for_changes_in_expression(P,M,A,var,op)

def __check_for_changes_in_string_element(P,M,A,var,se):
    return __check_variable_for_possible_changes_in_expression(P, M, A, var, se.index)

def __check_for_changes_in_string_slice(P,M,A,var,sl):
    cs = __check_variable_for_possible_changes_in_expression(P, M, A, var, sl.start_index)
    ce = __check_variable_for_possible_changes_in_expression(P, M, A, var, sl.end_index)
    return cs or ce

def __check_for_changes_in_multicompare(P,M,A,var,mc):
    return __check_for_changes_in_expression(P,M,A,var,mc)

def __check_for_changes_in_call(P,M,A,var,call_mod,call_name,fact_args):
    changes = False
    form_args = alg.arguments
    mod = find_module(P, call_mod)
    alg = find_algorhitm(M, call_name)
    assert len(form_args)==len(fact_args)
    for i in range(0, len(form_args)):
        form_arg = form_args[i]
        fact_arg = fact_args[i]
        if isinstance(fact_arg, program.Value) and fact.id==var.id and form_arg.is_output_value:
            child_var = Value()
            child_var.id = form_arg.id
            arg_changes = check_variable_for_possible_changes(P,M,A,child_var,alg.instructions)
            changes = changes or arg_changes
    return changes 

def __check_for_changes_in_function_call(P,M,A,var,fc):
    if fc.module=="internal" or fc.module=="system":
        # TODO реализовать д/функций лит_в_цел, лит_в_вещ и др.
        return False
    call_mod = fc.module
    call_name = fc.algorhitm
    fact_args = fc.arguments
    return __check_for_changes_in_call(P, M, A, var, call_mod, call_name, fact_args)
    

def check_variable_for_possible_changes(P,M,A,var,inst_list):
    changes_value = False
    for instr in instr_list:
        if isinstance(instr, program.Assignment_Instruction):
            changes_value = changes_value or __check_variable_for_possible_changes_in_expression(P, M, A, var, instr.left_part)
            changes_value = changes_value or __check_variable_for_possible_changes_in_expression(P, M, A, var, instr.right_part)
            if isinstance(instr.left_part, program.Value):
                if instr.left_part.id==var.id:
                    changes_value = True
        if isinstance(instr, program.Call_Instruction):
            call_mod = instr.module_name
            call_name = instr.algorhitm_name
            fact_args = instr.arguments
            changes_value = changes_value or __check_for_changes_in_call(P, M, A, var, call_mod, call_name, fact_args)
        if changes_value:
            break;
    return changes_value
    
def get_platform():
    if os.name=="posix":
        oss = "linux"
    elif os.name=="nt":
        oss = "windows"
    arch = "i386"
    return (arch, oss)
