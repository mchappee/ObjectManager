/* start condition definitions */
%x STRING COMMENT1 COMMENT2

/* pattern definitions */
D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			[Ee][+-]?{D}+
FS			(f|F|l|L)
IS			(u|U|l|L)*

%%

^[ \t]*#[ \t]*include[ \t]*((\"[^<>\n]*\"?)|("<"[^\"\n]*">"?)) TOKEN("include", 0);

^[ \t]*#[ \t]*(include|if|ifdef|else|define|endif)  TOKEN("pre_processor", 0);

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
"clob"     TOKEN("type", 0);

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
"close"     TOKEN("area", 0);

"is"        |
"cursor"    |
"exception" |
"when"      |
"return"    |
"fetch"     |
"exit"     TOKEN("keyword", 0);

"/*"		|
"*/"		|
"--"		TOKEN("comment". 0);

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
"replace"   TOKEN("sql",0);

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
"type"             TOKEN("objects",0);

{L}({L}|{D})*		/* identifier ignore */

0[xX]{H}+{IS}?		|
0{D}+{IS}?		|
{D}+{IS}?		|

{D}+{E}{FS}?		|
{D}*"."{D}+({E})?{FS}?	|
{D}+"."{D}*({E})?{FS}?	TOKEN("constant", 0);	

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
"?"   TOKEN("operator", 0);

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
