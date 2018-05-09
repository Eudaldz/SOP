#define MAGIC     0x01102017

p_meta_dades demanar_espai(size_t mida) 
{
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
