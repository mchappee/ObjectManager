/* start condition definitions */
%x STRING COMMENT1 COMMENT2

/* pattern definitions */
D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*

%option fast always-interactive noyywrap prefix="goh"
%option noyy_scan_buffer noyy_scan_bytes noyy_scan_string

%{

#define FALSE 0
#define TRUE 1

#define YY_INPUT(buf,result,max_size) \
{ \
  int c = next_char(); \
  result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
}

#define YY_DECL int lex ()
#define YY_USER_ACTION position += prev_leng; prev_leng = yyleng;


static void (*new_token)(int id, int precedence, char *text, int length, int position, int block_id, int nesting, int block_type);

static int (*next_char)();

%}

%%

%{
int position = 0, prev_leng = 0;
%}



^[ \t]*#[ \t]*include[ \t]*((\"[^<>\n]*\"?)|("<"[^\"\n]*">"?)) new_token (0,  0, yytext, yyleng, position, -1, 0, 0);

^[ \t]*#[ \t]*(include|if|ifdef|else|define|endif)  new_token (1,  0, yytext, yyleng, position, -1, 0, 0);

"varchar"  |
"varchar2" |
"asc"      |
"desc"     |
"char"     |
"number"   |
"date"     |
"long"     |
"rowid"    |
"raw"      |
"blob"     |
"clob"     new_token (2,  0, yytext, yyleng, position, -1, 0, 0);

"loop"      |
"end"       |
"if"        |
"else"      |
"then"      |
"begin"     |
"elseif"    |
"open"      |
"putline"   |
"getline"   |
"put"       |
"get"       |
"for"       |
"close"     new_token (3,  0, yytext, yyleng, position, -1, 0, 0);

"is"        |
"cursor"    |
"exception" |
"when"      |
"return"    |
"fetch"     |
"exit"     new_token (4,  0, yytext, yyleng, position, -1, 0, 0);

"/*"		|
"*/"		|
"--"		new_token (5,  0, yytext, yyleng, position, -1, 0, 0);

"select"        |
"insert"        |
"update"        |
"delete"        |
"values"        |
"unique"        |
"commit"        |
"from"          |
"where"         |
"set"           |
"and"           |
"into"          |
"storage"       |
"in"            |
"min"           |
"max"           |
" m "           |
" k "           |
"order"         |
"by"            |
"between"       |
"create"        |
"or"            |
"no_data_found" |
"dbms_output"   |
"alter"         |
"as"            |
"replace"   new_token (6, 0, yytext, yyleng, position, -1, 0, 0);

"table"            |
"index"            |
"view"             |
"datafile"         |
"function"         |
"package"          |
"procedure"        |
"rollback segment" |
"sequence"         |
"session"          |
"synonym"          |
"tablespace"       |
"trigger"          |
"type"             new_token (7, 0, yytext, yyleng, position, -1, 0, 0);

{L}({L}|{D})*		/* identifier ignore */

0[xX]{H}+{IS}?		|
0{D}+{IS}?		|
{D}+{IS}?		|

{D}+{E}{FS}?		|
{D}*"."{D}+({E})?{FS}?	|
{D}+"."{D}*({E})?{FS}?	new_token (8,  0, yytext, yyleng, position, -1, 0, 0);	

">>=" |
"<<=" |
"+="  |
"-="  |
"*="  |
"/="  |
"%="  |
"&="  |
"^="  |
"|="  |
">>"  |
"<<"  |
"++"  |
"->"  |
"&&"  |
"||"  |
"<="  |
">="  |
"=="  |
"!="  |
"="   |
"&"   |
"!"   |
"~"   |
"-"   |
"+"   |
"*"   |
"/"   |
"%"   |
"<"   |
">"   |
"^"   |
"|"   |
"?"   new_token (9,  0, yytext, yyleng, position, -1, 0, 0);

";"
"{"
"}"
","
":"

"("
")"
"["
"]"
"."


[ \t\v\n\f]		
.	     /* ignore bad characters */


%%


const char *
get_name ()

{
  const char *name = NULL;

  return name;
}


const char **
get_token_names ()

{
  static const char *token_names[] = {"include", "pre_processor", "type", "area", "keyword", "comment", "sql", "objects", "constant", "operator", NULL};

  return token_names;
}

const char **
get_block_names ()

{
  static const char *block_names[] = {NULL};
  return block_names;
}


int
get_token_no (const char *name)

{
  int number = -1;

  if (strcmp("include", name) == 0) {
    return 0;
  } else if (strcmp("pre_processor", name) == 0) {
    return 1;
  } else if (strcmp("type", name) == 0) {
    return 2;
  } else if (strcmp("area", name) == 0) {
    return 3;
  } else if (strcmp("keyword", name) == 0) {
    return 4;
  } else if (strcmp("comment", name) == 0) {
    return 5;
  } else if (strcmp("sql", name) == 0) {
    return 6;
  } else if (strcmp("objects", name) == 0) {
    return 7;
  } else if (strcmp("constant", name) == 0) {
    return 8;
  } else if (strcmp("operator", name) == 0) {
    return 9;
  }

  return number;
}


int
get_block_no (const char *name)

{
  int number = -1;

  return number;
}

void
token_func (void (*func)(int id, int precedence, char *text, int length, int position, int block_id, int nesting, int block_type))
{
  new_token = func;
}

void
char_func (int (*func)())
{
  next_char = func;
}

int
get_state ()
{
  return YY_START;
}

void
set_state (int state)
{
  BEGIN (state);
}