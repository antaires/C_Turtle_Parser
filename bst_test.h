#include "interp.h"

/*_______________________________________
 |          INTERPRETER TESTS            |
 |_______________________________________|
 |---------------------------------------|*/

/*basic BST functions*/
void test_symtab_insert(void);
void test_symtab_isin(void);
void test_symtab_update(void);
void test_set_get_varval(void);
/*integration of BST and interpreter*/
/*tests functions which deal with variables*/
void test_var(void);
void test_polish(void);
void test_varnum(void);
void test_set(void);
void test_do_(void);
void test_fd(void);
void test_rt(void);
void test_lt(void);
void test_instruction(void);
void test_inst_list(void);
void test_extensions(void);
/*evaluation of expressions*/
void test_evaluate(void);
/*basic prog and stack functions unchanged 
  and were tested in parse tests          */
void prog_from_str(Program* p, char* str);
/*graphics/SDL systems*/
void test_pos_draw_turn(void);
/*an informal test of visual performance*/
/*many visual tests were created, only 1 included*/
void test_visual(void);

