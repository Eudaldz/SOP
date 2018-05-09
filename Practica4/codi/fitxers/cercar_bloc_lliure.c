p_meta_dades cercar_bloc_lliure(size_t mida) {
  p_meta_dades current = primer_element;

  while (current && !(current->disponible && current->mida >= mida)) 
    current = current->seguent;

  return current;
}
