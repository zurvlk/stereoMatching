#ifndef __GRAPH__INCLUDED__
#define __GRAPH__INCLUDED__

typedef struct __EDGE_CAPACITY_FOR_QSORT__STRUCTURE__
{
	int arc;
	double capa;
}
ECapa;

typedef struct __EDGE_FOR_QSORT__STRUCTURE__
{
	int tail, head;
	double capa;
}
Edge;

typedef struct __GRAPH_STRUCTURE__
{
	int n;         /* グラフの点の数 */
	int nmax;
	int m;         /* グラフの枝の数 */
	int mmax;
	int src;
	int sink;
	/* 枝の情報 */
	int *tail;     /* tail[i]はi番目の枝の出る点 */
	int *head;     /* head[i]はi番目の枝の入る点 */
	double *capa;  /* capa[i]はi番目の枝の容量 */
	/* 点から出る枝の情報 */
	int *first;    /* first[i]は点iから出る最初の枝 */
	int *next;     /* next[i]は枝iの次に点tail[i]から出る枝 */
	int *prev;      /* prev[i]はnext[prev[i]]=iを表す(prev[i]=0ならfirst[tail[i]]=i) */
	/* 点に入る枝の情報 */
	int *revf;     /* 逆向き枝用first */
	int *revn;     /* 逆向き枝用next */
	int *revp;     /* 逆向き枝用ptr */
}
Graph;

void newGraph(Graph *G, int n, int m);
void delGraph(Graph *G);
void setEdge(Graph *G, int index, int tail, int head, double capa);
void setVNum(Graph *G, int n);
void setENum(Graph *G, int m);
void setSource(Graph *G, int node);
void setSink(Graph *G, int node);
void appendAdjEdge(Graph *G, int edge);
void deleteAdjEdge(Graph *G, int edge);
void initAdjList(Graph *G);
void newRevList(Graph *G);
void delRevList(Graph *G);
void appendRevEdge(Graph *G, int edge);
void deleteRevEdge(Graph *G, int edge);
void initRevList(Graph *G);
void showGraph(Graph *G);
void showAdjList(Graph *G);
void showRevList(Graph *G);
void readGraph(char filename[], Graph *G);
void dfs(Graph *G, int label[], int T[]);
void postOrder(Graph *G, int label[], int T[]);
void bfs(Graph *G, int label[], int T[]);
void bfsr(Graph *G, int label[], int T[]);

void initEdgeOrder(Graph *G, ECapa **E);
int compECapaASC(const void *a, const void *b);
void sortEdgeOrderASC(ECapa E[], int m);
int compECapaDSC(const void *a, const void *b);
void sortEdgeOrderDSC(ECapa E[], int m);
void showEdgeOrder(ECapa E[], int m);

int compEdgeASC(const void *a, const void *b);
int compEdgeDSC(const void *a, const void *b);

#endif


