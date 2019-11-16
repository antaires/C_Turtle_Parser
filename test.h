#include "parse.h"

/*---TESTING---*/
void test_init_prog(Program *p);
void test_init_node(void);
void test_init_token(void);
void test_set_w(Program* p);
void test_next_word(Program* p); 
/*test stack*/
void test_init_stack(void);
void test_free_stack(void);
void test_push(void);
void test_pop(void);
void test_init_stacknode(void);
void test_empty(void);
/*brackets*/
void test_brackets(void); 
void prog_from_str(Program* p, char* str);
/*grammar*/
void test_prog(void); 
void test_inst_list(void);
void test_instruction(void);
void test_fd(void);
void test_lt(void);
void test_rt(void);
void test_do_(void);
void test_var(void);
void test_varnum(void);
void test_set(void);
void test_polish(void);
void test_op(void); 
/*test extensions*/
void test_extensions(void);
/*test symbol table*/
void test_init_varnode(void);
void test_init_symtab(void);
void test_add_varnode(void);
void test_update_varnode(void);
void test_get_has_used(void);
void test_unused_var(void);
void test_free_symtab(void);
