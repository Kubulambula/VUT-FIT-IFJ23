#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "ast.h"
#include "symtable.h"
#include "lexer.h"
#include "buffer_string.h"
#include "semantic.h"
void print_symtable(SymTable* symtable)
{
    while(symtable != NULL)
    {
        printf("\n-------------\n");
        for (int i = 0; i < symtable->size+1; i++)
        {
            if(symtable->table[i] != NULL)
                printf("%d:'%s'\n",symtable->table[i]->scope,symtable->table[i]->name);
        }
        printf("---------------\n");
        fflush(stdout);
        symtable = symtable->previous;
    }
}
void prin()
{
    fprintf(stderr, "here\n");
}
Error funcCallCheck(ASTNode* func, Type* returnType, SymTable* tables, SymTable* codeTable, bool* nillable){
    int scope = 0;
    SymTable* global = tables;
    while(global->previous != NULL)
        global = global->previous;
    //check if function exists
    
    Symbol* target = SymTable_get(global,func->a);
    
    if(target == NULL || target->symbol_type != FUNCTION)
        return ERR_SEMATIC_UNDEFINED_FUNC;
    //write() check
    if(strcmp(func->a, "write") == 0)
    {
        ASTNode* arg = func->b;
        while(arg != NULL)
        {
            if(((ASTNode*)arg->a)->a != NULL)
                return ERR_SEMATIC_BAD_FUNC_ARG_TYPE;
            Type dump_type;
            bool dump_nilable;
            ERR = handle_expression(((ASTNode*)arg->a)->b, tables, &dump_type, codeTable, scope, &dump_nilable);
            if(ERR)
                return ERR;
        
            arg = arg->b;
        }
        return OK;
    }

    //arg check
    ASTNode* arg = func->b;
    FuncDefArg* symTable_arg = target->args;
    while(arg != NULL && symTable_arg != NULL)
    {
        if(symTable_arg->name == NULL || ((ASTNode*)arg->a)->a == NULL)
        {
            if(symTable_arg->name != NULL || ((ASTNode*)arg->a)->a != NULL)
                return ERR_SEMATIC_BAD_FUNC_ARG_TYPE;
        }
        else if(strcmp(symTable_arg->name,((ASTNode*)arg->a)->a) != 0)
            return ERR_SEMATIC_BAD_FUNC_ARG_TYPE;
        Type a_type;
        bool a_nilable;
        ERR = handle_expression(((ASTNode*)arg->a)->b, tables, &a_type, codeTable, scope, &a_nilable);
        if(ERR)
            return ERR;
        if (a_type != symTable_arg->type || (a_nilable != !symTable_arg->nilable))
            return ERR_SEMATIC_BAD_FUNC_ARG_TYPE;
        
        symTable_arg = symTable_arg->next;
        arg = arg->b;
    }
     
    if(arg != NULL && symTable_arg != NULL)
        return ERR_SEMATIC_BAD_FUNC_ARG_COUNT;
    
    *returnType = target->type;
    *nillable = target->nilable;
    scope++;
    symTable_arg = target->args;
    
  
    //check function args
    SymTable* argSymTable = malloc(sizeof(SymTable)); 
    if(argSymTable == NULL)
        return ERR_INTERNAL;
    if(!SymTable_init(argSymTable))
    {
        free(argSymTable);
        return ERR_INTERNAL;
    }
    argSymTable->previous = global;
    while (symTable_arg != NULL)
    {
        Symbol* argVar = Symbol_new();
        if(argVar == NULL)
        {
            SymTable_free(argSymTable);
            return ERR_INTERNAL;
        }
        argVar->initialized=true;
        argVar->name= symTable_arg->name;
        ERR = appendScope(&(argVar->name),0);
        if(ERR)
        {
            SymTable_free(argSymTable);
            return ERR;
        }
        argVar->nilable=symTable_arg->nilable;
        argVar->scope=scope;
        argVar->symbol_type=LET;
        argVar->type=symTable_arg->type;
        ERR = SymTable_insert(argSymTable,argVar);
        if(ERR)
        {
            Symbol_free(argVar);
            SymTable_free(argSymTable);
            return ERR;
        }
        symTable_arg= symTable_arg->next;
    }

    bool skip = false;
    const char predefined[10][11]= {"write","readString","readInt","readDouble","Int2Double","Double2Int","length","substring","ord","chr"};
    for (int i = 0; i < 11; i++)
    {
        if(strcmp(func->a,predefined[i]) == 0)
            skip = true;
    }
    
    if(!skip)
    {
        bool returning;
        ERR = handle_statements(target->func_def->b,argSymTable,codeTable,target->type,&scope,&returning,target->nilable,target->initialized,false);
        SymTable_free(argSymTable);
        
        if(ERR)
            return ERR;



        //returning check
        if(target->type != TYPE_NIL)
            if(!returning)
                return ERR_SEMATIC_NON_VOID_FUNC_DOESNT_RETURN_VALUE;
    }

    target->initialized=true;
    return OK;

}

Error appendScope(char**name,int scope)
{
    BufferString* nameScoped = malloc(sizeof(BufferString));
    if(!BufferString_init_from(nameScoped,*name))
        return ERR_INTERNAL;
    unsigned length = snprintf(NULL,0, "$%d ", scope);
    char* integer = malloc(length);
    sprintf(integer, "$%d", scope);

    if(!BufferString_append_str(nameScoped,integer))
        return ERR_INTERNAL;   
    free(*name);
    *name = BufferString_get_as_string(nameScoped);
    free(integer);
    return OK;
}


static Error handle_statement(ASTNode* statement ,SymTable* tables, SymTable*codeTable, Type expected_type,int* scope, bool* returned, bool nilable_return,bool second_pass){
    Symbol *generatedSymbol, *target;
    Type expReturnType;
    bool expNillable;
    bool aReturned, bReturned,nillable_modified = false;
    char* dollar;
    switch (statement->type){
    case VAR_DEF:
    case LET_DEF:
        if(!second_pass)
        {
            //make symtable symbol for variable
            generatedSymbol = Symbol_new();
            if (generatedSymbol == NULL)
                return ERR_INTERNAL;
            
            generatedSymbol->symbol_type = (statement->type == VAR_DEF)? VAR : LET;


            generatedSymbol->name = malloc(strlen((char*)(((ASTNode*)statement->b)->a)));
            if(generatedSymbol->name== NULL)
                return ERR_INTERNAL;
            strcpy(generatedSymbol->name,(char*)(((ASTNode*)statement->b)->a));                       
            generatedSymbol->type = (Type)(((ASTNode*)(statement->a))->a);
            generatedSymbol->nilable = (bool)(((ASTNode*)statement->a)->b);
            generatedSymbol->scope = *scope;
            
            ERR = SymTable_insert(tables, generatedSymbol);
            if(ERR){
                Symbol_free(generatedSymbol);
                return ERR;
            }
            
            // change the name to varname$scope
            ERR = appendScope((char**)&((ASTNode*)statement->b)->a, *scope);
            if (ERR)
                return ERR;

             // if in global, insert into code generator symtable
            if(expected_type == TYPE_NONE){
                target = Symbol_copy(generatedSymbol);
                if (target == NULL)
                    return ERR_INTERNAL;
                //into code table insert name$scope
                target->name = (char*)(((ASTNode*)statement->b)->a);

                ERR = SymTable_insert(codeTable, target);
                if (ERR){
                    Symbol_free(target);
                    return ERR;
                }
            }
        }else
        {
            dollar = strstr((char*)(((ASTNode*)statement->b)->a), "$");
            if(dollar != NULL)   // if jmeno promene obsahuje $
                *dollar = '\0';   // potom $ -> '\0'
            else
                return ERR_INTERNAL;
            generatedSymbol = SymTable_get(tables,(char*)(((ASTNode*)statement->b)->a));
            *dollar = '$';
        }


        // check valid inference
        if (((ASTNode*)statement->b)->b == NULL && generatedSymbol->type == TYPE_NIL) // if no expression and no specified type from syntax
            return ERR_SEMATIC_BAD_TYPE_INFERENCE;
        if (((ASTNode*)statement->b)->b != NULL)
        {
            ERR = handle_expression((exp_node*)(((ASTNode*)statement->b)->b), tables, &expReturnType, codeTable, *scope,&expNillable);
            if (ERR)
                return ERR;
            
            if(generatedSymbol->type == TYPE_NIL){ // no type specified from syntax - inference   
                if(expReturnType == TYPE_NIL || expReturnType == TYPE_BOOL) // if expression return is nil or bool
                    return ERR_SEMATIC_BAD_TYPE_INFERENCE;
                else{
                    generatedSymbol->initialized = true;
                    generatedSymbol->type = expReturnType;
                    generatedSymbol->nilable=expNillable; 
                }
            }else{ // type was specified from syntax - no inference
                if(expReturnType == TYPE_NIL && !generatedSymbol->nilable) // if expression returned nil and type is not nilable
                    return ERR_SEMATIC_BAD_TYPE_INFERENCE;
                else if(expReturnType == TYPE_NIL && generatedSymbol->nilable) // if expression returned nil and type is nilable
                    generatedSymbol->initialized = true;
                else if (generatedSymbol->type != expReturnType) // if expression returned differend type from specified type
                    return ERR_SEMATIC_BAD_TYPE_INFERENCE;
                else if(expNillable && !generatedSymbol->nilable)// assigning nillable to not nillable
                    return ERR_SEMATIC_BAD_TYPE_INFERENCE;
                else //expression has same type as var/let
                    generatedSymbol->initialized = true;
            }

        }
        
        return OK;

    case ASSIGN:
      
        if(second_pass)
        {
            dollar = strstr(statement->a, "$");
            if(dollar != NULL)   // if jmeno promene obsahuje $
                *dollar = '\0';   // potom $ -> '\0'
            else
                return ERR_INTERNAL;
            target = SymTable_get_recurse(tables,statement->a);
            *dollar = '$';
        }
        else
            target = SymTable_get_recurse(tables,statement->a);
        
        
        //var/let exists check
        if(target == NULL || target->symbol_type == FUNCTION)   
            return ERR_SEMATIC_UNDEFINED_VAR;
        //check let init
        if(target->symbol_type == LET && target->initialized)  
            return ERR_SEMATIC_REDEFINED;
        
        target->initialized=true;


        ERR = handle_expression(statement->b,tables,&expReturnType,codeTable,*scope,&expNillable);
        if (ERR)
            return ERR;
       
        if(expReturnType == TYPE_NIL && !target->nilable)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        else if(expReturnType == TYPE_NIL)
            {}                                              //DISCUSTING  
        else if(target->type != expReturnType)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        else if(expNillable && !target->nilable)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;
        if(!second_pass)
        {
            //in ast, replace var name with varname$scope
            
            ERR = appendScope((char**)&(statement->a), target->scope);
            if (ERR)
                return ERR;
        }
        return OK;
    case FUNC_CALL:
        ERR = funcCallCheck(statement,&expReturnType,tables,codeTable,&aReturned);
        prin();
        if (ERR)
            return ERR;
        return OK;
    case IFELSE:
        //check condition
        ERR = handle_expression(statement->a,tables,&expReturnType,codeTable,*scope,&expNillable);
        if (ERR)
            return ERR;
        if(expReturnType != TYPE_BOOL)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;

        //handle true branch
        ERR = handle_statements(((ASTNode*)statement->b)->a,tables,codeTable,expected_type,scope,&aReturned,nilable_return,second_pass,false);
        if(ERR)
            return ERR;
        //handle false branch
        ERR = handle_statements(((ASTNode*)statement->b)->b,tables,codeTable,expected_type,scope,&bReturned,nilable_return,second_pass,false);
        if(ERR)
            return ERR;
        
        if(aReturned && bReturned)
            *returned = true;
        return OK;
    case CHECK_IF_LET:
        target = SymTable_get_recurse(tables,statement->a);
        if (target == NULL || target->symbol_type != LET)   //CHECK WITH TEAM
            return ERR_SEMATIC_UNDEFINED_VAR;
        
        ERR = appendScope(statement->a,target->scope);
        if (ERR)
            return ERR;
        if(target->nilable)
        {
            target->nilable = false;
            nillable_modified=true;    
        }
        //handle true branch    
        ERR = handle_statements(((ASTNode*)statement->b)->a,tables,codeTable,expected_type,scope,&aReturned,nilable_return,second_pass,false);
        if(ERR)
            return ERR;
        if(nillable_modified)
        {
            nillable_modified=false;
            target->nilable=true;
        }
        //handle false branch
        ERR = handle_statements(((ASTNode*)statement->b)->b,tables,codeTable,expected_type,scope,&bReturned,nilable_return,second_pass,false);
        if(ERR)
            return ERR;
        if(aReturned && bReturned)
            *returned = true;
        return OK;
    case WHILE:
        expReturnType=TYPE_NIL;
        ERR = handle_expression(statement->a,tables,&expReturnType,codeTable,*scope,&expNillable);
        if (ERR)
            return ERR;
        if(expReturnType != TYPE_BOOL)
            return ERR_SEMATIC_INCOMPATIBLE_TYPES;

        ERR = handle_statements(statement->b,tables,codeTable,expected_type,scope,returned,nilable_return,second_pass,false);
        if (ERR)
            return ERR;
        return OK;
        break;
    case RETURN:
        switch (expected_type)
        {
            case TYPE_NONE:
                return ERR_SEMATIC_VOID_FUNC_RETURNS_VALUE;
                break;
            case TYPE_NIL:
                if (statement->a != NULL)
                    return ERR_SEMATIC_VOID_FUNC_RETURNS_VALUE;
                *returned=true;
                break;
            default:
                if (statement->a == NULL)
                    return ERR_SEMATIC_NON_VOID_FUNC_DOESNT_RETURN_VALUE;
                ERR = handle_expression(statement->a,tables,&expReturnType,codeTable,*scope,&expNillable);
                if (ERR)
                    return ERR;
                if(expReturnType == TYPE_NIL && !nilable_return)
                    return ERR_SEMATIC_BAD_FUNC_RETURN_TYPE;
                if(expReturnType == TYPE_NIL)
                    return OK;
                if(expReturnType != expected_type)
                    return ERR_SEMATIC_BAD_FUNC_RETURN_TYPE;
                if(expNillable && !nilable_return)
                    return ERR_SEMATIC_BAD_FUNC_RETURN_TYPE;
                break;
        }
        return OK;
    default:
        return ERR_INTERNAL;
    }
    return ERR_INTERNAL;
}

Error handle_statements(ASTNode* statement, SymTable* tables, SymTable* codeTable, Type expected_type, int* scope, bool* returned, bool nilable_return,bool second_pass,bool main){
    
    SymTable* localTable;
    if(!main)
    {
        localTable = malloc(sizeof(SymTable));
        if(!SymTable_init(localTable)){
            free(localTable);
            return ERR_INTERNAL;
        }
        localTable->previous = tables;
        (*scope)++;
    }else
    {
        localTable=tables;
    }
    while(statement != NULL){
        bool statementReturned = false;
        ERR = handle_statement(statement->a, localTable, codeTable, expected_type, scope, &statementReturned, nilable_return,second_pass);
        if(ERR){
            if (!main)
                SymTable_free(localTable);
            return ERR;
        }
        if(statementReturned)
            *returned = true;

        // go to the next statement
        statement = (ASTNode*)(statement->b);
    }
    if (!main)
        SymTable_free(localTable);
    return OK;
}


// za codeTable je zodpovedny main()
Error semantic(ASTNode *code_tree, SymTable* codeTable){
    SymTable* globalTable = malloc(sizeof(SymTable));
    if (globalTable == NULL)
        return ERR_INTERNAL;
    if (!SymTable_init(globalTable)){
        free(globalTable);
        return ERR_INTERNAL;
    }
        
    // Add functions to symtables
    ERR = add_functions_to_symtable(code_tree, globalTable, codeTable);
    if (ERR){
        SymTable_free(globalTable);
        return ERR;
    }

    // rearrange var definition statements and split them into declaration and assignment
    ERR = rearrange_global_statements(code_tree, codeTable, globalTable);
    if (ERR){
        SymTable_free(globalTable);
        return ERR;
    }
    //start body check
    bool returning = false;
    int scope = 0;
    ERR = handle_statements(code_tree->b, globalTable, codeTable, TYPE_NONE, &scope, &returning, false,false,true);
    
    SymTable_free(globalTable);
    if (ERR)
        return ERR;

    return OK;
}


Error add_functions_to_symtable(ASTNode* root, SymTable* global_table, SymTable* code_table){
    ASTNode* func_defs = root->a;
    while(func_defs != NULL){
        ASTNode* func_def = (ASTNode*)(func_defs->b);
        ASTNode* func_head = (ASTNode*)(func_def->a);
        ASTNode* func_head_signature = (ASTNode*)(func_head->a);

        // create & setup function symbol
        Symbol* func_symbol = Symbol_new();
        if (func_symbol == NULL)
            return ERR_INTERNAL;
        func_symbol->symbol_type = FUNCTION;
        func_symbol->func_def = func_def;
        func_symbol->name = (char*)(func_head_signature->a);
        func_symbol->type = (Type)(((ASTNode*)(func_head->b))->a);
        func_symbol->nilable = (bool)(((ASTNode*)(func_head->b))->b);
        func_symbol->args = (FuncDefArg*)(func_head_signature->b);

        ERR = SymTable_insert(global_table, func_symbol);
        if (ERR){
            Symbol_free(func_symbol);
            return ERR;
        }


        //check identical argument identifiers
        FuncDefArg* argument = func_symbol->args;
        while(argument != NULL)
        {
            FuncDefArg* compared_argument = argument->next;
            while(compared_argument != NULL)
            {
                if(strcmp(argument->identifier,compared_argument->identifier) == 0)
                    return ERR_SEMATIC_REDEFINED;
                compared_argument = compared_argument->next;
            }
            argument = argument->next;
        }

        // create & setup an identical symbol for the other SymTable, that is returned out of semantic
        func_symbol = Symbol_copy(func_symbol);
        if (func_symbol == NULL)
            return ERR_INTERNAL;

        ERR = SymTable_insert(code_table, func_symbol);
        if (ERR){
            Symbol_free(func_symbol);
            return ERR;
        }

        // get next function
        func_defs = func_defs->a;
    }

    return OK;
}


Error rearrange_global_statements(ASTNode* root, SymTable* codeTable, SymTable* globalTable){
    ASTNode** statement = (ASTNode**)&root->b;
    while(*statement != NULL)
    {
        if (!(((ASTNode*)((*statement)->a))->type == VAR_DEF || ((ASTNode*)((*statement)->a))->type == LET_DEF)){
            statement = (ASTNode**)&((*statement)->b);
            continue;
        }
        
        ASTNode* def_statement = *statement; // store the def statement

        exp_node* tempExp = ((ASTNode*)((ASTNode*)(def_statement->a))->b)->b; // expression of immideate assing
        ((ASTNode*)((ASTNode*)(def_statement->a))->b)->b = NULL; // throw it out the window

        if(tempExp != NULL){
            if ((Type)(((ASTNode*)(((ASTNode*)(def_statement->a))->a))->a) == TYPE_NIL){ // do inference
                Type expType;
                bool expNillable;
                ERR = handle_expression(tempExp, globalTable, &expType, codeTable, 0,&expNillable);
                if (ERR)
                    return ERR;
                if (expType == TYPE_NIL)
                    return ERR_SEMATIC_BAD_TYPE_INFERENCE;
                // assign the inferred type
                (((ASTNode*)(((ASTNode*)(def_statement->a))->a))->a) = (void*)expType;
                (((ASTNode*)(((ASTNode*)(def_statement->a))->a))->b) = (void*)expNillable;
            }

            // create new assign statement if necessary
            ASTNode* assign_statement = ASTNode_new(STATEMENT);
            if (assign_statement == NULL)
                return ERR_INTERNAL;
            ASTNode* assign = ASTNode_new(ASSIGN);
            if (assign == NULL){
                free(assign_statement);
                return ERR_INTERNAL;
            }
            
            assign_statement->a = (void*)assign;

            assign->a = malloc(strlen(((ASTNode*)((ASTNode*)((*statement)->a))->b)->a)+1);
            strcpy(assign->a,((ASTNode*)((ASTNode*)((*statement)->a))->b)->a);  // assign the name of the declared variable
            assign->b = (void*)tempExp; // assign the expression of the declared variable
            
            *statement = assign_statement;
            assign_statement->b = def_statement->b;

            // set statement to check to the next statement
            statement = (ASTNode**)&(assign_statement->b);
        }else{
            // set statement to check after the original statement
            statement = (ASTNode**)&(def_statement->b);
        }

        // insert def_statement as the first statement
        def_statement->b = root->b;
        root->b = def_statement; 
    }
    return OK;
}



