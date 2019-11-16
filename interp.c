/*_______________________________________
 |          TURTLE INTERPRETER           |
 |---------------------------------------|
 |                                       |
 |              EXTENSIONS               |
 |---------------------------------------|
 | 1. Uses a BST as a symbol table to    |
 |allow for efficient look-up & storage  |
 |of VARIABLES of any type. Keywords are |
 |not allowed (SET, DO etc) and otherwise|
 |follow rules of C variables.           |
 |                                       |
 | 2. Keeps additional grammatical       |
 |extensions of the parser ( Speed, Pos, |
 |Rect, Color, Heading, If).             |
 |                                       |
 | 3. Uses a linked list for the prog to |
 |handle any size programs.              |
 |                                       |  
 | 4. The prog. structure has been       |
 |designed as an ADT, with clear funcs to|
 |access its various elements, which     |
 |include the turtle (storing position,  |
 |heading, color, etc), the SDL/graphics |
 |info, and the program itself. (It is   |
 |however stored within this same file)  |
 |                                       |
 | 5. The stack structure used by the    |
 |parser for brackets is re-used to      |
 |evaluate polish expressions.           |
 |_______________________________________|
 |---------------------------------------|
 |           INTERP TESTING              |
 |---------------------------------------|
 |Interp testing occurs in bst_test.c    |
 |                                       |
 |Utilizes a 'test mode' that switches   |
 |exit() errors to return false thus     | 
 |allowing for assertion tests.          |
 |                                       |
 |Built off parser tests, and expanded to|
 |test the evaluation of expressions.    |
 |                                       |
 |Tests confirm that the BST symbol table|
 |works with the interpreter.            |
 |                                       |
 |Graphics are tested to make sure the   |
 |turtle structure is correctly updated  |
 |and works with SDL                     |
 |---------------------------------------|
 | Files: bst.c bst.h are symbol table   |
 |   bst_test.c/h are tests for interp   |
 |   stack.c is shared by parser/interp  |
 |   prog.c has program struct/SDL/turtle|
 |   helper.c has small funcs for clarity|
 |_______________________________________|
 |---------------------------------------|*/

#include "interp.h" 

#define FILENAME argv[1]

int main(int argc, char **argv)
{
   FILE *fp; 
   Program p;
   Symtab* s = init_symtab(MAXTOKENSIZE); 

   check_argc(argc);
   /*test_all_bst();*/ /*interp tests*/

   fp = gfopen(FILENAME); 
   init_prog(&p);
   prog_from_fp(&p, fp); 

   prog(&p, s);

   printf("\nInterpreted OK\n");

   /*wait for key press to close window*/
   while (!p.sdl.sw.finished){
      Neill_SDL_Events(&(p.sdl.sw));
   }

   free_prog(&p);   
   free_symtab(&s);
   fclose(fp); 

   return 0;
}

void prog_from_fp(Program *p, FILE* fp)
{
   char temp[MAXTOKENSIZE];
   while ( fscanf(fp, "%s", temp) == 1){
     set_w(p, temp); /*inits node and callocs space for node & word*/
  } 
}

/*-----------------------------------------*
 |          PRIMARY FUNCTIONS              |
 *-----------------------------------------*/
void prog(Program *p, Symtab* s)
{
   /*start at begining of program*/
   set_start(p); 

   if (get_start(p) == NULL){
      ERROR("Nothing to parse");
   }

   if ( !strsame(get_cw(p), "{") ){
      ERROR("Program expected {");
   } 
   
   if ( !brackets(p) ){
      ERROR("Missing or extra brackets");
   }

   next_word(p);
   inst_list(p, s);
}

void inst_list(Program *p, Symtab* s)
{
   /*stop at end of program*/
   if ( get_cw(p) == NULL ){
      return;
   } 

   if (strsame(get_cw(p), "}")){
      return;
   }
   instruction(p, s);
   inst_list(p, s);

   /*check for closing brace*/
   if ( get_cw(p) == NULL ){
      set_last(p);
   } 
   if ( strsame(get_cw(p), "}") ){
      return;
   }
   ERROR("Expected closing }");
}

void instruction(Program *p, Symtab* s)
{
   if (p == NULL){
      ERROR("Instruction() passed a null pointer");
   }
   if ( fd(p, s) || lt(p, s) || rt(p, s) || do_(p, s) || set(p, s) || 
        color(p, s) || heading(p, s) || pos(p, s) ||
        rect(p, s) || if_(p, s) || speed(p, s) ){
      return; 
   }
   ERROR("Statement expects FD, LT, RT, DO, SET, COLOR, HEADING, POS or RECT");
}

bool do_(Program *p, Symtab* s)
{
   char v[MAXTOKENSIZE], n[MAXTOKENSIZE];
   char temp[MAXTOKENSIZE]; 
   char start_i[MAXTOKENSIZE], end_i[MAXTOKENSIZE];
   int start, end, i;  
   Node* lbracket = NULL; 

   /*checking for: "DO X FROM # TO # {*/
   if ( !strsame(get_cw(p), "DO") ){
      return false;
   }
   next_word(p); 
   if ( !var(p) ){
      ON_ERROR("\nDO expects a variable");
   }
   /*Add variable to symbol table with dummy value*/
   strcpy( n , get_pw(p) ); 
   symtab_update(s, n, "0.0");

   if ( !strsame(get_cw(p), "FROM") ){
      ON_ERROR("\nDO expects FROM");
   }
   next_word(p); 
   if ( !varnum(p, s, v) ){ 
      ON_ERROR("\nDO expects a variable or number");
   }
   strcpy( start_i , get_pw(p) );

   if ( !strsame(get_cw(p), "TO") ){
      ON_ERROR("\nDO expects TO");
   }
   next_word(p); 
   if ( !varnum(p, s, v) ){ 
      ON_ERROR("\nDO expects a variable or number");
   }
   strcpy( end_i , v ); 

   if (!strsame(get_cw(p), "{")){
      ON_ERROR("DO expected {");
   }
   lbracket = get_cn(p); 
   next_word(p);

   /*check if start number valid and store value*/
   confirm_int(&start, start_i);
   symtab_update(s, n, start_i);  

   inst_list(p, s); 
 
   if (get_cn(p) == NULL || (!strsame(get_cw(p), "}"))){
      ERROR("DO expected } ");
   }

   next_word(p); 

   /*convert and check range values*/
   str_to_int(&start, start_i);
   str_to_int(&end  , end_i  );
   valid_range_check( start, end); 

   /*DO loop*/
   start++;
   for ( i = start; i < end; i++){
      int_to_str(temp, i);
      symtab_update(s, n, temp); /*increment*/ 
      set_cn(p, lbracket->next); /*return to loop start{*/
      inst_list(p, s); 
      next_word(p);
   }

   /*update final variable value*/
   symtab_update(s, n, end_i);  
   return true;
}

void confirm_int( int* i, char* s)
{
   /*assures DO start number is int*/
   str_to_int( i, s);
   int_to_str( s, *i );
}

void valid_range_check( int start, int end)
{
   if ( start < 0 || end < 0 ){
      ERROR("\nDO requires a positive range");
   }
   if ( start == end ){
      ERROR("\nDO does nothing, range must be at least 1");
   }
   if ( start > end ){
      ERROR("\nDO range must increment");
   }
}

bool set(Program *p, Symtab* s)
{
   /*checking for: "SET X :=" */
   char n[MAXTOKENSIZE]; 
   char v[MAXTOKENSIZE];
   Node* current;
   if ( !strsame(get_cw(p), "SET") ){
      return false;
   }
   next_word(p);
   if ( !var(p) ){
      ON_ERROR("Set expects a variable");
   }
   /*add variable to symbol table*/
   strcpy(n, get_pw(p)); 
   symtab_update(s, n, "0.0");

   if ( !strsame(get_cw(p), ":=") ) {
      ON_ERROR("\nSET expects :="); 
   }
   next_word(p);

   /*store start of polish expression for later evaluation*/
   current = get_cn(p);
   polish(p, s, v); 

   set_cn(p, current);
   if ( !strsame(current->w, ";") ){ 
      eval_polish(p, s, v); 
      next_word(p); 
      set_varval(s->root, n, v); 
   }
   return true;
}  

bool fd(Program *p, Symtab* s)
{
   char v[MAXTOKENSIZE];
   if ( strsame(get_cw(p), "FD") ){
      next_word(p); 
      if (!varnum(p, s, v)) { 
         ON_ERROR("\nFD expects variable or number"); 
      }
      draw_line(p, v);
      inst_list(p, s); 
      return true;
   } 
   return false; 
}

bool lt(Program *p, Symtab* s)
{
   char v[MAXTOKENSIZE];
   if ( strsame(get_cw(p), "LT") ){
      next_word(p); 
      if (!varnum(p, s, v)) { 
         ON_ERROR("\nLT expects variable or number"); 
      }
      turn_left(p, v); 
      inst_list(p, s);
      return true;
   }
   return false;  
}

bool rt(Program *p, Symtab* s)
{
   char v[MAXTOKENSIZE];
   if ( strsame(get_cw(p), "RT") ){
      next_word(p);
      if (!varnum(p, s, v)) { 
         ON_ERROR("\nRT expects variable or number"); 
      }
      turn_right(p, v); 
      inst_list(p, s);
      return true;
   }
   return false;
}

bool varnum(Program *p, Symtab* s, char* v)
{
   /*advances to next token*/
   char curr_word[MAXTOKENSIZE];

   if (p == NULL){
      ERROR("Varnum passed a NULL poitner");
   }
   if (get_cn(p) == NULL){
      return false;
   }
   if (get_cw(p) == NULL){
      return false;
   }
   strcpy( curr_word, get_cw(p) ); 

   if ( is_num(curr_word) ){
      strcpy(v, curr_word);
      next_word(p);
      return true; 
   }

   if ( var(p) ){
      /*look up var value in symbol table*/
      if ( symtab_isin(s, curr_word) ){ 
         get_varval(s->root, curr_word, v);/*copies value to v*/
         return true;
      }
   }
   return false;
}

bool is_num(char* curr_word)
{
   float num;
   if ( sscanf(curr_word, "%f", &num) == 1 ){
      return true;
   }
   return false;   
}

bool var(Program *p)
{
   /*Accepts any length var names within MAXTOKENSIZE. 
     Follows C variable name rules.
     Rejects variable names that are keywords.
     Advances to next token only if true.                 */
   char var[MAXTOKENSIZE] = "\0";

   if ( p == NULL){
      ERROR("\nVar() passed a null pointer");
   }
   if ( get_cw(p) == NULL){
      return false; 
   }

   strcpy( var, get_cw(p) );

   /*must start with letter*/
   if ( !isalpha(var[0]) ){
      return false;
   }   

   /*compare with keywords*/
   if ( !keyword(var) ){
      next_word(p);
      return true; 
   }

   return false;
}

bool keyword(char* var)
{
   if ( strsame(var, "FD")  || strsame(var, "COLOR" ) || 
        strsame(var, "RT" ) || strsame(var, "POS")    || 
        strsame(var, "IF")  || strsame(var,"SPEED" )  || 
        strsame(var, "DO")  || strsame(var, "SET")    ||
        strsame(var, "LT")  || strsame(var, "RECT")   || 
        strsame(var, "HEADING")){
      return true;
   }
   return false; 
}

void polish(Program *p, Symtab* s, char* v)
{ 
   /* Does not advance past semi-colon ;   
      Note - grammatically correct but illogical polish expressions
      are caught as erros by eval_polish() below*/
   if ( p == NULL ){
      ERROR("Polish() passed a NULL pointer");
   }
   if ( get_cn(p) == NULL){
      free_prog(p);
      ERROR("Polish expects ; ");
   }
   if ( get_cw(p) == NULL ){
      free_prog(p); 
      ERROR("\nPolish: current word is NULL");
   }

   if ( varnum(p, s, v) || op(p) ){
      polish(p, s, v);
      return;
   }

   if ( strsame(get_cw(p), ";") ){
      return; 
   } 

   free_prog(p);
   ERROR("Polish Expects ; ");
}

void eval_polish(Program* p, Symtab* s, char* v)
{
   /*if passes polish(), then can be evaluated
     Polish statements of ";", "*;", "A +;" ie - polish must 
     begin with a variable or number. Further, it must contain 
     the correct ratio of operations to var/nums as determined
     back pushing/popping stack                              */
   Stack stack;
   char current[MAXTOKENSIZE]; 
   char operator; 
   char operand1[MAXTOKENSIZE], operand2[MAXTOKENSIZE];
   char* temp = NULL; 
   char* result = (char*) calloc(MAXTOKENSIZE, sizeof(char));

   init_stack(&stack);
   strcpy(current, get_cw(p)); 

   while ( !strsame(get_cw(p), ";") ){
   /*if var/num, push it onto the stack*/
      if ( varnum(p, s, operand1) ){
         /*varnum gets value of variables/nums and sets
           operand1 to this value */
         push(&stack, operand1);
         strcpy(current, get_cw(p)); 
      } 

      /*if operator, pop operand1, pop operand2 and evaluate */
      if ( op(p) ){
         set_operator(&operator, current);

         store_pop(operand1, temp, &stack);
         store_pop(operand2, temp, &stack);

         free(result);
         result = evaluate(operator, operand1, operand2);
         push(&stack, result); 
         strcpy(current, get_cw(p)); 
      }
   }

   /*stop when hit ";" */
   free(result); 
   result = pop(&stack);
   if ( result == NULL ){
      ERROR("\nFailed to evaluate Polish expression");
   }
   if ( empty(&stack) ){
      free_stack(&stack); 
      strcpy(v, result);
      free(result); 
      return;
   }

   free(result);
   free_stack(&stack);
   ERROR("\nEval_polish could not empty stack");
}

void store_pop(char* op, char* temp, Stack* stack)
{
   temp = pop(stack);
   strcpy(op, temp);
   free(temp); 
   if (op == NULL){
      ERROR("\nFailed to evaluate polish expression - "
             "incorrect ratio of operator to operand");
   }
}

void set_operator(char* operator, char* word)
{
   if ( strsame(word, "+") ){
      *operator = '+'; 
   } else if ( strsame(word, "-") ){
      *operator = '-'; 
   } else if ( strsame(word, "*") ){
      *operator = '*'; 
   } else if ( strsame(word, "/") ){
      *operator = '/'; 
   } else {
      ERROR("Set_operator failed");
   }
}

char* evaluate(char operator, char* o1, char* o2)
{
   float op1 = 0.0, op2 = 0.0; 
   float r; 
   char* result = calloc(MAXTOKENSIZE, sizeof(char));

   str_to_float(&op1, o1);
   str_to_float(&op2, o2);

   switch(operator){
      case '+':
         r = op1 + op2;
         break;
      case '-':
         r = op1 - op2; 
         break; 
      case '*':
         r = op1 * op2;
         break;
      case '/':
         r = op1 / op2;
         break;
      default:
         ERROR("Could not evaluate polish expression");
   }

   float_to_str(result, r);
   return result; 
}

bool op(Program *p)
{
   /*advances to next token only if true*/
   char oper;

   if ( p == NULL ){
      ERROR("Op() passed a null pointer");
   }
   if ( get_cn(p) == NULL ){
      return false; 
   }
   if ( sscanf(get_cw(p), "%[-,+,*,/]s", &oper) == 1 ){
      next_word(p);
      return true; 
   }
   return false;
}

/*-----------------------------------------*
 *         EXTENSION FUNCTIONS             *
 *-----------------------------------------*/

bool if_(Program* p, Symtab* s)
{
   /*IF VAR COMPARISION_OP VARNUM { BLOCK } */
   char var1[MAXTOKENSIZE] = "-\0";
   char var2[MAXTOKENSIZE] = "-\0"; 
   char comp_op; 
   if ( strsame( get_cw(p), "IF") ) {
      next_word(p);
      if ( !var(p) ){
         ON_ERROR("\nIF expects a variable");
      }
      /*get value from symbol table - fails if variable undefined*/
      get_varval(s->root, get_pw(p), var1); 
      if ( strsame( var1, "-" ) ){
          ON_ERROR("\nIF - variable undefined");
      }

      comp_op = comparison_op( get_cw(p) );
      next_word(p); 
      
      if ( !varnum(p, s, var2) ){
         ON_ERROR("\nIF expects a second variable or number");
      }
      
      /*evaluate condition*/
      if( eval_condition(var1, var2, comp_op) ){
          if ( !strsame(get_cw(p), "{") ){
             ON_ERROR("\nIF expects '{'");
          }
         next_word(p);
         inst_list(p, s);
          if ( !strsame(get_cw(p), "}") ){
             ON_ERROR("\nIF expects '}'");
          }
         return true;
      }
      /*if comparison false, skip block*/
      while ( !strsame(get_cw(p), "}") ){
         next_word(p);
      }
      next_word(p);
      return true; 
   }
   
   return false;
}

char comparison_op( char* word )
{
   if strsame( word, "<" ){
      return '<'; 
   } 
   if strsame( word, ">" ){
      return '>'; 
   } 
   ERROR("\nIF expects a comparison operator < or >"); 
}

bool eval_condition(char* val1, char* val2, char comp_op)
{
   float v1, v2;
   str_to_float(&v1, val1);
   str_to_float(&v2, val2);

   switch(comp_op){
      case '>':
         if ( v1 > v2 ){
            return true;
         }
         return false; 
      case '<':
         if ( v1 < v2 ){
            return true;
         } 
         return false;
      default: 
        ERROR("\nIF failed to evaluate condition");
   }
}

bool color(Program* p, Symtab* s)
{
   /*sets line color*/
   /*COLOR <VARNUM>,<VARNUM>,<VARNUM>,<VARNUM> | 
           <COLORNAME>*/
   char v[MAXTOKENSIZE];
   char a[MAXTOKENSIZE];
   char r[MAXTOKENSIZE];
   char g[MAXTOKENSIZE];
   char b[MAXTOKENSIZE];

   if ( strsame(get_cw(p), "COLOR") ){
      next_word(p);
      if ( varnum(p, s, v) ) { 
         /*checking for varnum , varnum , varnum, varnum 
                 R , G , B , Alpha  */
         strcpy(r, v);
         if ( !strsame(get_cw(p), ",") ){
            ON_ERROR("\nCOLOR expects ','");
         }
         next_word(p);
         if ( !varnum(p, s, v) ){
            ON_ERROR("\nCOLOR expects a variable or number");
         }
         strcpy(g, v);

         if ( !strsame(get_cw(p), ",") ){
            ON_ERROR("\nCOLOR expects ','");
         }
         next_word(p);
         if ( !varnum(p, s, v) ){
            ON_ERROR("\nCOLOR expects a variable or number");
         }
         strcpy(b, v);

         if ( !strsame(get_cw(p), ",") ){
            ON_ERROR("\nCOLOR expects ','");
         }
         next_word(p);
         if ( !varnum(p, s, v) ){
            ON_ERROR("\nCOLOR expects a variable or number");
         }
         strcpy(a, v);

         set_color_value(p, r, g, b, a); 

      } else {
         /*check for all color keywords and set color*/
         prev_word(p); /*varnum advances*/
         if ( !set_color_name(p) ){
            ERROR("\nCOLOR passed invalid color name");
         }
         next_word(p);
      }
      /*set SDL color here*/
      SDL_SetRenderDrawColor(p->sdl.sw.renderer, 
                             p->t.color[red], 
                             p->t.color[green], 
                             p->t.color[blue], 
                             p->t.color[alpha]);
      inst_list(p, s); 
      return true;
   }
   return false; 
}

void set_color_value(Program* p, char* r, char* g, char* b, char* a)
{
   int rd, grn, bl, alph;
   str_to_int(&rd, r);
   str_to_int(&grn, g);
   str_to_int(&bl, b);
   str_to_int(&alph, a);

   p->t.color[red] = rd;
   p->t.color[green] = grn;
   p->t.color[blue] = bl;
   p->t.color[alpha] = alph;
}

bool set_color_name(Program* p)
{
   int i;
   int color_values[RGBA] = {0}; 
   int blck[RGBA] = {BLACK};
   int wht[RGBA] = {WHITE};
   int rd[RGBA] = {RED};
   int yllw[RGBA] = {YELLOW};
   int bl[RGBA] = {BLUE};
   int grn[RGBA] = {GREEN};
   int rng[RGBA] = {ORANGE};
   int prpl[RGBA] = {PURPLE};

   if (strsame(get_cw(p), "RED")){
      copy_arr(color_values, rd); 
   } else if (strsame(get_cw(p), "YELLOW")){
         copy_arr(color_values, yllw); 
   } else if (strsame(get_cw(p), "BLUE")){
         copy_arr(color_values, bl); 
   } else if (strsame(get_cw(p), "GREEN")){
         copy_arr(color_values, grn); 
   } else if (strsame(get_cw(p), "ORANGE")){
         copy_arr(color_values, rng); 
   } else if (strsame(get_cw(p), "PURPLE")){
         copy_arr(color_values, prpl); 
   } else if (strsame(get_cw(p), "BLACK")) {
         copy_arr(color_values, blck); 
   } else if (strsame(get_cw(p), "WHITE")){
         copy_arr(color_values, wht); 
   } else {
      return false;
   }

   for (i = 0; i < RGBA; i++){
      p->t.color[i] = color_values[i]; 
   }
   return true; 
}

bool heading(Program* p, Symtab* s)
{
   /*sets direction of turtle*/
   /*HEADING <HEADINGKEYWORD>*/
   if ( strsame(get_cw(p), "HEADING") ){
      next_word(p);
      if ( strsame(get_cw(p), "UP")    ||
           strsame(get_cw(p), "DOWN")  ||
           strsame(get_cw(p), "LEFT")  ||
           strsame(get_cw(p), "RIGHT") ) { 
         set_heading(p, get_cw(p)); 
         next_word(p);
         inst_list(p, s);
         return true; 
      } 
   }
   return false;
}

bool pos(Program* p, Symtab* s)
{
   /*sets position of turtle*/
   /*POS <VARNUM> , <VARNUM>
            x     ,     y       */
   char v[MAXTOKENSIZE];
   char x[MAXTOKENSIZE];
   char y[MAXTOKENSIZE];
   if ( strsame(get_cw(p), "POS") ){
      next_word(p);
      /*check for "CENTER" keyword*/
      if( strsame(get_cw(p), "CENTER") || 
          strsame(get_cw(p), "CENTRE") ){
         set_pos_int(p, CUSTOM_WIDTH/2, CUSTOM_HEIGHT/2);
         next_word(p);
      } else { /*check for hard-coded position*/
         if ( !varnum(p, s, v) ) { 
            ERROR("\nPOS expects a variable or number");
         }
         strcpy(x, v); 

         if ( !strsame(get_cw(p), ",") ){
            ERROR("\nPOS expects a ','");
         }
         next_word(p);
         if ( !varnum(p, s, v) ){
            ERROR("\nPOS expects a variable or number");
         }
         strcpy(y, v);

         set_pos(p, x, y);
      }  
      inst_list(p, s); 
      return true; 
   }
   return false;
}

bool rect(Program* p, Symtab* s)
{
   /*draws rectangle of given Width, Height */
   /*RECT <VARNUM> , <VARNUM>      FILL
            width  , height    (solid rect) */
   char v[MAXTOKENSIZE];
   char h[MAXTOKENSIZE];
   char w[MAXTOKENSIZE];
   bool fill = false;
   if ( strsame(get_cw(p), "RECT") ){
      next_word(p);
      if ( !varnum(p, s, v) ) { 
            ON_ERROR("\nRECT expects a variable or number");
      }
      strcpy(w, v); 

      if ( !strsame(get_cw(p), ",") ){
            ON_ERROR("\nRECT expects a ','");
      }
      next_word(p);
      if ( !varnum(p, s, v) ){
            ON_ERROR("\nRECT expects a variable or number");
      }
      strcpy(h, v); 

      /*check for FILL*/
      if ( strsame(get_cw(p), "FILL") ){
         fill = true; 
         next_word(p); 
      }

      draw_rect(p, w, h, fill); 
      inst_list(p, s); 
      return true; 
   }
   return false;
}

void draw_rect(Program *p, char* w, char* h, bool fill)
{
   int width, height;
   str_to_int(&width, w);
   str_to_int(&height, h);

   draw_rect_int(p, width, height, fill);  
}

bool speed(Program* p, Symtab* s)
{
   int num;
   /*SPEED <num> -- sets speed of the turtle*/
   if ( strsame(get_cw(p), "SPEED") ){
      next_word(p);
      if ( is_num(get_cw(p)) ){
         str_to_int(&num, get_cw(p)); 
         p->t.speed = num; 
         next_word(p);
         inst_list(p, s);
         return true; 
      } else {
         ON_ERROR("\nSPEED expects a number");
      }
   }
   return false;
}

/*-----------------------------------------*
 *           BRACKET FUNCTIONS             *
 *-----------------------------------------*/

bool brackets(Program *p)
{
   /*checks for correct use of brackets*/
   Stack s;
   char* top; 
   char* current = calloc(MAXTOKENSIZE, sizeof(char)); 
   char* right = "}";
   char* left = "{";
   Node* startnode;
   init_stack(&s);

   if ( p == NULL){
      ERROR("Brackets() passed a null pointer");
   }
   if ( get_cn(p) == NULL ){
      if ( empty(&s) ){
         free_stack(&s);
         return true;
      }
      free_stack(&s);
      return false;
   }

   startnode = get_cn(p); /*track starting position*/

   while ( get_cn(p) != NULL ){ /*go through entire str*/
      if ( isbracket(get_cw(p)) ){
         strcpy( current, get_cw(p) ); 
         if ( empty(&s) ){ /*first stack element*/
            push( &s, current); 
         } else {
            top = pop(&s); /*pop callocs space for value it returns*/
            if ( top == NULL ){
               ERROR("Incorrect brackets");
            }
            if ( (strsame(top,left)) && ( strsame(current,right)) ){
               free(top); /* PAIR {} - set them free */
            } else {      /*re-push top, push new word */
               push( &s, top );
               push( &s, current ); 
               free(top);
            }
         } 
      } 
      next_word(p); 
   }                           
   free(current);
   set_cn(p, startnode); /*return to original position*/
   s.top = s.start; 
   if ( empty(&s) ){
      free_stack(&s);
      return true;
   }
   free_stack(&s);
   return false; 
}

bool isbracket(char* word)
{
   if ( strsame(word, "{") || strsame(word, "}") ){
      return true;
   }
   return false; 
}
