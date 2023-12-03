#include "error.h"
#include "ast.h"
#include "symtable.h"
#include <stdlib.h>
#include "lexer.h"

static Error funcCallCheck(ASTNode*func,Type* returnType,SymTable* tables)
{
    SymTable* global = tables;
    while(global->previous != NULL)
        global = global->previous;
    
    Symbol* target = SymTable_get(global,func->a);
    if(target == NULL || target->type != FUNCTION)
        return ERR_SEMATIC_UNDEFINED_FUNC;
    
    FuncDefArg* args = func->b;
    while(args != NULL)
    {
        //problem
    }

}
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
static Symbol* SymTable_get_recurse(SymTable* symTable,char*name)
{
    while(symTable != NULL)
    {
        Symbol* target=SymTable_get(symTable,name);
        if (target != NULL)
        {
            return target;
        }
        symTable=symTable->previous;
    }
    return NULL;
}

static Error handle_expression(exp_node* node,SymTable* tables,Type* returnType)
{
    bool eq= false;  
    switch (node->type)
    {
        //LITERALS
    case TOKEN_LITERAL_INT: //int
        *returnType = INT;
        return OK;
        break;
    case TOKEN_LITERAL_DOUBLE: //double
        *returnType = DOUBLE;
        return OK;
        break;
    case TOKEN_LITERAL_STRING:  //string
        *returnType = STRING;
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
        Type out = NIL;
        Error err = funcCallCheck(node->left,&out,tables);
        if(err != OK)
            return err;
        if(out == NIL)
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
        if((a == INT && b == INT)||(a==DOUBLE&&b==DOUBLE))  //INT INT || DOUBLE DOUBLE
        {
            *returnType = a;
            return OK;
        }
        if(a == INT && b == DOUBLE && nonLiteral_in_exp(node->left) == TOKEN_KEYWORD_NIL); // LITERAL INT DOUBLE
        {
            *returnType = DOUBLE;
            return OK;
        }
        if(a == DOUBLE && b == INT && nonLiteral_in_exp(node->right) == TOKEN_KEYWORD_NIL);// DOUBLE LITERAL INT
        {
            *returnType = DOUBLE;
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
        if((a == INT && b == INT)||(a==DOUBLE&&b==DOUBLE))  //INT INT || DOUBLE DOUBLE
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
        if((a == INT && b == INT)||(a==DOUBLE&&b==DOUBLE)|| (a == STRING && b == STRING))  //INT INT || DOUBLE DOUBLE || STRING STRING
        {
            *returnType = a;
            return OK;
        }
        if(a == INT && b == DOUBLE && nonLiteral_in_exp(node->left) == TOKEN_KEYWORD_NIL); // LITERAL INT DOUBLE
        {
            *returnType = DOUBLE;
            return OK;
        }
        if(a == DOUBLE && b == INT && nonLiteral_in_exp(node->right) == TOKEN_KEYWORD_NIL);// DOUBLE LITERAL INT
        {
            *returnType = DOUBLE;
            return OK;
        }
        return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        break;
    
    //LOGICAL
    case TOKEN_OPERATOR_EQUALS: // ==
        eq=true;        
    case TOKEN_OPERATOR_LESS_THAN: // <
    case TOKEN_OPERATOR_GREATER_THAN:  // >
    case TOKEN_OPERATOR_LESS_THAN_OR_EQUAL:  // <=
    case TOKEN_OPERATOR_GREATER_THAN_OR_EQUAL: // >=
    case TOKEN_OPERATOR_NOT_EQUALS: // !=
        Type a,b;
        Error aEr = handle_expression(node->left,tables,&a);
        if(aEr != OK)
            return aEr;
        Error bEr = handle_expression(node->right,tables,&b);
        if(bEr != OK)
            return bEr;
        if(eq)
        {
            if(((exp_node*)node->left)->type==TOKEN_IDENTIFIER && b == NIL)
                *returnType = BOOL;
                return OK;
        }
        if (a != b)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        if(a == BOOL || a == NIL)
            return ERR_SYNTAX;
        if(a == INT && b == DOUBLE && nonLiteral_in_exp(node->left) != TOKEN_KEYWORD_NIL); // LITERAL INT DOUBLE
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        if(a == DOUBLE && b == INT && nonLiteral_in_exp(node->right) != TOKEN_KEYWORD_NIL);// DOUBLE LITERAL INT
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
            
        *returnType = BOOL;
        return OK;
        break;

    case TOKEN_NIL_COALESCING:  // ??
    case TOKEN_EXCLAMATION:
        *returnType = ((exp_node*)node->left)->type;
        return OK;
        break;

 
        
    }
}

static Error handle_statement(ASTNode* statement,SymTable* tables,Type expected_type)
{

    switch (statement->type)
    {
    case VAR_DEF:
    case LET_DEF:
        Symbol* var = Symbol_new();

        var->symbol_type=(statement->type == VAR_DEF)?VAR:LET;
        var->name=((ASTNode*)statement->b)->a;
        var->type=((ASTNode*)statement->a)->a;
        var->nilable=((ASTNode*)statement->a)->b;
        Error error = SymTable_insert(tables,var);
        if(error != OK)
        {
            free(var);
            return error;
        }
        // AMIDIATE ASIGN
        if (((ASTNode*)statement->b)->b == NULL)
        {
            if (var->type == NIL || var->nilable ==false)
                return ERR_SEMATIC_BAD_TYPE_INFERENCE;
            else
                return OK;
        }
        Type expReturnType;
        Error error = handle_expression(((ASTNode*)statement->b)->b,tables,&expReturnType);
        if (error != OK )
            return error;
        if(!var->nilable && expReturnType == NIL)
            return ERR_SEMATIC_BAD_TYPE_INFERENCE; 
        //type infering
        if(var->type == NIL)
        {
            var->type = expReturnType;    
        }else if (var->type != expReturnType)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        var->initialized=true;
        return OK;
        break;

    case ASSIGN:
        Symbol* target = SymTable_get_recurse(tables,statement->a);
        if(target == NULL || target->symbol_type == FUNCTION)   //var/let exists check
            return ERR_SEMATIC_UNDEFINED_VAR;
        
        if(target->symbol_type == LET && target->initialized)  
            return ERR_SEMATIC_REDEFINED;
        
        target->initialized=true;

        Type expReturnType;
        Error error = handle_expression(statement->b,tables,&expReturnType);
        if (error != OK)
            return error;

        if(target->type != expReturnType)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        
        break;
    case FUNC_CALL:
        
        SymTable* global = tables;
        while(global->previous!=NULL)
            global = global->previous;
        Symbol* target = SymTable_get(tables,statement->a);
        if(target == NULL || target->symbol_type == VAR || target->symbol_type == LET)   //var/let exists check
            return ERR_SEMATIC_UNDEFINED_FUNC;
        Type dump = NIL;
        Error err = funcCallCheck(statement->a,&dump,tables);
        if (err !=OK)
            return err;
        return OK;
        break;
    case IFELSE:

        break;
    case WHILE:

        break;
    case RETURN:

        break;

    }

}

static void error_free_all(SymTable* tables)
{
    while(tables !=NULL)
    {
        SymTable *temp=tables->previous;
        SymTable_free(tables);
        tables = temp;
    }
}



Error handle_statements(ASTNode*statement,SymTable* tables,Type expected_type)
{
    SymTable* localTable = malloc(sizeof(SymTable));
    SymTable_init(localTable);
    localTable->previous=tables;
    while(statement!= NULL)
    {
        Error err = handle_statement(statement,localTable,expected_type);
        if(err != OK)
        {
            SymTable_free(localTable);
            return err;
        }

        statement=statement->b;
    }
    return OK;
}


Error sematic(ASTNode *code_tree)
{
    //GLOBAL SymTable
    SymTable *globalTable = malloc(sizeof(SymTable));
    SymTable_init(globalTable);
    globalTable->previous = NULL;

    //PRE-DEFINED FUNCTIONS TO SYMTABLE


    //USER FUNCTIONS TO SYMTABLE
    ASTNode *functions = code_tree->a;
    while(functions != NULL)
    {
        
        ASTNode *func = (ASTNode*)(functions->b);
        if(func->b == NULL)
        {
            error_free_all(globalTable);
            return ERR_SEMATIC_UNDEFINED_FUNC;
        }
        Symbol *symbolFunc = Symbol_new();
        symbolFunc->symbol_type = FUNCTION;
        symbolFunc->args = (FuncDefArg*)((ASTNode*)(((ASTNode*)func->a)->a))->b;
        symbolFunc->name = ((ASTNode*)(((ASTNode*)func->a)->a))->a;
        symbolFunc->type = ((ASTNode*)func->a)->b;

        Error error= SymTable_insert(globalTable,symbolFunc);
        if(error != OK)
        {
            error_free_all(globalTable);
            Symbol_free(symbolFunc);
            return error;
        }

       functions = functions->a;
    }


    //FUNC BODY CHECK
    ASTNode *functions = code_tree->a;
    while(functions)
    {
        //HANDLE FUNC BODY
        ASTNode *func = functions->b;
        Symbol* symbolFunc = SymTable_get(globalTable,((ASTNode*)(((ASTNode*)func->a)->a))->a);
        
        //extra symtable with args
        SymTable *funcArgTable = malloc(sizeof(SymTable));
        SymTable_init(funcArgTable);
        funcArgTable->previous = globalTable;

      
      
        FuncDefArg* arg = symbolFunc->args;
        while(arg != NULL)
        {
            Symbol* symbolArg = Symbol_new();
            symbolArg->initialized=true;
            symbolArg->name=arg->identifier;
            symbolArg->nilable=false;
            symbolArg->symbol_type= LET;
            symbolArg->type=arg->type;                       
            Error error= SymTable_insert(funcArgTable,symbolArg);
            if(error != OK)
            {
                error_free_all(funcArgTable);
                Symbol_free(symbolArg);
                return error;
            }
            arg = arg->next;
        }


        //func body code check
        ASTNode *statement = func->b; 
        Error error = handle_statements(func->b,funcArgTable,symbolFunc->type);
        if (error != OK)
        {
            error_free_all(funcArgTable);
            return error; 
        }
        SymTable_free(funcArgTable);

        functions = functions->a;
    }

    ASTNode* main = code_tree->b;


    //MAIN BODY
    Error error = handle_statements(main,globalTable,NONE);



}

//funkce musí mít aspoň jeden statement? (return ?)
//zařídít fce body null

//return v globalu? NESMÍ BÝT


//typy u operátorů, (int op double ? )
//string operátory


//stránka 6, wtf nelze měnit hodnotu?
//parametry jsou neměnné
//můžou se předefinovat


//funkce void nemusí mít return,


//definice typu proměnné, not defined, a odvození typu při not defined

// když při definici nelze odvodit, chyba 8 (např. hondnota nil.)
//když nechybý typ, nemusí být = výraz
//chyba 7, špatný typ výrazu při přiřazení // expression v if () není typu bool

// v if () může být let id !!!
// musím checknout jestli proměnná id existuje

//VOID FCE NESMÍ MÍT RETURN, JINAK ERR 4 (ERR 4 TAKY KDYŽ TYP RETURN NESEDÍ)
//CHYBÍ LI RETURN V NOT VOID FCI, ERR 6
//VESTAVĚNÉ FCE DO SYMTABLE
/*
    INT + INT = INT
    INT + DOUBLE = DOUBLE
    STRING + STRING = STRING
    EXP1 ?? EXP2 (EXP1 = Nil) = EXP2
                  EXP1 != NIL = EXP1
    typ EXP1,2 musí být stejný,(bez nil)
    EXP1 == EXP2
    if (typ exp1 != type exp2) ==> err 7
    literály se potřebně přetypují (int>double) ale var a let NE!!!

    ostatní porovnání:
        nesmí být expy nil, nesmí se porovnávat des. číslo s celým  
    chyby v expech se hlásí jako semantic err
*/


//BOOL NESMÍ BÝT V UVNITŘ EXPU