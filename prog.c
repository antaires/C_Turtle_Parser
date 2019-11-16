#include "interp.h"

/*-----------------------------------------*
 |      PROG STRUCT FUNCTIONS INTERP       |
 *-----------------------------------------*/

void init_prog(Program *p)
{
   if (p == NULL) {
      ERROR("Init_prog() passed a null pointer");
   }
   p->start = NULL; 
   p->cw = NULL;

   set_color_value(p, SET_BLACK);

   p->t.pos.x = (int) CUSTOM_HEIGHT/2; 
   p->t.pos.y = (int) CUSTOM_WIDTH/2;
   p->t.heading = 0; /*stores degrees*/
   p->t.speed = 0;

   p->test_mode = false; /*test mode returns false for
                           ON_ERROR()               */

   Altered_SDL_Init(&(p->sdl.sw));
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

void next_word(Program *p)
{
   /*points current_word (cw) to next word in program*/
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


void prev_word(Program *p)
{
   /*points current_word (cw) to previous word in program*/
   if (p == NULL){
      ERROR("Prev_word() received a NULL pointer");
   }

   if (p->cw == NULL){
      return;
   }

   p->cw = p->cw->prev;
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

Node* init_token(char* word)
{
   Node* n = init_node();
   n->w = calloc(MAXTOKENSIZE, sizeof(char));

   strcpy(n->w, word); 

   return n; 
}

void free_prog(Program *p)
{
   /*wrapper for frpr to assure current word set to
     start word to prevent memory leaks             */
   set_start(p);
   frpr(p); 

   /*clean up sdl*/
   SDL_RenderClear(p->sdl.sw.renderer); 
   /* Clear up graphics subsystems */
   SDL_Quit();
   atexit(SDL_Quit);
}

void frpr (Program* p)
{
   /*frees program list*/
   /*to use, cw must == start*/
   if (p == NULL){
      return;
   }

   if (get_cn(p) == NULL){
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

void print_prog(Node* n)
{ 
   if (n != NULL){
      printf("\n%s", n->w);
      print_prog(n->next); 
   }
}

/*-----------------------------------------*
 |  Program Funcs relating to: TURTLE/SDL  |
 *-----------------------------------------*/

void draw_line(Program* p, char* v)
{
   int r, x2, y2;
   str_to_int(&r, v); 

   x2 = (int) (r * cos(p->t.heading * M_PI/180.0));
   y2 = (int) (r * sin(p->t.heading * M_PI/180.0));

   /*adjust for current position*/
   x2 += p->t.pos.x;
   y2 += p->t.pos.y; 

   /*SDL*/
   SDL_SetRenderDrawColor(p->sdl.sw.renderer, 
                          p->t.color[red], 
                          p->t.color[green], 
                          p->t.color[blue], 
                          p->t.color[alpha]);

   SDL_RenderDrawLine(p->sdl.sw.renderer, 
                      p->t.pos.x, p->t.pos.y, x2, y2);
   Neill_SDL_UpdateScreen(&(p->sdl.sw));
   delay(p); 

   /*update turtle pos*/
   p->t.pos.x = x2;
   p->t.pos.y = y2;
}

void set_heading(Program* p, char* w)
{
   int degree;

   if ( strsame(w, "UP"))   { degree = 90; }
   if ( strsame(w, "DOWN")) { degree = 270; }
   if ( strsame(w, "LEFT")) { degree = 180; }
   if ( strsame(w, "RIGHT")){ degree = 0; }

   p->t.heading = degree;
}

void set_pos(Program* p, char* vx, char* vy)
{
   int x, y;
   str_to_int(&x, vx);
   str_to_int(&y, vy);

   p->t.pos.x = x;
   p->t.pos.y = y;
}

void set_pos_int(Program* p, int xval, int yval)
{
   p->t.pos.x = xval;
   p->t.pos.y = yval;
}

void draw_rect_int(Program* p, int width, int height, bool fill)
{
   p->sdl.rectangle.w = width;
   p->sdl.rectangle.h = height;

   /*center it*/
   p->sdl.rectangle.x = p->t.pos.x - (width/2); 
   p->sdl.rectangle.y = p->t.pos.y - (height/2);

   if ( fill ) {
      SDL_RenderFillRect(p->sdl.sw.renderer, &(p->sdl.rectangle));
   } else {
      SDL_RenderDrawRect(p->sdl.sw.renderer, &(p->sdl.rectangle));
   } 
   delay(p); 
}

void turn_left(Program* p, char* v)
{
   int degrees;
   str_to_int(&degrees, v);
   degrees *= -1; 
   turn_turtle(p, degrees); 
}

void turn_right(Program* p, char* v)
{
   int degrees;
   str_to_int(&degrees, v);
   turn_turtle(p, degrees); 
}

void turn_turtle(Program* p, int degrees)
{
   p->t.heading += degrees; 
   p->t.heading = p->t.heading % 360;
}

void delay(Program* p)
{
   SDL_Delay(p->t.speed);
}

void copy_arr(int* copy, int* orig)
{
   int i;
   for (i = 0; i < RGBA; i++){
      copy[i] = orig[i];
   }
}

void Altered_SDL_Init(SDL_Simplewin *sw)
{
   /*to allow for custom screen size and window title*/
   if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      ERROR("Unable to initialize SDL");
   } 

   sw->finished = 0;
   sw->win= SDL_CreateWindow("Turtle graphics!",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          CUSTOM_WIDTH, CUSTOM_HEIGHT,
                          SDL_WINDOW_SHOWN);
   if(sw->win == NULL){
      ERROR("Unable to initialize SDL Window");
   }

   sw->renderer = SDL_CreateRenderer(sw->win, -1,
                   SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
   if(sw->renderer == NULL){
      ERROR("Unable to initialize SDL Renderer");
   }
   SDL_SetRenderDrawBlendMode(sw->renderer,SDL_BLENDMODE_BLEND);

   /* Create texture for display */
   sw->display = SDL_CreateTexture(sw->renderer, SDL_PIXELFORMAT_RGBA8888,
                   SDL_TEXTUREACCESS_TARGET, CUSTOM_WIDTH, CUSTOM_HEIGHT);
   if(sw->display == NULL){
      ERROR("Unable to initialize SDL texture");
   }
   SDL_SetRenderTarget(sw->renderer, sw->display);

   /* Clear screen & set draw colour to white */
   SDL_SetRenderDrawColor(sw->renderer, WHITE);
   SDL_RenderClear(sw->renderer);
}


