#include "interp.h"

Node* init_stacknode(char* word);

/*-----------------------------------------*
 *           STACK FUNCTIONS               *
 *-----------------------------------------*/

bool empty(Stack *s)
{
   if (s == NULL) {
      ERROR("empty() passed a null pointer");
   }
   if ( s->start == NULL){ /*changed top to start*/
      return true;
   }
   return false;
}

Node* init_stacknode(char* word)
{
   /*calloc space for node and string*/
   Node* n = init_token(word);
   if (n == NULL || word == NULL){
      free(n); 
      ERROR("Init_stacknode could not init_token");
   }
   return n; 
}

void init_stack(Stack* s)
{
   s->start = NULL;
   s->top = NULL;
}

void push(Stack* s, char* c)
{
   /*get mem for node and word*/
   Node* new = init_stacknode(c); 
 
   if ( s == NULL){
      free(new->w);
      free(new); 
      ERROR("Push() given a null pointer");
   }

   /*if first node to be added to stack*/
   if ( s->start == NULL ){
      s->start = new;
      s->top   = new;
      /*new->prev = NULL; */
      return;
   }
   new->prev = s->top; 
   s->top->next = new;
   s->top = s->top->next; 
}

char* pop(Stack* s)
{
   /*returns NULL pointer if stack empty*/
   char* temp = calloc(MAXTOKENSIZE, sizeof(char)); 

   if ( s == NULL){
      free(temp);
      ERROR("Pop() passed a null pointer");
   }

   if ( s->top == NULL || s->start == NULL ){
      free(temp); 
      return NULL;
   }

   strcpy(temp, s->top->w );  /*get top word */

   if (s->top->prev == NULL ){ /*last node in stack*/
      free(s->top->w); free(s->top);
      s->top = NULL; s->start = NULL;
      return temp;  
   }
   s->top = s->top->prev; 
   free(s->top->next->w); free(s->top->next);
   s->top->next = NULL;
   return temp;   /*return top element */
}

void free_stack(Stack *s)
{
   if (s == NULL){
      return;
   }
   if ( s->start == NULL){
      return;
   }
   if ( s->start->next != NULL){
      s->start = s->start->next; /*advance*/
      free(s->start->prev->w);   /*free prev word*/
      free(s->start->prev);      /*free prev node*/
      s->start->prev = NULL;
      free_stack(s);
   } else {         /*free last remaining node */
      free(s->start->w); 
      free(s->start); 
      s->start = NULL;
      s->top   = NULL;
   }
}

