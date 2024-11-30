import lexer
import parser

def read_file(file_path):
    with open(file_path, 'r') as file:
        return file.read()

def main():
    file_path = 'inputs/input4.txt'
    data = read_file(file_path)

    # Inicializar o lexer
    lex = lexer.Lexer()
    lex.build()

    # Inicializar o parser
    parser_instance = parser.Parser()

    # Analisar o texto
    parser_instance.parse(data)

if __name__ == '__main__':
    main()