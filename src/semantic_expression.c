#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "ast.h"
#include "symtable.h"
#include "lexer.h"
#include "buffer_string.h"

static Error funcCallCheck(ASTNode*func,Type* returnType,SymTable* tables)
{
    SymTable* global = tables;
    while(global->previous != NULL)
        global = global->previous;
    
    Symbol* target = SymTable_get(global,func->a);
    if(target == NULL || target->type != FUNCTION)
        return ERR_SEMATIC_UNDEFINED_FUNC;
    const char*write="write";
    if(strcmp(func->a,write))
        return OK;
    
    ASTNode* arg = func->b;
    FuncDefArg* symTable_arg=target->args;
    while(arg != NULL && symTable_arg != NULL)
    {
        if(symTable_arg->name == NULL || ((ASTNode*)arg->a)->a == NULL)
        {
            if(symTable_arg->name != NULL || ((ASTNode*)arg->a)->a != NULL)
                return ERR_SEMATIC_BAD_FUNC_ARG_TYPE;
        }
        else if(strcmp(symTable_arg->name,((ASTNode*)arg->a)->a) != 0)
            return ERR_SEMATIC_BAD_FUNC_ARG_TYPE;
        Type a;
        Error err = handle_expression(((ASTNode*)arg->a)->b,tables,&a);
        if(err != OK)
            return err;
        if (a != symTable_arg->type)
            return ERR_SEMATIC_BAD_FUNC_ARG_TYPE;
        
        symTable_arg = symTable_arg->next;
        arg->b;
    }
    if(arg != NULL && symTable_arg != NULL)
        return ERR_SEMATIC_BAD_FUNC_ARG_COUNT;
    *returnType = target->type;
    return OK;

}

static Error handle_expression(exp_node* node,SymTable* tables,Type* returnType)
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