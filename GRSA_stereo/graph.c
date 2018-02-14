#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#define INF DBL_MAX
#include "graph.h"

/* グラフ構造体が利用する配列のメモリを確保する関数 */
void newGraph(Graph *G, int n, int m)
{
	if (n < 1)
	{
		fprintf(stderr, "newGraph(): ERROR [2nd argument]\n");
		exit(EXIT_FAILURE);
	}
	if (m < 0)
	{
		fprintf(stderr, "newGraph(): ERROR [3rd argument]\n");
		exit(EXIT_FAILURE);
	}

	G->nmax = n; G->mmax = m;
	G->n = n++; G->m = m++;
	G->src = 0; G->sink = 0;

	G->tail = (int *) malloc(sizeof(int) * m);
	if (G->tail == NULL)
	{
		fprintf(stderr, "newGraph(): ERROR [malloc(G->tail)]\n");
		exit(EXIT_FAILURE);
	}
	G->head = (int *) malloc(sizeof(int) * m);
	if (G->head == NULL)
	{
		fprintf(stderr, "newGraph(): ERROR [malloc(G->head)]\n");
		exit(EXIT_FAILURE);
	}
	G->capa = (double *) malloc(sizeof(double) * m);
	if (G->capa == NULL)
	{
		fprintf(stderr, "newGraph(): ERROR [malloc(G->capa)]\n");
		exit(EXIT_FAILURE);
	}
	G->first = (int *) malloc(sizeof(int) * n);
	if (G->first == NULL)
	{
		fprintf(stderr, "newGraph(): ERROR [malloc(G->first)]\n");
		exit(EXIT_FAILURE);
	}
	G->next = (int *) malloc(sizeof(int) * m);
	if (G->next == NULL)
	{
		fprintf(stderr, "newGraph(): ERROR [malloc(G->next)]\n");
		exit(EXIT_FAILURE);
	}
	G->prev = (int *) malloc(sizeof(int) * m);
	if (G->prev == NULL)
	{
		fprintf(stderr, "newGraph(): ERROR [malloc(G->prev)]\n");
		exit(EXIT_FAILURE);
	}

	G->revf = NULL; G->revn = NULL; G->revp = NULL;
}

/* グラフ構造体が利用している配列のメモリを解放する関数 */
void delGraph(Graph *G)
{
	if (G->tail != NULL) { free(G->tail); }
	if (G->head != NULL) { free(G->head); }
	if (G->capa != NULL) { free(G->capa); }
	if (G->first != NULL) { free(G->first); }
	if (G->next != NULL) { free(G->next); }
	if (G->prev != NULL) { free(G->prev); }
	if (G->revf != NULL) { free(G->revf); }
	if (G->revn != NULL) { free(G->revn); }
	if (G->revp != NULL) { free(G->revp); }
}

/* 指定した番号の枝の情報をセットする関数 */
void setEdge(Graph *G, int index, int tail, int head, double capa)
{
	if ((index < 1) || (index > G->m))
	{
		fprintf(stderr, "setEdge(): ERROR [2nd argument]\n");
		printf("index: %d\n", index);
		exit(1);
	}

	G->tail[index] = tail;
	G->head[index] = head;
	G->capa[index] = capa;
}

/* 点の数を決め直す関数 */
void setVNum(Graph *G, int n)
{
	if ((n < 1) || (n > G->nmax))
	{
		fprintf(stderr, "setVNum(): ERROR [2nd arg]\n");
		exit(EXIT_FAILURE);
	}
	G->n = n;
}

/* 枝の数を決め直す関数 */
void setENum(Graph *G, int m)
{
	if ((m < 0) || (m > G->mmax))
	{
		fprintf(stderr, "setENum(): ERROR [2nd arg]\n");
		exit(EXIT_FAILURE);
	}
	G->m = m;
}

/* ソースをどの点にするか決める関数 */
void setSource(Graph *G, int node)
{
	if ((node < 0) || (node > G->n))
	{
		fprintf(stderr, "setSource(): ERROR [2nd argument]\n");
		exit(EXIT_FAILURE);
	}

	G->src = node;
}

/* シンクをどの点にするか決める関数 */
void setSink(Graph *G, int node)
{
	if ((node < 0) || (node > G->n))
	{
		fprintf(stderr, "setSink(): ERROR [2nd argument]\n");
		exit(EXIT_FAILURE);
	}

	G->sink = node;
}

/* Adjacency Listに指定した番号の枝を追加する関数 */
void appendAdjEdge(Graph *G, int edge)
{
	int tail;

	if ((edge < 1) || (edge > G->m))
	{
		fprintf(stderr, "appendAdjEdge(): ERROR [2nd argument]\n");
		exit(EXIT_FAILURE);
	}
	if (G->prev[edge] != -1)
	{
		fprintf(stderr, "appendAdjEdge(): ERROR [edge already in]\n");
		printf("edge: %d\n", edge);
		exit(EXIT_FAILURE);
	}

	tail = G->tail[edge];
	G->prev[edge] = 0;
	/* 実はif付けなくてもイける */
	if (G->first[tail]) {
		G->prev[G->first[tail]] = edge;
	}
	G->next[edge] = G->first[tail];
	G->first[tail] = edge;
}

/* Adjacency Listから指定した番号の枝を削除する関数 */
void deleteAdjEdge(Graph *G, int edge)
{
	int tail;

	if ((edge < 1) || (edge > G->m))
	{
		fprintf(stderr, "deleteAdjEdge(): ERROR [2nd argument]\n");
		exit(EXIT_FAILURE);
	}
	if (G->prev[edge] == -1)
	{
		fprintf(stderr, "deleteAdjEdge(): ERROR [edge not in]\n");
		exit(EXIT_FAILURE);
	}

	tail = G->tail[edge];
	if (!G->prev[edge])
	{
		G->first[tail] = G->next[edge];
	}
	else
	{
		G->next[G->prev[edge]] = G->next[edge];
	}
	/* 実はif付けなくてもイける */
	if (G->next[edge]) {
		G->prev[G->next[edge]] = G->prev[edge];
	}
	G->prev[edge] = -1;
}

/* 点から出る枝の情報を作成する関数 */
void initAdjList(Graph *G)
{
	int i;

	for (i = 1; i <= G->n; i++) { G->first[i] = 0; }
	for (i = 1; i <= G->m; i++) { /* G->next[i] = 0; */ G->prev[i] = -1; }

	for (i = G->m; i > 0; i--) { appendAdjEdge(G, i); }
}

/* 枝を逆向きに辿るために必要な配列用のメモリを確保する関数 */
void newRevList(Graph *G)
{
	G->revf = (int *) malloc(sizeof(int) * (G->n + 1));
	if (G->revf == NULL)
	{
		fprintf(stderr, "newRevList(): ERROR [malloc(G->revf)]\n");
		exit(EXIT_FAILURE);
	}
	G->revn = (int *) malloc(sizeof(int) * (G->m + 1));
	if (G->revn == NULL)
	{
		fprintf(stderr, "newRevList(): ERROR [malloc(G->revn)]\n");
		exit(EXIT_FAILURE);
	}
	G->revp = (int *) malloc(sizeof(int) * (G->m + 1));
	if (G->revp == NULL)
	{
		fprintf(stderr, "newRevList(): ERROR [malloc(G->revp)]\n");
		exit(EXIT_FAILURE);
	}
}

void delRevList(Graph *G)
{
	if (G->revf != NULL) { free(G->revf); G->revf = NULL; }
	if (G->revn != NULL) { free(G->revn); G->revn = NULL; }
	if (G->revp != NULL) { free(G->revp); G->revp = NULL; }
}

/* appendAdjEdge()の逆向き版 */
void appendRevEdge(Graph *G, int edge)
{
	int head;

	if ((edge < 1) || (edge > G->m))
	{
		fprintf(stderr, "appendRevEdge(): ERROR [2nd argument]\n");
		exit(EXIT_FAILURE);
	}
	if (G->revp[edge] != -1)
	{
		fprintf(stderr, "appendRevEdge(): ERROR [edge already in]\n");
		exit(EXIT_FAILURE);
	}

	head = G->head[edge];
	G->revp[edge] = 0;
	/* 実はif付けなくてもイける */
	if (G->revf[head]) {
		G->revp[G->revf[head]] = edge;
	}
	G->revn[edge] = G->revf[head];
	G->revf[head] = edge;
}

/* deleteAdjEdge()の逆向き版 */
void deleteRevEdge(Graph *G, int edge)
{
	int head;

	if ((edge < 1) || (edge > G->m))
	{
		fprintf(stderr, "deleteRevEdge(): ERROR [2nd argument]\n");
		exit(EXIT_FAILURE);
	}
	if (G->revp[edge] == -1)
	{
		fprintf(stderr, "deleteRevEdge(): ERROR [edge not in]\n");
		exit(EXIT_FAILURE);
	}

	head = G->head[edge];
	if (!G->revp[edge])
	{
		G->revf[head] = G->revn[edge];
	}
	else
	{
		G->revn[G->revp[edge]] = G->revn[edge];
	}
	/* 実はif付けなくてもイける */
	if (G->revn[edge]) {
		G->revp[G->revn[edge]] = G->revp[edge];
	}
	G->revp[edge] = -1;
}

/* initAdjList()の逆向き版 */
void initRevList(Graph *G)
{
	int i;

	for (i = 1; i <= G->n; i++) { G->revf[i] = 0; }
	for (i = 1; i <= G->m; i++) { /* G->revn[i] = 0; */ G->revp[i] = -1; }

	for (i = 1; i <= G->m; i++) { appendRevEdge(G, i); }
}

/* グラフの情報を表示する関数 */
void showGraph(Graph *G)
{
	int i;

	printf("Number of vertices: %d\n", G->n);
	printf("Number of edges: %d\n", G->m);

	puts("----------   Edges   --------");
	puts("   No.            capacity   ");
	//puts("------+---------+-----------+");
	for (i = 1; i <= G->m; i++)
	{
		printf("%5d: (%3d,%3d), ",//%4.1lf\n",
		       i, G->tail[i], G->head[i]);
		if (G->capa[i] != INF) printf("%lf\n", G->capa[i]);
	   	else printf("∞\n");
	}

	puts("----   Source & Sink   ----");
	printf("source: %d\nsink: %d\n", G->src, G->sink);
}

/* Adjacency Listの詳細を表示する関数 */
void showAdjList(Graph *G)
{
	int i, j;

	puts(" ---------   Adjacency List   ----------");
	puts("   No.");
	for (i = 1; i <= G->n; i++)
	{
		printf("%5d: ", i);
		for (j = G->first[i]; j; j = G->next[j])
		{
			printf("%d[%d] -> ", j, G->prev[j]);
		}
		puts("X");
	}
}

/* showAdjList()の逆向き版 */
void showRevList(Graph *G)
{
	int i, j;

	puts(" ---------   Inverse Adjacency List   ----------");
	puts("   No.");
	for (i = 1; i <= G->n; i++)
	{
		printf("%5d: ", i);
		for (j = G->revf[i]; j; j = G->revn[j])
		{
			printf("%d[%d] -> ", j, G->revp[j]);
		}
		puts("X");
	}
}

/* ファイルからグラフの情報を読み取るための関数 */
void readGraph(char filename[], Graph *G)
{
	FILE *fp;
	char buf[100];
	int n, m;
	int i, tail, head;
	double capa;
	char c;
	int v;

	fp = fopen(filename, "rt");
	if (fp == NULL)
	{
		perror("File Read Error");
		exit(EXIT_FAILURE);
	}

	G->n = 0; G->m = 0; i = 0;
	while (fgets(buf, sizeof(buf), fp) != NULL)
	{
		switch (buf[0])
		{
		case 'p':
			sscanf(buf, "p max %d %d\n", &n, &m);
			i = 1; newGraph(G, n, m);
			break;
		case 'a':
			if (!i)
			{
				fclose(fp);
				fprintf(stderr, "readGraph(): ERROR [file form]\n");
				exit(EXIT_FAILURE);
			}
			sscanf(buf, "a %d %d %lf\n", &tail, &head, &capa);
			setEdge(G, i++, tail, head, capa);
			break;
		case 'n':
			if (!i)
			{
				fclose(fp);
				fprintf(stderr, "readGraph(): ERROR [file form]\n");
				exit(EXIT_FAILURE);
			}
			sscanf(buf, "n %d %c\n", &v, &c);
			if (c == 's')
			{
				setSource(G, v);
			}
			else if (c == 't')
			{
				setSink(G, v);
			}
			break;
		}
	}

	fclose(fp);
	if (i <= m)
	{
		fprintf(stderr, "readGraph(): ERROR [incorrect data]\n");
		exit(EXIT_FAILURE);
	}
}

/* [以降3つの関数について] 順番が必要ないときはnumを使わず、数字の1にする */

/* 深さ優先探索(先行順)のアルゴリズム */
void dfs(Graph *G, int label[], int T[])
{
	int i, num;
	int edge, tail, head;
	int *current;

	if ((G->src < 1) || (G->src > G->n))
	{
		fprintf(stderr, "dfs(): ERROR [G->src]\n");
		exit(EXIT_FAILURE);
	}

	current = (int *) malloc(sizeof(int) * (G->n + 1));
	if (current == NULL)
	{
		fprintf(stderr, "dfs(): ERROR [malloc]\n");
		exit(EXIT_FAILURE);
	}

	for (i = 1; i <= G->n; i++)
	{
		label[i] = 0; T[i] = 0;
		current[i] = G->first[i];
	}

	label[G->src] = 1; num = 2;
	tail = G->src; edge = current[tail];
	while (T[tail] || edge)
	{
		if (!edge)
		{
			tail = G->tail[T[tail]];
		}
		else
		{
			head = G->head[edge];
			current[tail] = G->next[edge];
			if (!label[head])
			{
				label[head] = num++; T[head] = edge;
				tail = head;
			}
		}
		edge = current[tail];
	}

	free(current);
}

/* 深さ優先探索(後行順)のアルゴリズム */
void postOrder(Graph *G, int label[], int T[])
{
	int i, num;
	int edge, tail, head;
	int *current;

	if ((G->src < 1) || (G->src > G->n))
	{
		fprintf(stderr, "dfs(): ERROR [G->src]\n");
		exit(EXIT_FAILURE);
	}

	current = (int *) malloc(sizeof(int) * (G->n + 1));
	if (current == NULL)
	{
		fprintf(stderr, "postOrder(): ERROR [malloc]\n");
		exit(EXIT_FAILURE);
	}

	for (i = 1; i <= G->n; i++)
	{
		label[i] = 0; T[i] = 0;
		current[i] = G->first[i];
	}

	label[G->src] = 1; num = 1;
	tail = G->src; edge = current[tail];
	while (T[tail] || edge)
	{
		if (!edge)
		{
			label[tail] = num++; tail = G->tail[T[tail]];
		}
		else
		{
			head = G->head[edge];
			current[tail] = G->next[edge];
			if (!label[head])
			{
				T[head] = edge; label[head] = 1;
				tail = head;
			}
		}
		edge = current[tail];
	}
	label[tail] = num;
}

/* 幅優先探索のアルゴリズム */
void bfs(Graph *G, int label[], int T[])
{
	int i, num;
	int edge, tail, head;
	int ins;
	int *nextnode;

	if ((G->src < 1) || (G->src > G->n))
	{
		fprintf(stderr, "dfs(): ERROR [G->src]\n");
		exit(EXIT_FAILURE);
	}

	nextnode = (int *) malloc(sizeof(int) * (G->n + 1));
	if (nextnode == NULL)
	{
		fprintf(stderr, "bfs(): ERROR [malloc]\n");
		exit(EXIT_FAILURE);
	}

	for (i = 1; i <= G->n; i++) { label[i] = 0; T[i] = 0; }

	label[G->src] = 1; num = 2;
	nextnode[ins = G->src] = 0;
	for (tail = G->src; tail; tail = nextnode[tail])
	{
		for (edge = G->first[tail]; edge; edge = G->next[edge])
		{
			head = G->head[edge];
			if (!label[head])
			{
				label[head] = num++; T[head] = edge;
				//if (head == G->sink) { return; }
				nextnode[ins] = head; nextnode[ins = head] = 0;
			}
		}
	}

	free(nextnode);
}

 /* t側からの幅優先探索のアルゴリズム */
void bfsr(Graph *G, int label[], int T[])
{
	int i, num;
	int edge, tail, head;
	int ins;
	int *nextnode;

	if ((G->sink < 1) || (G->sink > G->n))
	{
		fprintf(stderr, "dfs(): ERROR [G->src]\n");
		exit(EXIT_FAILURE);
	}

	nextnode = (int *) malloc(sizeof(int) * (G->n + 1));
	if (nextnode == NULL)
	{
		fprintf(stderr, "bfs(): ERROR [malloc]\n");
		exit(EXIT_FAILURE);
	}

	for (i = 1; i <= G->n; i++) { label[i] = 0; T[i] = 0; }

	label[G->sink] = 1; num = 2;
	nextnode[ins = G->sink] = 0;
	for (head = G->sink; head; head = nextnode[head])
	{
		for (edge = G->revf[head]; edge; edge = G->revn[edge])
		{
			tail = G->tail[edge];
			if (!label[tail])
			{
				label[tail] = num++; T[tail] = edge;
				//if (head == G->sink) { return; }
				nextnode[ins] = tail; nextnode[ins = tail] = 0;
			}
		}
	}

	free(nextnode);
}

/* Graph構造体からECapa構造体の配列を作成する関数 */
void initEdgeOrder(Graph *G, ECapa **E)
{
	int i;

	if ((G->n < 1) || (G->m < 0))
	{
		fprintf(stderr, "initEdgeStruct(): ERROR [1st arg]\n");
		exit(EXIT_FAILURE);
	}

	*E = (ECapa *) malloc(sizeof(ECapa) * (G->m + 1));
	if (*E == NULL)
	{
		fprintf(stderr, "initEdgeStruct(): ERROR [malloc(*E)]\n");
		exit(EXIT_FAILURE);
	}
	for (i = 1; i <= G->m; i++)
	{
		(*E)[i].arc = i; (*E)[i].capa = G->capa[i];
	}
}

/* qsort()用のECapa型比較関数(昇順) */
int compECapaASC(const void *a, const void *b)
{
	if (((ECapa *) a)->capa < ((ECapa *) b)->capa)      { return (-1); }
	else if (((ECapa *) a)->capa > ((ECapa *) b)->capa) { return (1); }
	else                                                { return (0); }
}

/* ECapa構造体の配列を昇順にソートする関数 */
void sortEdgeOrderASC(ECapa E[], int m)
{
	qsort(E + 1, m, sizeof(ECapa), compECapaASC);
}

/* qsort()用のECapa型比較関数(降順) */
int compECapaDSC(const void *a, const void *b)
{
	if (((ECapa *) a)->capa > ((ECapa *) b)->capa)      { return (-1); }
	else if (((ECapa *) a)->capa < ((ECapa *) b)->capa) { return (1); }
	else                                                { return (0); }
}

/* ECapa構造体の配列を降順にソートする関数 */
void sortEdgeOrderDSC(ECapa E[], int m)
{
	qsort(E + 1, m, sizeof(ECapa), compECapaDSC);
}

/* ECapa構造体の中身を確認する関数 */
void showEdgeOrder(ECapa E[], int m)
{
	int i;

	for (i = 1; i <= m; i++)
	{
		printf("[%d]: %lf\n", E[i].arc, E[i].capa);
	}
}


/* Edge構造体の配列をqsort()するための比較関数(昇順) */
int compEdgeASC(const void *a, const void *b)
{
	if (((Edge *) a)->capa < ((Edge *) b)->capa)      { return (-1); }
	else if (((Edge *) a)->capa > ((Edge *) b)->capa) { return (1); }
	else                                              { return (0); }
}

/* Edge構造体の配列をqsort()するための比較関数(降順) */
int compEdgeDSC(const void *a, const void *b)
{
	if (((Edge *) a)->capa > ((Edge *) b)->capa)      { return (-1); }
	else if (((Edge *) a)->capa < ((Edge *) b)->capa) { return (1); }
	else                                              { return (0); }
}
