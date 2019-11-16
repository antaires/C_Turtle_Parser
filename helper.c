# include "interp.h"

/*-----------------------------------------*
 *          HELPER FUNCTIONS  (INTERP)      *
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
      ERROR("Missing textfile, nothing to interpret");
   }
}

