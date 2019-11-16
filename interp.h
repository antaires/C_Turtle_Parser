#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include "neillsdl2.h"
#include "bst.h"

#define MAXTOKENSIZE 50
#define PROGNAME "test.txt"
#define strsame(A, B) (strcmp(A, B) == 0)
/*test mode to prevent exit()*/
#define TEST_MODE if (istest_mode(p)){return false;};
#define ON_ERROR(PHRASE) TEST_MODE {fprintf(stderr, "ERROR %s occured in %s, line %d\n", PHRASE, __FILE__, __LINE__); exit(2); }

#define KEYWORDS 11

/*colors*/
#define RGBA 4
#define BLACK  0, 0, 0, 255
#define SET_BLACK "0", "0", "0", "255"
#define WHITE  255, 255, 255, 255
#define RED    255, 33, 0, 255
#define BLUE   0, 38, 255, 255
#define YELLOW 255, 255, 0, 255
#define ORANGE 255, 148, 0, 255
#define GREEN  0, 193, 29, 255
#define PURPLE 103, 0, 193, 255

/*sdl*/
#define CUSTOM_WIDTH 500
#define CUSTOM_HEIGHT 500
#define MILLSEC_DELAY 1000

enum _headings {up, down, right, left};

enum _colors {red, green, blue, alpha, black, yellow, orange, purple};
typedef enum _colors colors; 

struct _pos {
   int x;
   int y;
};
typedef struct _pos Pos; 

/*turtle structure*/
struct _turtle {
   Pos pos; 
   int color[RGBA]; 
   int heading; 
   int speed;
};
typedef struct _turtle Turtle;

/*SDL structure*/
struct _sdl {
   SDL_Simplewin sw;
   SDL_Rect rectangle;
};
typedef struct _sdl SDL; 

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
   Turtle t; 
   SDL sdl; 
   bool test_mode; 
};
typedef struct _prog Program; 

/*stack structures*/
struct _stack{
   Node* start; 
   Node* top;
};
typedef struct _stack Stack;

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
/*---GRAMMER EXTENSIONS---*/
bool if_(Program* p, Symtab* s); 
bool color(Program* p, Symtab* s);
bool heading(Program* p, Symtab* s);
bool pos(Program* p, Symtab* s);
bool rect(Program* p, Symtab* s);
bool speed(Program* p, Symtab* s); 
/*---INTERP---*/
void eval_polish(Program* p, Symtab* s, char* v);
char* evaluate(char oper, char* op1, char* op2);
void set_operator(char* oper, char* word);
char comparison_op(char* word);
bool eval_condition(char* val1, char* val2, char comp_op); 
/*---brackets---*/
bool brackets(Program *p);
bool isbracket(char* word);
/*---Stack---*/
bool empty(Stack *s);
void init_stack(Stack* s);
void push(Stack* s, char* c);
char* pop(Stack* s);
void free_stack(Stack *s);
/*---Prog ADT---*/
char* get_cw(Program *p);
char* get_pw(Program *p);
void set_w(Program *p, char* word);
void set_cn(Program* p, Node* n);
Node* get_cn(Program* p);
void next_word(Program* p);
void prev_word(Program *p);
Node* get_pn(Program* p);
Node* get_nn(Program* p);
Node* get_start(Program* p);
void set_start(Program* p);
void set_last(Program* p);
void free_prog(Program *p);
/*---internal Prog---*/
Node* init_node(void);
Node* init_token(char* word);
void frpr(Program *p); 
/*---turtle---*/
void set_color(Program* p, int color_name);
void set_heading(Program* p, char* w);
void delay(Program* p);
void copy_arr(int* copy, int* orig);
void set_color_value(Program* p, char* r, char* g, char* b, char* a);
bool set_color_name(Program* p);
/*---SDL---*/
void Altered_SDL_Init(SDL_Simplewin *sw);
void draw_line(Program* p, char* v); 
void turn_right(Program* p, char* v);
void turn_left(Program* p, char* v);
void turn_turtle(Program* p, int degrees);
void set_pos(Program* p, char* xval, char* yval);
void set_pos_int(Program* p, int xval, int yval);
void draw_rect(Program *p, char* w, char* h, bool fill);
void draw_rect_int(Program *p, int w, int h, bool fill);
/*---HELPER---*/
void prog_from_fp(Program *p, FILE* fp);
FILE* gfopen(char* filename); 
void init_prog(Program *p);
void print_prog(Node* start);
void set_current( char* word, char* current );
void str_to_int(int* n, char* s);
void int_to_str(char* s, int n);
void str_to_float(float* n, char* s);
void float_to_str(char* s, float n);
int deg_to_rad(int radians);
int rad_to_deg(int degrees);
bool keyword(char* var);
void enter_test_mode(Program *p);
void exit_test_mode(Program *p);
bool istest_mode(Program* p);
void check_argc(int argc);
void confirm_int( int* i, char* s);
void valid_range_check( int start, int end);
bool is_num(char* curr_word);
void store_pop(char* op, char* temp, Stack* stack);

void test_all_bst(void);
