%option noyywrap
%{
#include <glib.h>
static void (*new_token)(guint16 id, char *text, guint16 length);
static int (*next_char)(void);
void token_func(void (*func)(guint16 id, char *text, guint16 length));
void char_func(int (*func)(void));
#define YY_INPUT(buf,result,max_size) \
{ \
  int c = next_char(); \
  result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
}
%}
%%
"//" new_token(0, yytext, yyleng);
(loop|end|if|else|then|begin|is|cursor|open|fetch|close|exit)/(([\t]*\()|[^a-zA-Z0-9_]?) new_token(1, yytext, yyleng);
varchar|varchar2|asc|desc|char|number|date|long|rowid|raw|blob|clob new_token(2, yytext, yyleng);
(#[ \t]*include)[ \t]*(("\""[^<>\n]*"\""?)|("<"[^\"\n]*">"?)) new_token(3, yytext, yyleng);
(select|insert|values|unique|commit|update|delete|from|where|set|and|into|in|order|by|between|create|or|replace|alter|add|datafile|storage) new_token(4, yytext, yyleng);
"//" new_token(5, yytext, yyleng);
"//" new_token(6, yytext, yyleng);
"/*" new_token(7, yytext, yyleng);
"*/" new_token(8, yytext, yyleng);
"//" new_token(9, yytext, yyleng);
(\"|\') new_token(10, yytext, yyleng);
[\n] new_token(11, yytext, yyleng);
"/" new_token(12, yytext, yyleng);
"*" new_token(13, yytext, yyleng);
[ \t]*|[a-zA-Z0-9_]*|[\\+=\-~`!@#$\%%^&()<>\[\]\{\}\|:;?.,]* new_token(14, yytext, yyleng);
%%
void token_func(void (*func)(guint16 id, char *text, guint16 length))
{
	new_token = func;
}
void char_func(int (*func)(void))
{
	next_char = func;
}
