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
        | const_decl
X       | sub_def
        | class_def
        ;

    global_var_decl
        : modifiers KW_OUR type name ';'
        | modifiers KW_OUR '(' list ')' ';'            //TODO list
        ;

    const_decl
        : modifiers KW_CONST name '=' expr ';'
        ;

X   sub_def
X       | modifiers type KW_SUB name param_list block
X       | modifiers KW_SUB name param_list block
X       | modifiers type name param_list block
X       | modifiers name param_list block               //not allowed at top level
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
    class_def
        : modifiers TOK_CLASS name inheritance_list '{' class_body '}'
        | modifiers TOK_CLASS name inheritance_list ';'
        ;

    inheritance_list
        : /* empty */
        | TOK_INHERITS superclass_list
        ;

    superclass_list
        : superclass
        | superclass_list ',' superclass
        ;

    superclass
        : modifiers name
        ;

    class_body
        : /* empty */
        | class_body class_member
        ;

    class_member
        : inline_methoddef
        | modifiers '{' member_list '}'
        ;

    inline_methoddef
        | modifiers type IDENTIFIER param_list block
        | modifiers IDENTIFIER param_list base_constructor_list block
        | modifiers type IDENTIFIER param_list ';'
        | modifiers IDENTIFIER param_list ';'
        ;

    base_constructor_list
        : /* empty */
        | ':' base_constructors
        ;

    base_constructors
        : base_constructor
        | base_constructors ',' base_constructor
        ;

    base_constructor
        : name arg_list
        ;

    member_list
        : /* empty */
        | member_list member
        ;

    member
        | modifiers type IDENTIFIER ';'
        | modifiers type IDENTIFIER '=' expr ';'
        | modifiers type IDENTIFIER arg_list ';'
        | const_decl        /* unscoped only */
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

    expr
        : assignment_expr
        | TOK_UNSHIFT expr_list
        | TOK_PUSH expr_list
        | TOK_SPLICE expr_list
        | TOK_EXTRACT expr_list
        | TOK_MAP expr_list
        | TOK_FOLDR expr_list
        | TOK_FOLDL expr_list
        | TOK_SELECT expr_list
        ;

    assignment_expr
        : cond_expr
        | cond_expr assignment_operator assignment_expression
        ;

    cond_expr
        : coalescing_expr
        | coalescing_expr '?' expr ':' cond_expr
        ;

    coalescing_expr:
        : log_or_expr
        | log_or_expr NULL_COALESCING coalescing_expr
        | log_or_expr VALUE_COALESCING coalescing_expr
        ;

    log_or_expr
        : log_and_expr
        | log_or_expr LOGICAL_OR log_and_expr
        ;

    log_and_expr
        : or_expr
        | log_and_expr LOGICAL_AND or_expr
        ;

    or_expr
        : xor_expr
        | or_expr '|' xor_expr
        ;

    xor_expr
        : and_expr
        | xor_expr '^' and_expr
        ;

    and_expr
        : relational_expr
        | and_expr '&' relational_expr
        ;

    relational_expr
        : exists_expr
        | relational_expr relational_operator exists_expr
        | relational_expr REGEX_MATCH REGEX
        | relational_expr REGEX_NMATCH REGEX
        | relational_expr REGEX_MATCH REGEX_SUBST
        | relational_expr REGEX_MATCH REGEX_TRANS
        | relational_expr REGEX_MATCH REGEX_EXTRACT
        ;

    exists_expr
        : instanceof_expr
        | TOK_EXISTS exists_expr
        ;

    instanceof_expr
        : shift_expr
        | instanceof_expr TOK_INSTANCEOF name
        ;

    shift_expr
        : add_expr
        | shift_expr SHIFT_LEFT add_expr
        | shift_expr SHIFT_RIGHT add_expr
        ;

    add_expr
        : mult_expr
        | add_expr '+' mult_expr
        | add_expr '-' mult_expr
        ;

    mult_expr
        : prefix_expr
        | mult_expr '*' prefix_expr
        | mult_expr '/' prefix_expr
        | mult_expr '%' prefix_expr
        ;

    prefix_expr
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
        | KW_NEW name arg_list
        ;

    postfix_expr
X       : primary_expr
X       | postfix_expr arg_list
X       | postfix_expr '[' expr ']'
X       | postfix_expr '{' expr '}'
X       | postfix_expr '.' IDENTIFIER
        | postfix_expr '.' literal      //anything that can be converted to string
X       | postfix_expr '.' '(' expr ')'
X       | postfix_expr '++'
X       | postfix_expr '--'
        ;

    primary_expr
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
        | BACKQUOTE
        | IMPLICIT_ARG_REF
        | IMPLICIT_ELEMENT
X       | KW_SELF
X       | KW_NOTHING
X       | KW_NULL
X       | KW_TRUE
X       | KW_FALSE
        | QFLOAT
X       | INTEGER
        | DATETIME
        | NUMBER
        | BINARY
        | string
        ;

    string
        : QUOTED_WORD
        | QUOTED_WORD string
        ;

X   hash
X       : hash_element
X       | hash ',' hash_element
X       | hash ','
X       ;

X   hash_element
X       : expr ':' expr
X       ;

    relational_operator
        : '>'
        | '<'
        | LOGICAL_NE
        | LOGICAL_LE
        | LOGICAL_GE
        | LOGICAL_CMP
        | LOGICAL_EQ
        | ABSOLUTE_EQ
        | ABSOLUTE_NE
        ;

    assignment_operator
        : PLUS_EQUALS
        | MINUS_EQUALS
        | AND_EQUALS
        | OR_EQUALS
        | MODULA_EQUALS
        | MULTIPLY_EQUALS
        | DIVIDE_EQUALS
        | XOR_EQUALS
        | SHIFT_LEFT_EQUALS
        | SHIFT_RIGHT_EQUALS
        | '='
        ;

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
