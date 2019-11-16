/*---------------------------------------*
 |             TURTLE PARSER             |
 |---------------------------------------|
 |                                       |
 |           PARSER EXTENSIONS           |
 |                                       |
 | Please note that there are further    |
 | structural exentions in the interp    |
 | including accepting any variable name |
 | (excluding keywords like DO / SET etc)|
 |---------------------------------------|
 | 1. Uses a linked list structure to    |
 |    handle any length program.         |
 | 2. Warns users of unused / undeclared |
 |    variables (but parses them OK).    |
 |       - DO also warns users of wrong  |
           values (negative, equal etc)  |
 | 3. Uses an ADT design for main program|
 |    structure with functions to access |
 |    all its elements (but is contained |
 |    within the same .c file).          |
 | 4. Has a Test Mode that makes funcs   |
 |    return bools rather than Exit()    |
 | 5. Uses a stack structure for brackets|
 | 6. Uses a linked list struct for var  |
 |    lookup and storage.                |
 | 7. Has grammar extensions that allow: |
 |       a. IF A < B or IF A > B {block} |
 |       b. set position POS # , #       |
 |       c. set heading HEADING UP       |
 |          (or DOWN RIGHT LEFT)         |
 |       d. draw rectangles RECT W , H   |
 |       e. set draw speed SPEED #       |
 |          (0 = fastest)                |
 |       f. set draw COLOR keyword/rgba  | 
 |_______________________________________|
 |---------------------------------------|
 |                                       |
 |           PARSER TESTING              |
 |                                       |
 | Parser testing occurs in test.c test.h|
 |Testing occured throughout development |
 |
 |Utilizes a 'test mode' that alters the |
 |exit() errors to bool false values to  |
 |allow for assertion tests.             |
 |                                       | 
 |Small, simple functions were tested as |
 |they were built. Testing of the overall|
 |parser was accomplished using small    | 
 |test programs once all functions passed|
 |individual tests.                      |
 |---------------------------------------|
 | Files: symbol.c is symbol table       |
 |   test.c/h are tests for parser       |
 |   stack.c is shared by parser/interp  |
 |   prog.c has program struct/SDL/turtle|
 |   helper.c has small funcs for clarity|
 |_______________________________________|
 |---------------------------------------|
 */

#include "parse.h" 

#define FILENAME argv[1]

int main(int argc, char **argv)
{
   FILE *fp; 
   Program p;
   /*tracks variable declaration and use*/
   Symtab* s = init_symtab();

   check_argc(argc);
   /*test_all();*/

   fp = gfopen(FILENAME); 
   init_prog(&p);

   prog_from_fp(&p, fp); 

   prog(&p, s);

   if( unused_var(s) ){
      printf("\nWarning: unused variables");
   }

   printf("\nParsed OK\n");

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
   ERROR("Statement expects FD, LT, RT, DO, SET, COLOR,"
         " HEADING, POS, IF, SPEED or RECT");
}

bool do_(Program *p, Symtab* s)
{
   char v[MAXTOKENSIZE]; 
   char n[MAXTOKENSIZE]; 
   char start_i[MAXTOKENSIZE]; 
   char end_i[MAXTOKENSIZE];
   int start;
   int end;

   /*checking for: "DO X FROM # TO # {*/
   if ( !strsame(get_cw(p), "DO") ){
      return false;
   }
   next_word(p); 
   if ( !var(p) ){
      ON_ERROR("DO expects a variable");
   }
   /*add/update var in symtab*/
   strcpy( n , get_pw(p) );
   symtab_update(s, n, "0.0");

   if ( !strsame(get_cw(p), "FROM") ) {
      ON_ERROR("DO expects FROM");  
   }
   next_word(p); 
   if ( !varnum(p, s, v) ){
      ON_ERROR("DO expects a variable or number");
   }
   strcpy( start_i , get_pw(p) );

   if ( !strsame(get_cw(p), "TO") ){
      ON_ERROR("DO expects TO");
   }
   next_word(p); 
   if ( !varnum(p, s, v) ){ 
      ON_ERROR("DO expects a second variable or number");
   }
   strcpy( end_i , get_pw(p) ); 

   if (!strsame(get_cw(p), "{")){
      ON_ERROR("DO expected {");
   }
   next_word(p);
   symtab_update(s, n, start_i);  

   inst_list(p, s);
 
   if (get_cn(p) == NULL){
      ON_ERROR("DO expected } ");
   }
   if (!strsame(get_cw(p), "}")){
      ON_ERROR("DO expected }");
   }
   next_word(p); 

   /*convert and check range values*/
   str_to_int(&start, start_i);
   str_to_int(&end  , end_i  );

   valid_range_check( start, end); 
 
   return true;
}

void valid_range_check( int start, int end)
{
   /*grammatically correct but illogical DO ranges get warnings*/
   if ( start < 0 || end < 0 ){
      printf("\nWarning: DO requires positive numbers");
   }
   if ( start == end ){
      printf("\nWarning: DO does nothing, range must be at least 1");
   }
   if ( start > end ){
      printf("\nWarning: DO should increment");
   }
}

bool set(Program *p, Symtab* s)
{
   /*checking for: "SET X :=" */
   char n[MAXTOKENSIZE]; 
   char v[MAXTOKENSIZE];

   if ( !strsame(get_cw(p), "SET") ){
      return false;
   }
   next_word(p);
   if ( !var(p) ){
      ON_ERROR("\nSet expects a variable");
   }
   /*add variable to symbol table/update dummy value*/
   strcpy(n, get_pw(p)); 
   symtab_update(s, n, "0.0");

   if ( !strsame(get_cw(p), ":=") ) {
      ON_ERROR("\nSET expects ':='");  
   }
   next_word(p);

   polish(p, s, v);

   if ( strsame(get_cw(p), ";") ){ 
      next_word(p);
      strcpy( v , "1"); /*dummy value*/
      update_varnode(s, n, v); 
      return true;
   }
   ON_ERROR("SET expects ending ';'");
}  

bool fd(Program *p, Symtab* s)
{
   char v[MAXTOKENSIZE];
   if ( strsame(get_cw(p), "FD") ){
      next_word(p); 
      if (!varnum(p, s, v)) { 
         ON_ERROR("\nFD expects a variable or number"); 
      }
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
         ON_ERROR("\nLT expects a variable or number"); 
      }
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
         ON_ERROR("\nRT expects a variable or number"); 
      }
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
      ERROR("Varnum passed a NULL pointer");
   }
   if (get_cn(p) == NULL || get_cw(p) == NULL){
      return false;
   }

   strcpy( curr_word, get_cw(p) ); 

   if ( is_num(curr_word) ){ 
      next_word(p);
      return true; 
   }

   if ( var(p) ){
      /*look up var value in symbol table*/
      if ( has_var(s, curr_word) ){ 
         strcpy(v, get_varval(s, curr_word) );
      } else {
         printf("\nWarning: variable %s undefined", curr_word);
      }
      return true;
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
   /*only accepts single letters A-Z*/
   /*advances to next token only if true*/
   char var[MAXTOKENSIZE] = "\0";
   if ( p == NULL){
      ERROR("\nVar() passed a null pointer");
   }
   if ( get_cn(p) == NULL || get_cw(p) == NULL){
      return false; 
   }

   if ( strlen(get_cw(p)) < 2 ){
      if ( sscanf(get_cw(p), "%[A-Z]s", var) == 1 ){
         next_word(p); 
         return true; 
      }
   }
   return false;
}

void polish(Program *p, Symtab* s, char* v)
{ 
   /* does not advance after semi-colon ; reached*/  
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
   char var1[MAXTOKENSIZE];
   char var2[MAXTOKENSIZE]; 
   /*IF VAR COMPARISION_OP VARNUM { BLOCK } */
   if ( strsame( get_cw(p), "IF") ) {
      next_word(p);
      if ( !var(p) ){
         ON_ERROR("IF expects a variable");
      }
      /*get value from symbol table -- fails if variable undefined*/
      strcpy( var1, get_varval(s, get_pw(p)) ); 

      if ( !comparison_op(get_cw(p)) ){
         ON_ERROR("IF expects a comparison operator");
      }
      next_word(p); 
      
      if ( !varnum(p, s, var2) ){
         ON_ERROR("IF expects a second variable or number");
      }
      
      if ( !strsame(get_cw(p), "{") ){
          ON_ERROR("IF expects '{'");
      }
      next_word(p);
      inst_list(p, s);
      if ( !strsame(get_cw(p), "}") ){
          ON_ERROR("IF expects '}'");
      }
      return true;
   }
   
   return false;
}

bool comparison_op( char* word )
{
   if ( strsame( word, "<" ) || strsame( word, ">" ) ){
      return true;
   } 
   return false; 
}

bool color(Program* p, Symtab* s)
{
   /*sets line color*/
   /*COLOR <VARNUM>,<VARNUM>,<VARNUM>,<VARNUM> | 
           <COLORNAME>*/
   char v[MAXTOKENSIZE];

     if ( strsame(get_cw(p), "COLOR") ){
      next_word(p);
      if ( varnum(p, s, v) ) { 
         /*checking for varnum , varnum , varnum, varnum 
                 R , G , B , Alpha  */
         if ( !strsame(get_cw(p), ",") ){
            ON_ERROR("\nCOLOR expects ',' after variable or number");
         }
         next_word(p);
         if ( !varnum(p, s, v) ){
            ON_ERROR("\nCOLOR expects a variable or number");
         }

         if ( !strsame(get_cw(p), ",") ){
            ON_ERROR("\nCOLOR expects ',' after variable or number");
         }
         next_word(p);
         if ( !varnum(p, s, v) ){
            ON_ERROR("\nCOLOR expects a variable or number");
         }

         if ( !strsame(get_cw(p), ",") ){
            ON_ERROR("\nCOLOR expects ',' after variable or number");
         }
         next_word(p);
         if ( !varnum(p, s, v) ){
            ON_ERROR("\nCOLOR expects a variable or number");
         }

      } else {
         /*check for valid color name keyword*/
         if ( !valid_color(p) ){
            ON_ERROR("COLOR: Invalid color name");
         }
         next_word(p);
      }

      inst_list(p, s); 
      return true;
   }
   return false; 
}

bool valid_color(Program* p)
{
   if (strsame(get_cw(p), "RED") || strsame(get_cw(p), "YELLOW") ||
       strsame(get_cw(p), "BLUE")|| strsame(get_cw(p), "GREEN")  ||
       strsame(get_cw(p), "ORANGE") || strsame(get_cw(p), "PURPLE") ||
       strsame(get_cw(p), "BLACK") || strsame(get_cw(p), "WHITE") ||
       strsame(get_cw(p), "RANDOM")){
      return true;
   } 
   return false;
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
         next_word(p);
         inst_list(p, s);
         return true; 
      }
      ON_ERROR("\nHeading expects UP DOWN LEFT or RIGHT");
   }
   return false;
}

bool pos(Program* p, Symtab* s)
{
   /*sets position of turtle*/
   /*POS <VARNUM> , <VARNUM>
            x     ,     y       */
   char v[MAXTOKENSIZE];

   if ( strsame(get_cw(p), "POS") ){
      next_word(p);
      if( strsame(get_cw(p), "CENTER") || strsame(get_cw(p), "CENTRE") ){
         next_word(p);
      } else { /*check for hard-coded position*/
         if ( !varnum(p, s, v) ) { 
            ON_ERROR("\nPOS expects a variable or number");
         }
         if ( !strsame(get_cw(p), ",") ){
             ON_ERROR("\nPOS expects a ','");
         }
         next_word(p);
         if ( !varnum(p, s, v) ){
            ON_ERROR("\nPOS expects a variable or number");
         }
      }  
      inst_list(p, s); 
      return true; 
   }
   return false;
}

bool rect(Program* p, Symtab* s)
{
   /*RECT <VARNUM> , <VARNUM>  FILL
            width  ,  height  (solid) */
   char v[MAXTOKENSIZE];

   if ( strsame(get_cw(p), "RECT") ){
      next_word(p);
      if ( !varnum(p, s, v) ) { 
         ON_ERROR("\nRECT expects a variable or number");
      }

      if ( !strsame(get_cw(p), ",") ){
          ON_ERROR("\nRect expects a ','");
      }
      next_word(p);
      if ( !varnum(p, s, v) ){
         ON_ERROR("\nRECT expects a variable or number");
      }

      if ( strsame(get_cw(p), "FILL") ){
         next_word(p); 
      }

      inst_list(p, s);
      return true; 
   }
   return false;
}

bool speed(Program* p, Symtab* s)
{
   /*SPEED <num> -- sets speed of the turtle*/
   if ( strsame(get_cw(p), "SPEED") ){
      next_word(p);
      /*checking if num and storing it to var num*/
      if ( is_num(get_cw(p)) ){ 
         next_word(p);
         inst_list(p, s);
         return true; 
      }
      ON_ERROR("\nTurtle SPEED keyword should be followed by a number");
   }
   return false;
}

/*-----------------------------------------*
 *           BRACKET FUNCTIONS             *
 *-----------------------------------------*/
bool brackets(Program *p)
{
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
   if ( p->cw == NULL ){
      if ( empty(&s) ){
         free_stack(&s);
         return true;
      }
      free_stack(&s);
      return false;
   }

   startnode = p->cw; /*track starting position*/

   while ( p->cw != NULL ){ /*go through entire str*/
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
               /* PAIR {} - set them free */
               free(top); 
            } else { /*re-push top, push new word */
               push( &s, top );
               push( &s, current ); 
               free(top);
            }
         } 
      } 
      next_word(p); 
   }                           
   free(current);
   p->cw = startnode; /*return cw to original position*/
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

/*-----------------------------------------*
 |     PROG STRUCTURE FUNCTIONS            |
 *-----------------------------------------*/

void init_prog(Program *p)
{
   if (p == NULL) {
      ERROR("Init_prog() passed a null pointer");
   }
   p->start = NULL; 
   p->cw = NULL;
   p->test_mode = false; 
}

char* get_cw(Program *p)
{
   /*returns current word*/
   if (p == NULL){
      ERROR("Get_cw() passed a NULL pointer");
   }

   if (p->cw == NULL){
      return NULL; 
   }

   return p->cw->w; 
}

char* get_pw(Program *p)
{
   /*returns word before current word*/
   if (p == NULL){
      ERROR("Get_prevw() passed a NULL pointer");
   }

   if (p->cw->prev == NULL){
      return NULL; 
   }

   return p->cw->prev->w; 
}

char* get_nw(Program *p)
{
   /*returns word after current word*/
   if (p == NULL){
      ERROR("Get_prevw() passed a NULL pointer");
   }

   if (p->cw == NULL || get_cw(p) == NULL || p->cw->next == NULL){
      return NULL; 
   }

   return p->cw->next->w; 
}

void next_word(Program *p)
{
   if (p == NULL){
      ERROR("Next_word() received a NULL pointer");
   }

   if (p->cw == NULL){
      return;
   }

   if ( p->cw->next == NULL){
       p->cw = NULL; 
       return;
   }
   p->cw->next->prev = p->cw;
   p->cw = p->cw->next;
}

Node* get_cn(Program* p)
{
   /*returns the current node*/
   if (p == NULL){
      ERROR("Get_cn() passed a NULL pointer");
   }
   return p->cw; 
}

void set_cn(Program* p, Node* n)
{
   /*points current word to node n*/
   p->cw = n; 
}

Node* get_pn(Program* p)
{
   /*returns node before current*/
   return p->cw->prev; 
}

Node* get_nn(Program* p)
{
   /*returns node after current*/
   if (p->cw == NULL){
      return NULL;
   }
   return p->cw->next; 
}

Node* get_start(Program* p)
{
   /*returns start node of program*/
   return p->start;
}

void set_start(Program* p)
{
   p->cw = p->start;
}

void set_last(Program* p)
{
   /*go through prog to get last node */
   set_start(p);
   while ( get_nn(p) != NULL){
      next_word(p);
   }
}

void enter_test_mode(Program *p)
{
   p->test_mode = true;
}

void exit_test_mode(Program *p)
{
   p->test_mode = false;
}

bool istest_mode(Program* p)
{
   if (p->test_mode == true){
      return true;
   }
   return false;
}

void free_prog(Program *p)
{
   /*wrapper for frpr to assure current word set to
     start word to prevent memory leaks             */
   p->cw = p->start;
   frpr(p); 
}



/*-----------------------------------------*
 *      INTERNAL PROG FUNCTIONS            *
 *-----------------------------------------*/

void frpr (Program* p)
{
   /*to use, p->cw must == p->start*/
   if (p == NULL){
      return;
   }

   if (p->cw == NULL){
      return;
   }

   if ( p->start->next != NULL){
      next_word(p); /*advance to next node*/
      free(p->start->w);
      free(p->start);
      p->start = p->cw; 
      free_prog(p);
   } else {         /*free last remaining node */
      if (p->start->w != NULL){
         free(p->start->w);
      }
      if (p->start != NULL){
         free(p->start); 
      }
   }
}

Node* init_node(void)
{
   Node* node = calloc(1, sizeof(Node)); 
   if (node == NULL){
      ERROR("Failed to calloc space for node");
   }
   node->next = NULL; 
   return node; 
}

void set_w(Program *p, char* word)
{
   /*calloc space for node and string*/
   Node* n = init_token(word);

   if (n == NULL){
      ERROR("Set_w could not init_token");
   }

   if ( p == NULL || word == NULL){
      free(n); 
      ERROR("set_w() received a NULL pointer");
   }

   /*if first word to be added*/
   if ( p->cw == NULL ){
      p->start = n;  
      p->cw = n; 
      return;
   }

   p->cw->next = n;
   p->cw = p->cw->next; 
}

Node* init_token(char* word)
{
   Node* n = init_node();
   n->w = calloc(MAXTOKENSIZE, sizeof(char));

   strcpy(n->w, word); 

   return n; 
}

/*-----------------------------------------*
 *          HELPER FUNCTIONS               *
 *-----------------------------------------*/

FILE* gfopen(char* filename)
{
   FILE* fp;
   if (!(fp = fopen(filename, "r"))){
      ERROR("Cannot open file"); 
   }
   return fp;
}

void str_to_float(float* f, char* s)
{
   if ( sscanf(s, "%f", f) != 1 ){
      ERROR("Do failed to convert number");
   }
}

void float_to_str(char* s, float f)
{
   sprintf(s, "%f", f);
} 

void str_to_int(int* n, char* s)
{
   if ( sscanf(s, "%d", n) != 1 ){
      ERROR("Do failed to convert number");
   }
}

void int_to_str(char* s, int n)
{
   sprintf(s, "%d", n);
} 

void check_argc(int argc)
{
   if (argc != 2){
      ERROR("Missing textfile, nothing to parse");
   }
}

