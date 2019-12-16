#coding=UTF-8

import xml.sax.handler
import xml.sax
import xml.dom.minidom
import string


class Term:
    def __init__(self):
        self.type = None
        self.source_string = ""
    def __str__(self):
        return self.source_string
    
class Return_Value(Term):
    def __init__(self):
        Term.__init__(self)
    
class Expression(Term):
    def __init__(self):
        Term.__init__(self)
        self.terms = []

class Operator(Expression):
    def __init__(self):
        Expression.__init__(self)
        self.operator = None
        
class Constant(Term):
    def __init__(self):
        Term.__init__(self)
        self.value = None
    
class New_Line(Constant):
    def __init__(self):
        Constant.__init__(self)
        self.type = "string"
        self.value = "\n"
        
class Value(Term):
    def __init__(self):
        Term.__init__(self)
        self.id = None

class String_Element(Value):
    def __init__(self):
        Term.__init__(self)
        self.id = None
        self.index = Term()
        self.type = "char"
        
class Array_Element(Value):
    def __init__(self):
        Term.__init__(self)
        self.indeces = []

class String_Slice(Value):
    def __init__(self):
        Term.__init__(self)
        self.id = None
        self.start_index = Term()
        self.end_index = Term()
        self.type = "string"
        
class Function_Call(Term):
    def __init__(self):
        Term.__init__(self)
        self.module = None
        self.algorhitm = None
        self.arguments = []

class Multicompare(Expression):
    def __init__(self):
        Term.__init__(self)
        self.operators = list()
        self.terms = list()
        self.type = "bool"


class Instruction:
    def __init__(self):
        self.source_string = ""
    def __str__(self):
        return self.source_string


class Assignment_Instruction(Instruction):
    def __init__(self):
        Instruction.__init__(self)
        self.left_part = None
        self.right_part = None


class Call_Instruction(Instruction):
    def __init__(self):
        Instruction.__init__(self)
        self.module_name = ""
        self.algorhitm_name = ""
        self.arguments = []


class Variable_Declaration(Instruction):
    def __init__(self):
        Instruction.__init__(self)
        self.variables = []


class If_Then_Else_Statement(Instruction):
    def __init__(self):
        Instruction.__init__(self)
        self.condition = Term()
        self.then_body = []
        self.else_body = []


class Switch_Case_Else_Statement(Instruction):
    def __init__(self):
        Instruction.__init__(self)
        self.case_conditions = []
        self.case_bodies = []
        self.else_body = []


class Loop_Statement(Instruction):
    def __init__(self):
        Instruction.__init__(self)
        self.type = "simple"
        self.while_condition = None
        self.break_condition = None
        self.for_reference = None
        self.from_term = None
        self.to_term = None
        self.step_term = None
        self.times_term = None
        self.loop_body = []

   
class Exit(Instruction):
    def __init__(self):
        Instruction.__init__(self)
        
class Break(Instruction):
    def __init__(self):
        Instruction.__init__(self)

class Input(Instruction):
    def __init__(self):
        Instruction.__init__(self)
        self.terms = []

class Output(Instruction):
    def __init__(self):
        Instruction.__init__(self)
        self.terms = []


class File_Input(Instruction):
    def __init__(self):
        Instruction.__init__(self)
        self.terms = []


class File_Output(Instruction):
    def __init__(self):
        Instruction.__init__(self)
        self.terms = []
        

class Assertion(Instruction):
    def __init__(self):
        Instruction.__init__(self)
        self.condition = None


class Variable:
    def __init__(self):
        self.id = -1
        self.type = "void"
        self.name = ""
        self.is_argument = False
        self.is_input_argument = True
        self.is_output_argument = False
        self.dimension = 0
        self.bounds = [None, None, None]



class Reference:
    def __init__(self):
        self.id = -1


class Algorhitm:
    def __init__(self):
        self.name = ""
        self.return_type = "void"
        self.arguments = []
        self.local_variables = []
        self.instructions = []
        self.input_assertions = []
        self.output_assertions = []
        self.source_string = ""
        

class Module:
    def __init__(self):
        self.name = ""
        self.type = "implemented"
        self.global_variables = []
        self.algorhitms = []
        self.initializer_instructions = []
        self.is_main = False
        self.source_string = ""


class Program:
    def __init__(self):
        self.modules = []

# ------------- Парсинг XML
    
def parse(data):
    #handler = ProgramHandler()
    #xml.sax.parseString(data,handler)
    prog = xml.dom.minidom.parseString(data).documentElement
    assert prog.tagName=="program"
    p = Program()
    for cn in prog.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE and cn.getAttribute("type")=="implemented":
            p.modules.append(__parse_module(cn))
    __generate_source_strings(p)
    return p

def __parse_module(root):
    assert root.tagName=="module"
    m = Module()
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE:
            tag = cn.tagName
            if tag=="globals":
                m.global_variables = __parse_variables(cn,False)
            elif tag=="initializer":
                m.initializer_instructions = __parse_instructions(cn)
            elif tag=="algorhitm":
                m.algorhitms.append(__parse_algorhitm(cn))
    return m

def __parse_variables(root,is_argument):
    ACCEPT_TAGS = ["arguments", "globals", "locals"]
    assert root.tagName in ACCEPT_TAGS
    g = []
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE:
            g.append(__parse_variable(cn,is_argument))
    return g

def __parse_variable(root, is_argument):
    assert root.tagName=="variable"
    var = Variable()
    var.id = int(root.getAttribute("id"))
    var.name = root.getAttribute("name")
    var.type = root.getAttribute("type")
    var.is_argument = is_argument;
    if is_argument:
        flag = root.getAttribute("flag")
        var.is_input_argument = flag.count("IN")>0
        var.is_output_argument = flag.count("OUT")>0
    if var.type.endswith("[]"):
        var.dimension = int(root.getAttribute("dimension"))
        var.bounds = list()
        counter = 0
        cur_bound = [None, None]
        for cn in root.childNodes:
            if cn.nodeType==cn.ELEMENT_NODE and cn.nodeName=="bound":
                b = __parse_screened_term(cn)
                cur_bound[counter%2] = b
                if counter%2==1:
                    var.bounds.append(cur_bound)
                    cur_bound = [None, None]
                counter += 1
    return var

def __parse_instructions(root):
    ACCEPT_TAGS = ["body", "initializer","then","else"]
    assert root.tagName in ACCEPT_TAGS
    instructions = []
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE:
            m = globals()["__parse_"+cn.tagName.lower()]
            instructions.append(m(cn))
    return instructions

def __parse_exit(root):
    p = root.parentNode
    while True:
        if p.tagName=="algorhitm":
            return Exit();
        elif p.tagName=="loop":
            return Break();
        else:
            p = p.parentNode

def __parse_algorhitm(root):
    assert root.tagName=="algorhitm"
    alg = Algorhitm()
    alg.name = root.getAttribute("name")
    alg.return_type = root.getAttribute("type")
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE:
            tag = cn.tagName
            if tag=="arguments":
                alg.arguments = __parse_variables(cn,True)
            elif tag=="locals":
                alg.local_variables = __parse_variables(cn,False)
            elif tag=="body":
                alg.instructions = __parse_instructions(cn)
            elif tag=="input_assert":
                a = Assertion()
                a.condition = __parse_screened_term(cn)
                alg.input_assertions.append(a)
            elif tag=="output_assert":
                a = Assertion()
                a.condition = __parse_screened_term(cn)
                alg.output_assertions.append(a)
    return alg

def __parse_declare(root):
    vd = Variable_Declaration()
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE and cn.tagName=="value":
            id = int(cn.getAttribute("id"))
            i = 0
            bounds = [None, None, None]
            lefts = [None, None, None]
            rights = [None, None, None]
            for cnn in cn.childNodes:
                if cnn.nodeType==cn.ELEMENT_NODE and cnn.tagName=="bound":
                    bn = i/2
                    if i%2==0:
                        arr = lefts
                    else:
                        arr = rights
                    arr[bn] = __parse_screened_term(cnn)
                    i = i+1
            bounds[0] = (lefts[0], rights[0])
            bounds[1] = (lefts[1], rights[1])
            bounds[2] = (lefts[2], rights[2])
            vd.variables.append( (id, bounds ))
    return vd

def __parse_input(root):
    inp = Input()
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE:
            inp.terms.append(__parse_term(cn))
    return inp

def __parse_output(root):
    out = Output()
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE:
            out.terms.append(__parse_term(cn))
    return out

def __parse_assignment(root):
    assign = Assignment_Instruction()
    terms = []
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE:
            terms.append(__parse_term(cn))
    assert len(terms)==2
    assign.left_part = terms[0]
    assign.right_part = terms[1]
    return assign

def __parse_call(root):
    call = Call_Instruction()
    call.module_name = root.getAttribute("module");
    call.algorhitm_name = root.getAttribute("algorhitm")
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE:
            call.arguments.append(__parse_term(cn))
    return call

def __parse_loop(root):
    loop = Loop_Statement()
    loop.type = root.getAttribute("type")
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE:
            tag = cn.tagName
            if tag=="for":
                loop.for_reference = __parse_for(cn)
            elif tag=="from":
                loop.from_term = __parse_screened_term(cn)
            elif tag=="to":
                loop.to_term = __parse_screened_term(cn)
            elif tag=="step":
                loop.step_term = __parse_screened_term(cn)
            elif tag=="times":
                loop.times_term = __parse_screened_term(cn)
            elif tag=="while":
                loop.while_condition = __parse_screened_term(cn)
            elif tag=="body":
                loop.loop_body = __parse_instructions(cn)
            elif tag=="break_loop":
                loop.break_condition = __parse_screened_term(cn)
    return loop

def __parse_for(root):
    val = Value()
    val.type = "int"
    val.id = int(root.getAttribute("id"))
    return val

def __parse_screened_term(root):
    terms = []
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE:
            terms.append(__parse_term(cn))
    assert len(terms)==1
    return terms[0]


def __parse_if(root):
    if_ = If_Then_Else_Statement()
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE:
            tag = cn.tagName
            if tag=="condition":
                if_.condition = __parse_screened_term(cn)
            elif tag=="then":
                if_.then_body = __parse_instructions(cn)
            elif tag=="else":
                if_.else_body = __parse_instructions(cn)
    return if_

def __parse_switch(root):
    switch = Switch_Case_Else_Statement()
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE:
            tag = cn.tagName
            if tag=="case":
                condition, body = __parse_case(cn)
                switch.case_conditions.append(condition)
                switch.case_bodies.append(body)
            elif tag=="else":
                switch.else_body = __parse_instructions(cn)
    return switch

def __parse_case(root):
    condition = None
    body = None
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE:
            tag = cn.tagName
            if tag=="condition":
                condition = __parse_screened_term(cn)
            elif tag=="body":
                body = __parse_instructions(cn)
    return condition, body

def __parse_assert(root):
    a = Assertion()
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE:
            a.condition = __parse_term(cn)
    return a
    

def __parse_term(root):
    ACCEPT_TAGS = ["value", "constant", "expression", 
                   "call", "new_line","string_element",
                   "string_slice","array_element","operator",
                   "return_value","multicompare"]
#    print root.tagName
    assert root.tagName in ACCEPT_TAGS
    tag = root.tagName
    if tag=="value":
        return __parse_value(root)
    elif tag=="constant":
        return __parse_constant(root)
    elif tag=="expression":
        return __parse_expression(root)
    elif tag=="call":
        return __parse_function_call(root)
    elif tag=="new_line":
        return New_Line()
    elif tag=="string_element":
        return __parse_string_element(root)
    elif tag=="string_slice":
        return __parse_string_slice(root)
    elif tag=="array_element":
        return __parse_array_element(root)
    elif tag=="operator":
        return __parse_operator(root)
    elif tag=="return_value":
        return __parse_return_value(root)
    elif tag=="multicompare":
        return __parse_multicompare(root)
    else:
        return None

def __parse_value(root):
    val = Value()
    val.type = root.getAttribute("type")
    val.id = int(root.getAttribute("id"))
    return val

def __parse_return_value(root):
    val = Return_Value()
    node = root
    while node.tagName!="algorhitm":
        node = node.parentNode
    val.type = node.getAttribute("type")
    val.source_string = u"знач"
    return val

import base64

def __parse_constant(root):
    c = Constant()
    c.type = root.getAttribute("type")
    els = []
    for cn in root.childNodes:
        if cn.nodeType==cn.TEXT_NODE:
            els.append(cn.data)
    assert len(els)<=1
    if len(els)==0:
        els.append("")
    encoded = els[0]
    decoded = base64.b64decode(encoded)
    value = unicode(decoded,"UTF-8")
    if c.type=="string" or c.type=="char":
        c.value = value
    elif c.type=="int":
        c.value = int(value)
    elif c.type=="double":
        c.value = float(value)
    elif c.type=="bool":
        c.value = value!="0"
    return c

def __parse_function_call(root):
    call = Function_Call()
    call.module = root.getAttribute("module")
    call.algorhitm = root.getAttribute("algorhitm")
    call.type = root.getAttribute("type")
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE:
            call.arguments.append(__parse_term(cn))
    return call

def __parse_expression(root):
    expr = Expression()
    expr.type = root.getAttribute("type")
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE:
            expr.terms.append(__parse_term(cn))
    return expr
    

def __parse_operator(root):
    op = Operator()
    op.type = root.getAttribute("type")
    op.operator = root.getAttribute("kind")
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE:
            op.terms.append(__parse_term(cn))
    return op

def __parse_multicompare(root):
    mc = Multicompare()
    operators = list()
    operands = list()
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE:
            if cn.tagName=="operators":
                for cnn in cn.childNodes:
                    if cnn.nodeType==cnn.ELEMENT_NODE and cnn.tagName=="compare":
                        kind = cnn.getAttribute("kind")
                        operators.append(kind)
            if cn.tagName=="operands":
                for cnn in cn.childNodes:
                    if cnn.nodeType==cnn.ELEMENT_NODE:
                        operands.append(__parse_term(cnn))
    mc.operators = operators
    mc.terms = operands
    return mc

def __parse_string_element(root):
    se = String_Element()
    se.id = int(root.getAttribute("id"))
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE:
            se.index = __parse_term(cn)
    return se

def __parse_array_element(root):
    se = Array_Element()
    se.type = root.getAttribute("type")
    se.id = int(root.getAttribute("id"))
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE:
            se.indeces.append(__parse_term(cn))
    return se


def __parse_string_slice(root):
    se = String_Slice()
    se.id = int(root.getAttribute("id"))
    bounds = []
    for cn in root.childNodes:
        if cn.nodeType==cn.ELEMENT_NODE:
            bounds.append(__parse_term(cn))
    assert len(bounds)==2
    se.start_index = bounds[0]
    se.end_index = bounds[1]
    return se
    
def __generate_source_strings(program):
    for module in program.modules:
        __generate_source_strings_for_module(program, module)

def __generate_source_strings_for_module(program, module):
    __generate_source_strings_for_body(program, module, None, module.initializer_instructions)
    for alg in module.algorhitms:
        __generate_source_string_for_algorhitm(program, module, alg)
    if module.name!="":
        module.source_string = u"исп "+module.name+"\nкон_исп" 

def __generate_source_strings_for_body(P, M, A, B):
    for instr in B:
        clazz = unicode(instr.__class__).split(".")[-1].lower()
        m = globals()["__generate_source_string_for_"+clazz]
        m(P,M,A,instr)
        
def __generate_source_string_for_break(P,M,A,I):
    return u'выход'

def __generate_source_string_for_assignment_instruction(P,M,A,I):
    __generate_source_string_for_term(P,M,A,I.left_part)
    __generate_source_string_for_term(P,M,A,I.right_part)
    I.source_string = unicode(I.left_part)+":="+unicode(I.right_part)

def __generate_source_string_for_call_instruction(P,M,A,I):
    t = []
    for arg in I.arguments:
        __generate_source_string_for_term(P,M,A,arg)
        t.append(unicode(arg))
    I.source_string = I.algorhitm_name
    if len(I.arguments)>0:
        I.source_string += "(" + string.join(t, ", ") + ")"

def __find_variable(M,A,id):
    r = None
    for v in M.global_variables:
        if v.id == id:
            r = v;
            break;
    if not r is None:
         return r
    for v in A.local_variables:
        if v.id == id:
            r = v;
            break;
    if not r is None:
        return r;
    for v in A.arguments:
        if v.id == id:
            r = v;
            break;
    assert not r is None
    return r
    
def __generate_source_string_for_variable_declaration(P,M,A,I):
    first_id, first_bounds = I.variables[0]
    first = __find_variable(M,A,first_id)
    t = first.type
    r = ""
    if t.startswith("int"):
        r = u"цел"
    elif t.startswith("double"):
        r = u"вещ"
    elif t.startswith("bool"):
        r = u"лог"
    elif t.startswith("char"):
        r = u"сим"
    elif t.startswith("string"):
        r = u"лит"
    if t.endswith("[]"):
        r += u"таб"
    r += " "
    ds = []
    for id, bounds in I.variables:
        v = __find_variable(M,A,id)
        d = v.name
        if v.type.endswith("[]"):
            bs = []
            for [left, right] in v.bounds:
                __generate_source_string_for_term(P,M,A,left)
                __generate_source_string_for_term(P,M,A,right)
                bs.append(str(left)+":"+str(right))
            d += "[" + string.join(bs, ",") + "]"
        ds.append(d)
    r += string.join(ds,", ")
    I.source_string = r
    
def __generate_source_string_for_if_then_else_statement(P,M,A,I):
    __generate_source_string_for_term(P,M,A,I.condition)
    __generate_source_strings_for_body(P,M,A,I.then_body)
    __generate_source_strings_for_body(P,M,A,I.else_body)
    I.source_string = u"если "+unicode(I.condition)+u" то "
    if len(I.else_body)>0:
        I.source_string += u"\nиначе"
    I.source_string += u"\nвсе"

def __generate_source_string_for_switch_case_else_statement(P,M,A,I):
    I.source_string = u"выбор"
    for c in I.case_conditions:
        __generate_source_string_for_term(P,M,A,c)
        I.source_string += u"\nпри "+unicode(c)+":"
    for b in I.case_bodies:
        __generate_source_strings_for_body(P,M,A,b)
    __generate_source_strings_for_body(P,M,A,I.else_body)
    if len(I.else_body)>0:
        I.source_string += u"\nиначе"
    I.source_string += u"\nвсе"
    
    
def __generate_source_string_for_loop_statement(P,M,A,I):
    __generate_source_strings_for_body(P, M, A, I.loop_body)
    __generate_source_string_for_term(P, M, A, I.while_condition)
    __generate_source_string_for_term(P, M, A, I.break_condition)
    __generate_source_string_for_term(P, M, A, I.from_term)
    __generate_source_string_for_term(P, M, A, I.to_term)
    __generate_source_string_for_term(P, M, A, I.step_term)
    __generate_source_string_for_term(P, M, A, I.times_term)
    if not I.while_condition is None:
        I.source_string = u"нц пока "+unicode(I.while_condition)
    elif not I.times_term is None:
        I.source_string = u"нц "+unicode(I.times_term)+u" раз"
    elif not I.for_reference is None:
        id = I.for_reference.id
        v = __find_variable(M, A, id)
        I.source_string = u"нц для "+v.name+" "
        if not I.from_term is None:
            I.source_string += u"от "+unicode(I.from_term)
        if not I.to_term is None:
            I.source_string += u" до "+unicode(I.to_term)
        if not I.step_term is None:
            I.source_string += u" шаг "+unicode(I.step_term)
    else:
        I.source_string = u"нц"
    I.source_string += u"\nкц"
    if not I.while_condition is None:
        I.source_string += u" при "+unicode(I.while_condition)
    
def __generate_source_string_for_exit(P,M,A,I):
    I.source_string = u"выход"

def __generate_source_string_for_input(P,M,A,I):
    ts = []
    for t in I.terms:
        __generate_source_string_for_term(P,M,A,t)
        ts.append(unicode(t))
    I.source_string = u"ввод "+string.join(ts,", ")
    
def __generate_source_string_for_output(P,M,A,I):
    ts = []
    for t in I.terms:
        __generate_source_string_for_term(P,M,A,t)
        ts.append(unicode(t))
    I.source_string = u"вывод "+string.join(ts,", ")
        
def __generate_source_string_for_file_input(P,M,A,I):
    ts = []
    for t in I.terms:
        __generate_source_string_for_term(P,M,A,t)
        ts.append(unicode(t))
    I.source_string = u"ф_ввод "+string.join(ts,", ")
    
def __generate_source_string_for_file_output(P,M,A,I):
    ts = []
    for t in I.terms:
        __generate_source_string_for_term(P,M,A,t)
        ts.append(unicode(t))
    I.source_string = u"ф_вывод "+string.join(ts,", ")    

def __generate_source_string_for_assertion(P,M,A,I):
    __generate_source_string_for_term(P,M,A,I.condition)
    I.source_string = u"утв "+unicode(I.condition)
    
def __generate_source_string_for_term(P,M,A,T):
    if T is None:
        return
    clazz = unicode(T.__class__).split(".")[-1].lower()
    m = globals()["__generate_source_string_for_"+clazz]
    m(P,M,A,T)
    
def __generate_source_string_for_expression(P,M,A,T):
    assert len(T.terms)==1
    __generate_source_string_for_term(P,M,A,T.terms[0])
    T.source_string = "("+str(T.terms[0])+")"
    
def __generate_source_string_for_multicompare(P,M,A,T):
    assert len(T.operators)+1 == len(T.terms)
    T.source_string = ""
    for i in range(0,len(T.operators)):
        __generate_source_string_for_term(P,M,A,T.terms[i])
        T.source_string += str(T.terms[i])
        T.source_string += T.operators[i]
    __generate_source_string_for_term(P, M, A, T.terms[-1])
    T.source_string += str(T.terms[-1])
    return T.source_string
                   

def __generate_source_string_for_operator(P,M,A,T):
    ts = []
    for t in T.terms:
        __generate_source_string_for_term(P, M, A, t)
        ts.append(unicode(t))
    if T.operator=="==":
        op = "="
    elif T.operator=="!=":
        op = "<>"
    elif T.operator=="!":
        op = u"не "
    elif T.operator=="||":
        op = u" или "
    elif T.operator=="&&":
        op = u" и "
    elif T.operator=="--":
        op = "-"
    elif T.operator=="++":
        op = "+"
    else:
        op = T.operator
    if len(ts)==1:
        T.source_string = op + unicode(ts[0])
    else:
        T.source_string = string.join(ts,op)

def __generate_source_string_for_constant(P,M,A,T):
    if T.type=="string":
        T.source_string = "\""+T.value+"\""
    elif T.type=="char":
        T.source_string = "'"+T.value+"'"
    elif T.type=="bool":
        if T.value:
            T.source_string = u"да"
        else:
            T.source_string = u"нет"
    else:
        T.source_string = unicode(T.value)
    return T.source_string

def __generate_source_string_for_new_line(P,M,A,T):
    T.source_string = u"нс"
    
def __generate_source_string_for_value(P,M,A,T):
    id = T.id
    v = __find_variable(M, A, id)
    T.source_string = v.name
    
def __generate_source_string_for_return_value(P,M,A,T):
    return u"знач"

def __generate_source_string_for_string_element(P,M,A,T):
    id = T.id
    if id==-1:
        var_name = u"знач"
    else:
        v = __find_variable(M, A, id)
        var_name = v.name
    __generate_source_string_for_term(P, M, A, T.index)
    T.source_string = var_name + "["+unicode(T.index)+"]"
    
def __generate_source_string_for_array_element(P,M,A,T):
    id = T.id
    v = __find_variable(M, A, id)
    inds = []
    for i in T.indeces:
        __generate_source_string_for_term(P, M, A, i)
        inds.append(unicode(i))
    T.source_string = v.name + "["+string.join(inds, ",")+"]"
    
def __generate_source_string_for_string_slice(P,M,A,T):
    id = T.id
    v = __find_variable(M, A, id)
    __generate_source_string_for_term(P, M, A, T.start_index)
    __generate_source_string_for_term(P, M, A, T.end_index)
    T.source_string = v.name + "["+unicode(T.start_index)+":"+unicode(T.end_index)+"]"

def __generate_source_string_for_function_call(P,M,A,T):
    args = []
    for arg in T.arguments:
        __generate_source_string_for_term(P, M, A, arg)
        args.append(unicode(arg))
    T.source_string = T.algorhitm
    if len(args)>0:
        T.source_string += "("+string.join(args, ",")+")"

def __generate_source_string_for_algorhitm(P,M,A):
    r = u"алг "
    if A.return_type=="int":
        r += u"цел "
    elif A.return_type=="double":
        r += u"вещ "
    elif A.return_type=="char":
        r += u"сим "
    elif A.return_type=="string":
        r += u"лит "
    elif A.return_type=="bool":
        r += u"лог "
    r += A.name
    args = []
    for a in A.arguments:
        s = ""
        if a.type.startswith("int"):
            s = u"цел"
        elif a.type.startswith("double"):
            s = u"вещ"
        elif a.type.startswith("char"):
            s = u"сим"
        elif a.type.startswith("string"):
            s = u"лит"
        elif a.type.startswith("bool"):
            s = u"лог"
        if a.type.endswith("[]"):
            s += u"таб"
        s += " "
        m = ""
        if a.is_input_argument:
            m += u"арг"
        if a.is_output_argument:
            m += u"рез"
        n = a.name
        b = ""
        bs = []
        if a.type.endswith("[]"):
            for bound in a.bounds:
                if not bound is None:
                    left, right = bound
                    __generate_source_string_for_term(P, M, A, left)
                    __generate_source_string_for_term(P, M, A, right)
                    bs.append(unicode(left)+":"+unicode(right))
            b = string.join(bs,",")
        decl = m+" "+s+" "+n+b
        args.append(decl)
    if len(args)>0:
        r += "("+string.join(args,", ")+")"
    r += "\n"
    for ia in A.input_assertions:
        __generate_source_string_for_term(P, M, A, ia)
        r += u"\nдано "+unicode(ia)
    for oa in A.input_assertions:
        __generate_source_string_for_term(P, M, A, oa)
        r += u"\nнадо "+unicode(oa)
    r += u"\nнач"
    r += u"\nкон"
    A.source_string = r
    __generate_source_strings_for_body(P, M, A, A.instructions)
    
