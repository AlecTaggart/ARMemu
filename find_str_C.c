#include <stdio.h>
#include <string.h>

int find_str_C(char src[], char sub[]){
   int i;
   for (i = 0; i < strlen(src); ++i){
      if(src[i] == sub[0]){
         int j;
         for (j = 1; j < strlen(sub); ++j){
            if(src[i +j] == sub[j]){
               if(j == strlen(sub)-1){
                  return i;
               }
            }else{
               break;
            }
         }
      }
   }return -1;

}