#include "test.h" 

/*_______________________________________
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
 |_______________________________________|
 |---------------------------------------|
 */


void test_all(void)
{
   Program p;

   /*test Prog set-up functions*/
   init_prog(&p);
   enter_test_mode(&p);  
   test_init_prog(&p); 
   test_init_node();
   test_init_token();
   test_set_w(&p);
   test_init_token();
   test_next_word(&p); 

   /*test Stack set-up functions*/
   test_init_stack(); 
   test_init_stacknode();
   test_free_stack(); 
   test_push(); 
   test_pop(); 
   test_empty();

   /*test brackets*/
   test_brackets();

   /*test basic rules */
   test_op(); 
   test_var();
   test_varnum();
   test_polish(); 
   test_set();   
   test_do_(); 
   test_fd(); 
   test_rt(); 
   test_lt(); 
   test_instruction();
   test_inst_list();  
   test_prog(); 

   /*test extensions*/ 
   test_extensions();

   /*test symbol table*/
   test_init_varnode();
   test_init_symtab();
   test_add_varnode();
   test_update_varnode();
   test_get_has_used();
   test_free_symtab(); 

   /*clean up*/
   p.cw = p.start;
   free_prog(&p);
}

/*-----------------------------------------*
 *          TEST STACK FUNCTIONS           *
 *-----------------------------------------*/

void test_init_stack(void)
{
   Stack s;
   init_stack(&s);
   assert( s.start == NULL );
   assert( s.top   == NULL ); 

   free_stack(&s); 
}

void test_init_stacknode(void)
{
   char* word1 = "}";
   Node* n = init_stacknode(word1); 

   assert( n->next == NULL );
   assert( n->prev == NULL );
   assert( strsame(n->w, word1) == true );

   free(n->w); 
   free(n); 
}

void test_free_stack(void)
{
   /*all heaps freed, no leaks possible*/
   Stack s; 
   char* word1 = "word1";
   char* word2 = "word2";
   char* word3 = "word3";
   Node* n1 = init_stacknode(word1); 
   Node* n2 = init_stacknode(word2); 
   Node* n3 = init_stacknode(word3); 
   init_stack(&s);

   s.start = n1;
   n1->prev = NULL;
   n1->next = n2;
   n2->prev = n1; 
   n2->next = n3;
   n3->prev = n2;
   n3->next = NULL;
   s.top = n3; 

   assert( s.start == n1 );
   assert( s.top   == n3 );
   assert( s.start->next == n2 ); 
   assert( s.start->next->next == n3 );
   assert( s.start->prev == NULL);
   assert( s.top->next == NULL); 

   free_stack(&s);
}

void test_push(void)
{
   Stack s; 
   /*create correct stack of {{}{}} */
   char* a = "{";
   char* b = "{";
   char* c = "}";
   char* d = "{";
   char* e = "}";
   char* f = "}";
   init_stack(&s);

   /*start with empty stack*/
   assert( s.start == NULL );
   assert( s.top   == NULL );

   /*add first node*/
   push(&s, a);
   assert( strsame(s.start->w, a) == true ); 
   assert( strsame(s.top->w, a)   == true );
   assert( s.top->prev == NULL);  

   push(&s, b);
   assert( strsame(s.start->w, a) == true );/*start remains same*/
   assert( strsame(s.top->w, b) == true );    /*top advances to b */
   assert( strsame(s.top->prev->w, a) == true); /*top points back to start*/

   push(&s, c);
   assert( strsame(s.start->w,a) == true ); 
   assert( strsame(s.top->w, c) == true ); 
   assert( strsame(s.top->prev->w, b) == true);

   push(&s, d);
   assert( strsame(s.start->w,a) == true ); 
   assert( strsame(s.top->w, d) == true ); 
   assert( strsame(s.top->prev->w, c) == true); 

   push(&s, e);
   assert( strsame(s.start->w,a) == true ); 
   assert( strsame(s.top->w, e) == true ); 
   assert( strsame(s.top->prev->w, d) == true);

   push(&s, f);
   assert( strsame(s.start->w,a) == true ); 
   assert( strsame(s.top->w, f) == true ); 
   assert( strsame(s.top->prev->w, e) == true);

   free_stack(&s);
}

void test_pop(void)
{
   Stack s;
   char* pa, *pb, *pc, *pd, *pe, *pf; 
   char* a = "{";
   char* b = "{";
   char* c = "}";
   char* d = "{";
   char* e = "}";
   char* f = "}";
   init_stack(&s);

   /*takes a stack with {{}{}} at start */
   push(&s, a);
   push(&s, b);
   push(&s, c);
   push(&s, d);
   push(&s, e);
   push(&s, f);

   assert( s.start != NULL );
   assert( s.top   != NULL );

   pf = pop(&s); 
   assert ( strsame(pf, f) == true ); 
   pe = pop(&s); 
   assert ( strsame(pe, e) == true ); 
   pd = pop(&s); 
   assert ( strsame(pd, d) == true );
   pc = pop(&s); 
   assert ( strsame(pc, c) == true ); 
   pb = pop(&s); 
   assert ( strsame(pb, b) == true ); 
   pa = pop(&s); 
   assert ( strsame(pa, a) == true ); 
   free(pa); 
   /*popping empty stack returns NULL pointer*/
   pa = pop(&s); 
   assert ( pa == NULL );

   free(pf);
   free(pe);
   free(pd);
   free(pc);
   free(pb);
   free(pa);
   s.top = s.start;
   free_stack(&s);
}

void test_empty(void)
{
   Stack empty_stack;
   Stack s;
   char* word1 = "word1";
   char* word2 = "word2";
   char* word3 = "word3";
   Node* n1 = init_stacknode(word1); 
   Node* n2 = init_stacknode(word2); 
   Node* n3 = init_stacknode(word3); 
   init_stack(&empty_stack); 
   init_stack(&s);

   s.start = n1;
   n1->prev = NULL;
   n1->next = n2;
   n2->prev = n1; 
   n2->next = n3;
   n3->prev = n2;
   n3->next = NULL;
   s.top = n3; 

   assert( empty(&empty_stack) == true );
   assert( empty(&s)           == false);

   free_stack(&empty_stack);
   free_stack(&s);
}

void test_brackets(void)
{
   /*create correct progs*/
   Program correct1, correct2, correct3, correct4; 
   char* cor_str1 = "{ }";
   char* cor_str2 = "{ { FD 12 } { RT 10 } }";
   char* cor_str3 = "{ FD { { FD 12 } { } } }";
   char* cor_str4 = "{ DO A FROM 1 TO 10 { RT A } }";
   /*create wrong progs*/
   Program wrong1, wrong2, wrong3, wrong4, wrong5, wrong6; 
   char* wro_str1 = "{ { FD 12 } { { RT 10 }";
   char* wro_str2 = "{";
   char* wro_str3 = "}";  
   char* wro_str4 = "{ } FD 10 }";
   char* wro_str5 = "} { RT 40.5 } {";
   char* wro_str6 = "{ { { SET A := B C + ; { } } } } }";

   char* left_bracket  = "{";
   char* right_bracket = "}";
   char* not_bracket   = "FD";

   /*test isbracket()*/
   assert( isbracket(left_bracket)  == true );
   assert( isbracket(right_bracket) == true );
   assert( isbracket(not_bracket)   == false );

   /*init progs*/
   init_prog(&correct1);
   init_prog(&correct2);
   init_prog(&correct3);
   init_prog(&correct4);
   init_prog(&wrong1);
   init_prog(&wrong2);
   init_prog(&wrong3);
   init_prog(&wrong4);
   init_prog(&wrong5);
   init_prog(&wrong6);

   /*enter test mode*/
   enter_test_mode(&correct1);
   enter_test_mode(&correct2);
   enter_test_mode(&correct3);
   enter_test_mode(&correct4);
   enter_test_mode(&wrong1);
   enter_test_mode(&wrong2);
   enter_test_mode(&wrong3);
   enter_test_mode(&wrong4);
   enter_test_mode(&wrong5);
   enter_test_mode(&wrong6);

   /*generate progs from strings*/
   prog_from_str(&correct1, cor_str1); 
   prog_from_str(&correct2, cor_str2); 
   prog_from_str(&correct3, cor_str3); 
   prog_from_str(&correct4, cor_str4); 
   prog_from_str(&wrong1, wro_str1);
   prog_from_str(&wrong2, wro_str2); 
   prog_from_str(&wrong3, wro_str3);
   prog_from_str(&wrong4, wro_str4); 
   prog_from_str(&wrong5, wro_str5); 
   prog_from_str(&wrong6, wro_str6); 

   /*set cw to start*/
   correct1.cw = correct1.start; 
   correct2.cw = correct2.start; 
   correct3.cw = correct3.start; 
   correct4.cw = correct4.start;
   wrong1.cw   = wrong1.start;
   wrong2.cw   = wrong2.start; 
   wrong3.cw   = wrong3.start;
   wrong4.cw   = wrong4.start;  
   wrong5.cw   = wrong5.start;  
   wrong6.cw   = wrong6.start;  

   assert( brackets(&correct1) == true ); 
   assert( brackets(&correct2) == true ); 
   assert( brackets(&correct3) == true );  
   assert( brackets(&correct4) == true ); 
   assert( brackets(&wrong1)   == false ); 
   assert( brackets(&wrong2)   == false ); 
   assert( brackets(&wrong3)   == false );  
   assert( brackets(&wrong4)   == false );  
   assert( brackets(&wrong5)   == false );  
   assert( brackets(&wrong6)   == false );  

   free_prog(&correct1);
   free_prog(&correct2);
   free_prog(&correct3);
   free_prog(&correct4);
   free_prog(&wrong1);
   free_prog(&wrong2);
   free_prog(&wrong3);
   free_prog(&wrong4); 
   free_prog(&wrong5); 
   free_prog(&wrong6); 
}

void prog_from_str(Program* p, char* str)
{
   char temp[MAXTOKENSIZE];
   int offset; 

   if (p == NULL || str == NULL){
      ERROR("\nProg_from_str() passed a null pointer");
   }

   while ( sscanf(str, "%s%n", temp, &offset) == 1){
     set_w(p, temp); /*inits node, callocs space node & word*/
     str += offset;  /*move down string length of prev word*/ 
  }

   p->cw = p->start; 
}

/*-----------------------------------------*
 *            TEST PROG FUNCTIONS          *
 *-----------------------------------------*/

void test_init_prog(Program* p)
{
   init_prog(p); 
   assert( p->start == NULL ); 
   assert( p->cw    == NULL );
   assert( p->test_mode == false );  
}

void test_init_node(void)
{
   Node* n = init_node(); 
  
   assert( n       != NULL );
   assert( n->next == NULL);

   free(n);
}

void test_init_token(void)
{
   char* word1 = "word1\0";
   Node* n1 = init_token(word1);

   assert( n1    != NULL );
   assert( n1->w != NULL); 
   assert( strsame(n1->w, word1) );

   free(n1->w);
   free(n1); 
}

void test_set_w(Program* p)
{
   char* word1 = "word1\0";
   char* word2 = "word2\0";
   char* word3 = "word3\0";

   set_w(p, word1); 
   assert( strsame(p->start->w, word1) ); /*word1 added as first word*/
   assert( strsame(p->cw->w, word1) );    /*word1 added as cw word   */

   set_w(p, word2); 
   assert( strsame(p->start->w, word1) ); /*word1 remains first word*/
   assert( strsame(p->cw->w, word2) );    /*word2 added as cw word  */

   set_w(p, word3); 
   assert( strsame(p->start->w, word1) ); /*word1 remains first word*/
   assert( strsame(p->cw->w, word3) );    /*word3 added as cw word  */
}

void test_next_word(Program* p)
{
   char* word1 = "word1\0";
   char* word2 = "word2\0";
   char* word3 = "word3\0";

   /*takes p with word1->word2->word3, cw == word3*/
   assert( p->start != NULL );
   assert( p->cw    != NULL );
   assert( strsame(p->start->w, word1) );  
   assert( strsame(p->cw->w, word3) );  

   p->cw = p->start;
   assert( strsame(p->cw->w, word1) );     /*cw same as start*/

   next_word(p); 
   assert( strsame(p->start->w, word1) );  /*start word unchanged*/
   assert( strsame(p->cw->w, word2) );     /*cw advances to word2*/

   next_word(p); 
   assert( strsame(p->start->w, word1) );  /*start word unchanged*/
   assert( strsame(p->cw->w, word3) );     /*cw advances to word3*/

   next_word(p); 
   assert( strsame(p->start->w, word1) );  /*start word unchanged*/
   assert( p->cw == NULL );     /*cw remains word3, last word*/

   /*null cases - cw == NULL here*/
   next_word(p);
   assert( p->cw == NULL ); /*nothing happens*/

   /*return p to start state*/
   p->cw = p->start; 
   next_word(p); /*cw = word2*/
   next_word(p); /*cw = word3*/

   /*passing a null pointer gives printed error*/
}

/*-----------------------------------------*
 *            TEST RULE FUNCTIONS          *
 *-----------------------------------------*/
void test_op(void)
{
   Program p; 
   char* str = "+ - * / blah 200 op 450\0"; 

   init_prog(&p);
   prog_from_str(&p, str);
   enter_test_mode(&p);
   
   /*move cw to start*/
   p.cw = p.start;

   assert( op(&p) == true ); 
   assert( op(&p) == true ); 
   assert( op(&p) == true ); 
   assert( op(&p) == true ); 

   assert( op(&p) == false ); 
   next_word(&p);
   assert( op(&p) == false ); 
   next_word(&p);
   assert( op(&p) == false ); 
   next_word(&p);
   assert( op(&p) == false ); 
   next_word(&p);
   
   assert( op(&p) == false );  /*p->cw == NULL here*/

   p.cw = p.start;
   free_prog(&p); 
}

void test_var(void)
{
   /*var only takes SINGLE CAPITOL letters */
   Program p;
   char* str = "V G A af 2K .3 E3F * RT";

   init_prog(&p);
   prog_from_str(&p, str);
   enter_test_mode(&p);

   p.cw = p.start; 

   assert( var(&p) == true );
   assert( var(&p) == true );
   assert( var(&p) == true );

   assert( var(&p) == false );
   next_word(&p);
   assert( var(&p) == false );
   next_word(&p);
   assert( var(&p) == false );
   next_word(&p);
   assert( var(&p) == false );
   next_word(&p);
   assert( var(&p) == false );
   next_word(&p);
   assert( var(&p) == false );

   p.cw = p.start;
   free_prog(&p); 
}

void test_polish(void)
{
   Program p1, p2, p3, p4, p5, p6;
   Symtab* s = init_symtab();
   char v[MAXTOKENSIZE]; 
   char* correct1 = "A B + ; "; 
   char* correct2 = "10 ; FD C";
   char* correct3 = ";";
   char* wrong1   = "F G + FD";
   char* correct4 = "- + ; FD"; /*by grammer rules, this correct*/
   char* correct5 = "G + ; "; 

   /*adding all var names used to symbol table*/
   char v1[MAXTOKENSIZE] = "1.0";
   Varnode* x1 = init_varnode("A", v1);
   Varnode* x2 = init_varnode("B", v1);
   Varnode* x3 = init_varnode("C", v1);
   Varnode* x4 = init_varnode("F", v1);
   Varnode* x5 = init_varnode("Z", v1);
   add_varnode(s, x1);
   add_varnode(s, x2);
   add_varnode(s, x3);
   add_varnode(s, x4);
   add_varnode(s, x5);

   init_prog(&p1);
   init_prog(&p2);
   init_prog(&p3);
   init_prog(&p4);
   init_prog(&p5);
   init_prog(&p6);

   /*test mode*/
   enter_test_mode(&p1);
   enter_test_mode(&p2);
   enter_test_mode(&p3);
   enter_test_mode(&p4);
   enter_test_mode(&p5);
   enter_test_mode(&p6);

   prog_from_str(&p1, correct1);
   prog_from_str(&p2, correct2);
   prog_from_str(&p3, correct3);
   prog_from_str(&p4, wrong1);
   prog_from_str(&p5, correct4);
   prog_from_str(&p6, correct5);

   p1.cw = p1.start; 
   p2.cw = p2.start; 
   p3.cw = p3.start; 
   p4.cw = p4.start; 
   p5.cw = p5.start;
   p6.cw = p6.start;
   
   /*correct prog runs without printed error message,
     does not advance once hits ; character  */       
   polish(&p1, s, v);
   assert( strsame(get_cw(&p1), ";") );
 
   polish(&p2, s, v);
   assert( strsame(get_cw(&p2), ";") );

   polish(&p3, s, v);
   assert( strsame(get_cw(&p3), ";") ); 

   /*incorrect progs cause printed error message
   polish(&p4, s, v); */

   /*op first - illogical but grammatically correct*/
   polish(&p5, s, v); 
   assert( strsame(get_cw(&p5), ";") ); 

   polish(&p6, s, v); 
   assert( strsame(get_cw(&p6), ";") ); 

   /*fclose(fp);*/
   p1.cw = p1.start;
   free_prog(&p1); 
   free_prog(&p2); 
   free_prog(&p3); 
   free_prog(&p4); 
   free_prog(&p5);
   free_prog(&p6);
   free_symtab(&s);
}

void test_varnum(void)
{
   Program p;
   char n1[MAXTOKENSIZE] = "A";
   char n2[MAXTOKENSIZE] = "B";
   char v1[MAXTOKENSIZE] = "1.0";
   char v2[MAXTOKENSIZE] = "2.0";
   Varnode* x1 = init_varnode(n1, v1);
   Varnode* x2 = init_varnode(n2, v2);
   char* str = "A B 12.5 1000 ; sd + :=";
   Symtab* s = init_symtab(); 
   char v[MAXTOKENSIZE];

   init_prog(&p);
   enter_test_mode(&p);
   prog_from_str(&p, str);
   p.cw = p.start; 

   /*add variables to symbol table*/
   add_varnode(s, x1);
   add_varnode(s, x2);
   assert( has_var(s, p.start->w) == true );
   assert( has_var(s, p.start->next->w) == true );
   
   /*test varnum() returns T for variables/numbers and F for other*/
   /*test varnum() retreives variable value from symtab*/
   assert( varnum(&p, s, v) == true);
   assert( strsame(get_varval(s, n1), v1) ); 
   assert( varnum(&p, s, v) == true); 
   assert( strsame(get_varval(s, n2), v2) ); 
   assert( varnum(&p, s, v) == true); 
   assert( varnum(&p, s, v) == true); 

   assert( varnum(&p, s, v) == false); 
   next_word(&p);
   assert( varnum(&p, s, v) == false);
   next_word(&p); 
   assert( varnum(&p, s, v) == false); 
   next_word(&p);
   assert( varnum(&p, s, v) == false); 

   p.cw = p.start;
   free_symtab(&s); 
   free_prog(&p);
}

void test_set(void)
{
   Program p1, p2, p3, p4, p5, p6;
   Symtab* s = init_symtab();
   char* str1 = "SET A := 1 ;";
   char* str2 = "SET B := 2 A + ;";
   char* str3 = "SET C := 3 5 * 3 + ;";
   char* str4 = "A := 9 C ;"; 
   char* str5 = "SET 10 := A 5 + ;";
   char* str6 = "SET D := ";
   /*symbol table*/
   Varnode* x1 = init_varnode("A", "1.0");
   Varnode* x2 = init_varnode("C", "45.3");
   /*add variables to symbol table*/
   add_varnode(s, x1);
   add_varnode(s, x2);

   init_prog(&p1);
   init_prog(&p2);
   init_prog(&p3);
   init_prog(&p4);
   init_prog(&p5);
   init_prog(&p6);

   /*test mode*/
   enter_test_mode(&p1);
   enter_test_mode(&p2);
   enter_test_mode(&p3);
   enter_test_mode(&p4);
   enter_test_mode(&p5);
   enter_test_mode(&p6);

   prog_from_str(&p1, str1);
   prog_from_str(&p2, str2);
   prog_from_str(&p3, str3);
   prog_from_str(&p4, str4);
   prog_from_str(&p5, str5);
   prog_from_str(&p6, str6);

   assert( set(&p1, s) == true ); 
   assert( set(&p2, s) == true ); 
   assert( set(&p3, s) == true ); 
   /*missing "SET" at start returns false*/
   assert( set(&p4, s) == false );
   /*missing variable*/
   assert( set(&p5, s) == false );
   /*missing ; causes program-halting ERROR
   assert( set(&p6, s) == false ); */

   free_prog(&p1);
   free_prog(&p2);
   free_prog(&p3);
   free_prog(&p4);
   free_prog(&p5);
   free_prog(&p6);
   free_symtab(&s);
}

void test_do_(void)
{
   Program p1, p2, p3, p4, p5, p6;
   Symtab* s = init_symtab();
   char* str1 = "DO A FROM 1 TO 10 { FD A RT 45 } }";
   char* str2 = "DO A 1 TO 10 { FD A LT B } }";
   char* str3 = "A FROM 1 TO 10 { RT 10 } }";
   char* str4 = "DO B FROM 4 TO 15 { FD 10 "; 
   char* str5 = "DO C FROM 1 TO 15 FD 10 }";
   char* str6 = "DO Z FROM 10 TO 100000 { { RT Z }";
   /*create/ fill symbol table*/
   Varnode* x1 = init_varnode("B", "1.0");
   /*add variables to symbol table*/
   add_varnode(s, x1);

   init_prog(&p1);
   init_prog(&p2);
   init_prog(&p3);
   init_prog(&p4);
   init_prog(&p5);
   init_prog(&p6);

   /*test mode*/
   enter_test_mode(&p1);
   enter_test_mode(&p2);
   enter_test_mode(&p3);
   enter_test_mode(&p4);
   enter_test_mode(&p5);
   enter_test_mode(&p6);

   prog_from_str(&p1, str1);
   prog_from_str(&p2, str2);
   prog_from_str(&p3, str3);
   prog_from_str(&p4, str4);
   prog_from_str(&p5, str5);
   prog_from_str(&p6, str6);

   assert( do_(&p1, s) == true );
   assert( do_(&p2, s) == false );
   assert( do_(&p3, s) == false );
   /*missing brackets causes program halting ERROR
   assert( do_(&p4, s) == false ); 
   assert( do_(&p5, s) == false );
   assert( do_(&p6, s) == false ); */

   free_prog(&p1);
   free_prog(&p2);
   free_prog(&p3);
   free_prog(&p4);
   free_prog(&p5);
   free_prog(&p6);
   free_symtab(&s);
}

void test_fd(void)
{
   Program p1, p2, p3, p4, p5, p6, p7;
   Symtab* s = init_symtab();
   char* str1 = "FD 300";
   char* str2 = "FD Z";
   char* str3 = "FD 4.5";
   char* str4 = "RT X"; 
   char* str5 = "500";
   char* str6 = "FD RT";
   char* str7 = "FD";
   Varnode* x1 = init_varnode("Z", "1.0");
   Varnode* x2 = init_varnode("X", "5.0");
   /*add variables to symbol table*/
   add_varnode(s, x1);
   add_varnode(s, x2);

   init_prog(&p1);
   init_prog(&p2);
   init_prog(&p3);
   init_prog(&p4);
   init_prog(&p5);
   init_prog(&p6);
   init_prog(&p7);

   /*test mode*/
   enter_test_mode(&p1);
   enter_test_mode(&p2);
   enter_test_mode(&p3);
   enter_test_mode(&p4);
   enter_test_mode(&p5);
   enter_test_mode(&p6);
   enter_test_mode(&p7);

   prog_from_str(&p1, str1);
   prog_from_str(&p2, str2);
   prog_from_str(&p3, str3);
   prog_from_str(&p4, str4);
   prog_from_str(&p5, str5);
   prog_from_str(&p6, str6);
   prog_from_str(&p7, str7);

   assert( fd(&p1, s) == true );
   assert( fd(&p2, s) == true );
   assert( fd(&p3, s) == true );
   assert( fd(&p4, s) == false );
   assert( fd(&p5, s) == false );
   assert( fd(&p6, s) == false );
   assert( fd(&p7, s) == false );

   free_prog(&p1);
   free_prog(&p2);
   free_prog(&p3);
   free_prog(&p4);
   free_prog(&p5);
   free_prog(&p6);
   free_prog(&p7);
   free_symtab(&s);
}

void test_rt(void)
{
   Program p1, p2, p3, p4, p5, p6, p7;
   Symtab* s = init_symtab();
   char* str1 = "RT 300";
   char* str2 = "RT Z";
   char* str3 = "RT 4.5";
   char* str4 = "FD X"; 
   char* str5 = "500";
   char* str6 = "RT FD";
   char* str7 = "RT"; 
   Varnode* x1 = init_varnode("Z", "1.0");
   Varnode* x2 = init_varnode("X", "5.0");
   /*add variables to symbol table*/
   add_varnode(s, x1);
   add_varnode(s, x2);

   init_prog(&p1);
   init_prog(&p2);
   init_prog(&p3);
   init_prog(&p4);
   init_prog(&p5);
   init_prog(&p6);
   init_prog(&p7);

   /*test mode*/
   enter_test_mode(&p1);
   enter_test_mode(&p2);
   enter_test_mode(&p3);
   enter_test_mode(&p4);
   enter_test_mode(&p5);
   enter_test_mode(&p6);
   enter_test_mode(&p7);

   prog_from_str(&p1, str1);
   prog_from_str(&p2, str2);
   prog_from_str(&p3, str3);
   prog_from_str(&p4, str4);
   prog_from_str(&p5, str5);
   prog_from_str(&p6, str6);
   prog_from_str(&p7, str7);

   assert( rt(&p1, s) == true );
   assert( rt(&p2, s) == true );
   assert( rt(&p3, s) == true );
   assert( rt(&p4, s) == false );
   assert( rt(&p5, s) == false );
   assert( rt(&p6, s) == false );
   assert( rt(&p7, s) == false );

   free_prog(&p1);
   free_prog(&p2);
   free_prog(&p3);
   free_prog(&p4);
   free_prog(&p5);
   free_prog(&p6);
   free_prog(&p7);
   free_symtab(&s);
}

void test_lt(void)
{
   Program p1, p2, p3, p4, p5, p6, p7;
   Symtab* s = init_symtab();
   char* str1 = "LT 300";
   char* str2 = "LT Z";
   char* str3 = "LT 4.5";
   char* str4 = "FD X"; 
   char* str5 = "5000";
   char* str6 = "LT FD";
   char* str7 = "LT"; 
   Varnode* x1 = init_varnode("Z", "1.0");
   Varnode* x2 = init_varnode("X", "5.0");
   /*add variables to symbol table*/
   add_varnode(s, x1);
   add_varnode(s, x2);

   init_prog(&p1);
   init_prog(&p2);
   init_prog(&p3);
   init_prog(&p4);
   init_prog(&p5);
   init_prog(&p6);
   init_prog(&p7);

   /*test mode*/
   enter_test_mode(&p1);
   enter_test_mode(&p2);
   enter_test_mode(&p3);
   enter_test_mode(&p4);
   enter_test_mode(&p5);
   enter_test_mode(&p6);
   enter_test_mode(&p7);

   prog_from_str(&p1, str1);
   prog_from_str(&p2, str2);
   prog_from_str(&p3, str3);
   prog_from_str(&p4, str4);
   prog_from_str(&p5, str5);
   prog_from_str(&p6, str6);
   prog_from_str(&p7, str7);

   assert( lt(&p1, s) == true );
   assert( lt(&p2, s) == true );
   assert( lt(&p3, s) == true );
   assert( lt(&p4, s) == false );
   assert( lt(&p5, s) == false );
   assert( lt(&p6, s) == false );
   assert( lt(&p7, s) == false );

   free_prog(&p1);
   free_prog(&p2);
   free_prog(&p3);
   free_prog(&p4);
   free_prog(&p5);
   free_prog(&p6);
   free_prog(&p7);
   free_symtab(&s);
}

void test_instruction(void)
{
   Program p1, p2, p3, p4, p5, p6, p7;
   Symtab* s = init_symtab();
   char* str1 = "FD 2.3";
   char* str2 = "RT 400 }";
   char* str3 = "SET A := B C + ;";
   char* str4 = "DO B FROM 1 TO 10 { FD B }"; 
   char* str5 = "5000";
   char* str6 = "A FROM 1 TO 200";
   char* str7 = "}"; 
   Varnode* x1 = init_varnode("B", "1.0");
   Varnode* x2 = init_varnode("C", "5.0");
   /*add variables to symbol table*/
   add_varnode(s, x1);
   add_varnode(s, x2);

   init_prog(&p1);
   init_prog(&p2);
   init_prog(&p3);
   init_prog(&p4);
   init_prog(&p5);
   init_prog(&p6);
   init_prog(&p7);

   /*test mode*/
   enter_test_mode(&p1);
   enter_test_mode(&p2);
   enter_test_mode(&p3);
   enter_test_mode(&p4);
   enter_test_mode(&p5);
   enter_test_mode(&p6);
   enter_test_mode(&p7);

   prog_from_str(&p1, str1);
   prog_from_str(&p2, str2);
   prog_from_str(&p3, str3);
   prog_from_str(&p4, str4);
   prog_from_str(&p5, str5);
   prog_from_str(&p6, str6);
   prog_from_str(&p7, str7);

   /*p1, p2, p3, p4 return and are correct*/
   instruction(&p1, s); 
   instruction(&p2, s); 
   instruction(&p3, s); 
   instruction(&p4, s); 
   /*p5, p6, p7 print an ERROR message:
   instruction(&p5, s); 
   instruction(&p6, s); 
   instruction(&p7, s); */

   free_prog(&p1);
   free_prog(&p2);
   free_prog(&p3);
   free_prog(&p4);
   free_prog(&p5);
   free_prog(&p6);
   free_prog(&p7);
   free_symtab(&s);
}

void test_inst_list(void)
{
   Program p1, p2, p3, p4, p5, p6, p7;
   Symtab* s = init_symtab();
   char* str1 = "FD 10 }";
   char* str2 = "RT 400 DO A FROM 1 TO 10 { LT A } }";
   char* str3 = "SET A := B C + ; }";
   char* str4 = "}"; 
   char* str5 = "5000";
   char* str6 = "A FROM 1 TO 200";
   char* str7 = "FD 23.0"; 
   Varnode* x1 = init_varnode("B", "1.0");
   Varnode* x2 = init_varnode("C", "5.0");
   /*add variables to symbol table*/
   add_varnode(s, x1);
   add_varnode(s, x2);

   init_prog(&p1);
   init_prog(&p2);
   init_prog(&p3);
   init_prog(&p4);
   init_prog(&p5);
   init_prog(&p6);
   init_prog(&p7);

   /*test mode*/
   enter_test_mode(&p1);
   enter_test_mode(&p2);
   enter_test_mode(&p3);
   enter_test_mode(&p4);
   enter_test_mode(&p5);
   enter_test_mode(&p6);
   enter_test_mode(&p7);

   prog_from_str(&p1, str1);
   prog_from_str(&p2, str2);
   prog_from_str(&p3, str3);
   prog_from_str(&p4, str4);
   prog_from_str(&p5, str5);
   prog_from_str(&p6, str6);
   prog_from_str(&p7, str7);

   /*p1, p2, p3, p4 return and are correct*/
   inst_list(&p1, s);
   inst_list(&p2, s);
   inst_list(&p3, s);
   inst_list(&p4, s);
   /*p5, p6, p7 print errors 
   inst_list(&p5, s);  
   inst_list(&p6, s);   
   inst_list(&p7, s);   */

   free_prog(&p1);
   free_prog(&p2);
   free_prog(&p3);
   free_prog(&p4);
   free_prog(&p5);
   free_prog(&p6);
   free_prog(&p7); 
   free_symtab(&s);
}

void test_extensions(void)
{
   Program p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12;
   Symtab* s = init_symtab();
   char* str1 = "SPEED 0 }";
   char* str2 = "SPEED A }";
   char* str3 = "SPEED }";
   char* str4 = "HEADING UP }";
   char* str5 = "HEADING ROIGHT }";
   char* str6 = "HEADING 45 }";
   char* str7 = "IF A < 200 { RT 10 }";
   char* str8 = "IF A >  }";
   char* str9 = "IF < 3 }";
   char* str10 = "RECT 100 , 300 FILL RT 10 } ";
   char* str11 = "RECT 12 45 FILL }";
   char* str12 = "RECT 55 }";

   init_prog(&p1);
   init_prog(&p2);
   init_prog(&p3);
   init_prog(&p4);
   init_prog(&p5);
   init_prog(&p6);
   init_prog(&p7);
   init_prog(&p8);
   init_prog(&p9);
   init_prog(&p10);
   init_prog(&p11);
   init_prog(&p12);

   /*test mode*/
   enter_test_mode(&p1);
   enter_test_mode(&p2);
   enter_test_mode(&p3);
   enter_test_mode(&p4);
   enter_test_mode(&p5);
   enter_test_mode(&p6);
   enter_test_mode(&p7);
   enter_test_mode(&p8);
   enter_test_mode(&p9);
   enter_test_mode(&p10);
   enter_test_mode(&p11);
   enter_test_mode(&p12);

   prog_from_str(&p1, str1);
   prog_from_str(&p2, str2);
   prog_from_str(&p3, str3);
   prog_from_str(&p4, str4);
   prog_from_str(&p5, str5);
   prog_from_str(&p6, str6);
   prog_from_str(&p7, str7);
   prog_from_str(&p8, str8);
   prog_from_str(&p9, str9);
   prog_from_str(&p10, str10);
   prog_from_str(&p11, str11);
   prog_from_str(&p12, str12);

   /*Speed*/
   assert( speed(&p1, s) == true );
   assert( speed(&p2, s) == false );
   assert( speed(&p3, s) == false );
   /*Heading*/
   assert( heading(&p4, s) == true );
   assert( heading(&p5, s) == false );
   assert( heading(&p6, s) == false );
   /*If*/
   assert( if_(&p7, s) == true );
   assert( if_(&p8, s) == false );
   assert( if_(&p9, s) == false );
   /*Rect*/
   assert( rect(&p10, s) == true );
   assert( rect(&p11, s) == false );
   assert( rect(&p12, s) == false );

   free_prog(&p1);
   free_prog(&p2);
   free_prog(&p3);
   free_prog(&p4);
   free_prog(&p5);
   free_prog(&p6);
   free_prog(&p7); 
   free_prog(&p8); 
   free_prog(&p9); 
   free_prog(&p10); 
   free_prog(&p11); 
   free_prog(&p12);  
   free_symtab(&s);
}

void test_prog(void)
{
   Program p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15;
   Symtab* s = init_symtab();
   char* str1 = "{ FD 12.3 }";
   char* str2 = "{ DO A FROM 1 TO 10 { RT 20 DO B FROM 2 TO 20 { LT 57.777 } } }";
   char* str3 = "{ SET A := 500.5 ; RT 15 }";
   char* str4 = "{ DO A FROM 4 TO 44 { LT A RT 33.576 } }"; 
   char* str5 = "FD 2.4 } ";
   char* str6 = "{ SET A := 5 ; ";
   char* str7 = "{"; 
   char* str8 = "{ FD 23.0 } ; "; 
   char* str9 = "{ FD 30 LT 45 FD 30 LT 45 FD 30 LT 45 FD 30 LT 45 FD 30 LT 45 FD 30 LT 45 FD 30 LT 45 FD 30 LT 45 }";
   char* str10 = "{ DO A FROM 1 TO 8 { FD 30 LT 45 } }";
   char* str11 = "{ DO A FROM 1 TO 100 { SET C := A 1.5 * ; FD C RT 62 } }";
   char* str12 = "{ DO A FROM 1 TO 50 { FD A RT 30 DO B FROM 1 TO 8 { SET C := A 5 / ; FD C RT 45 } } }";
   /*testing Expansions*/
   char* str13 = "{ POS 30 , C FD 10 COLOR RED RT 30 }";
   char* str14 = "{ HEADING RIGHT COLOR A , 10 , 44.5 , 100 FD 77 }";
   char* str15 = "{ RT 1.440 FD 9 HEADING UP }";
   /*symbol table update/fill*/
   Varnode* x1 = init_varnode("B", "1.0");
   Varnode* x2 = init_varnode("C", "5.0");
   add_varnode(s, x1);
   add_varnode(s, x2);

   init_prog(&p1);
   init_prog(&p2);
   init_prog(&p3);
   init_prog(&p4);
   init_prog(&p5);
   init_prog(&p6);
   init_prog(&p7);
   init_prog(&p8);
   init_prog(&p9);
   init_prog(&p10);
   init_prog(&p11);
   init_prog(&p12);
   init_prog(&p13);
   init_prog(&p14);
   init_prog(&p15);

   /*test mode*/
   enter_test_mode(&p1);
   enter_test_mode(&p2);
   enter_test_mode(&p3);
   enter_test_mode(&p4);
   enter_test_mode(&p5);
   enter_test_mode(&p6);
   enter_test_mode(&p7);
   enter_test_mode(&p8);
   enter_test_mode(&p9);
   enter_test_mode(&p10);
   enter_test_mode(&p11);
   enter_test_mode(&p12);
   enter_test_mode(&p13);
   enter_test_mode(&p14);
   enter_test_mode(&p15);

   prog_from_str(&p1, str1);
   prog_from_str(&p2, str2);
   prog_from_str(&p3, str3);
   prog_from_str(&p4, str4);
   prog_from_str(&p5, str5);
   prog_from_str(&p6, str6);
   prog_from_str(&p7, str7);
   prog_from_str(&p8, str8);
   prog_from_str(&p9, str9);
   prog_from_str(&p10, str10);
   prog_from_str(&p11, str11);
   prog_from_str(&p12, str12);
   prog_from_str(&p13, str13);
   prog_from_str(&p14, str14);
   prog_from_str(&p15, str15);

   /*p1, p2, p3, p4 are correct */
   prog(&p1, s); 
   prog(&p2, s); 
   prog(&p3, s);
   prog(&p4, s);
   /*p5, p6, p7, p8 are bracket errors that hault the program 
   prog(&p5, s);  
   prog(&p6, s);  
   prog(&p7, s); 
   prog(&p8, s);  */ 
   /*p9, p10, p11, p12 are correct progs from turtle sheet*/
   prog(&p9, s); 
   prog(&p10, s); 
   prog(&p11, s);
   prog(&p12, s);
   /*correct expansion progs*/
   prog(&p13, s);
   prog(&p14, s);
   prog(&p15, s);

   free_prog(&p1);
   free_prog(&p2);
   free_prog(&p3);
   free_prog(&p4);
   free_prog(&p5);
   free_prog(&p6);
   free_prog(&p7); 
   free_prog(&p8); 
   free_prog(&p9); 
   free_prog(&p10); 
   free_prog(&p11); 
   free_prog(&p12); 
   free_prog(&p13); 
   free_prog(&p14); 
   free_prog(&p15); 
   free_symtab(&s);
}

void test_init_varnode(void)
{
   char var_name[MAXTOKENSIZE] = "A";
   char value[MAXTOKENSIZE] = "5.75"; 
   Varnode* v = init_varnode(var_name, value);
   assert( v != NULL );
   assert( strsame(v->name, var_name) );
   assert( strsame(v->val, value) ); 
   assert( v->used == false ); 
   assert( v->next == NULL ); 
   free(v); 
}

void test_init_symtab(void)
{
   Symtab* st = init_symtab();
   assert( st != NULL );
   assert( st->start == NULL );
   assert( st->last  == NULL );
   assert( st->curr  == NULL );
   free(st);
}


void test_add_varnode(void)
{
   char n1[MAXTOKENSIZE] = "A";
   char n2[MAXTOKENSIZE] = "B";
   char n3[MAXTOKENSIZE] = "C";
   char v1[MAXTOKENSIZE] = "1.0";
   char v2[MAXTOKENSIZE] = "2.0";
   char v3[MAXTOKENSIZE] = "3.0";
   Symtab* st = init_symtab();
   Varnode* x1 = init_varnode(n1, v1);
   Varnode* x2 = init_varnode(n2, v2);
   Varnode* x3 = init_varnode(n3, v3);

   assert( st->start == NULL );
   assert( st->last  == NULL );
   assert( st->curr  == NULL );

   add_varnode(st, x1);
   assert( st->start == x1 );
   assert( st->last  == x1 );
   assert( st->curr  == x1 );
   assert( st->last->next == NULL );  

   add_varnode(st, x2);
   assert( st->start == x1 );
   assert( st->last  == x2 ); 
   assert( st->last->next == NULL );  

   add_varnode(st, x3); 
   assert( st->start == x1 );
   assert( st->last  == x3 ); 
   assert( st->last->next == NULL );  

   free(x1);
   free(x2);
   free(x3);
   free(st); 
}

void test_update_varnode(void)
{
   char n1[MAXTOKENSIZE] = "A";
   char n2[MAXTOKENSIZE] = "B";
   char n3[MAXTOKENSIZE] = "C";
   char v1[MAXTOKENSIZE] = "1.0";
   char v2[MAXTOKENSIZE] = "2.0";
   char v3[MAXTOKENSIZE] = "3.0";
   char v4[MAXTOKENSIZE] = "4.0";
   char v5[MAXTOKENSIZE] = "5.0";
   char v6[MAXTOKENSIZE] = "6.0";
   Symtab* st = init_symtab();
   Varnode* x1 = init_varnode(n1, v1);
   Varnode* x2 = init_varnode(n2, v2);
   Varnode* x3 = init_varnode(n3, v3);

   add_varnode(st, x1); 
   add_varnode(st, x2);
   add_varnode(st, x3);

   st->curr = st->start;
   update_varnode(st, n1, v4); 
   assert( strsame(st->curr->val, v4) );

   update_varnode(st, n2, v5);
   assert( strsame(st->curr->val, v5) );

   update_varnode(st, n3, v6);
   assert( strsame(st->curr->val, v6) ); 

   /*prints warning for undeclared variable*/
   update_varnode(st, "D", v1);
   

   free(x1);
   free(x2);
   free(x3);
   free(st);
}

void test_get_has_used(void)
{
   char n1[MAXTOKENSIZE] = "A";
   char n2[MAXTOKENSIZE] = "B";
   char n3[MAXTOKENSIZE] = "C";
   char v1[MAXTOKENSIZE] = "1.0";
   char v2[MAXTOKENSIZE] = "2.0";
   char v3[MAXTOKENSIZE] = "3.0";
   Symtab* st = init_symtab();
   Varnode* x1 = init_varnode(n1, v1);
   Varnode* x2 = init_varnode(n2, v2);
   Varnode* x3 = init_varnode(n3, v3);
   Varnode* p;

   add_varnode(st, x1);
   add_varnode(st, x2);
   add_varnode(st, x3); 

   /*before accessed, all variables used == false*/
   p = st->start;
   while ( p != NULL){
      assert( p->used == false );
      p = p->next;
   }
   assert( unused_var(st) == true );

   assert( strsame(get_varval(st, n1), v1) );
   assert( strsame(get_varval(st, n2), v2) );
   assert( strsame(get_varval(st, n3), v3) );
   /*accessing undeclared var prints error message
   assert( strsame(get_varval(st, 'D'), v3) );*/

   /*test has_varval*/
   assert( has_var(st, "A") == true );
   assert( has_var(st, "B") == true );
   assert( has_var(st, "C") == true );
   assert( has_var(st, "D") == false );
   assert( has_var(st, "Z") == false );
   assert( has_var(st, "E") == false );

   /*after accessed, all variables used == true*/
   p = st->start;
   while ( p != NULL){
      assert( p->used == true );
      p = p->next;
   }
   /*test unused_var()*/
   assert( unused_var(st) == false );

   free(x1);
   free(x2);
   free(x3);
   free(st);
}

void test_free_symtab(void)
{
   char n1[MAXTOKENSIZE] = "A";
   char n2[MAXTOKENSIZE] = "B";
   char n3[MAXTOKENSIZE] = "C";
   char v1[MAXTOKENSIZE] = "1.0";
   char v2[MAXTOKENSIZE] = "2.0";
   char v3[MAXTOKENSIZE] = "3.0";
   Symtab* st = init_symtab();
   Varnode* x1 = init_varnode(n1, v1);
   Varnode* x2 = init_varnode(n2, v2);
   Varnode* x3 = init_varnode(n3, v3);

   assert( st->start == NULL );
   assert( st->last  == NULL );
   assert( st->curr  == NULL );

   add_varnode(st, x1);
   assert( st->start == x1 );
   assert( st->last  == x1 );
   assert( st->curr  == x1 );
   assert( st->last->next == NULL );  

   add_varnode(st, x2);
   assert( st->start == x1 );
   assert( st->last  == x2 ); 
   assert( st->last->next == NULL );  

   add_varnode(st, x3); 
   assert( st->start == x1 );
   assert( st->last  == x3 ); 
   assert( st->last->next == NULL );  

   /*valrind: all heaps freed, 0 errors*/
   free_symtab(&st);
}

