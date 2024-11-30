import ply.lex as lex

class Lexer:
    # Lista de palavras reservadas
    reserved = {
        'imprime': 'IMPRIME',
        'falso': 'FALSO',
        'verdadeiro': 'VERDADEIRO',
        'inteiro': 'TIPO_INTEIRO',
        'logico': 'TIPO_LOGICO'
    }

    # Lista de tokens
    tokens = [
        'ID',
        'NUMERO',
        # Operadores Aritméticos
        'MAIS',
        'MENOS',
        'VEZES',
        'DIVIDE',
        # Operadores Relacionais
        'MAIOR',
        'MENOR',
        'IGUAL',
        'DIFERENTE',
        'MAIOR_IGUAL',
        'MENOR_IGUAL',
        # Outros
        'ATRIBUICAO',
        'ABRE_PAREN',
        'FECHA_PAREN',
        'QUEBRA_LINHA',
        'PONTO_VIRGULA'
    ] + list(reserved.values())

    # Regras para tokens simples
    t_MAIS = r'\+'
    t_MENOS = r'-'
    t_VEZES = r'\*'
    t_DIVIDE = r'/'
    t_MAIOR = r'>'
    t_MENOR = r'<'
    t_IGUAL = r'=='
    t_DIFERENTE = r'!='
    t_MAIOR_IGUAL = r'>='
    t_MENOR_IGUAL = r'<='
    t_ATRIBUICAO = r'='
    t_ABRE_PAREN = r'\('
    t_FECHA_PAREN = r'\)'
    t_QUEBRA_LINHA = r'\n'
    t_PONTO_VIRGULA = r';'

    # Ignora espaços e tabs
    t_ignore = ' \t'

    def __init__(self):
        self.lexer = lex.lex(module=self)

    # Regra para números inteiros
    def t_NUMERO(self, t):
        r'\d+'
        t.value = int(t.value)
        return t

    # Regra para identificadores e palavras reservadas
    def t_ID(self, t):
        r'[a-zA-Z_][a-zA-Z0-9_]*'
        t.type = self.reserved.get(t.value, 'ID')
        return t

    # Contador de linhas
    def t_newline(self, t):
        r'\n+'
        t.lexer.lineno += len(t.value)

    # Tratamento de erros
    def t_error(self, t):
        print(f"Caractere ilegal '{t.value[0]}' na linha {t.lexer.lineno}")
        t.lexer.skip(1)

    # Função para teste
    def test(self, data):
        self.lexer.input(data)
        while True:
            tkn = self.lexer.token()
            if not tkn:
                break
            print(tkn)

    def build(self, **kwargs):
        self.lexer = lex.lex(module=self, **kwargs)