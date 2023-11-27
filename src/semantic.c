#include "error.h"
#include "ast.h"
#include "symtable.h"
#include <stdlib.h>


Symbol* SymTable_get_recurse(SymTable* symTable,char*name)
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

Error handle_expression(exp_node* node,SymTable* tables,Type* returnType)
{
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
    case TOKEN_IDENTIFIER: //var/let
        Symbol* target = SymTable_get_recurse(tables,node->value.s);
        if(target == NULL || target->symbol_type == FUNCTION || target->symbol_type == UNKNOWN)
            return ERR_SEMATIC_UNDEFINED_VAR;
        *returnType = target->type;
        return OK;
        break;
   
        //OPERATORS
    case TOKEN_OPERATOR_MINUS: // - 
        break;
    case TOKEN_OPERATOR_MULTIPLICATION: // *
        break;
    case TOKEN_OPERATOR_DIVISION: // /
        break;

    //can handle int,double,string    
    case TOKEN_OPERATOR_PLUS:   // +
    case TOKEN_OPERATOR_LESS_THAN: // <
    case TOKEN_OPERATOR_GREATER_THAN:  // >
    case TOKEN_OPERATOR_LESS_THAN_OR_EQUAL:  // <=
    case TOKEN_OPERATOR_GREATER_THAN_OR_EQUAL: // >=
    case TOKEN_OPERATOR_EQUALS: // ==
    case TOKEN_OPERATOR_NOT_EQUALS: // !=
        Type a,b;
        Error aEr = handle_expression(node->left,tables,&a);
        if(aEr != OK)
            return aEr;
        Error bEr = handle_expression(node->right,tables,&b);
        if(bEr != OK)
            return bEr;
        if (a != b)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        *returnType = a;
        return OK;
        break;
    }
}

Error handle_statements(ASTNode* statement,SymTable* tables,Type returnType)
{
    while(statement != NULL)
    {   switch (statement->type)
        {
        case VAR_DEF:
            Symbol* var = Symbol_new();
            var->symbol_type=VAR;
            var->name=statement->a;
            var->type=statement->b;
            //var->nilable=;
            Error error = SymTable_insert(tables,var);
            if(error != OK)
            {
                free(var);
                return error;
            }
            break;
        case LET_DEF:
            Symbol* let = Symbol_new();
            let->symbol_type=LET;
            let->name=statement->a;
            let->type=statement->b;
            //let->nilable=;
            Error error = SymTable_insert(tables,let);
            if(error != OK)
            {
                free(let);
                return error;
            }
            break;
        case ASSIGN:
            Symbol* target = SymTable_get_recurse(tables,statement->a);
            if(target == NULL || target->symbol_type == FUNCTION)
                return ERR_SEMATIC_UNDEFINED_VAR;
            
            if(target->symbol_type == LET && target->initialized)
                return ERR_SEMATIC_REDEFINED;
            
            target->initialized=true;

            Type returnType;
            Error error = handle_expression(statement->b,tables,&returnType);
            if (error != OK)
                return error;

            if(target->type != returnType)
                return ERR_SEMATIC_INCOMPATIBLE_TYPES;
            
            break;
        case FUNC_CALL:

            break;
        case IFELSE:

            break;
        case WHILE:

            break;
        case RETURN:

            break;

        }
        statement = statement->b;

    }
    return OK;
}

void error_free_all(SymTable* tables)
{
    while(tables !=NULL)
    {
        SymTable *temp=tables->previous;
        SymTable_free(tables);
        tables = temp;
    }
}






Error sematic(ASTNode *code_tree)
{
    //GLOBAL SymTable
    SymTable *tables = malloc(sizeof(SymTable));
    SymTable_init(tables);
    tables->previous = NULL;


    //FUNCTIONS TO SYMTABLE
    ASTNode *functions = code_tree->a;
    while(functions != NULL)
    {
        
        ASTNode *func = (ASTNode*)(functions->b);
        Symbol *symbolFunc = Symbol_new();
        symbolFunc->symbol_type = FUNCTION;
        symbolFunc->args = (FuncDefArg*)((ASTNode*)(((ASTNode*)func->a)->a))->b;
        symbolFunc->name = ((ASTNode*)(((ASTNode*)func->a)->a))->a;
        symbolFunc->type = ((ASTNode*)func->a)->b;

        Error error= SymTable_insert(tables,symbolFunc);
        if(error != OK)
        {
            error_free_all(tables);
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
        Symbol* symbolFunc = SymTable_get(tables,((ASTNode*)(((ASTNode*)func->a)->a))->a);
        
        //func symtable  
        SymTable *innerTables = malloc(sizeof(SymTable));
        SymTable_init(innerTables);
        innerTables->previous = tables;

        //func body code check
        ASTNode *statement = func->b; 
        Error error = handle_statements(func->b,innerTables,symbolFunc->type);
        if (error != OK)
        {
            error_free_all(innerTables);
            return error; 
        }
        SymTable_free(innerTables);

        functions = functions->a;
    }

    ASTNode* main = code_tree->b;


    //MAIN BODY
    Error error = handle_statements(main,tables,NIL);



}

//funkce musí mít aspoň jeden statement? (return ?)
//zařídít fce body null

//return v globalu? NESMÍ BÝT

// odvozeni typu?

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