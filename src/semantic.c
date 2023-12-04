#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "ast.h"
#include "symtable.h"
#include "lexer.h"
#include "buffer_string.h"
#include "semantic.h"

Error funcCallCheck(ASTNode*func,Type* returnType,SymTable* tables,SymTable* codeTable,int scope)
{
    SymTable* global = tables;
    while(global->previous != NULL)
        global = global->previous;
    //check if function exists
    Symbol* target = SymTable_get(global,func->a);
    if(target == NULL || target->type != FUNCTION)
        return ERR_SEMATIC_UNDEFINED_FUNC;
    //write() check
    const char*write="write";
    if(strcmp(func->a,write))
    {
        ASTNode* arg = func->b;
        while(arg !=NULL)
        {
            if(((ASTNode*)arg->a)->a != NULL)
                return ERR_SEMATIC_BAD_FUNC_ARG_TYPE;
            arg=arg->b;
        }
        return OK;
    }
    //arg check
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
        Error err = handle_expression(((ASTNode*)arg->a)->b,tables,&a,target->initialized);
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

    scope++;
    symTable_arg=target->args;
    //check function body
    SymTable* argSymTable = malloc(sizeof(SymTable)); 
    SymTable_init(argSymTable);
    argSymTable->previous= global;
    while (symTable_arg != NULL)
    {
        Symbol* argVar = Symbol_new();
        argVar->initialized=true;
        argVar->name= symTable_arg->name;
        argVar->nilable=symTable_arg->nilable;
        argVar->scope=scope;
        argVar->symbol_type=LET;
        argVar->type=symTable_arg->type;
        SymTable_insert(argSymTable,argVar);
        symTable_arg= symTable_arg->next;
    }
    ASTNode* function= 
    scope++;
    bool returning;
    Error err = handle_statements(target->func_def->b,argSymTable,codeTable,target->type,scope,&returning,target->initialized);


    target->initialized=true;
    return OK;

}

Error appendScope(char*name,char**out,int scope,SymTable* tables)
{
    BufferString* nameScoped;
        if(!BufferString_init_from(nameScoped,name))
            return ERR_INTERNAL;
        char integer[6];
        integer[0]='$';
        sprintf(integer+1, "%d", scope);
        if(!BufferString_append_str(nameScoped,integer))
            return ERR_INTERNAL;   
        free(name);
    *out = BufferString_get_as_string(nameScoped);
}
static Error handle_statement(ASTNode* statement,SymTable* tables,SymTable*codeTable,Type expected_type,int scoping,bool* returned,bool done)
{

    switch (statement->type)
    {
    case VAR_DEF:
    case LET_DEF:
        //make symtable symbol for variable
        Symbol* var = Symbol_new();
        var->symbol_type=(statement->type == VAR_DEF)?VAR:LET;
        var->name=((ASTNode*)statement->b)->a;
        var->type=((ASTNode*)statement->a)->a;
        var->nilable=((ASTNode*)statement->a)->b;
        var->scope = scoping;
        Error error = SymTable_insert(tables,var);
        if(error != OK)
        {
            free(var);
            return error;
        }
        //in ast, replace var name with varname$scope
        char* namedScope;
        Error err = appendScope(((ASTNode*)statement->b)->a,&namedScope,scoping,tables);
        if (err != OK)
            return err;
        ((ASTNode*)statement->b)->a = namedScope;
        //if in global, insert into code generator symtable
        if(expected_type == TYPE_NONE)
        {
            Symbol *generatedSymbol = Symbol_new();
            generatedSymbol->name=namedScope;
            SymTable_insert(codeTable,generatedSymbol);
        }
        // AMIDIATE ASIGN
        if (((ASTNode*)statement->b)->b == NULL)
        {
            if (var->nilable ==false)
                return ERR_SEMATIC_BAD_TYPE_INFERENCE;  //kills variables TYPE_NIL without infering
            else
                return OK;
        }
        Type expReturnType;
        Error error = handle_expression(((ASTNode*)statement->b)->b,tables,&expReturnType,done);
        if (error != OK )
            return error;
        if(var->type == TYPE_NIL)
        {   
            if(expReturnType == TYPE_NIL || expReturnType == TYPE_BOOL)
                return ERR_SEMATIC_BAD_TYPE_INFERENCE;
            else
            {
                var->initialized = true;
                var->type = expReturnType; 
                return OK;
            }
        }else
        {
            if(expReturnType == TYPE_NIL && !var->nilable)
                return ERR_SEMATIC_BAD_TYPE_INFERENCE;
            if(expReturnType == TYPE_NIL)
            {
                var->initialized = true;
                return OK;
            }
            if (var->type != expReturnType)
                return ERR_SEMATIC_BAD_TYPE_INFERENCE;
            else
            {
                var->initialized = true;
                return OK;
            }
        }
        break;

    case ASSIGN:

        Symbol* target = SymTable_get_recurse(tables,statement->a);
        
        //var/let exists check
        if(target == NULL || target->symbol_type == FUNCTION)   
            return ERR_SEMATIC_UNDEFINED_VAR;
        //check let init
        if(target->symbol_type == LET && target->initialized)  
            return ERR_SEMATIC_REDEFINED;
        
        target->initialized=true;

        Type expReturnType;
        Error error = handle_expression(statement->b,tables,&expReturnType,done);
        if (error != OK)
            return error;
       
        //in ast, replace var name with varname$scope
        BufferString* nameScoped;
        error = appendScope(statement->a,&nameScoped,target->scope,tables);
        if(error != OK)
            return error;
        statement->a=nameScoped;


        if(expReturnType == TYPE_NIL && !target->nilable)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        if(expReturnType == TYPE_NIL)
            return OK;        
        if(target->type != expReturnType)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        break;
    case FUNC_CALL:
        //get func symbol from global symtable
        SymTable* global = tables;
        while(global->previous!=NULL)
            global = global->previous;
        Symbol* target = SymTable_get(tables,statement->a);
        if(target == NULL || target->symbol_type == VAR || target->symbol_type == LET)   
            return ERR_SEMATIC_UNDEFINED_FUNC;
        //check function arguments and func body
        Type dump = TYPE_NONE;
        Error err = funcCallCheck(statement->a,&dump,tables,codeTable,scoping);
        if (err !=OK)
            return err;
        return OK;
        break;
    case IFELSE:
        //check condition
        Type condReturn=TYPE_NIL;
        Error err = handle_expression(statement->a,tables,&condReturn,done);
        if (err != OK)
            return err;
        if(condReturn != TYPE_BOOL)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;

        SymTable* localTable = malloc(sizeof(SymTable));
        SymTable_init(localTable);
        localTable->previous=tables;
        bool aReturn,bReturn = false;
        err = handle_statements(((ASTNode*)statement->b)->a,localTable,codeTable,expected_type,scoping+1,&aReturn,done);
        Symbol_free(localTable);
        if(err != OK)
            return err;
        localTable = malloc(sizeof(SymTable));
        SymTable_init(localTable);
        
        err = handle_statements(((ASTNode*)statement->b)->b,localTable,codeTable,expected_type,scoping+1,&bReturn,done);
        SymTable_free(localTable);
        if(err != OK)
            return err;
        if(aReturn ^ bReturn)
            return ERR_SEMATIC_NON_VOID_FUNC_DOESNT_RETURN_VALUE;
        
        return OK;
        break;
    case WHILE:
        Type condReturn=TYPE_NIL;
        Error err = handle_expression(statement->a,tables,&condReturn,done);
        if (err != OK)
            return err;
        if(condReturn != TYPE_BOOL)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        SymTable* localTable = malloc(sizeof(SymTable));
        SymTable_init(localTable);
        localTable->previous=tables;
        err = handle_statements(statement->b,localTable,codeTable,expected_type,scoping+1,returned,done);
        SymTable_free(localTable);
        if (err != OK)
            return err;
        return OK;
        break;
    case RETURN:
        switch (expected_type)
        {
            case TYPE_NONE:
                return OK;
                break;
            case TYPE_NIL:
                if (statement->a != NULL)
                    return ERR_SEMATIC_VOID_FUNC_RETURNS_VALUE;
                break;
            default:
                if (statement->a == NULL)
                    return ERR_SEMATIC_NON_VOID_FUNC_DOESNT_RETURN_VALUE;
                Type condReturn=TYPE_NIL;
                Error err = handle_expression(statement->a,tables,&condReturn,done);
                if (err != OK)
                    return err;
                if(condReturn != expected_type)
                    return ERR_SEMATIC_BAD_FUNC_RETURN_TYPE;
                break;
        }
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



Error handle_statements(ASTNode*statement,SymTable* tables,SymTable*codeTable,Type expected_type,int scoping,bool* returned,bool done)
{

    SymTable* localTable = malloc(sizeof(SymTable));
    SymTable_init(localTable);
    localTable->previous=tables;
    while(statement!= NULL)
    {   
        bool returning=false;
        Error err = handle_statement(statement,localTable,codeTable,expected_type,scoping,&returning,done);
        if(err != OK)
        {
            SymTable_free(localTable);
            return err;
        }
        *returned = returning;
        statement=statement->b;
    }
    return OK;
}


Error sematic(ASTNode *code_tree,SymTable* codeTable)
{
    //GLOBAL SymTable
    
    SymTable_init(codeTable);
    codeTable->previous = NULL;

    SymTable* globalTable;
    SymTable_init(globalTable);
    
    //USER FUNCTIONS TO SYMTABLE
    ASTNode *functions = code_tree->a;
    while(functions != NULL)
    {
        
        ASTNode *func = functions->b;
       
        Symbol *symbolFunc = Symbol_new();
        symbolFunc->symbol_type = FUNCTION;
        symbolFunc->args = ((ASTNode*)(((ASTNode*)func->a)->a))->b;
        symbolFunc->name = ((ASTNode*)(((ASTNode*)func->a)->a))->a;
        symbolFunc->type = ((ASTNode*)func->a)->b;
        symbolFunc->func_def= func;
        Error error= SymTable_insert(globalTable,symbolFunc);
        if(error != OK)
        {
            error_free_all(globalTable);
            Symbol_free(symbolFunc);
            return error;
        }

       functions = functions->a;
    }
    //INIT GLOBAL VARIABLES
    ASTNode** statement = &code_tree->b;
    while(statement!=NULL)
    {
        if (((ASTNode*)(*statement)->a)->type == VAR_DEF || ((ASTNode*)(*statement)->a)->type == LET_DEF)
        {
            //insert into symtable
            exp_node* tempExp = ((ASTNode*)((ASTNode*)(*statement)->a)->b)->b;
            ((ASTNode*)((ASTNode*)(*statement)->a)->b)->b = NULL;
            bool dump;
            Error err = handle_statement((*statement)->a,globalTable,codeTable,TYPE_NONE,0,&dump,false);
            if(err != OK)
                return err;
         

            //move def to root
            //statement is first found defvar/letvar

            ASTNode*newRoot = *statement;  //newRoot = root->right->right->left
            ASTNode*tempRoot = code_tree->b; //tempRoot = root->right
            code_tree->b= newRoot; // root->right = root->right->right->left       
            //*statement=(*statement)->b; //statement = root->right->right->left->right 

            //replace with assign if needed 
            if(((ASTNode*)newRoot->b)->b != NULL)   
            {
                ASTNode* newAssign = ASTNode_new(ASSIGN);
                newAssign->a=((ASTNode*)((ASTNode*)newRoot->a)->b)->a;
                newAssign->b=((ASTNode*)((ASTNode*)newRoot->a)->b)->b;
                (*statement)->a = newAssign;
                //(*statement)->b= newRoot->b; //statement->b = root->right->left->right->left ?
            } 
            newRoot->b=tempRoot;
            
        }
        statement = &(*statement)->b;
    }

    //start body check
    int scoping = 0;
    ASTNode *globalStatement = code_tree->b;
    while (globalStatement != NULL &&(globalStatement->type == LET_DEF || globalStatement->type == VAR_DEF))
        globalStatement = globalStatement->b;
    bool returning=false;
    Error err = handle_statements(code_tree->b,globalTable,codeTable,TYPE_NONE,0,&returning,false);
    if (err != OK)
        return err;
    if(returning)
        return ERR_SEMATIC_VOID_FUNC_RETURNS_VALUE;
    /*  
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

        SymTable *funcBodyTable = malloc(sizeof(SymTable));
        SymTable_init(funcBodyTable);
        funcBodyTable->previous = funcArgTable;
        
        //func body code check
        ASTNode *statement = func->b; 
        Error error = handle_statements(func->b,funcBodyTable,symbolFunc->type);
        if (error != OK)
        {
            error_free_all(funcBodyTable);
            return error; 
        }
        SymTable_free(funcBodyTable);
        SymTable_free(funcArgTable);

        functions = functions->a;
    }

    ASTNode* main = code_tree->b;


    //MAIN BODY
    Error error = handle_statements(main,globalTable,TYPE_NONE);
*/


}

//funkce musí mít aspoň jeden statement? (return ?)
//zařídít fce body null

//return v globalu? NESMÍ BÝT


//typy u operátorů, (int opTYPE_DOUBLE ? )
//string operátory


//stránka 6, wtf nelze měnit hodnotu?
//parametry jsou neměnné
//můžou se předefinovat


//funkce void nemusí mít return,


//definice typu proměnné, not defined, a odvození typu při not defined

// když při definici nelze odvodit, chyba 8 (např. hondnota nil.)
//když nechybý typ, nemusí být = výraz
//chyba 7, špatný typ výrazu při přiřazení // expression v if () není typu TYPE_BOOL

// v if () může být let id !!!
// musím checknout jestli proměnná id existuje

//VOID FCE NESMÍ MÍT RETURN, JINAK ERR 4 (ERR 4 TAKY KDYŽ TYP RETURN NESEDÍ)
//CHYBÍ LI RETURN V NOT VOID FCI, ERR 6
//VESTAVĚNÉ FCE DO SYMTABLE
/*
    INT + INT = INT
    INT +TYPE_DOUBLE =TYPE_DOUBLE
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


//TYPE_BOOL NESMÍ BÝT V UVNITŘ EXPU







//volání fce parametr _