    %%
    /////////////////////////////////////////
    // TOP LEVEL && NAMESPACE
    /////////////////////////////////////////
X   script
X       : script_member
X       | script script_member
X       ;

X   script_member
X       : namespace_member
X       | KW_MODULE IDENTIFIER '{' module_decls '}'
X       | statement
X       ;

X   module_decls
X       : module_decl
X       | module_decls module_decl
X       ;

X   module_decl
X       : IDENTIFIER '=' expr ';'
X       ;

X   namespace_decl
X       : modifiers KW_NAMESPACE IDENTIFIER '{' namespace_body '}'
X       | modifiers KW_NAMESPACE IDENTIFIER ';'
X       ;

X   namespace_body
X       : /* empty */
X       | namespace_body namespace_member
X       ;

X   namespace_member
X       : namespace_decl
X       | global_var_decl
X       | modifiers KW_CONST name '=' expr ';'
X       | modifiers type KW_SUB name param_list block
X       | modifiers KW_SUB name param_list block
X       | modifiers type name param_list block
X       | modifiers name param_list block               //not allowed at top level
X       | class_def
X       ;

X   global_var_decl
X       : modifiers KW_OUR type name ';'
X       ;

X   modifiers
X       : /* empty */
X       | modifiers modifier
X       ;

X   modifier
X       : KW_PRIVATE
X       | KW_STATIC
X       | KW_SYNCHRONIZED
X       | KW_DEPRECATED
X       | KW_PUBLIC
X       | KW_FINAL
X       | KW_ABSTRACT
X       ;

    /////////////////////////////////////////
    // CLASSES
    /////////////////////////////////////////
X   class_def
X       : modifiers TOK_CLASS name inheritance_list '{' class_body '}'
X       | modifiers TOK_CLASS name inheritance_list ';'
X       ;

X   inheritance_list
X       : /* empty */
X       | TOK_INHERITS superclass_list
X       ;

X   superclass_list
X       : superclass
X       | superclass_list ',' superclass
X       ;

X   superclass
X       : modifiers name
X       ;

X   class_body
X       : /* empty */
X       | class_body class_member
X       ;

X   class_member
X       : modifiers '{' member_list '}'
X       | modifiers type IDENTIFIER param_list block
X       | modifiers IDENTIFIER param_list base_constructor_list block
X       | modifiers type IDENTIFIER param_list ';'
X       | modifiers IDENTIFIER param_list ';'
X       ;

X   base_constructor_list
X       : /* empty */
X       | ':' base_constructors
X       ;

X   base_constructors
X       : base_constructor
X       | base_constructors ',' base_constructor
X       ;

X   base_constructor
X       : name arg_list
X       ;

X   member_list
X       : /* empty */
X       | member_list member
X       ;

X   member
X       | modifiers type IDENTIFIER ';'
X       | modifiers type IDENTIFIER '=' expr ';'
X       | modifiers type IDENTIFIER arg_list ';'
X       | modifiers KW_CONST IDENTIFIER '=' expr ';'
X       ;









    /////////////////////////////////////////
    // STATEMENTS
    /////////////////////////////////////////
X   block
X       : '{' statements '}'
X       ;

X   statements
X       : /* empty */
X       | statements statement
X       ;

X   statement
X       : ';'
X       | block
X       | expr ';'
X       | KW_TRY statement KW_CATCH '(' type IDENTIFIER ')' statement
X       | KW_RETHROW ';'
X       | KW_THROW expr_list ';'
X       | KW_ON_EXIT statement
X       | KW_ON_SUCCESS statement
X       | KW_ON_ERROR statement
X       | KW_IF '(' expr ')' statement
X       | KW_IF '(' expr ')' statement KW_ELSE statement
X       | KW_WHILE '(' expr ')' statement
X       | KW_DO statement KW_WHILE '(' expr ')' ';'
X       | KW_FOR '(' expr_opt ';' expr_opt ';' expr_opt ')' statement
X       | KW_FOREACH expr KW_IN '(' expr ')' statement
X       | KW_RETURN expr_opt ';'
X       | KW_THREAD_EXIT ';'
X       | KW_BREAK ';'
X       | KW_CONTINUE ';'
X       | KW_SWITCH '(' expr ')' '{' case_block '}'
X       ;

X   case_block
X       : case_code
X       | case_block case_code
X       ;

X   case_code
X       : KW_CASE '>=' expr ':' statements
X       | KW_CASE '<' expr ':' statements
X       | KW_CASE '==' expr ':' statements
X       | KW_CASE '<' expr ':' statements
X       | KW_CASE '>' expr ':' statements
X       | KW_CASE '=~' REGEX ':' statements
X       | KW_CASE '!~' REGEX ':' statements
X       | KW_CASE REGEX ':' statements
X       | KW_CASE expr ':' statements
X       | KW_DEFAULT ':' statements
X       ;

X   expr_opt
X       : /* empty */
X       | expr
X       ;


















    /////////////////////////////////////////
    // EXPRESSIONS
    /////////////////////////////////////////
X   arg_list
X       : '(' ')'
X       | '(' expr_list ')'
X       ;

X   expr_list
X       : expr
X       | expr_list ',' expr
X       ;

X   expr
X       : assignment_expr
X       ;

X   assignment_expr
X       : cond_expr
X       | cond_expr '=' assignment_expression
X       | cond_expr '+=' assignment_expression
X       | cond_expr '-=' assignment_expression
X       | cond_expr '&=' assignment_expression
X       | cond_expr '|=' assignment_expression
X       | cond_expr '%=' assignment_expression
X       | cond_expr '*=' assignment_expression
X       | cond_expr '/=' assignment_expression
X       | cond_expr '^=' assignment_expression
X       | cond_expr '<<=' assignment_expression
X       | cond_expr '>>=' assignment_expression
X       ;

X   cond_expr
X       : coalescing_expr
X       | coalescing_expr '?' expr ':' cond_expr
X       ;

X   coalescing_expr:
X       : log_or_expr
X       | log_or_expr '??' coalescing_expr
X       | log_or_expr '?*' coalescing_expr
X       ;

X   log_or_expr
X       : log_and_expr
X       | log_or_expr '||' log_and_expr
X       ;

X   log_and_expr
X       : or_expr
X       | log_and_expr '&&' or_expr
X       ;

X   or_expr
x       : xor_expr
X       | or_expr '|' xor_expr
X       ;

X   xor_expr
X       : and_expr
X       | xor_expr '^' and_expr
X       ;

X   and_expr
X       : relational_expr
X       | and_expr '&' relational_expr
X       ;

X   relational_expr
X       : instanceof_expr
X       | relational_expr '>' instanceof_expr
X       | relational_expr '<' instanceof_expr
X       | relational_expr '!=' instanceof_expr
X       | relational_expr '<>' instanceof_expr
X       | relational_expr '<=' instanceof_expr
X       | relational_expr '>=' instanceof_expr
X       | relational_expr '<=>' instanceof_expr
X       | relational_expr '==' instanceof_expr
X       | relational_expr '===' instanceof_expr
X       | relational_expr '!==' instanceof_expr
X       | relational_expr '=~' REGEX
X       | relational_expr '!~' REGEX
X       | relational_expr '=~' REGEX_SUBST
X       | relational_expr '=~' REGEX_TRANS
X       | relational_expr '=~' REGEX_EXTRACT
X       ;

X   instanceof_expr
X       : shift_expr
X       | instanceof_expr KW_INSTANCEOF name
X       ;

X   shift_expr
X       : add_expr
X       | shift_expr '<<' add_expr
X       | shift_expr '>>' add_expr
X       ;

X   add_expr
X       : mult_expr
X       | add_expr '+' mult_expr
X       | add_expr '-' mult_expr
X       ;

X   mult_expr
X       : prefix_expr
X       | mult_expr '*' prefix_expr
X       | mult_expr '/' prefix_expr
X       | mult_expr '%' prefix_expr
X       ;

X   prefix_expr
X       : postfix_expr
X       | '+' prefix_expr
X       | '-' prefix_expr
X       | '~' prefix_expr
X       | '!' prefix_expr
X       | '\' prefix_expr
X       | '++' prefix_expr
X       | '--' prefix_expr
X       | KW_ELEMENTS prefix_expr
X       | KW_KEYS prefix_expr
X       | KW_SHIFT prefix_expr
X       | KW_POP prefix_expr
X       | KW_CHOMP prefix_expr
X       | KW_TRIM prefix_expr
X       | KW_BACKGROUND prefix_expr
X       | KW_DELETE prefix_expr
X       | KW_REMOVE prefix_expr
X       | KW_EXISTS instanceof_expr
X       | KW_UNSHIFT expr_list
X       | KW_PUSH expr_list
X       | KW_SPLICE expr_list
X       | KW_EXTRACT expr_list
X       | KW_MAP expr_list
X       | KW_FOLDR expr_list
X       | KW_FOLDL expr_list
X       | KW_SELECT expr_list
X       ;

    postfix_expr
X       : primary_expr
X       | postfix_expr arg_list
X       | postfix_expr '[' expr ']'
X       | postfix_expr '{' expr '}'
X       | postfix_expr '.' primary_expr
X       | postfix_expr '++'
X       | postfix_expr '--'
        ;

X   primary_expr
X       : name
X       | type IDENTIFIER
X       | modifiers type KW_SUB param_list block
X       | modifiers KW_SUB param_list block
X       | '(' expr ')'
X       | '(' expr_list ',' ')'
X       | '(' expr_list ')'
X       | '(' hash ')'
X       | '(' ')'
X       | '{' '}'
X       | '{' hash '}'
X       | KW_CAST '<' type '>' '(' expr ')'
X       | KW_NEW name arg_list
X       | BACKQUOTE
X       | IMPLICIT_ARG_REF
X       | IMPLICIT_ELEMENT
X       | KW_SELF
X       | KW_NOTHING
X       | KW_NULL
X       | KW_TRUE
X       | KW_FALSE
X       | QFLOAT
X       | INTEGER
X       | ABSOLUTE_DATE
X       | RELATIVE_DATE
X       | NUMBER
X       | BINARY
X       | string
X       ;

X   string
X       : STRING
X       | STRING string
X       ;

X   hash
X       : hash_element
X       | hash ',' hash_element
X       | hash ','
X       ;

X   hash_element
X       : expr ':' expr
X       ;

X   name
X       : IDENTIFIER
X       | IDENTIFIER '::' name
X       ;

X   type
X       : name
X       | '*' name
X       ;

X   param_list
X       : '(' ')'
X       | '(' params ')'
X       ;

X   params
X       : param
X       | param ',' params
X       ;

X   param
X       : type IDENTIFIER
X       | type IDENTIFIER '=' expr
X       ;
