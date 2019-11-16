#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include "neillsdl2.h"

#define MAXTOKENSIZE 50
#define strsame(A, B) (strcmp(A, B) == 0)
#define TEST_MODE if (istest_mode(p)){return false;}
#define ERROR(PHRASE) {fprintf(stderr, "ERROR %s occured in %s, line %d\n", PHRASE, __FILE__, __LINE__); exit(2); }
#define ON_ERROR(PHRASE) TEST_MODE {fprintf(stderr, "ERROR %s occured in %s, line %d\n", PHRASE, __FILE__, __LINE__); exit(2); }

/*colors*/
#define RGBA 4
#define BLACK  0, 0, 0, 255
#define WHITE  255, 255, 255, 255
#define RED    255, 33, 0, 255
#define BLUE   0, 38, 255, 255
#define YELLOW 255, 255, 0, 255
#define ORANGE 255, 148, 0, 255
#define GREEN  0, 193, 29, 255
#define PURPLE 103, 0, 193, 255

enum _bool {false, true};
typedef enum _bool bool;

enum _headings {up, down, right, left};

enum _colors {red, green, blue, alpha, black, yellow, orange, purple};
typedef enum _colors colors; 

/*program structures*/
struct _node {
   char* w;
   struct _node *next;
   struct _node *prev; 
};
typedef struct _node Node;

struct _prog{
   Node* start; 
   Node* cw; 
   bool test_mode;
};
typedef struct _prog Program; 

/*stack structures*/
struct _stack{
   Node* start; 
   Node* top;
};
typedef struct _stack Stack;

/*symbol table structures*/
struct _var {
   char name[MAXTOKENSIZE];  
   char val[MAXTOKENSIZE]; 
   bool used; 
   struct _var *next; 
};
typedef struct _var Varnode; 

struct _symbol {
   Varnode* start; 
   Varnode* last; 
   Varnode* curr; 
};
typedef struct _symbol Symtab;

/*---PRIMARY---*/
void prog(Program *p, Symtab* s); 
void inst_list(Program *p, Symtab* s);
void instruction(Program *p, Symtab* s);
bool fd(Program *p, Symtab* s);
bool lt(Program *p, Symtab* s);
bool rt(Program *p, Symtab* s);
bool do_(Program *p, Symtab* s);
bool var(Program *p);
bool varnum(Program *p, Symtab* s, char* v);
bool set(Program *p, Symtab* s);
void polish(Program *p, Symtab* s, char* v);
bool op(Program *p);
/*---EXTENSIONS---*/
bool if_(Program* p, Symtab* s); 
bool color(Program* p, Symtab* s);
bool heading(Program* p, Symtab* s);
bool pos(Program* p, Symtab* s);
bool rect(Program* p, Symtab* s);
bool speed(Program* p, Symtab* s); 
/*---STACK---*/
bool brackets(Program *p);
void init_stack(Stack *s);
void push(Stack *s, char* c);
char* pop(Stack *s);
bool empty(Stack *s); 
Node* init_stacknode(char* c);
void free_stack(Stack *s);
/*---Prog---*/
char* get_cw(Program *p);
char* get_pw(Program *p);
char* get_nw(Program *p);
void set_w(Program *p, char* word);
void next_word(Program* p);
void free_prog(Program *p);
/*---internal Prog---*/
void frpr(Program *p); 
Node* init_node(void);
Node* init_token(char* word);
/*---SYMTAB---*/
void symtab_update(Symtab* st, char* n, char* v);
Varnode* init_varnode(char* n, char* v);
Symtab* init_symtab(void);
void add_varnode(Symtab* st, Varnode* vn);
void update_varnode(Symtab* st, char* n, char* v); 
char* get_varval(Symtab* st, char* n);
bool has_var(Symtab* st, char* n);
bool unused_var(Symtab* st); 
void print_symtab(Symtab* st);
void free_symtab(Symtab** st); 
/*---INTERP---*/
char* evaluate(char op, char* op1, char* op2);
void set_operator(char* op, char* word);
bool comparison_op(char* word);
/*---HELPER---*/
void prog_from_fp(Program *p, FILE* fp);
FILE* gfopen(char* filename); 
void init_prog(Program *p);
void print_prog(Node* start);
bool isbracket(char* word);
void set_current( char* word, char* current );
void str_to_int(int* n, char* s);
void int_to_str(char* s, int n);
void str_to_float(float* n, char* s);
void float_to_str(char* s, float n);
bool valid_color(Program* p);
void exit_test_mode(Program *p);
void enter_test_mode(Program *p);
bool istest_mode(Program* p);
void check_argc(int argc);
void set_last(Program* p);
Node* get_pn(Program* p);
Node* get_nn(Program* p);
Node* get_start(Program* p);
void set_start(Program* p);
void valid_range_check( int start, int end);
bool is_num(char* curr_word);
Node* get_cn(Program* p);
void set_cn(Program* p, Node* n);

void test_all(void);
