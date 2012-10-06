
#include <stdio.h>
#include <stdlib.h>

/**
 * Generage permutations for a sequence of lenght 1 < N < 10.
 * usage: permgen <N>
 *
 * algorithm:
 *   http://en.wikipedia.org/wiki/Permutation
 *   'Generation in lexicographic order'
 */
int main(int argn, char** argc)
{
  if(argn != 2)
  {
    printf("usage: permgen <N>\n");
    printf("  N: 1 < N < 10\n");
    return 1;
  }

  char n_ = argc[1][0];
  if(n_ == 0 || argc[1][1] != 0 || !('2' <= n_ && n_ <= '9'))
  {
    printf("usage: permgen <N>\n");
    printf("  N: 1 < N < 10\n");
    return 1;
  }
  int n = argc[1][0] - '0';

  char* v = "123456789";
  v[n] = 0;

  while(1)
  {
    printf("%s\n", v);

    int k = n - 1; while(--k >= 0 && v[k] > v[k + 1]);
    if(k < 0) break;
    int l = n; while(v[k] > v[--l]);

    register char tmp = v[k]; v[k] = v[l]; v[l] = tmp;
    for(k = k + 1, l = n - 1; k < l; k++, l--) { tmp = v[k]; v[k] = v[l]; v[l] = tmp; }
  }

  return 0;
}
