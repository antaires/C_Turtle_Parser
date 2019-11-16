/*_______________________________________
 |                                       |
 |       BST Struct for SYMBOL TABLE     |
 |---------------------------------------|
 | Can handle any variable name length   |
 | within SIZE declared on init_symtab() |
 |_______________________________________|
 |---------------------------------------|*/

#include <assert.h>
#include "bst.h"

/*primary*/
void insert_node( Bode* r, Bode* n);
Bode* create_node( Symtab* s);
void set_node_value( Bode* b, char* n, char* v);
void isin_tree( Bode* n, char* v, bool* isin);
void free_tree( Bode* n); 

/*helper*/
void set_new_root(Symtab* s, Bode* n); 
void go_left(Bode* r, Bode* n);
void go_right(Bode* r, Bode* n);
void check_left(Bode* n, char* v, bool* isin);
void check_right(Bode* n, char* v, bool* isin);
void print_tree( Bode* n);

/*Create empty symtab*/
Symtab* init_symtab(int size) 
{
   Symtab* s = (Symtab*) calloc(1,sizeof(Symtab));
   if(s == NULL){
      ERROR("Creation of Symbol table Failed\n");
   }

   if(size < 1) {
      ERROR("Size must be 1 or greater\n");
   }

   s->max_str = size;
   s->num_nodes = 0; 

   return s; 
}

/* Add one element into the symbol table */
void symtab_insert(Symtab* s, char* name, char* v)
{ 
   Bode* b;

   if ( v == NULL || s == NULL || name == NULL){
      ERROR("\ninsert_symtab() passed a NULL value"); 
   }

   if ( strlen(v) < 1){
      return;
   }

   b = create_node(s); 

   set_node_value(b, name, v);

   if ( s->root == NULL ){
      set_new_root(s, b); 
      return;  
   }

   insert_node(s->root, b);
}

void set_new_root(Symtab* s, Bode* n)
{
   s->root = n;
}

/* Returns true if v is in the array, false elsewise */
bool symtab_isin(Symtab* s, char* v)
{
   bool isin = false;

   if ( v == NULL || s == NULL ){
      return false; 
   }

   if ( strlen(v) < 1 ){
      return false; 
   }

   isin_tree(s->root, v, &isin);

   if ( isin ){
      return true; 
   }
   return false; 
}

void symtab_update(Symtab* s, char* n, char* v)
{
   /*if variable v in tree, update value, otherwise
     add it to tree                               */
   if ( symtab_isin(s, n) ){
      set_varval(s->root, n, v);
   } else {
      symtab_insert(s, n, v);
   }
}

/* Clears all space used, and sets pointer to NULL */
void free_symtab(Symtab** s)
{
   Symtab* p_d;

   if ( s == NULL ){
      return; 
   }

   p_d = *s;
   if ( p_d == NULL ){
      return; 
   }

   free_tree(p_d->root); 

   free(p_d);    
   p_d = NULL;
}

void free_tree(Bode* n)
{
   if (n == NULL){
      return; 
   }

   /*free string on n*/
   if (n->pstr != NULL){
      free(n->pstr);
      free(n->pval);
   }

   /*go left and right*/
   if (n->left != NULL){
      free_tree(n->left);
   } 
   if (n->right != NULL){
      free_tree(n->right);
   }

   free(n);
}

void get_varval(Bode* b, char* n, char* val)
{
   /*recursively search tree and returns variable value*/
   int compare; 

   if ( b == NULL || n == NULL ) {
      return;
   }

   compare = strcmp(n, b->pstr);

   if ( compare == 0 ) {
      /*found variable in tree*/
      strcpy( val , b->pval );
      return; 
   }

   if ( compare < 0 ){  
      /*check left*/
      if (b->left == NULL){
         return; 
      } else {
         get_varval(b->left, n, val);
      }

   } else {              
      /*check right*/
      if (b->right == NULL){
         return; 
      } else {
         get_varval(b->right, n, val); 
      } 
   }
   return; 
}

void set_varval(Bode* b, char* n, char* v)
{
   /*recursively search tree and update variable value*/
   int compare; 

   if ( b == NULL || v == NULL ) {
      return;
   }

   compare = strcmp(n, b->pstr);

   if ( compare == 0 ) {
      /*found variable in tree*/
      strcpy( b->pval, v);
      return; 
   }

   if ( compare < 0 ){  
      /*check left*/
      if (b->left == NULL){
         return; 
      } else {
         set_varval(b->left, n, v);
      }

   } else {              
      /*check right*/
      if (b->right == NULL){
         return; 
      } else {
         set_varval(b->right, n, v); 
      } 
   }
   return; 
}

void isin_tree(Bode* n, char* v, bool* isin)
{
   /*recursively search tree*/
   int compare; 

   if ( n == NULL || v == NULL ) {
      *isin = false;
      return;
   }

   compare = strcmp(v, n->pstr);

   if ( compare == 0 ) {
      *isin = true;
      return; 
   }

   if ( compare < 0 ){  
 
      check_left(n, v, isin); 

   } else {              

      check_right(n, v, isin); 

   }
   return; 
}

void check_left(Bode* n, char* v, bool* isin)
{
   if (n->left == NULL){
      *isin = false;
      return; 
   } else {
      isin_tree(n->left, v, isin);
   }
}

void check_right(Bode* n, char* v, bool* isin)
{
   if (n->right == NULL){
      *isin = false;
      return; 
   } else {
      isin_tree(n->right, v, isin); 
   }
}

Bode* create_node(Symtab* s)
{
   Bode* n = (Bode*) calloc(1, sizeof(Bode));

   if ( n == NULL ){
      ERROR("\nFailed to make binary search tree node"); 
   }

   n->left  = NULL;
   n->right = NULL; 
   n->max_str = s->max_str;

   return n; 

}

void set_node_value(Bode* b, char* n, char* v)
{
   /*get space for word and point pstr to space*/
   b->pstr = (char*) calloc(b->max_str, sizeof(char));
   b->pval = (char*) calloc(b->max_str, sizeof(char));

   if (b->pstr == NULL || b->pval == NULL){
      ERROR("String allocation memory failure\n");
   }  

   strncpy(b->pstr, n, strlen(n));
   strncpy(b->pval, v, strlen(v));
}

void insert_node(Bode* r, Bode* n)
{
   /* recursively find correct location and add node there */
   int compare; 

   if ( r == NULL ) {return;}

   compare = strcmp(n->pstr, r->pstr);
   if ( compare == 0 ){    /*don't add duplicates*/
      if (n != NULL) {
         free(n->pstr);
         free(n->pval);
         free(n); 
      }
      return; 
   }

   if ( compare < 0 ){

      go_left(r, n); 

   } else { 

     go_right(r, n);              

   }
} 

void go_left(Bode* r, Bode* n)
{
   if (r->left == NULL){
      /*add node to r left*/
      r->left = n;
   } else {
      insert_node(r->left, n);
   }
}

void go_right(Bode* r, Bode* n)
{
   if (r->right == NULL){
      /*add node to r right*/
      r->right = n;
   } else {
      insert_node(r->right, n); 
   }
}

void print_tree(Bode* n) 
{
   if ( n == NULL ) {
      return; 
   } else {
      printf("\n%s = %s", n->pstr, n->pval);
      print_tree(n->left);
      print_tree(n->right); 
   }
   printf("\n");
}
