import ply.yacc as yacc
from lexer import Lexer
from variable import Variable

class Parser:
    # Constantes para tipos
    NUMB_TYPE = 1
    BOOL_TYPE = 2
    
    def __init__(self):
        self.symtable = {}  # Tabela de símbolos
        self.var_temp_count = 0
        self.generated_code = []  # Código intermediário
        
        # Build the lexer and parser
        self.lexer = Lexer()
        self.lexer.build()
        self.tokens = self.lexer.tokens
        self.parser = yacc.yacc(module=self)
    
    # Precedência de operadores
    precedence = (
        ('right', 'UMINUS'),
        ('left', 'VEZES', 'DIVIDE'),
        ('left', 'MAIS', 'MENOS'),
        ('left', 'IGUAL', 'DIFERENTE', 'MENOR', 'MENOR_IGUAL', 'MAIOR', 'MAIOR_IGUAL')
    )
    
    # Regras gramaticais
    def p_program(self, p):
        '''program : 
                   | program stmt PONTO_VIRGULA'''
        if len(p) > 1:
            self.generated_code.append(p[2]['code'])
    
    def p_stmt(self, p):
        '''stmt : expr
                | ID ATRIBUICAO expr
                | declaration
                | IMPRIME ABRE_PAREN expr FECHA_PAREN'''
        if len(p) == 2:  # expr
            p[0] = p[1]
        elif len(p) == 4:  # ID = expr
            var_name = p[1]
            expr = p[3]
            if var_name not in self.symtable:
                raise NameError(f"Variable '{var_name}' not declared")
            var_type = self.get_var_type(var_name)
            expr_type = expr['type']
            print(f"Debug: Assigning to variable '{var_name}' of type '{var_type}' with expression of type '{expr_type}'")
            if var_type == expr_type:
                self.set_var_value(var_name, expr['val'])
                assign_code = f"{var_name} = {expr['place']}"
                if expr['code']:
                    full_code = f"{expr['code']}\n{assign_code}"
                else:
                    full_code = assign_code
                p[0] = {
                    'code': full_code,
                    'place': var_name,
                    'val': expr['val'],
                    'type': var_type
                }
            else:
                raise TypeError(f"Type mismatch in assignment to '{var_name}'")
        elif len(p) == 5:  # IMPRIME(expr)
            expr = p[3]
            p[0] = {
                'code': f"{expr['code']}\nparam {expr['place']}\ncall imprime",
                'place': '',
                'val': expr['val'],
                'type': expr['type']
            }
    
    def p_declaration(self, p):
        '''declaration : type ID'''
        var_type = p[1]
        var_name = p[2]
        self.declare_var(var_name, var_type)
        p[0] = {'code': '', 'place': var_name, 'type': var_type}
    
    def p_type(self, p):
        '''type : TIPO_INTEIRO
                | TIPO_LOGICO'''
        if p[1] == 'inteiro':
            p[0] = self.NUMB_TYPE
        elif p[1] == 'logico':
            p[0] = self.BOOL_TYPE
    
    def p_expr_binop(self, p):
        '''expr : expr MAIS expr
                | expr MENOS expr
                | expr VEZES expr
                | expr DIVIDE expr
                | expr IGUAL expr
                | expr DIFERENTE expr
                | expr MENOR expr
                | expr MENOR_IGUAL expr
                | expr MAIOR expr
                | expr MAIOR_IGUAL expr'''
        left = p[1]
        right = p[3]
        op = p[2]
    
        # Determinar o tipo do resultado
        if op in ['+', '-', '*', '/']:
            if left['type'] == self.NUMB_TYPE and right['type'] == self.NUMB_TYPE:
                result_type = self.NUMB_TYPE
                val = None
                if op == 'MAIS':
                    val = left['val'] + right['val']
                elif op == 'MENOS':
                    val = left['val'] - right['val']
                elif op == 'VEZES':
                    val = left['val'] * right['val']
                elif op == 'DIVIDE':
                    if right['val'] == 0:
                        raise ZeroDivisionError("Division by zero")
                    val = left['val'] / right['val']
            else:
                raise TypeError(f"Invalid types for '{op}' operation")
        else:  # Operadores de comparação
            if left['type'] != right['type']:
                raise TypeError(f"Type mismatch in '{op}' comparison")
            result_type = self.BOOL_TYPE
            val = None
            if op == 'IGUAL':
                val = left['val'] == right['val']
            elif op == 'DIFERENTE':
                val = left['val'] != right['val']
            elif op == 'MENOR':
                val = left['val'] < right['val']
            elif op == 'MENOR_IGUAL':
                val = left['val'] <= right['val']
            elif op == 'MAIOR':
                val = left['val'] > right['val']
            elif op == 'MAIOR_IGUAL':
                val = left['val'] >= right['val']
        
        temp = self.newtemp()
        # Concatenar os códigos das sub-expressões
        codes = []
        if left['code']:
            codes.append(left['code'])
        if right['code']:
            codes.append(right['code'])
        # Gerar a instrução atual
        codes.append(f"{temp} = {left['place']} {op} {right['place']}")
        full_code = "\n".join(codes)
        
        p[0] = {
            'val': val,
            'type': result_type,
            'place': temp,
            'code': full_code
        }
    
    def p_expr_uminus(self, p):
        '''expr : MENOS expr %prec UMINUS'''
        expr = p[2]
        if expr['type'] == self.NUMB_TYPE:
            temp = self.newtemp()
            val = -expr['val']
            if expr['code']:
                code = f"{expr['code']}\n{temp} = -{expr['place']}"
            else:
                code = f"{temp} = -{expr['place']}"
            p[0] = {
                'val': val,
                'type': self.NUMB_TYPE,
                'place': temp,
                'code': code
            }
        else:
            raise TypeError("Unary minus can only be applied to numbers")
    
    def p_expr_group(self, p):
        '''expr : ABRE_PAREN expr FECHA_PAREN'''
        p[0] = p[2]
    
    def p_expr_number(self, p):
        '''expr : NUMERO'''
        p[0] = {
            'val': p[1],
            'type': self.NUMB_TYPE,
            'place': str(p[1]),
            'code': ''
        }
    
    def p_expr_boolean(self, p):
        '''expr : ID
                | FALSO
                | VERDADEIRO'''
        if p[1] == 'falso':
            p[0] = {
                'val': False,
                'type': self.BOOL_TYPE,
                'place': 'falso',
                'code': ''
            }
        elif p[1] == 'verdadeiro':
            p[0] = {
                'val': True,
                'type': self.BOOL_TYPE,
                'place': 'verdadeiro',
                'code': ''
            }
        else:
            var_name = p[1]
            if var_name not in self.symtable:
                raise NameError(f"Variable '{var_name}' not declared")
            var = self.symtable[var_name]
            p[0] = {
                'val': var.value,
                'type': var.type,
                'place': var_name,
                'code': ''
            }
    
    def p_error(self, p):
        if p:
            print(f"Syntax error at '{p.value}'")
        else:
            print("Syntax error at EOF")
    
    def parse(self, text):
        self.generated_code = []  # Limpar código gerado
        self.parser.parse(text, lexer=self.lexer.lexer)
        
        # Adicionar inicialização das variáveis booleanas
        init_code = "falso = False\nverdadeiro = True"
        self.generated_code.insert(0, init_code)
        
        print("\n".join(self.generated_code)+"\n")
    
    # Funções auxiliares para manipulação de variáveis
    def get_var_value(self, name):
        if name in self.symtable:
            return self.symtable[name].value
        raise NameError(f"Variable '{name}' not found")
    
    def get_var_type(self, name):
        if name in self.symtable:
            return self.symtable[name].type
        raise NameError(f"Variable type not found for '{name}'")
    
    def set_var_value(self, name, value):
        if name in self.symtable:
            self.symtable[name].value = value
        else:
            raise NameError(f"Variable '{name}' not declared")
    
    def declare_var(self, name, var_type):
        if name not in self.symtable:
            self.symtable[name] = Variable(name, 0, var_type)
        else:
            raise NameError(f"Variable '{name}' already declared")
    
    def newtemp(self):
        self.var_temp_count += 1
        return f"t{self.var_temp_count}"