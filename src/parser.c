#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "parser.h"

static inline lexer_T* set_token_back(lexer_T* lexer, int ammount) {

    lexer->i -= ammount;
    lexer->c = lexer->contents[lexer->i];

    return lexer;
}
static inline void PrintErr(parser_T* parser) {
    printf("\n\nERR: Type of %s is not valid for print declaration. Types S, I, C and ~ are for variable declarations.\n\n",parser->current_token->value);
    exit(1);
}
static inline TypeAndValue* UpdTAV(TypeAndValue* TAV) {

    if(
        /*If ForSetup.ValueIndex is nil, all the rest will be nil*/
        TAV->ForSetup.ValueIndex==(void*)0
    ) {
        TAV->ForSetup.ValueIndex=0, TAV->ForSetup.TypeIndex=0;
        TAV->ForPrint.ValueIndex=0,TAV->ForPrint.TypeIndex=0;  
    }

    return TAV;
}

parser_T* init_parser(lexer_T* lexer) {
    parser_T* parser = calloc(1,sizeof(struct PARSER_STRUCT));

    TypeAndValue* TAV = calloc(1,sizeof(TypeAndValue));
    UpdTAV(TAV);
    
    parser->lexer = lexer;
    parser->current_token = lexer_get_next_token(lexer);
    parser->prev_token = parser->current_token;

    return parser;
}
void parser_eat(TypeAndValue* TAV,parser_T* parser, int token_type) {
    
    if(strcmp(parser->current_token->value,"print")==0) {
        parser->prev_token = parser->current_token;
        parser->current_token = lexer_get_next_token(parser->lexer);

        if(parser->current_token->type==TOKEN_LSQRBRACK) {
            parser->current_token = lexer_get_next_token(parser->lexer);
        } else {
            printf("\n\nERR: the print function needs params: [TYPE](ItemToPrint). Missing [TYPE].\n\n");
            exit(1);
        }

        /*
            * S, I, C and ~ are all declaraction types. The print method will have 
            * type Str:String, Char:Char, Int:Integer or empty square brackets 
            for print type any 
        */
        
        // Getting type. Cannot be just S
        char type[12];
        if(strcmp(parser->current_token->value,"S")==0||strcmp(parser->current_token->value,"s")==0) {
            type[0]=parser->current_token->value[0];
            parser->current_token = lexer_get_next_token(parser->lexer);
            type[1]=parser->current_token->value[0];
            set_token_back(parser->lexer,2);
        }
        if(strcmp(parser->current_token->value,"I")==0||strcmp(parser->current_token->value,"i")==0) {
            type[0]=parser->current_token->value[0];
            parser->current_token = lexer_get_next_token(parser->lexer);
            type[1]=parser->current_token->value[0];
            set_token_back(parser->lexer,2);
        }
        if(strcmp(parser->current_token->value,"C")==0||strcmp(parser->current_token->value,"c")==0) {
            type[0]=parser->current_token->value[0];
            parser->current_token = lexer_get_next_token(parser->lexer);
            type[1]=parser->current_token->value[0];
            set_token_back(parser->lexer,2);
        }

        if(strcmp(type,"St")==0||strcmp(type,"st")==0) {
            printf("String ~ ");
            strcpy(TAV->ForPrint.Type[TAV->ForPrint.TypeIndex],parser->current_token->value);
            parser->current_token = lexer_get_next_token(parser->lexer);
        } else if(strcmp(type,"In")==0||strcmp(type,"in")==0) {
            printf("Integer ~ ");
            strcpy(TAV->ForPrint.Type[TAV->ForPrint.TypeIndex],parser->current_token->value);
            parser->current_token = lexer_get_next_token(parser->lexer);
        } else if(strcmp(type,"Ch")==0||strcmp(type,"ch")==0) {
            printf("Character ~ ");
            strcpy(TAV->ForPrint.Type[TAV->ForPrint.TypeIndex],parser->current_token->value);
            parser->current_token = lexer_get_next_token(parser->lexer);
        } else {
            if(parser->current_token->type!=TOKEN_RSQRBRACK) {
                printf("\n\nErr: The type '%s' is not valid.\nHint: If you don't want to print a certain type, leave the [TYPE] empty.\n\n",parser->current_token->value);
                exit(1);
            }
            printf("Any ~ ");
            /*
                *****************************
                * No need to get the new    *
                * token, there is no type   *
                * found, therefore we would *
                * be getting the token of ] *
                * which would mess up the   *
                *     application.          *
                ***************************

                Setting the current_token's type to zero so we don't run into undefined token error.
            */
            strcpy(TAV->ForPrint.Type[TAV->ForPrint.TypeIndex],"Any");
            parser->current_token->type=0;
        }

        if(parser->current_token->type==TOKEN_RSQRBRACK) {
            //parser->prev_token = parser->current_token;
            parser->current_token = lexer_get_next_token(parser->lexer);
        }
    }

    if(parser->current_token->type==TOKEN_PRESET) {
        parser->current_token = lexer_get_next_token(parser->lexer);
    } else {
        if(parser->current_token->type == token_type) {
            if(strcmp(parser->current_token->value,"~")==0) {
                strcpy(TAV->ForSetup.Type[TAV->ForSetup.TypeIndex],"Any");
            }
            parser->current_token = lexer_get_next_token(parser->lexer);
        } else {
            printf("Unexpected token '%s' with type %d",parser->current_token->value,parser->current_token->type);
            exit(1);
        }
    }
}
AST_T* parser_parse(parser_T* parser) {
    TypeAndValue* TAV = calloc(1,sizeof(TypeAndValue));

    return parser_parse_statements(TAV,parser);
}
AST_T* parser_parse_statement(parser_T* parser) {
    //parser->current_token->type=0;
    switch(parser->current_token->type) {
        case TOKEN_ID: return parser_parse_id(parser);
        case TOKEN_PRESET: return parser_parse_preVar(parser);
        case TOKEN_PRESET_TYPE_SETVAR: return parser_parse_preVar(parser);
    }
    /*if(parser->current_token->value[0]=='c') {
        parser->current_token->type=0;
        return parser_parse_id(parser);
    }*/

    return init_ast(AST_NOOP);
}
AST_T* parser_parse_preVar(parser_T* parser) {

    TypeAndValue* TAV = calloc(1,sizeof(TypeAndValue));
    UpdTAV(TAV);

    if(parser->current_token->type==TOKEN_PRESET)
        parser_eat(TAV,parser,TOKEN_PRESET);
    
    if(parser->current_token->type==TOKEN_PRESET_TYPE_SETVAR)
        parser_eat(TAV,parser,TOKEN_PRESET_TYPE_SETVAR);
    
    if(parser->current_token->type==TOKEN_LSQRBRACK) {
        printf("\n\n\033[1;35mWarning:\033[0m # directives do not need [TYPE].\n\n");
        parser_eat(TAV,parser,TOKEN_LSQRBRACK);
    }
    if(parser->current_token->type==TOKEN_RSQRBRACK)
        parser_eat(TAV,parser,TOKEN_RSQRBRACK);
    
    char* PreVarName = parser->current_token->value;
    
    AST_T* variable_definition_value = parser_parse_expr(parser);
    AST_T* variable_definition = init_ast(AST_VARIABLE_DEFINITION);

    variable_definition->variable_definition_variable_name = PreVarName;
    variable_definition->variable_definition_value = parser->current_token->value;
    
    /* We need to make sure to get the token after the string and semi colon. */
    parser_eat(TAV,parser,TOKEN_STRING);
    parser_eat(TAV,parser,TOKEN_SEMI);

    parser_parse_id(parser);
    
    return parser;
}
AST_T* parser_parse_statements(TypeAndValue* TAV,parser_T* parser) {

    AST_T* compound = init_ast(AST_COMPOUND);
    compound->compound_value = calloc(1,sizeof(struct AST_STRUCT));

    AST_T* ast_statement = parser_parse_statement(parser);
    compound->compound_value[0] = ast_statement;
    compound->compound_size++;

    while(parser->current_token->type == TOKEN_SEMI) {
        parser_eat(TAV,parser, TOKEN_SEMI);

        AST_T* ast_statement = parser_parse_statement(parser);

        if(ast_statement) {
            compound->compound_size++;
            compound->compound_value = realloc(compound->compound_value,compound->compound_size*sizeof(struct AST_STRUCT));
            compound->compound_value[compound->compound_size-1] = ast_statement;
        }
    }

    return compound;
}
AST_T* parser_parse_expr(parser_T* parser) {
    TypeAndValue* TAV = calloc(1,sizeof(TypeAndValue));

    switch(parser->current_token->type) {
        case TOKEN_STRING: return parser_parse_string(TAV,parser);
        case TOKEN_ID: return parser_parse_id(parser);
    }

    return init_ast(AST_NOOP);
}
AST_T* parser_parse_factor(parser_T* parser) {

}
AST_T* parser_parse_term(parser_T* parser) {

}
AST_T* parser_parse_function_call(parser_T* parser) {
    AST_T* function_call = init_ast(AST_FUNCTION_CALL);
    TypeAndValue* TAV = calloc(1,sizeof(TypeAndValue));
    UpdTAV(TAV);

    parser_eat(TAV, parser, TOKEN_LPARENT);

    function_call->function_call_name = parser->prev_token->value;

    function_call->function_call_arguments = calloc(1,sizeof(struct AST_STRUCT));
    AST_T* ast_expr = parser_parse_expr(parser);
    function_call->function_call_arguments[0] = ast_expr;
    function_call->function_call_arguments_size++;

    while(parser->current_token->type == TOKEN_COMMA) {
        parser_eat(TAV,parser, TOKEN_COMMA);

        AST_T* ast_expr = parser_parse_expr(parser);
        function_call->function_call_arguments_size++;
        function_call->function_call_arguments = realloc(function_call->function_call_arguments,function_call->function_call_arguments_size*sizeof(struct AST_STRUCT));
        function_call->function_call_arguments[function_call->function_call_arguments_size-1] = ast_expr;
    }

    parser_eat(TAV, parser, TOKEN_RPARENT);

    return function_call;
}
AST_T* parser_parse_variable_definition(parser_T* parser) {
    TypeAndValue* TAV = calloc(1,sizeof(TypeAndValue));
    UpdTAV(TAV);

    parser_eat(TAV,parser, TOKEN_ID);

    if(parser->current_token->type == TOKEN_LSQRBRACK)
        parser_eat(TAV,parser,TOKEN_LSQRBRACK);
    else if(!(parser->current_token->type == TOKEN_LSQRBRACK))
        if(!(parser->current_token->type==TOKEN_TYPE_ANY)) {
            printf("\n\nERR: Must have '[' and ']' surrounding type other than type Any '~'\n\n");
            exit(1);
        }
    if(parser->current_token->type==TOKEN_TYPE_STRING) /* = [S] or [s]*/
        parser_eat(TAV,parser,TOKEN_TYPE_STRING);
    else if(parser->current_token->type==TOKEN_TYPE_INT) /* = [I] or [i]*/
        parser_eat(TAV,parser,TOKEN_TYPE_INT);
    else if(parser->current_token->type==TOKEN_TYPE_CHAR) /* = [C] or [c]*/
        parser_eat(TAV,parser,TOKEN_TYPE_CHAR);
    else if(parser->current_token->type==TOKEN_TYPE_ANY) /* = ~*/ {
        strcpy(TAV->ForSetup.Type[TAV->ForSetup.TypeIndex],"Any");
        parser_eat(TAV,parser,TOKEN_TYPE_ANY);
    }
    else /* = []. Same as ~, just a different way of saying it is of type Any.*/
        goto PARSE_RIGHT_BRACKET;
    PARSE_RIGHT_BRACKET:
    if(parser->current_token->type == TOKEN_RSQRBRACK)
        parser_eat(TAV,parser, TOKEN_RSQRBRACK);
    char* variable_definition_variable_name = parser->current_token->value;
    parser_eat(TAV,parser, TOKEN_ID);
    parser_eat(TAV,parser, TOKEN_COLON);
     

    AST_T* variable_definition_value = parser_parse_expr(parser);
    AST_T* variable_definition = init_ast(AST_VARIABLE_DEFINITION);


    variable_definition->variable_definition_variable_name = variable_definition_variable_name;
    variable_definition->variable_definition_value = variable_definition_value;

    return variable_definition;
}
AST_T* parser_parse_variable(TypeAndValue* TAV,parser_T* parser) {

    char* token_value = parser->current_token->value;
    parser_eat(TAV,parser, TOKEN_ID);

    if(parser->current_token->type==TOKEN_LSQRBRACK) {
        parser_eat(TAV,parser, TOKEN_LSQRBRACK);
    }
    if(parser->current_token->type==TOKEN_RSQRBRACK) {
        parser_eat(TAV,parser,TOKEN_RSQRBRACK);
    }

    if(parser->current_token->type == TOKEN_LPARENT)
        return parser_parse_function_call(parser);
    
    AST_T* ast_variable = init_ast(AST_VARIABLE);
    ast_variable->variable_name = token_value;

    return ast_variable;
}
AST_T* parser_parse_string(TypeAndValue* TAV,parser_T* parser) {
    AST_T* ast_string = init_ast(AST_STRING);

    ast_string->string_value = parser->current_token->value;

    strcpy(TAV->ForSetup.Value[TAV->ForSetup.ValueIndex],ast_string->string_value);
    strcpy(TAV->ForPrint.Value[TAV->ForPrint.ValueIndex],ast_string->string_value);
    TAV->ForSetup.ValueIndex++;
    TAV->ForPrint.ValueIndex++;

    parser_eat(TAV,parser, TOKEN_STRING);

    return ast_string;
}
AST_T* parser_parse_id(parser_T* parser) {
    TypeAndValue* TAV = calloc(1,sizeof(TypeAndValue));
    UpdTAV(TAV);

    if(strcmp(parser->current_token->value,"make")==0){
        return parser_parse_variable_definition(parser);
    }
    else
        return parser_parse_variable(TAV,parser);
}
lexer_T* parser_parser_pre_variable(lexer_T* lexer) {
    TypeAndValue* TAV = calloc(1,sizeof(TypeAndValue));
    UpdTAV(TAV);
    parser_T* parser = calloc(1,sizeof(struct PARSER_STRUCT));
    init_parser(lexer);

    if(lexer->c == '[') {
        lexer_advance_with_token(lexer,init_token(TOKEN_LSQRBRACK,lexer_get_current_char_as_string(lexer)));
        if(lexer->c=='S') {
            lexer_advance_with_token(lexer,init_token(TOKEN_TYPE_STRING,lexer_get_current_char_as_string(lexer)));
        }
    }

    return lexer;
}