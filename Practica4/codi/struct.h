#include <stddef.h>
#include <unistd.h>

#define MIDA_META_DADES  sizeof(struct m_meta_dades)
typedef struct m_meta_dades *p_meta_dades;

struct m_meta_dades {
  size_t  mida;
  int     disponible;
  int     magic;
  p_meta_dades seguent;
};
