#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "token.h"

lexer_T* init_lexer(char *contents) {
    lexer_T* lexer = calloc(1,sizeof(struct LEXER_STRUCT));
    lexer->contents = contents;
    lexer->i = 0;
    lexer->c = contents[lexer->i];

    return lexer;
}
void lexer_advance(lexer_T *lexer) {
    
    if(lexer->c != '\0' && lexer->i < strlen(lexer->contents)) {

        lexer->i++;
        lexer->c = lexer->contents[lexer->i];

        if(lexer->c == ':') {
            lexer->i++;
            lexer->c = lexer->contents[lexer->i];

            /* Checking whether or not a string starts after the colon. */
            if(lexer->c != '"') {
                lexer->i++;
                lexer->c = lexer->contents[lexer->i];
                if(lexer->c != '"') {
                    printf("\n\nErr: The declared variable has no value to to.\nPossible Reasons: 2 or more spaces before double quotes.\n\n");
                    exit(1);
                }
                else {
                    lexer->i--;
                }
            }
            lexer->i--;
            lexer->c = lexer->contents[lexer->i];
        }
                
    }
}
void lexer_skip_whitespace(lexer_T *lexer) {
    while(lexer->c == ' ' || lexer->c == 10) {
        lexer_advance(lexer);
    }
}
token_T* lexer_get_next_token(lexer_T* lexer) {

    while(lexer->c != '\0' && lexer->i < strlen(lexer->contents)) {
        if(lexer->c == ' ' || lexer->c == 10) 
            lexer_skip_whitespace(lexer);
        

        if(isalnum(lexer->c)) {
            /* Finding type declarations. Will be inside of square brackets */
            if(lexer->c == 'S') {
                return lexer_advance_with_token(lexer,init_token(TOKEN_TYPE_STRING,lexer_get_current_char_as_string(lexer)));
            }
            if(lexer->c == 'I')
                return lexer_advance_with_token(lexer,init_token(TOKEN_TYPE_INT,lexer_get_current_char_as_string(lexer)));
            if(lexer->c == 'C')
                return lexer_advance_with_token(lexer,init_token(TOKEN_TYPE_CHAR,lexer_get_current_char_as_string(lexer)));
            if(lexer->c=='A')
                return lexer_advance_with_token(lexer,init_token(TOKEN_TYPE_A,lexer_get_current_char_as_string(lexer)));
            return lexer_collect_id(lexer);
        }
        
        if(lexer->c == '"')
            return lexer_collect_string(lexer);
        
        //SWITCH:
        switch(lexer->c) {
            case ':': return lexer_advance_with_token(lexer,init_token(TOKEN_COLON,lexer_get_current_char_as_string(lexer))); break;
            case '~': return lexer_advance_with_token(lexer,init_token(TOKEN_TYPE_ANY,lexer_get_current_char_as_string(lexer)));break;
            case '[': return lexer_advance_with_token(lexer,init_token(TOKEN_LSQRBRACK,lexer_get_current_char_as_string(lexer))); break;
            case ']': return lexer_advance_with_token(lexer,init_token(TOKEN_RSQRBRACK,lexer_get_current_char_as_string(lexer))); break;
            case '(': return lexer_advance_with_token(lexer,init_token(TOKEN_LPARENT,lexer_get_current_char_as_string(lexer))); break;
            case ')': return lexer_advance_with_token(lexer,init_token(TOKEN_RPARENT,lexer_get_current_char_as_string(lexer))); break;
            case ';': return lexer_advance_with_token(lexer,init_token(TOKEN_SEMI,lexer_get_current_char_as_string(lexer))); break;
            case ',': return lexer_advance_with_token(lexer,init_token(TOKEN_COMMA,lexer_get_current_char_as_string(lexer))); break;
            case '{': return lexer_advance_with_token(lexer,init_token(TOKEN_LCURL,lexer_get_current_char_as_string(lexer))); break;
            case '}': return lexer_advance_with_token(lexer,init_token(TOKEN_RCURL,lexer_get_current_char_as_string(lexer))); break;
            case '!': return lexer_advance_with_token(lexer,init_token(TOKEN_PREVAR_END_SYMBOL,lexer_get_current_char_as_string(lexer))); break;
            //case '#': return lexer_advance_with_token(lexer,init_token(TOKEN_PRESET_TYPE_SETVAR,lexer_get_current_char_as_string(lexer))); break;
        }
    }
    return init_token(TOKEN_EOF, "\0");
}
token_T* lexer_collect_string(lexer_T* lexer) {
    lexer_advance(lexer);
    char *value = calloc(1,sizeof(char));
    value[0] = '\0';

    while(lexer->c != '"') {
        char *s = lexer_get_current_char_as_string(lexer);
        value = realloc(value,(strlen(value)+strlen(s)+1)*sizeof(char));
        strcat(value,s);

        lexer_advance(lexer);
    }

    lexer_advance(lexer);

    return init_token(TOKEN_STRING,value);
}
token_T* lexer_collect_id(lexer_T* lexer) {
    char *value = calloc(1,sizeof(char));
    value[0] = '\0';

    while(isalnum(lexer->c)) {
        char *s = lexer_get_current_char_as_string(lexer);
        value = realloc(value,(strlen(value)+strlen(s)+1)*sizeof(char));
        strcat(value,s);

        lexer_advance(lexer);
    }

    return init_token(TOKEN_ID,value);
}
token_T* lexer_advance_with_token(lexer_T* lexer, token_T* token) {
    lexer_advance(lexer);

    return token;
}
char* lexer_get_current_char_as_string(lexer_T* lexer) {
    char *str = calloc(2,sizeof(char));
    str[0] = lexer->c;
    str[1] = '\0';

    return str;
}