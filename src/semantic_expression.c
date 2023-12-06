#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "ast.h"
#include "symtable.h"
#include "lexer.h"
#include "buffer_string.h"
#include "semantic.h"

#include "syntax_ll.h"
#include "syntax_precedent.h"


bool nonLiteral_in_exp(exp_node* node){
    if(node == NULL)
        return false;

    if(node->type == TOKEN_KEYWORD_FUNC || node->type == TOKEN_IDENTIFIER)
        return true;
    
    if(nonLiteral_in_exp(node->left))
        return true;
    if(nonLiteral_in_exp(node->right))
        return true;

    return false;
}


bool is_litereal(exp_node* node){
    return node->type == TOKEN_LITERAL_INT || node->type == TOKEN_LITERAL_DOUBLE || node->type == TOKEN_LITERAL_STRING || node->type == TOKEN_LITERAL_NIL;
}


Error handle_expression(exp_node* node, SymTable* tables, Type* returnType, SymTable *code_table, int scoping, bool* nillable, bool *init)
{
    if(node == NULL)
        return ERR_INTERNAL;
    
    Symbol *target;
    Type aType, bType;
    bool aNillable, bNillable, aInit, bInit;
    char* dollar;

    switch (node->type){

    //LITERALS
    case TOKEN_LITERAL_INT: //int
        *init = true;
        *nillable = false;
        *returnType = TYPE_INT;
        return OK;

    case TOKEN_LITERAL_DOUBLE: //double
        *init = true;
        *nillable = false;
        *returnType = TYPE_DOUBLE;
        return OK;

    case TOKEN_LITERAL_STRING:  //string
        *init = true;
        *nillable = false;
        *returnType = TYPE_STRING;
        return OK;

    case TOKEN_LITERAL_NIL: //nill
        *init = true;
        *nillable = true;
        *returnType = TYPE_NIL;
        return OK;

    //VARIABLES
    case TOKEN_IDENTIFIER:
        dollar = strstr(node->value.s, "$");
        if(dollar != NULL)   // if jmeno promene obsahuje $
            *dollar = '\0';   // potom $ -> '\0'

        target = SymTable_get_recurse(tables, node->value.s);
        if(target == NULL || target->symbol_type == FUNCTION)
            return ERR_SEMATIC_UNDEFINED_VAR;
        if (!target->initialized)
            return ERR_SEMATIC_UNDEFINED_VAR;
        
        *init = target->initialized;
        *returnType = target->type;
        *nillable = target->nilable;

        if(dollar != NULL)
            *dollar = '$';
        else
            appendScope(&node->value.s, target->scope);
        
        return OK;

    case TOKEN_KEYWORD_FUNC:
        *init = true; 
        return funcCallCheck((ASTNode*)(node->left), returnType, tables, code_table, nillable);

    //OPERATORS
    case TOKEN_OPERATOR_MINUS: // - 
    case TOKEN_OPERATOR_MULTIPLICATION: // *
        ERR = handle_expression(node->left,tables, &aType, tables, scoping, &aNillable, &aInit);
        if(ERR)
            return ERR;
        ERR = handle_expression(node->right,tables, &bType, tables, scoping, &bNillable, &bInit);
        if(ERR)
            return ERR;
        
        if(!(aInit && bInit))
            return ERR_SEMATIC_UNDEFINED_VAR;

        if (aNillable || bNillable)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        
        if((aType == TYPE_INT && bType == TYPE_INT) || (aType == TYPE_DOUBLE && bType == TYPE_DOUBLE)){ // INT INT || DOUBLE DOUBLE
            *returnType = aType;
            *nillable = false;
            return OK;
        }
        if(aType == TYPE_INT && bType == TYPE_DOUBLE && is_litereal(node->left)){ // LITERAL_INT DOUBLE
            // implicit conversion
            node->left->type = TOKEN_LITERAL_DOUBLE;
            node->left->value.d = (double)(node->left->value.i);
            *returnType = TYPE_DOUBLE;
            *nillable = false;
            return OK;
        }
        if(aType == TYPE_DOUBLE && bType == TYPE_INT && is_litereal(node->right)){ // DOUBLE LITERAL_INT
            // implicit conversion
            node->right->type = TOKEN_LITERAL_DOUBLE;
            node->right->value.d = (double)(node->right->value.i);
            *returnType = TYPE_DOUBLE;
            *nillable = false;
            return OK;
        }
        return ERR_SEMATIC_INCOMPATIBLE_TYPES ; //all other wrong types

    case TOKEN_OPERATOR_DIVISION: // /
        ERR = handle_expression(node->left, tables, &aType, tables, scoping, &aNillable, &aInit);
        if(ERR)
            return ERR;
        ERR = handle_expression(node->right, tables, &bType, tables, scoping, &bNillable, &bInit);
        if(ERR)
            return ERR;
        
        if(!(aInit && bInit))
            return ERR_SEMATIC_UNDEFINED_VAR;

        if (aNillable || bNillable)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        
        if (aType == TYPE_DOUBLE && bType == TYPE_DOUBLE){  //TYPE_DOUBLE TYPE_DOUBLE
            *returnType = aType;
            *nillable = false;
            return OK;
        }
        if (aType == TYPE_INT && bType == TYPE_INT){ // INT INT
            node->type = TOKEN_OPERATOR_I_DIVISION;
            *returnType = aType;
            *nillable = false;
            return OK;
        }
        // if (aType == TYPE_INT && bType == TYPE_DOUBLE && is_litereal(node->left)){
        //     // implicit conversion
        //     node->left->type = TOKEN_LITERAL_DOUBLE;
        //     node->left->value.d = (double)(node->left->value.i);
        //     *returnType = TYPE_DOUBLE;
        //     *nillable = false;
        //     return OK;
        // }
        // if (aType == TYPE_DOUBLE && bType == TYPE_INT && is_litereal(node->right)){
        //     // implicit conversion
        //     node->right->type = TOKEN_LITERAL_DOUBLE;
        //     node->right->value.d = (double)(node->right->value.i);
        //     *returnType = TYPE_DOUBLE;
        //     *nillable = false;
        //     return OK;
        // }

        return ERR_SEMATIC_INCOMPATIBLE_TYPES;

    case TOKEN_OPERATOR_PLUS:   // +
        ERR = handle_expression(node->left,tables, &aType, tables, scoping, &aNillable, &aInit);
        if(ERR)
            return ERR;
        ERR = handle_expression(node->right,tables, &bType, tables, scoping, &bNillable, &bInit);
        if(ERR)
            return ERR;
        
        if(!(aInit && bInit))
            return ERR_SEMATIC_UNDEFINED_VAR;

        if (aNillable || bNillable)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;

        if((aType == TYPE_INT && bType == TYPE_INT) || (aType == TYPE_DOUBLE && bType == TYPE_DOUBLE)){  //INT INT ||TYPE_DOUBLE TYPE_DOUBLE
            *returnType = aType;
            *nillable = false;
            return OK;
        }
        if(aType == TYPE_STRING && bType == TYPE_STRING){   // STRING STRING
            if(!(aInit && bInit))
                return ERR_SEMATIC_UNDEFINED_VAR;

            if (aNillable || bNillable)
                return ERR_SEMATIC_INCOMPATIBLE_TYPES;
            
            node->type = TOKEN_CONCATENATE;
            *returnType = TYPE_STRING;
            *nillable = false;
            return OK;
        }

        if(aType == TYPE_INT && bType == TYPE_DOUBLE && is_litereal(node->left)){ // LITERAL_INT TYPE_DOUBLE
            // implicit conversion
            node->left->type = TOKEN_LITERAL_DOUBLE;
            node->left->value.d = (double)(node->left->value.i);
            *returnType = TYPE_DOUBLE;
            *nillable = false;
            return OK;
        }
        if(aType == TYPE_DOUBLE && bType == TYPE_INT && is_litereal(node->right)){ // DOUBLE LITERAL_INT
            node->right->type = TOKEN_LITERAL_DOUBLE;
            node->right->value.d = (double)(node->right->value.i);
            *returnType = TYPE_DOUBLE;
            *nillable = false;
            return OK;
        }
        return ERR_SEMATIC_INCOMPATIBLE_TYPES;
    
    //LOGICAL
    case TOKEN_OPERATOR_NOT_EQUALS: // !=       
    case TOKEN_OPERATOR_EQUALS: // ==
        ERR = handle_expression(node->left, tables, &aType, tables, scoping, &aNillable, &aInit);
        if(ERR)
            return ERR;
        ERR = handle_expression(node->right, tables, &bType, tables, scoping, &bNillable, &bInit);
        if(ERR)
            return ERR;
        
        *returnType = TYPE_BOOL;
        *nillable = false;
        
        if (aType == bType)
            return OK;

        if (aType == TYPE_INT && bType == TYPE_DOUBLE && is_litereal(node->left)){ // LITERAL_INT TYPE_DOUBLE
            // implicit conversion
            node->left->type = TOKEN_LITERAL_DOUBLE;
            node->left->value.d = (double)(node->left->value.i);
            *returnType = TYPE_DOUBLE;
            *nillable = false;
            return OK;
        }
        if (aType == TYPE_DOUBLE && bType == TYPE_INT && is_litereal(node->right)){ // DOUBLE LITERAL_INT
            node->right->type = TOKEN_LITERAL_DOUBLE;
            node->right->value.d = (double)(node->right->value.i);
            *returnType = TYPE_DOUBLE;
            *nillable = false;
            return OK;
        }

        if (node->left->type == TOKEN_LITERAL_NIL && bNillable) // compare nil literal and nilable
            return OK;

        if (aNillable && node->right->type == TOKEN_LITERAL_NIL) // nilable and compare nil literal
            return OK;

        return ERR_SEMATIC_INCOMPATIBLE_TYPES;

    case TOKEN_OPERATOR_LESS_THAN: // <
    case TOKEN_OPERATOR_GREATER_THAN:  // >
    case TOKEN_OPERATOR_LESS_THAN_OR_EQUAL:  // <=
    case TOKEN_OPERATOR_GREATER_THAN_OR_EQUAL: // >=
        ERR = handle_expression(node->left, tables, &aType, tables, scoping, &aNillable, &aInit);
        if(ERR)
            return ERR;
        ERR = handle_expression(node->right, tables, &bType, tables, scoping, &bNillable, &bInit);
        if(ERR)
            return ERR;

        *returnType = TYPE_BOOL;
        *nillable = false;
        
        if(!(aInit && bInit))
            return ERR_SEMATIC_UNDEFINED_VAR;
        
        if (aNillable || bNillable)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;

        if (aType == bType)
            return OK;

        if (aType == TYPE_INT && bType == TYPE_DOUBLE && is_litereal(node->left)){ // LITERAL_INT TYPE_DOUBLE
            // implicit conversion
            node->left->type = TOKEN_LITERAL_DOUBLE;
            node->left->value.d = (double)(node->left->value.i);
            *returnType = TYPE_DOUBLE;
            *nillable = false;
            return OK;
        }
        if (aType == TYPE_DOUBLE && bType == TYPE_INT && is_litereal(node->right)){ // DOUBLE LITERAL_INT
            node->right->type = TOKEN_LITERAL_DOUBLE;
            node->right->value.d = (double)(node->right->value.i);
            *returnType = TYPE_DOUBLE;
            *nillable = false;
            return OK;
        }

        return ERR_SEMATIC_INCOMPATIBLE_TYPES;

    case TOKEN_NIL_COALESCING:  // ??
        ERR = handle_expression(node->left, tables, &aType, tables, scoping, &aNillable, &aInit);
        if(ERR)
            return ERR;
        ERR = handle_expression(node->right, tables, &bType, tables, scoping, &bNillable, &bInit);
        if(ERR)
            return ERR;
    
        *returnType = aType;
        *nillable = false;

        if(aType != bType || aType == TYPE_BOOL)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        
        if(!bInit)
            return ERR_SEMATIC_UNDEFINED_VAR;

        if (bNillable)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;   
        
        return OK;

    case TOKEN_EXCLAMATION:
        ERR = handle_expression(node->left, tables, &aType, tables, scoping, &aNillable, &aInit);
        if(ERR)
            return ERR;
        
        *returnType = aType;
        *nillable = false;
        return OK;
    
    case TOKEN_CONCATENATE:
        // concatenate was already checked
        *returnType = TYPE_STRING;
        *nillable = false;
        return OK;
    
    case TOKEN_OPERATOR_I_DIVISION:
        // IDIV was already checked
        *returnType = TYPE_INT;
        *nillable = false;
        return OK;

    default:
        fprintf(stderr, "Unhandled operator in expression\n");
        return ERR_INTERNAL;
    }
}