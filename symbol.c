#include "parse.h"

/*_______________________________________
 |          PARSER SYMBOL TABLE          |
 |---------------------------------------|
 | + accepts single char variables       |
 | + prints warnings for variable errors |
 | (undefined, unused) whereas for interp|
 | these errors would hault the program  |
 | + linked list structure to store any  |
 | number of variables                   |
 |_______________________________________|
 |---------------------------------------|*/

/*-----------------------------------------*
 |         SYMBOL TABLE FUNCTIONS          |
 *-----------------------------------------*/

void symtab_update(Symtab* st, char* n, char* v)
{
   /*updates value if variable in table else adds it to table*/
   Varnode* new;
   if ( has_var(st, n) ){
      update_varnode(st, n, v);
      return;
   }
   new = init_varnode(n, v);
   add_varnode(st, new); 
}

Varnode* init_varnode(char* n, char* v)
{
   /*callocs space and returns pointer*/
   Varnode* var = calloc(1, sizeof(Varnode));
   if (var == NULL){
      ERROR("Failed to initialize varnode");
   }
   strcpy(var->name, n);
   strcpy(var->val, v); 
   var->next = NULL; 
   var->used = false;
   return var; 
}

Symtab* init_symtab(void)
{
   /*callocs space and returns pointer*/
   Symtab* st = calloc(1, sizeof(Symtab));
   if (st == NULL){
      ERROR("Failed to initialize symbol table");
   }
   st->start = NULL;
   st->last  = NULL;
   st->curr  = NULL;
   return st; 
}

void add_varnode(Symtab* st, Varnode* vn)
{
   /*adds variable to table*/
   if (st == NULL){
      ERROR("Add_varnode passed null pointer");
   }
   /*first varnode*/
   if ( st->start == NULL ){
      st->start = vn;
      st->last  = vn;
      st->curr  = vn; 
      return; 
   }
   /*add new as last varnode*/
   st->last->next = vn; 
   st->last = st->last->next; 
}

void update_varnode(Symtab* st, char* n, char* v)
{
   /*updates variable values*/
   st->curr = st->start; 
   while ( st->curr != NULL ){
      if ( strsame(n, st->curr->name) ){
         strcpy(st->curr->val, v); 
         return;
      }
      st->curr = st->curr->next; 
   }
   printf("\nWarning: Variable not defined before use");
}

char* get_varval(Symtab* st, char* n)
{
   /*returns value and sets used to true*/
   Varnode* p = st->start; 
   while ( p != NULL ){
      if ( strsame(n, p->name) ){ 
         p->used = true; 
         return p->val;
      }
      p = p->next; 
   }
   printf("\nWarning: Variable %s not defined before use", n);
   return "1"; /*dummy variable*/
}

bool has_var(Symtab* st, char* n)
{
   Varnode* p = st->start;
   while( p != NULL ){
      if ( strsame(p->name, n) ){
         return true;
      }
      p = p->next;
   }
   return false;
}

bool unused_var(Symtab* st)
{
   Varnode* p = st->start;
   while( p != NULL ){
      if ( p->used == false ){
         return true;
      }
      p = p->next; 
   }
   return false;
}

void free_symtab(Symtab** st)
{
   Symtab* pst = *st; 
   Varnode* p = pst->start;
   
   /*walk through varnodes and free*/
   while ( p != NULL ){
      pst->start = p;
      p = p->next;
      free(pst->start); 
   }
   free(pst);
   st = NULL; 
}

void print_symtab(Symtab* st)
{
   Varnode* p = st->start; 
   while (p != NULL){
      printf("\nVAR %s VAL %s ", p->name, p->val);
      p = p->next; 
   }
}

