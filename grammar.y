    %%
    /////////////////////////////////////////
    // TOP LEVEL && NAMESPACE
    /////////////////////////////////////////
X   script
X       : script_member
X       | script script_member
X       ;

    script_member
X       : namespace_member
        | KW_MODULE IDENTIFIER '{' module_decls '}'
X       | statement
        ;

    module_decls
        : module_decl
        | module_decls module_decl
        ;

    module_decl
        : IDENTIFIER '=' expr ';'
        ;

X   namespace_decl
X       : modifiers KW_NAMESPACE IDENTIFIER '{' namespace_body '}'
X       | modifiers KW_NAMESPACE IDENTIFIER ';'
X       ;

X   namespace_body
X       : /* empty */
X       | namespace_body namespace_member
X       ;

    namespace_member
X       : namespace_decl
        | global_var_decl
X       | modifiers KW_CONST name '=' expr ';'
X       | modifiers type KW_SUB name param_list block
X       | modifiers KW_SUB name param_list block
X       | modifiers type name param_list block
X       | modifiers name param_list block               //not allowed at top level
X       | class_def
        ;

    global_var_decl
        : modifiers KW_OUR type name ';'
        | modifiers KW_OUR '(' list ')' ';'            //TODO list
        ;

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

    class_member
        : modifiers '{' member_list '}'
X       | modifiers type IDENTIFIER param_list block
X       | modifiers IDENTIFIER param_list base_constructor_list block
X       | modifiers type IDENTIFIER param_list ';'
X       | modifiers IDENTIFIER param_list ';'
        ;

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

    member_list
        : /* empty */
        | member_list member
        ;

    member
        | modifiers type IDENTIFIER ';'
        | modifiers type IDENTIFIER '=' expr ';'
        | modifiers type IDENTIFIER arg_list ';'
        | modifiers KW_CONST IDENTIFIER '=' expr ';'
        ;









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

    statement
X       : ';'
X       | block
X       | expr ';'
        | TOK_TRY statement TOK_CATCH '(' type IDENTIFIER ')' statement
        | TOK_RETHROW ';'
        | TOK_THROW expr ';'
        | TOK_ON_EXIT statement
        | TOK_ON_SUCCESS statement
        | TOK_ON_ERROR statement
        | TOK_IF '(' expr ')' statement %prec IFX
        | TOK_IF '(' expr ')' statement TOK_ELSE statement
        | TOK_WHILE '(' expr ')' statement
        | TOK_DO statement TOK_WHILE '(' expr ')' ';'
        | TOK_FOR '(' expr_opt ';' expr_opt ';' expr_opt ')' statement
        | TOK_FOREACH expr TOK_IN expr statement
        | TOK_RETURN expr_opt ';'
        | TOK_THREAD_EXIT ';'
        | TOK_BREAK ';'
        | TOK_CONTINUE ';'
        | TOK_SWITCH '(' expr ')' '{' case_block '}'
        ;

    case_block
        : case_code
        | case_block case_code
        ;

    case_code
        : TOK_CASE LOGICAL_GE expr ':' statements
        | TOK_CASE LOGICAL_LE expr ':' statements
        | TOK_CASE LOGICAL_EQ expr ':' statements
        | TOK_CASE '<' expr ':' statements
        | TOK_CASE '>' expr ':' statements
        | TOK_CASE REGEX_MATCH REGEX ':' statements
        | TOK_CASE REGEX_NMATCH REGEX ':' statements
        | TOK_CASE REGEX ':' statements
        | TOK_CASE expr ':' statements
        | TOK_DEFAULT ':' statements
        ;



















    /////////////////////////////////////////
    // EXPRESSIONS
    /////////////////////////////////////////
    expr_opt
        : /* empty */
        | expr
        ;

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
X       | TOK_UNSHIFT expr_list
X       | TOK_PUSH expr_list
X       | TOK_SPLICE expr_list
X       | TOK_EXTRACT expr_list
X       | TOK_MAP expr_list
X       | TOK_FOLDR expr_list
X       | TOK_FOLDL expr_list
X       | TOK_SELECT expr_list
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
X       : exists_expr
X       | relational_expr '>' exists_expr
X       | relational_expr '<' exists_expr
X       | relational_expr '!=' exists_expr
X       | relational_expr '<>' exists_expr
X       | relational_expr '<=' exists_expr
X       | relational_expr '>=' exists_expr
X       | relational_expr '<=>' exists_expr
X       | relational_expr '==' exists_expr
X       | relational_expr '===' exists_expr
X       | relational_expr '!==' exists_expr
X       | relational_expr '=~' REGEX
X       | relational_expr '!~' REGEX
X       | relational_expr '=~' REGEX_SUBST
X       | relational_expr '=~' REGEX_TRANS
X       | relational_expr '=~' REGEX_EXTRACT
X       ;

X   exists_expr
X       : instanceof_expr
X       | KW_EXISTS exists_expr
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
