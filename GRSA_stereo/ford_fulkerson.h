#include "graph.h"

#ifndef __FORD_FULKERSON__INCLUDED__
#define __FORD_FULKERSON__INCLUDED__

double edmonds_karp(Graph G, double f[], int label[]);
double ek_dual(Graph G, double f[], int label[]);
double boykov_kolmogorov(Graph G, double f[], int t[]);
double bk_single(Graph G, double f[], int tree[]);

#endif


