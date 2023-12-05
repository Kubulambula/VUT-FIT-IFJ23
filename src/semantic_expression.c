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


static Token nonLiteral_in_exp(exp_node* node)
{
    if(node == NULL)
        return TOKEN_KEYWORD_NIL;
    if(node->type == TOKEN_KEYWORD_FUNC || node->type == TOKEN_IDENTIFIER)
        return TOKEN_KEYWORD_VAR;

    if(nonLiteral_in_exp(node->left) != TOKEN_KEYWORD_NIL)
        return TOKEN_KEYWORD_VAR;
    if(nonLiteral_in_exp(node->right) != TOKEN_KEYWORD_NIL)
        return TOKEN_KEYWORD_VAR;

    return TOKEN_KEYWORD_NIL;
}



Error handle_expression(exp_node* node, SymTable* tables, Type* returnType, SymTable *code_table, int scoping, bool* nillable)
{
    if(node != NULL){
        Symbol *target;
        Type out,a,b;
        Error err, aEr, bEr;
        char* dollar;
        bool return_nillable_a, return_nillabel_b;


        switch (node->type)
        {

        //LITERALS
        case TOKEN_LITERAL_INT: //int
            *nillable = false;
            *returnType = TYPE_INT;
            return OK;
            break;
        case TOKEN_LITERAL_DOUBLE: //double
            *nillable = false;
            *returnType =TYPE_DOUBLE;
            return OK;
            break;
        case TOKEN_LITERAL_STRING:  //string
            *nillable = false;
            *returnType = TYPE_STRING;
            return OK;
            break;
        case TOKEN_LITERAL_NIL: //nill
            *nillable = true;
            *returnType = TYPE_NIL;
            return OK;
            break;

        //VARIABLES
        case TOKEN_IDENTIFIER:
            dollar = strstr(node->value.s, "$");
            if(dollar != NULL){   // if jmeno promene obsahuje $
                *dollar = '\0';   // potom $ -> '\0'
            }

            target = SymTable_get_recurse(tables,node->value.s);
            if(target == NULL || target->symbol_type == FUNCTION)
                return ERR_SEMATIC_UNDEFINED_VAR;
            if (!target->initialized)
                return ERR_SEMATIC_UNDEFINED_VAR;
            *returnType = target->type;
            *nillable = target->nilable;

            if(dollar != NULL){
                *dollar = '$';
            }
            else{
                appendScope(&node->value.s, scoping);
            }
            
            return OK;
            break;

        case TOKEN_KEYWORD_FUNC: 
            out = TYPE_NIL;
            err = funcCallCheck((ASTNode*)node->left, &out , tables, code_table, scoping, nillable);
            if(err != OK)
                return err;
            *returnType = out;
            return OK;
            break;

        //OPERATORS
        case TOKEN_OPERATOR_MINUS: // - 
        case TOKEN_OPERATOR_MULTIPLICATION: // *
            aEr = handle_expression(node->left,tables,&a, tables, scoping, &return_nillable_a);
            if(aEr != OK)
                return aEr;
            bEr = handle_expression(node->right,tables,&b, tables, scoping, &return_nillabel_b);
            if(bEr != OK)
                return bEr;
            *nillable = return_nillable_a || return_nillabel_b;
            if((a == TYPE_INT && b == TYPE_INT)||(a==TYPE_DOUBLE&&b==TYPE_DOUBLE))  //INT INT ||TYPE_DOUBLETYPE_DOUBLE
            {
                *returnType = a;
                return OK;
            }
            if(a == TYPE_INT && b ==TYPE_DOUBLE && nonLiteral_in_exp(node->left) == TOKEN_KEYWORD_NIL) // LITERAL INTTYPE_DOUBLE
            {
                *returnType =TYPE_DOUBLE;
                return OK;
            }
            if(a ==TYPE_DOUBLE && b == TYPE_INT && nonLiteral_in_exp(node->right) == TOKEN_KEYWORD_NIL)//TYPE_DOUBLE LITERAL INT
            {
                *returnType =TYPE_DOUBLE;
                return OK;
            }
            return ERR_SEMATIC_INCOMPATIBLE_TYPES ; //all other wrong types
            break;

        case TOKEN_OPERATOR_DIVISION: // /
            aEr = handle_expression(node->left,tables,&a, tables, scoping, &return_nillable_a);
            if(aEr != OK)
                return aEr;
            bEr = handle_expression(node->right,tables,&b, tables, scoping, &return_nillabel_b);
            if(bEr != OK)
                return bEr;

            *nillable = return_nillable_a || return_nillabel_b;
            if(a==TYPE_DOUBLE && b==TYPE_DOUBLE)  //TYPE_DOUBLE TYPE_DOUBLE
            {
                *returnType = a;
                return OK;
            }
            if(a == TYPE_INT && b == TYPE_INT){     // INT INT
                node->type = TOKEN_OPERATOR_I_DIVISION;
                *returnType = a;
                return OK;
            }
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
            break; 

        case TOKEN_OPERATOR_PLUS:   // +
            aEr = handle_expression(node->left,tables,&a, tables, scoping, &return_nillable_a);
            if(aEr != OK)
                return aEr;
            bEr = handle_expression(node->right,tables,&b, tables, scoping, &return_nillabel_b);
            if(bEr != OK)
                return bEr;

            *nillable = return_nillable_a || return_nillabel_b;
            if((a == TYPE_INT && b == TYPE_INT)||(a==TYPE_DOUBLE && b==TYPE_DOUBLE))  //INT INT ||TYPE_DOUBLE TYPE_DOUBLE
            {
                *returnType = a;
                return OK;
            }
            if(a == TYPE_STRING && b == TYPE_STRING){   // STRING STRING
                node->type = TOKEN_CONCATENATE;
                *returnType = TYPE_STRING;
                return OK;
            }
            if(a == TYPE_INT && b ==TYPE_DOUBLE && nonLiteral_in_exp(node->left) == TOKEN_KEYWORD_NIL) // LITERAL INT TYPE_DOUBLE
            {
                *returnType =TYPE_DOUBLE;
                return OK;
            }
            if(a ==TYPE_DOUBLE && b == TYPE_INT && nonLiteral_in_exp(node->right) == TOKEN_KEYWORD_NIL)//TYPE_DOUBLE LITERAL INT
            {
                *returnType =TYPE_DOUBLE;
                return OK;
            }
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
            break;
        
        //LOGICAL
        case TOKEN_OPERATOR_NOT_EQUALS: // !=       
        case TOKEN_OPERATOR_EQUALS: // ==
        case TOKEN_OPERATOR_LESS_THAN: // <
        case TOKEN_OPERATOR_GREATER_THAN:  // >
        case TOKEN_OPERATOR_LESS_THAN_OR_EQUAL:  // <=
        case TOKEN_OPERATOR_GREATER_THAN_OR_EQUAL: // >=
            aEr = handle_expression(node->left,tables,&a, tables, scoping, &return_nillable_a);
            if(aEr != OK)
                return aEr;
            bEr = handle_expression(node->right,tables,&b, tables, scoping, &return_nillabel_b);
            if(bEr != OK)
                return bEr;

            *nillable = return_nillable_a || return_nillabel_b;
            if(node->type != TOKEN_OPERATOR_EQUALS && node->type != TOKEN_OPERATOR_NOT_EQUALS && (a == TYPE_NIL || b == TYPE_NIL))
                return ERR_SEMATIC_INCOMPATIBLE_TYPES;
            if(a == TYPE_BOOL || b == TYPE_BOOL)
                return ERR_SEMATIC_INCOMPATIBLE_TYPES;
            if(a == TYPE_INT && b ==TYPE_DOUBLE && nonLiteral_in_exp(node->left) != TOKEN_KEYWORD_NIL) // LITERAL INTTYPE_DOUBLE
                return ERR_SEMATIC_INCOMPATIBLE_TYPES;
            if(a ==TYPE_DOUBLE && b == TYPE_INT && nonLiteral_in_exp(node->right) != TOKEN_KEYWORD_NIL) //TYPE_DOUBLE LITERAL INT
                return ERR_SEMATIC_INCOMPATIBLE_TYPES;
            if (a != b)
                return ERR_SEMATIC_INCOMPATIBLE_TYPES;
                
            *returnType = TYPE_BOOL;
            
            return OK;
            break;

        case TOKEN_NIL_COALESCING:  // ??
            aEr = handle_expression(node->left,tables,&a, tables, scoping, &return_nillable_a);
            if(aEr != OK)
                return aEr;
            bEr = handle_expression(node->right,tables,&b, tables, scoping, &return_nillabel_b);
            if(bEr != OK)
                return bEr;
        
            if(a != b || a == TYPE_BOOL || return_nillabel_b == true)   
                return ERR_SEMATIC_INCOMPATIBLE_TYPES;
            *nillable = false;
            break;

        case TOKEN_EXCLAMATION:
            err = handle_expression(node->left, tables, &a, code_table, scoping, &return_nillable_a);
            if(err)
                return err;
            if(return_nillable_a == false){
                return  ERR_SEMATIC_INCOMPATIBLE_TYPES;
            }
            *nillable = false;
            *returnType = a;
            return OK;
            break;

        default:
            return ERR_INTERNAL;
            break;
        }
    }
    return OK;
}