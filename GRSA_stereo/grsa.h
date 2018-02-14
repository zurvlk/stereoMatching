#include "graph.h"
#include "bmp.h"

#define _CONSIDE_ALL_PAIRS_ 1

typedef struct __INPUT_BITMAPS__ {
    img *raw_left;
    img *raw_right;
    img *truth;
    img *output;
    int *left;
    int *right;
    int width;
    int height;
    int label_max;
    int scale;
}Image;

typedef struct __SUBMODULAR_SUBSETS__ {
    int **ls;
    int **pairs;
    int number;
    int T;
}Subsets;

int function;
int dterm;
int vterm;
int harf;
int la;
int segm;
int flag_infty;
int subset;
int allpairs;

void readStrBitmap(Image *image, char filename[], int scale);
int gen_submodular_subsets(int label_size, int range_size, Subsets *ss);
int nc2(int n);
double theta(double n, double T);
double p(int *label, int height, int width);
double energy(Graph *G, int *label, int *I, double T, int lambda);
double energy_str(Graph *G, int *label,  double T, int lambda, int height, int width, int label_max, int *left, int *right, img *raw_left, img *raw_right);
double pairwise(double i, double j, double T, int lambda);
double data(int i, int label);
int set_edge(Graph *G, int *ls, int *label, double T, int lambda, int label_max, int *left, int *right, img *raw_left, img *raw_right);
int is_convex(int i, int j, double T);
int isin_array(int *ls, int target);
double energy_ito(int *x, int max_label, img *tmp_l2, img *tmp_r2, int lambda, int T);
int calwidth(int width, int node);

int make_label_index(Graph *G, int *label, int *label_index, int alpha, int beta);
void set_single_edges(Graph *G, int height, int width);
int cmparray(int *array1, int *array2, int size);
void cpyarray(int *terget, int *source, int size);
double err_rate(img *output, img *truth, int scale);
void set_capacity_abswap(Graph *G, int *label, int alpha, int beta, double T, int lambda, img *left, img *right, int label_max);
void set_abswap_edge(Graph *G, int height, int width);
