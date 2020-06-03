#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "parser.h"

parser_T* init_parser(lexer_T* lexer) {
    parser_T* parser = calloc(1,sizeof(struct PARSER_STRUCT));
    
    parser->lexer = lexer;
    parser->current_token = lexer_get_next_token(lexer);

    return parser;
}
void parser_eat(parser_T* parser, int token_type) {
    
    if(strcmp(parser->current_token->value,"print")==0) {
        parser->current_token = lexer_get_next_token(parser->lexer);

        if(parser->current_token->type==TOKEN_LSQRBRACK) {
            parser->current_token = lexer_get_next_token(parser->lexer);
        }

        if(strcmp(parser->current_token->value,"S")==0) {
            printf("PRINTING STRING!");
            parser->current_token = lexer_get_next_token(parser->lexer);
        }
        
    } else {
            
        if(parser->current_token->type == token_type) {
            parser->current_token = lexer_get_next_token(parser->lexer);
        } else {
            printf("Unexpected token '%s' with type %d",parser->current_token->value,parser->current_token->type);
            exit(1);
        }
    }
}
AST_T* parser_parse(parser_T* parser) {
    return parser_parse_statements(parser);
}
AST_T* parser_parse_statement(parser_T* parser) {
    switch(parser->current_token->type) {
        case TOKEN_ID: return parser_parse_id(parser);
    }
    if(parser->current_token->value[0]=='c')
        parser->current_token->type=0;
        return parser_parse_id(parser);
}
AST_T* parser_parse_statements(parser_T* parser) {

    AST_T* compound = init_ast(AST_COMPOUND);
    compound->compound_value = calloc(1,sizeof(struct AST_STRUCT));
    AST_T* ast_statement = parser_parse_statement(parser);
    compound->compound_value[0] = ast_statement;

    while(parser->current_token->type == TOKEN_SEMI) {
        parser_eat(parser, TOKEN_SEMI);

        AST_T* ast_statement = parser_parse_statement(parser);
        compound->compound_size++;
        compound->compound_value = realloc(compound->compound_value,compound->compound_size*sizeof(struct AST_STRUCT));
        compound->compound_value[compound->compound_size-1] = ast_statement;
    }

    return compound;
}
AST_T* parser_parse_expr(parser_T* parser) {
    switch(parser->current_token->type) {
        case TOKEN_STRING: return parser_parse_string(parser);
    }
}
AST_T* parser_parse_factor(parser_T* parser) {

}
AST_T* parser_parse_term(parser_T* parser) {

}
AST_T* parser_parse_function_call(parser_T* parser) {

}
AST_T* parser_parse_variable_definition(parser_T* parser) {
    parser_eat(parser, TOKEN_ID);
    parser_eat(parser, TOKEN_LSQRBRACK);
    if(parser->current_token->type==3) {
        parser_eat(parser,TOKEN_TYPE_STRING);
    }
    parser_eat(parser, TOKEN_RSQRBRACK);
    char* variable_definition_variable_name = parser->current_token->value;
    parser_eat(parser, TOKEN_ID);
    parser_eat(parser, TOKEN_COLON);
     

    AST_T* variable_definition_value = parser_parse_expr(parser);
    AST_T* variable_definition = init_ast(AST_VARIABLE_DEFINITION);


    variable_definition->variable_definition_variable_name = variable_definition_variable_name;
    variable_definition->variable_definition_value = variable_definition_value;

    return variable_definition;
}
AST_T* parser_parse_variable(parser_T* parser) {

    char* token_value = parser->current_token->value;
    parser_eat(parser, TOKEN_ID);

    if(parser->current_token->type==TOKEN_LSQRBRACK) {
        parser_eat(parser, TOKEN_LSQRBRACK);
    }
    if(parser->current_token->type==TOKEN_RSQRBRACK) {
        parser_eat(parser,TOKEN_RSQRBRACK);
    }

    if(parser->current_token->type == TOKEN_LPARENT)
        return parser_parse_function_call(parser);
    
    AST_T* ast_variable = init_ast(AST_VARIABLE);
    ast_variable->variable_name = token_value;

    return ast_variable;
}
AST_T* parser_parse_string(parser_T* parser) {
    AST_T* ast_string = init_ast(AST_STRING);

    ast_string->string_value = parser->current_token->value;
    parser_eat(parser, TOKEN_STRING);

    return ast_string;
}
AST_T* parser_parse_id(parser_T* parser) {
    if(strcmp(parser->current_token->value,"make")==0){
        return parser_parse_variable_definition(parser);
    }
    else
        return parser_parse_variable(parser);
}