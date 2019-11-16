/*_______________________________________
 |                                       |
 |      BST - SYMBOL TABLE HEADER        |
 |_______________________________________|
 |---------------------------------------|*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR(PHRASE) {fprintf(stderr, "ERROR %s occured in %s, line %d\n", PHRASE, __FILE__, __LINE__); exit(2); }

enum _bool {false, true};
typedef enum _bool bool; 

typedef struct _bode {
   char *pstr; 
   char *pval;
   struct _bode* left;
   struct _bode* right; 
   int max_str; 
} Bode;

struct _st {
   Bode* root;
   int max_str;   
   int num_nodes;
};
typedef struct _st Symtab; 

/*Create empty symtab*/
Symtab* init_symtab(int size); 

/* Add one element into the symtab */
void symtab_insert(Symtab* s, char* n, char* v);

/* Returns true if n is in the array, false elsewise */
bool symtab_isin(Symtab* s, char* n);

/* Adds new element or updates value if already in tree*/
void symtab_update(Symtab* s, char* n, char* v);

/* Update value of variable in symtab*/
void set_varval(Bode* b, char* n, char* v);

/* Return value of variable in symtab*/
void get_varval(Bode* b, char* n, char* val); 

/* Finish up */
/* Clears all space used, and sets pointer to NULL */
void free_symtab(Symtab** s);

void print_tree(Bode* n);
