#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "struct.h"

p_meta_dades primer_element = NULL;
p_meta_dades darrer_element = NULL;

#define ALIGN8(x) (((((x)-1)>>3)<<3)+8)
#define MAGIC     0x01102017

void *malloc(size_t mida);

p_meta_dades cercar_bloc_lliure(size_t mida) {
  p_meta_dades current = primer_element;

  while (current && !(current->disponible && current->mida >= mida)) 
    current = current->seguent;

  return current;
}

p_meta_dades demanar_espai(size_t mida) {
  p_meta_dades meta_dades;

  meta_dades = (void *) sbrk(0);

  if (sbrk(MIDA_META_DADES + mida) == (void *) -1)
    return (NULL);

  meta_dades->mida = mida;
  meta_dades->disponible = 0;
  meta_dades->magic = MAGIC;
  meta_dades->seguent = NULL;

  if (darrer_element)
    darrer_element->seguent = meta_dades;

  darrer_element = meta_dades;

  return meta_dades;
}

void *malloc(size_t mida) {
  void *p;
  p_meta_dades meta_dades;

  if (mida <= 0) {
    return NULL;
  }

  mida = ALIGN8(mida);
  fprintf(stderr, "Malloc %zu bytes\n", mida);

  if (!primer_element) // Es el primer cop que es crida a malloc?
  {
    meta_dades = demanar_espai(mida);
    if (!meta_dades)
      return(NULL);
    primer_element = meta_dades;
  }
  else {  // Hem cridat abans al malloc
    meta_dades = cercar_bloc_lliure(mida);
    if (meta_dades) { // meta_dades trobat 
      meta_dades->disponible = 0;
    } else {     // no s'ha trobat meta_dades 
      meta_dades = demanar_espai(mida);
      if (!meta_dades)
        return (NULL);
    }
  } 

  p = (void *) meta_dades;

  // Es retorna a l'usuari el punter a l'espai
  // de memoria que pot fer servir per a les dades

  return (p + MIDA_META_DADES); 
}

/*
 * Punt1
 */
void free(void* ptr) {
    printf("FREE CALL: s'ha cridat a free. \n");//write(1, "Malloc\n", 7)
    if(ptr==NULL){
        exit;
    }else{
        p_meta_dades meta_dades = ptr - MIDA_META_DADES;
        if(meta_dades->magic == MAGIC){
            if(meta_dades->disponible == 0){
                meta_dades->disponible = 1;
            }else{
                printf("FREE ERROR: aquest bloc ja estava disponible.\n");
            }
        }else{
            printf("FREE ERROR: magic no coincideix.\n");
        }
    }
}

/*
 * Punt 3
 */
void *calloc(size_t nelem, size_t elsize){
    //printf("CALLOC CALL: s'ha cridat a calloc. \n");
}

/*
 * Punt 4: te errors
 */
void *realloc(void *ptr, size_t mida){
    printf("REALLOC CALL: s'ha cridat a realloc. \n");
    if(ptr = NULL){//a)
        return malloc(mida);
    }else{
        printf("Cal fer coses.\n");
        p_meta_dades meta_dades = ptr - MIDA_META_DADES;
        meta_dades->mida;
        printf("S'ha obtingut el bloc de meta_dades\n");
        if(meta_dades->mida <= mida){//b)
            return ptr;
        }else{//c)
            printf("He fet la comptaracio");
            void *ptr_aux = malloc(mida);
            printf("Em fet el malloc que toca");
            memcpy(ptr_aux, ptr, meta_dades->mida);
            printf("Em fet la copia :)");
            free(ptr);
            return ptr_aux;
        }
    }
}
