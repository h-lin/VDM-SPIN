%{
#include "vdmj.tab.h"
%}
%%
\n                 return EOL;
\r\n               return EOL;
\r                 return EOL;
[ \t]              ;
^[ ]*#.*$          ;
^enter.*$          ;
^leave.*$          ;
"mk_"              return MK;
"true"             return TRUE;
"false"            return FALSE;
"\""               return '"';
"+"                ;
"-"                return '-';
"*"                ;
"("                return '(';
")"                return ')';
"["                return '[';
"]"                return ']';
"{"                return '{';
"}"                return '}';
"&"                ;
"^"                ;
"|"                return '|';
"?"                ;
"="                return '=';
"."                ;
","                return ',';
"<"                return '<';
">"                return '>';
[-]?[0-9]+                    return DIGIT;
[\"][a-zA-Z0-9_]*[\"]         return STR;
[<][a-zA-Z_][a-zA-Z0-9_]*[>]  return QUOTE;
[m][k][_][a-zA-Z_][a-zA-Z0-9_]*   return RECORD;
%%
