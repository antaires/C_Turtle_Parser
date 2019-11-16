#include "bst_test.h"

/*_________________________________________
|                                          |
|             INTERPRETER TESTING          |
|------------------------------------------|
|  These tests confirm that the BST symbol |
| table works with the interpreter. Further|
| tests build off of previous parser tests |
| and confirm that the interpreter works   |
| with the SDL/Turtle info. Additional     |
| tests cover evaluation of polish and     |
| direct/math functions. Graphics tests    |
| confirm that the turtle is updated       |
| correctly given RT, LT, FD etc           |
|__________________________________________|
--------------------------------------------
*/

void test_all_bst(void)
{
   test_symtab_insert();
   test_symtab_update();
   test_set_get_varval();
   test_var(); 
   test_polish();
   test_varnum();
   test_set();
   test_do_();
   test_fd();
   test_rt();
   test_lt();
   test_instruction();
   test_inst_list();
   test_extensions();
   test_evaluate();
   /*SDL/graphics*/
   test_pos_draw_turn();
   test_visual();
}

/* Add one element into the symtab */
void test_symtab_insert(void)
{
   Symtab* s = init_symtab(MAXTOKENSIZE);

   symtab_insert(s, "A", "1.0");
   symtab_insert(s, "Alpha", "1.0");
   symtab_insert(s, "Bat", "1.0");
   symtab_insert(s, "Cricket", "1.0");
   symtab_insert(s, "Zzzz888", "1.0");

   assert( symtab_isin(s, "A")       == true );
   assert( symtab_isin(s, "Alpha")   == true );
   assert( symtab_isin(s, "Bat")     == true );
   assert( symtab_isin(s, "Cricket") == true );
   assert( symtab_isin(s, "Zzzz888") == true );
   /*not in symtab*/
   assert( symtab_isin(s, "B")       == false );
   assert( symtab_isin(s, "Alphan")  == false );
   assert( symtab_isin(s, "Zzzzzz")  == false );
   assert( symtab_isin(s, "ricket")  == false );
}

/* Return value of variable in symtab*/
void test_set_get_varval(void)
{
    Symtab* s = init_symtab(MAXTOKENSIZE);
   char val[MAXTOKENSIZE] = "\0"; 

   /*add variables*/
   symtab_insert(s, "Ape", "1.0");
   symtab_insert(s, "Bunker", "1.0");
   symtab_insert(s, "Wrote", "1.0");
   symtab_insert(s, "mississippi", "1.0");

   /*set new variable values*/
   set_varval(s->root, "Ape", "2.0");
   /*get new variable value to val*/
   get_varval(s->root, "Ape", val);
   assert( strsame(val, "2.0") == true );

   set_varval(s->root, "Bunker", "55");
   get_varval(s->root, "Bunker", val);
   assert( strsame(val, "55") == true );

   set_varval(s->root, "Wrote", "-5000");
   get_varval(s->root, "Wrote", val);
   assert( strsame(val, "-5000") == true );

   set_varval(s->root, "mississippi", "13");
   get_varval(s->root, "mississippi", val);
   assert( strsame(val, "13") == true ); 
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

/* Adds new element or updates value if already in tree*/
void test_symtab_update(void)
{
   Symtab* s = init_symtab(MAXTOKENSIZE);
   char val[MAXTOKENSIZE] = "\0";

   symtab_insert(s, "Ape", "1.0");
   symtab_insert(s, "Bat", "1.0");
   symtab_insert(s, "Cat", "1.0");
   symtab_insert(s, "Dat", "1.0");

   /*updates values*/ /*THIS TESTS get_varval()*/
   symtab_update(s, "Ape", "2.0");
   get_varval(s->root, "Ape", val);
   assert( strsame(val, "2.0") == true );

   symtab_update(s, "Bat", "2.0");
   get_varval(s->root, "Bat", val);
   assert( strsame(val, "2.0") == true );   

   symtab_update(s, "Cat", "2.0");
   get_varval(s->root, "Cat", val);
   assert( strsame(val, "2.0") == true );

   symtab_update(s, "Dat", "2.0");
   get_varval(s->root, "Dat", val);
   assert( strsame(val, "2.0") == true );

   /*adds new element*/
   symtab_update(s, "Rat", "5.0");
   get_varval(s->root, "Rat", val);
   assert( strsame(val, "5.0") == true );

   symtab_update(s, "glacier", "20000.0");
   get_varval(s->root, "glacier", val);
   assert( strsame(val, "20000.0") == true );

   symtab_update(s, "blurg", "-5.5");
   get_varval(s->root, "blurg", val);
   assert( strsame(val, "-5.5") == true );
}

void test_var(void)
{
   /*should accept any valid C variable name*/
   Program p;
   char* str = "Var Gat Art 3 2K .3 COLOR * RT";

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
   Symtab* s = init_symtab(MAXTOKENSIZE);
   char v[MAXTOKENSIZE]; 
   char* correct1 = "art bat + ; "; 
   char* correct2 = "10 ; FD Car";
   char* correct3 = ";"; /*rejected by interpreter*/
   char* wrong1   = "F G + FD";
   char* correct4 = "- + ; FD"; /*rejected by interpreter*/
   char* correct5 = "G + ; "; /*rejected by interpreter*/
   /*Note: the statements which are rejected by interpreter 
     initially pass polish() and fail eval_polish, not tested
     in this function test                                    */

   /*adding all var names used to symbol table*/
   symtab_insert(s, "art", "1.0");
   symtab_insert(s, "bat", "1.0");
   symtab_insert(s, "Car", "1.0");
   symtab_insert(s, "F", "1.0");
   symtab_insert(s, "Z", "1.0");
   symtab_insert(s, "G", "1.0");

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

   /*rejected by interpeter but accepted by parser
     these tests pass Polish() here, but are rejected
     by eval_polish by the stack                    */
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
   char n1[MAXTOKENSIZE] = "Ant";
   char n2[MAXTOKENSIZE] = "brat";
   char v1[MAXTOKENSIZE] = "1.0";
   char v2[MAXTOKENSIZE] = "2.0";
   char* str = "Ant brat 12.5 1000 ; sd + :=";
   Symtab* s = init_symtab(MAXTOKENSIZE); 
   char v[MAXTOKENSIZE];

   symtab_insert(s, n1, v1);
   symtab_insert(s, n2, v2);

   init_prog(&p);
   enter_test_mode(&p);
   prog_from_str(&p, str);
   p.cw = p.start; 
   
   /*test varnum() returns T for variables/numbers and F for other*/
   /*test varnum() retreives variable value from symtab*/
   assert( varnum(&p, s, v) == true);
   get_varval(s->root, n1, v);
   assert( strsame(v, v1)   == true ); 
   assert( varnum(&p, s, v) == true); 

   get_varval(s->root, n2, v );
   assert( strsame(v, v2)   == true ); 
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
   Symtab* s = init_symtab(MAXTOKENSIZE);
   char* str1 = "SET Aardvark := 1 ;";
   char* str2 = "SET brazil := 2 Ant + ;";
   char* str3 = "SET C := 3 5 * 3 + ;";
   char* str4 = "A := 9 C ;"; 
   char* str5 = "SET 10 := Ant 5 + ;";
   char* str6 = "SET Dog := ";
   /*symbol table*/
   symtab_insert(s, "Ant", "5.0");
   symtab_insert(s, "C", "5.0");
   symtab_insert(s, "Dog", "5.0");

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
   assert( set(&p6, s) == false );*/

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
   Symtab* s = init_symtab(MAXTOKENSIZE);
   char* str1 = "DO alpha FROM 1 TO 10 { FD alpha RT 45 } }";
   char* str2 = "DO blurgh 1 TO 10 { FD blurgh LT B } }";
   char* str3 = "A FROM 1 TO 10 { RT 10 } }";
   char* str4 = "DO bat FROM 4 TO 15 { FD 10 "; 
   char* str5 = "DO C FROM 1 TO 15 FD 10 }";
   char* str6 = "DO Z FROM 10 TO 100000 { { RT Z }";
   /*symbol table*/
   symtab_insert(s, "B", "1.0");

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
   Symtab* s = init_symtab(MAXTOKENSIZE);
   char* str1 = "FD 300";
   char* str2 = "FD Z";
   char* str3 = "FD 4.5";
   char* str4 = "RT X"; 
   char* str5 = "500";
   char* str6 = "FD RT";
   char* str7 = "FD";
   /*add variables to symbol table*/
   symtab_insert(s, "Z", "1.0");
   symtab_insert(s, "X", "5.0");

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
   Symtab* s = init_symtab(MAXTOKENSIZE);
   char* str1 = "RT 300";
   char* str2 = "RT Z";
   char* str3 = "RT 4.5";
   char* str4 = "FD X"; 
   char* str5 = "500";
   char* str6 = "RT FD";
   char* str7 = "RT"; 
   symtab_insert(s, "Z", "1.0");
   symtab_insert(s, "X", "5.0");

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
   Symtab* s = init_symtab(MAXTOKENSIZE);
   char* str1 = "LT 300";
   char* str2 = "LT Z";
   char* str3 = "LT 4.5";
   char* str4 = "FD X"; 
   char* str5 = "5000";
   char* str6 = "LT FD";
   char* str7 = "LT"; 
   symtab_insert(s, "Z", "1.0");
   symtab_insert(s, "X", "5.0");

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
   Symtab* s = init_symtab(MAXTOKENSIZE);
   char* str1 = "FD 2.3";
   char* str2 = "RT 400 }";
   char* str3 = "SET A := B C + ;";
   char* str4 = "DO B FROM 1 TO 10 { FD B }"; 
   char* str5 = "5000";
   char* str6 = "A FROM 1 TO 200";
   char* str7 = "}"; 
   /*add variables to symbol table*/
   symtab_insert(s, "B", "1.0");
   symtab_insert(s, "C", "5.0");

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
   Symtab* s = init_symtab(MAXTOKENSIZE);
   char* str1 = "FD 10 }";
   char* str2 = "RT 400 DO A FROM 1 TO 10 { LT A } }";
   char* str3 = "SET A := B C + ; }";
   char* str4 = "}"; 
   char* str5 = "5000";
   char* str6 = "A FROM 1 TO 200";
   char* str7 = "FD 23.0"; 
   /*add variables to symbol table*/
   symtab_insert(s, "B", "1.0");
   symtab_insert(s, "C", "5.0");

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
   Symtab* s = init_symtab(MAXTOKENSIZE);
   char* str1 = "SPEED 0 }";
   char* str2 = "SPEED art }";
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

   symtab_insert(s, "art", "1.0");
   symtab_insert(s, "A", "1.0");

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


void test_evaluate(void)
{
   char* result;
   char* mask   = "0000.000";

   /*test addition*/
   result = evaluate('+', "5.0", "3.0"); 
   assert( strsame( result, "8.000000" )); 
   free(result);
   result = evaluate('+', "3.12", "5000.5"); 
   assert( strncmp( result, "5003.620", strlen(mask) ) == 0 );
   free(result);
   result = evaluate('+', "0.0", "3.456"); 
   assert( strncmp( result, "3.456000", strlen(mask) ) == 0 );
   free(result);
   result = evaluate('+', "1.5", "1.5"); 
   assert( strncmp( result, "3.000000", strlen(mask) ) == 0 );
   free(result);

   /*test subtraction*/
   result = evaluate('-', "3.00", "1.00"); 
   assert( strncmp( result, "2.000000", strlen(mask) ) == 0 );
   free(result);
   result = evaluate('-', "5.00", "10.00"); 
   assert( strncmp( result, "-5.000000", strlen(mask) ) == 0 );
   free(result);
   result = evaluate('-', "5000.00", "20.00"); 
   assert( strncmp( result, "4980.000", strlen(mask) ) == 0 );
   free(result);

   /*test multiplicatin*/
   result = evaluate('*', "3.00", "1.00"); 
   assert( strncmp( result, "3.000000", strlen(mask) ) == 0 );
   free(result);
   result = evaluate('*', "12.0", "5.00"); 
   assert( strncmp( result, "60.00000", strlen(mask) ) == 0 );
   free(result);
   result = evaluate('*', "-1.0", "5.00"); 
   assert( strncmp( result, "-5.000000", strlen(mask) ) == 0 );
   free(result);
   result = evaluate('*', "0.0", "12.345"); 
   assert( strncmp( result, "0.000000", strlen(mask) ) == 0 );
   free(result);
   result = evaluate('*', "-1000.0", "5.123456"); 
   assert( strncmp( result, "-5123.456", strlen(mask) ) == 0 );
   free(result);

   /*test division*/
   result = evaluate('/', "3.00", "1.00"); 
   assert( strncmp( result, "3.000000", strlen(mask) ) == 0 );
   free(result);
   result = evaluate('/', "25.00", "5.00"); 
   assert( strncmp( result, "5.000000", strlen(mask) ) == 0 );
   free(result);
   result = evaluate('/', "-25.00", "5.00"); 
   assert( strncmp( result, "-5.000000", strlen(mask) ) == 0 );
   free(result);
   result = evaluate('/', "-25.00", "-5.00"); 
   assert( strncmp( result, "5.000000", strlen(mask) ) == 0 );
   free(result);
   result = evaluate('/', "5.00", "25.00"); 
   assert( strncmp( result, "0.200000", strlen(mask) ) == 0 );
   free(result);
   result = evaluate('/', "10.00", "100.00"); 
   assert( strncmp( result, "0.100000", strlen(mask) ) == 0 );
   free(result);
}

/*Graphics*/
void test_pos_draw_turn(void)
{

/*
void draw_line(Program* p, char* v); 
void turn_right(Program* p, char* v);
void turn_left(Program* p, char* v);
*/

   Program p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12;
   Symtab* s = init_symtab(MAXTOKENSIZE);
   char* str1 = "POS 0 , 0";
   char* str2 = "POS 250 , 200";
   char* str3 = "POS 50 , 5";
   char* str4 = "RT 90";
   char* str5 = "RT 180";
   char* str6 = "RT 225";
   char* str7 = "LT 90";
   char* str8 = "LT 45";
   char* str9 = "LT 180";
   char* str10 = "FD 10";
   char* str11 = "FD 50";
   char* str12 = "LT 90 FD 100 }";

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

   /*pos*/
   pos(&p1, s);
   assert( p1.t.pos.x == 0 );
   assert( p1.t.pos.y == 0 );
   pos(&p2, s);
   assert( p2.t.pos.x == 250 );
   assert( p2.t.pos.y == 200 );
   pos(&p3, s);
   assert( p3.t.pos.x == 50 );
   assert( p3.t.pos.y == 5 );

   /*turns / also tests helper functions that convert radians/degrees
     and strings to ints etc*/
   rt(&p4, s);
   assert( p4.t.heading == 90 );
   rt(&p5, s);
   assert( p5.t.heading == 180 );
   rt(&p6, s);
   assert( p6.t.heading == 225 );
   
   lt(&p7, s);
   assert( p7.t.heading == -90 );
   lt(&p8, s);
   assert( p8.t.heading == -45 );
   lt(&p9, s);
   assert( p9.t.heading == -180 );

   /*draw line/ FD*/
   assert( p10.t.pos.x == 250 ); /*turtle starts in center of screen*/
   assert( p10.t.pos.y == 250 );
   fd(&p10, s);
   assert( p10.t.pos.x == 250 + 10 );
   assert( p10.t.pos.y == 250 );

   assert( p11.t.pos.x == 250 ); /*turtle starts in center of screen*/
   assert( p11.t.pos.y == 250 );
   fd(&p11, s);
   assert( p11.t.pos.x == 250 + 50 );
   assert( p11.t.pos.y == 250 );

   assert( p12.t.pos.x == 250 ); /*turtle starts in center of screen*/
   assert( p12.t.pos.y == 250 );
   lt(&p12, s);                  /*turtle turns left (facing up)*/
   fd(&p12, s);
   assert( p12.t.pos.x == 250 );
   assert( p12.t.pos.y == 250 - 100 ); /*moving up is subtraction*/

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

/*Many simple visual tests were made after all other tests were passed
  to visually confirm that the turtle drew as expected 
  These are 2 examples of many*/
void test_visual(void)
{
   Program p, p2; 
   Symtab *s = init_symtab(50);
   char* str = "{ SPEED 0 HEADING RIGHT DO A FROM 0 TO 500 { COLOR 150 , A , A , 255 SET B := A A + ; FD A LT 90 IF A < 200 { RECT A , A } IF A > 200 { RECT A , B } } POS CENTER COLOR 150 , A , A , 255 SET B := 11 5 * ; DO C FROM 0 TO 200 { RECT C , C FILL FD C RT 20 COLOR C , C , 150 , C } POS CENTRE DO D FROM 0 TO 500 { RECT D , D FD D RT D COLOR D , D , 150 , D } RECT 20 , 40 FILL }";
   char* str2 = "{ SPEED 0 POS CENTER COLOR 255 , 229 , 84 , 150 RECT 500 , 500 FILL COLOR 255 , 229 , 84 , 250 RECT 450 , 450 FILL DO a_nt FROM 1 TO 20 { POS CENTER SET beta := a_nt 10 * a_nt + ; SET C := beta a_nt * ; COLOR a_nt , C , a_nt , beta FD beta RECT beta , a_nt DO X FROM 1 TO 50 { RT beta FD beta COLOR C , a_nt , C , beta  } LT 90 DO C FROM 1 TO 50 { FD a_nt RT C } IF a_nt < 30 { RECT a_nt , beta } } }";

   init_prog(&p);
   prog_from_str(&p, str);
   prog(&p, s); 
   
   init_prog(&p2);
   prog_from_str(&p2, str2);
   prog(&p2, s); 

   free_prog(&p); 
   free_prog(&p2); 
   free_symtab(&s);
}





