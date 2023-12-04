#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "ast.h"
#include "symtable.h"
#include "lexer.h"
#include "buffer_string.h"
#include "semantic.h"

static Token nonLiteral_in_exp(exp_node* node)
{
    if(node == NULL)
        return TOKEN_KEYWORD_NIL;
    if(node->type= TOKEN_KEYWORD_FUNC || node->type == TOKEN_IDENTIFIER)
        return TOKEN_KEYWORD_VAR;

    if(nonLiteral_in_exp(node->left) != TOKEN_KEYWORD_NIL)
        return TOKEN_KEYWORD_VAR;
    if(nonLiteral_in_exp(node->right) != TOKEN_KEYWORD_NIL)
        return TOKEN_KEYWORD_VAR;

    return TOKEN_KEYWORD_NIL;
}



Error handle_expression(exp_node* node,SymTable* tables,Type* returnType)
{
    
    bool eq= false;  
    switch (node->type)
    {
        //LITERALS
    case TOKEN_LITERAL_INT: //int
        *returnType = TYPE_INT;
        return OK;
        break;
    case TOKEN_LITERAL_DOUBLE: //double
        *returnType =TYPE_DOUBLE;
        return OK;
        break;
    case TOKEN_LITERAL_STRING:  //string
        *returnType = TYPE_STRING;
        return OK;
        break;
    case TOKEN_LITERAL_NIL: //nill
        *returnType = TYPE_NIL;
        return OK;
        break;
        //VARIABLES
    case TOKEN_IDENTIFIER:
        Symbol* target = SymTable_get_recurse(tables,node->value.s);
        if(target == NULL || target->symbol_type == FUNCTION)
            return ERR_SEMATIC_UNDEFINED_VAR;
        if (!target->initialized)
            return ERR_SEMATIC_UNDEFINED_VAR;
        *returnType = target->type;
        return OK;
        break;

    case TOKEN_KEYWORD_FUNC:                                          
        Type out = TYPE_NIL;
        Error err = funcCallCheck(node->left,&out,tables);
        if(err != OK)
            return err;
        if(out == TYPE_NIL)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        *returnType = out;
        return OK;
        break;

    //OPERATORS
    case TOKEN_OPERATOR_MINUS: // - 
    case TOKEN_OPERATOR_MULTIPLICATION: // *
        Type a,b;
        Error aEr = handle_expression(node->left,tables,&a);
        if(aEr != OK)
            return aEr;
        Error bEr = handle_expression(node->right,tables,&b);
        if(bEr != OK)
            return bEr;
        if((a == TYPE_INT && b == TYPE_INT)||(a==TYPE_DOUBLE&&b==TYPE_DOUBLE))  //INT INT ||TYPE_DOUBLETYPE_DOUBLE
        {
            *returnType = a;
            return OK;
        }
        if(a == TYPE_INT && b ==TYPE_DOUBLE && nonLiteral_in_exp(node->left) == TOKEN_KEYWORD_NIL); // LITERAL INTTYPE_DOUBLE
        {
            *returnType =TYPE_DOUBLE;
            return OK;
        }
        if(a ==TYPE_DOUBLE && b == TYPE_INT && nonLiteral_in_exp(node->right) == TOKEN_KEYWORD_NIL);//TYPE_DOUBLE LITERAL INT
        {
            *returnType =TYPE_DOUBLE;
            return OK;
        }
        return ERR_SEMATIC_INCOMPATIBLE_TYPES ; //all other wrong types
        break;

    case TOKEN_OPERATOR_DIVISION: // /
        Type a,b;
        Error aEr = handle_expression(node->left,tables,&a);
        if(aEr != OK)
            return aEr;
        Error bEr = handle_expression(node->right,tables,&b);
        if(bEr != OK)
            return bEr;
        if((a == TYPE_INT && b == TYPE_INT)||(a==TYPE_DOUBLE&&b==TYPE_DOUBLE))  //INT INT ||TYPE_DOUBLETYPE_DOUBLE
        {
            *returnType = a;
            return OK;
        }
        return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        break;
    case TOKEN_OPERATOR_PLUS:   // +
        Type a,b;
        Error aEr = handle_expression(node->left,tables,&a);
        if(aEr != OK)
            return aEr;
        Error bEr = handle_expression(node->right,tables,&b);
        if(bEr != OK)
            return bEr;
        if((a == TYPE_INT && b == TYPE_INT)||(a==TYPE_DOUBLE&&b==TYPE_DOUBLE)|| (a == TYPE_STRING && b == TYPE_STRING))  //INT INT ||TYPE_DOUBLETYPE_DOUBLE || STRING STRING
        {
            *returnType = a;
            return OK;
        }
        if(a == TYPE_INT && b ==TYPE_DOUBLE && nonLiteral_in_exp(node->left) == TOKEN_KEYWORD_NIL); // LITERAL INTTYPE_DOUBLE
        {
            *returnType =TYPE_DOUBLE;
            return OK;
        }
        if(a ==TYPE_DOUBLE && b == TYPE_INT && nonLiteral_in_exp(node->right) == TOKEN_KEYWORD_NIL);//TYPE_DOUBLE LITERAL INT
        {
            *returnType =TYPE_DOUBLE;
            return OK;
        }
        return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        break;
    
    //LOGICAL
    case TOKEN_OPERATOR_NOT_EQUALS: // !=
        eq=true;        
    case TOKEN_OPERATOR_EQUALS: // ==
    case TOKEN_OPERATOR_LESS_THAN: // <
    case TOKEN_OPERATOR_GREATER_THAN:  // >
    case TOKEN_OPERATOR_LESS_THAN_OR_EQUAL:  // <=
    case TOKEN_OPERATOR_GREATER_THAN_OR_EQUAL: // >=
        if(eq)
        {
            if(((exp_node*)node->left)->type==TOKEN_IDENTIFIER && node->right == NULL)
                *returnType = TYPE_BOOL;
                return OK;
        }

        Type a,b;
        Error aEr = handle_expression(node->left,tables,&a);
        if(aEr != OK)
            return aEr;
        Error bEr = handle_expression(node->right,tables,&b);
        if(bEr != OK)
            return bEr;
        
        if(a == TYPE_BOOL ||b == TYPE_BOOL)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        if(a == TYPE_INT && b ==TYPE_DOUBLE && nonLiteral_in_exp(node->left) != TOKEN_KEYWORD_NIL); // LITERAL INTTYPE_DOUBLE
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        if(a ==TYPE_DOUBLE && b == TYPE_INT && nonLiteral_in_exp(node->right) != TOKEN_KEYWORD_NIL);//TYPE_DOUBLE LITERAL INT
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        if (a != b)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
            
        *returnType = TYPE_BOOL;
        return OK;
        break;

    case TOKEN_NIL_COALESCING:  // ??
        Type a,b;
        Error aEr = handle_expression(node->left,tables,&a);
        if(aEr != OK)
            return aEr;
        Error bEr = handle_expression(node->right,tables,&b);
        if(bEr != OK)
            return bEr;
        
        if(a != b || a == TYPE_BOOL)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        
    case TOKEN_EXCLAMATION:
        *returnType = ((exp_node*)node->left)->type;
        return OK;
        break;
    }
}