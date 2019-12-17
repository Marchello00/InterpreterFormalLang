%{
    #include <parser.h>
    #include <interpreter.h>
    #include <string>
    #define YYSTYPE std::string
    #include "interp_bison.hpp"
    #include <syntax_tree.h>
    void yyerror(Interpreter *interpreter, const std::string &s);

    bool set_file(char *filename = nullptr) {
        if (filename) {
            yyin = fopen(filename, "r");
            if (!yyin) { return false; }
            yypush_buffer_state(yy_create_buffer(yyin, YY_BUF_SIZE));
        } else {
            yyin = stdin;
        }
        return true;
    }

    FlexInterpreter flex_interpreter;

    size_t yyread(FILE* file, char* buf, size_t max)
    {
        // Interactive input is signaled by yyin==nullptr.
        if (file == stdin) {
            if (flex_interpreter.completeLine) {
                fputs((flex_interpreter.atStart ?
                    flex_interpreter.ps1.c_str() :
                    flex_interpreter.ps2.c_str()), stdout);
                fflush(stdout);
            }
            char ibuf[max+1];
            size_t len = 0;
            if (fgets(ibuf, max+1, stdin)) {
                len = strlen(ibuf);
                memcpy(buf, ibuf, len);
                // Show the prompt next time if we've read a full line.
                flex_interpreter.completeLine = (ibuf[len-1] == '\n');
            } else if (ferror(stdin)) {
                return -1;
            }
            return len;
        } else { // not interactive
            size_t len = fread(buf, 1, max, file);
            if (len == 0 && ferror(file)) {
                return -1;
            }
            if (len == 0) {
                flex_interpreter.eof = true;
            }
            return len;
        }
    }

    #define YY_INPUT(buf, result, max_size) result = yyread(yyin, buf, max_size)

    #define YY_USER_ACTION  if (!isspace(*yytext)) { flex_interpreter.atStart = false; }
%}

%option yylineno
%option noyywrap

%%

[/][/].*\n              ;
[0-9]+                  {
                            yylval = yytext;
                            return NUM;
                        }
[ \t\r\n]               ;
read_int                return READ_INT;
write                   return WRITE;
int                     return INT;
if                      return IF;
else                    return ELSE;
while                   return WHILE;
for                     return FOR;
[a-zA-Z_][a-zA-Z0-9_]*  {
                            yylval = yytext;
                            return VAR;
                        }
==                      return EQ;
[<]=                    return LESS_EQ;
>=                      return GR_EQ;
[<]                     return LESS;
>                       return GR;
&&                      return AND;
[|][|]                  return OR;
=                       return ASSIGN;
[-*+/{};(),]            return *yytext;
.                       {
                            flex_interpreter.atStart = true;
                            yyerror(nullptr, "Invalid character");
                        }
%%
