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
        | sub_def
        | class_def
        ;

    global_var_decl
        : modifiers KW_OUR type name ';'
        | modifiers KW_OUR '(' list ')' ';'            //TODO list
        ;

    const_decl
        : modifiers KW_CONST name '=' expr ';'
        ;

    sub_def
        | modifiers type_opt KW_SUB name '(' param_list ')' block
        | modifiers type_opt name '(' param_list ')' base_constructor_list block
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
        | modifiers type_opt IDENTIFIER '(' param_list ')' base_constructor_list block
        | modifiers type_opt IDENTIFIER '(' param_list ')' ';'
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
        : name '(' arg_list ')'
        ;

    member_list
        : /* empty */
        | member_list member
        ;

    member
        | modifiers type IDENTIFIER ';'
        | modifiers type IDENTIFIER '=' expr ';'
        | modifiers type IDENTIFIER '(' arg_list ')' ';'
        | const_decl        /* unscoped only */
        ;









    /////////////////////////////////////////
    // STATEMENTS
    /////////////////////////////////////////
    block
        : '{' statements '}'
        ;

    statements
        : /* empty */
        | statements statement
        ;

    statement
X       : ';'
        | block
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

    arg_list
        : expr_opt
        ;

    expr
        : list_expr
        | TOK_UNSHIFT list_expr
        | TOK_PUSH list_expr
        | TOK_SPLICE list_expr
        | TOK_EXTRACT list_expr
        | TOK_MAP list_expr
        | TOK_FOLDR list_expr
        | TOK_FOLDL list_expr
        | TOK_SELECT list_expr
        ;

    list_expr
        : assignment_expr
        | assignment_expr ','
        | list_expr ',' assignment_expr
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
        : unary_expr
        | mult_expr '*' unary_expr
        | mult_expr '/' unary_expr
        | mult_expr '%' unary_expr
        ;

    unary_expr
        : postfix_expr
        | P_INCREMENT unary_expr
        | P_DECREMENT unary_expr
        | unary_operator unary_expr
        | TOK_NEW name '(' arg_list ')'
        ;

    postfix_expr
        : primary_expr
        | postfix_expr '(' arg_list ')'
        | postfix_expr '[' expr ']'
        | postfix_expr '{' expr '}'
    //    | QORE_CAST '(' expr ')'
        | postfix_expr '.' member_accessor
        | postfix_expr P_INCREMENT
        | postfix_expr P_DECREMENT
        ;

    primary_expr
X       : name
X       | '(' expr ')'
X       | '(' hash ')'
X       | '(' ')'
X       | '{' '}'
X       | '{' hash '}'
        | type IDENTIFIER
        | BACKQUOTE
        | IMPLICIT_ARG_REF
        | IMPLICIT_ELEMENT
X       | KwSelf
X       | KwNothing
X       | KwNull
X       | KwTrue
X       | KwFalse
        | QFLOAT
X       | INTEGER
        | DATETIME
        | NUMBER
        | BINARY
        | string
    //lambda:       modifiers type_opt TOK_SUB '(' param_list ')' block
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
X       : expr ':' assignment_expr
X       ;

    unary_operator:
        : TOK_ELEMENTS
        | TOK_KEYS
        | TOK_SHIFT
        | TOK_POP
        | TOK_CHOMP
        | TOK_TRIM
        | TOK_BACKGROUND
        | TOK_DELETE
        | TOK_REMOVE
        | '-'
        | '+'
        | '~'
        | '!'
        | '\\'
        ;

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

    member_accessor
        : IDENTIFIER
        | literal                //anything that can be converted to string
        | '(' expr ')'
        ;

X   name
X       : IDENTIFIER
X       | IDENTIFIER '::' name
X       ;

    type
        : IDENTIFIER
        | SCOPED_REF
        | '*' IDENTIFIER
        | '*' SCOPED_REF
        ;

    type_opt
        : /* empty */           //this is possible only as return type of a function/closure/method and means 'nothing'
        | type
        ;

    param_list
        : /* empty */
        | params
        ;

    params
        : param
        | param ','
        | param ',' params
        ;

    param
        : type IDENTIFIER
        | type IDENTIFIER '=' expr
        ;