#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "ford_fulkerson.h"
#include "queue_stack.h"

#define INF 1000000.0

void newAuxGraph(Graph G, Graph *Gf, double f[], int inverse[]);

/* 幅優先探索でパスを見つけるアルゴリズム */
double edmonds_karp(Graph G, double f[], int label[])
{
	Graph Gf;
	int *T, *inverse, *nextnode, ins;
	int i, tail, head, arc, rev;
	double flow, maxflow = 0.0;

	initAdjList(&G);
	for (arc = G.first[G.src]; arc; arc = G.next[arc]) { maxflow += f[arc]; }

	/* どの枝から辿られてきたかをメモする配列 */
	T = (int *) malloc((G.n + 1) * sizeof(int));
	if (T == NULL)
	{
		fprintf(stderr, "edmons_karp(): ERROR [T = malloc()]\n");
		exit(EXIT_FAILURE);
	}
	/* 次に調べるべき点をメモしておける配列(Queueの代わり) */
	nextnode = (int *) malloc((G.n + 1) * sizeof(int));
	if (nextnode == NULL)
	{
		fprintf(stderr, "edmons_karp(): ERROR [nextnode = malloc()]\n");
		exit(EXIT_FAILURE);
	}
	/* 逆向き枝の番号をメモしておく配列 */
	inverse = (int *) malloc(((G.m << 1) + 1) * sizeof(int));
	if (inverse == NULL)
	{
		fprintf(stderr, "edmons_karp(): ERROR [inverse = malloc()]\n");
		exit(EXIT_FAILURE);
	}

	newAuxGraph(G, &Gf, f, inverse);

	for (; ; )
	{
		/* 幅優先探索でパスを見つける */
		for (i = G.n; i; i--) { label[i] = 0; T[i] = 0; }
		tail = Gf.src; label[tail] = 1;
		nextnode[tail] = 0; ins = tail;
		for (; tail; tail = nextnode[tail])
		{
			for (arc = Gf.first[tail]; arc; arc = Gf.next[arc])
			{
				head = Gf.head[arc];
				if (!label[head])
				{
					T[head] = arc;
					if (head == Gf.sink) { goto FOUND_PATH; }
					label[head] = 1;
					nextnode[ins] = head;
					nextnode[ins = head] = 0;
				}
			}
		}
		break;

	/* パスが見つかったので,フローを流す */
	FOUND_PATH:
		/* 流すフローの量を決める */
		flow = INF;
		for (arc = T[Gf.sink]; arc; arc = T[Gf.tail[arc]])
		{
			if (Gf.capa[arc] < flow) { flow = Gf.capa[arc]; }
		}
		maxflow += flow;

		/* フローを流す */
		for (arc = T[Gf.sink]; arc; arc = T[Gf.tail[arc]])
		{
			Gf.capa[arc] -= flow;
			if (Gf.capa[arc] == 0.0) { deleteAdjEdge(&Gf, arc); }

			rev = inverse[arc];
			/* ソースに入る枝とシンクから出る枝は無視 */
			if ((Gf.capa[rev] == 0.0)
			    && (Gf.head[rev] != Gf.src) && (Gf.tail[rev] != Gf.sink))
			{
				appendAdjEdge(&Gf, rev);
			}
			Gf.capa[rev] += flow;
		}
	}

	free(T); free(nextnode);
	for (i = G.m; i; i--) { f[i] = Gf.capa[inverse[i]]; }
	delGraph(&Gf); free(inverse);

	return (maxflow);
}

double ek_dual(Graph G, double f[], int label[])
{
	Graph Gf;
	int i, v, w, arc, rev;
	int *inv, *T, *queue, ins;
	double flow, maxflow = 0.0;
	int *first, *next, *to, which;

	initAdjList(&G);
	for (arc = G.first[G.src]; arc; arc = G.next[arc]) { maxflow += f[arc]; }

	inv = (int *) malloc(((G.m << 1) + 1) * sizeof(int));
	if (inv == NULL)
	{
		fprintf(stderr, "ek_dual(): ERROR [inv = malloc()]\n");
		exit(EXIT_FAILURE);
	}
	T = (int *) malloc((G.n + 1) * sizeof(int));
	if (T == NULL)
	{
		fprintf(stderr, "ek_dual(): ERROR [T = malloc()]\n");
		exit(EXIT_FAILURE);
	}
	queue = (int *) malloc((G.n + 1) * sizeof(int));
	if (queue == NULL)
	{
		fprintf(stderr, "ek_dual(): ERROR [queue = malloc()]\n");
		exit(EXIT_FAILURE);
	}

	newAuxGraph(G, &Gf, f, inv);

	for (; ; )
	{
		for (i = Gf.n; i; i--) { label[i] = 0; T[i] = 0; }
		queue[ins = Gf.sink] = 0; queue[v = Gf.src] = ins;
		label[v] = 1; label[ins] = 2;
		which = 1, first = Gf.first, next = Gf.next, to = Gf.head;
		for (; v; v = queue[v])
		{
			if (label[v] != which)
			{
				if (which == 2)
				{
					first = Gf.first; next = Gf.next;
					to = Gf.head; which = 1;
				}
				else
				{
					first = Gf.revf; next = Gf.revn;
					to = Gf.tail; which = 2;
				}
			}
			for (arc = first[v]; arc; arc = next[arc])
			{
				w = to[arc];
				if (!label[w])
				{
					label[w] = which; T[w] = arc;
					queue[ins] = w; queue[ins = w] = 0;
				}
				else if (label[w] != which)
				{
					goto FOUND_PATH;
				}
			}
		}
		break;

	FOUND_PATH:
		if (which == 1) { rev = T[w]; T[w] = arc; arc = rev; }
		while (arc) { rev = T[v = Gf.head[arc]]; T[v] = arc; arc = rev; }

		flow = INF;
		for (arc = T[Gf.sink]; arc; arc = T[Gf.tail[arc]])
		{
			if (flow > Gf.capa[arc]) { flow = Gf.capa[arc]; }
		}
		maxflow += flow;

		for (arc = T[Gf.sink]; arc; arc = T[Gf.tail[arc]])
		{
			Gf.capa[arc] -= flow;
			if (Gf.capa[arc] == 0.0)
			{
				deleteAdjEdge(&Gf, arc); deleteRevEdge(&Gf, arc);
			}
			rev = inv[arc];
			if ((Gf.capa[rev] == 0.0) 
			    && (Gf.head[rev] != Gf.src) && (Gf.tail[rev] != Gf.sink))
			{
				appendAdjEdge(&Gf, rev); appendRevEdge(&Gf, rev);
			}
			Gf.capa[rev] += flow;
		}
	}

	free(queue); free(T);
	for (i = G.m; i; i--) { f[i] = Gf.capa[inv[i]]; }
	free(inv); delGraph(&Gf);
	for (i = G.n; i; i--)
	{
		if (label[i] == 2) { label[i] = 0; }
	}

	return (maxflow);
}

/* kolmogorovのアルゴリズムの木の成長片側バージョン */
double bk_single(Graph G, double f[], int tree[])
{
	int i;
	Graph Gf;
	int *parent, *inverse;
	int *O, O_top, O_ins;
	QStack A;
	int tail, head, arc, rev;
	double flow, maxflow = 0.0;
	int root, edge, node;
	int min_dist, min_arc;
	int *dist, next_dist, *ts, timer;

	initAdjList(&G);
	for (arc = G.first[G.src]; arc; arc = G.next[arc]) { maxflow += f[arc]; }

	/* 自分の親に繋げる枝をメモする配列 */
	parent = (int *) calloc(G.n + 1, sizeof(int));
	if (parent == NULL)
	{
		fprintf(stderr, "bk_single(): ERROR [parent = malloc()]\n");
		exit(EXIT_FAILURE);
	}
	/* 逆向き枝の番号を保存しておける配列 */
	inverse = (int *) malloc(((G.m << 1) + 1) * sizeof(int));
	if (inverse == NULL)
	{
		fprintf(stderr, "bk_single(): ERROR [inverse = malloc()]\n");
		exit(EXIT_FAILURE);
	}
	/* Active */
	newQStack(&A, G.n + 1);
	/* Orphan集合を表す配列(Queue,Stackの機能を実現) */
	O = (int *) malloc((G.n + 1) * sizeof(int));
	if (O == NULL)
	{
		fprintf(stderr, "bk_single(): ERROR [O = malloc()]\n");
		exit(EXIT_FAILURE);
	}
	/* 木の根からの深さを表すための配列 */
	dist = (int *) malloc((G.n + 1) * sizeof(int));
	if (dist == NULL)
	{
		fprintf(stderr, "bk_single(): ERROR [dist = malloc()]\n");
		exit(EXIT_FAILURE);
	}
	/* 各ノードのタイムスタンプを表す配列 */
	ts = (int *) malloc((G.n + 1) * sizeof(int));
	if (ts == NULL)
	{
		fprintf(stderr, "bk_single(): ERROR [ts = malloc()]\n");
		exit(EXIT_FAILURE);
	}

	newAuxGraph(G, &Gf, f, inverse);

	for (i = Gf.n; i; i--) { tree[i] = 0; }
	ts[Gf.src] = timer = 1; dist[Gf.src] = 0;
	tree[Gf.src] = 1;
	initQStack(&A); enqueueQStack(&A, Gf.src);
	for (; ; )
	{
		/* Growth Stage */
		for (; !isEmptyQStack(A); dequeueQStack(&A))
		{
			tail = getQueueNextQStack(A); next_dist = dist[tail] + 1;
			for (arc = Gf.first[tail]; arc; arc = Gf.next[arc])
			{
				head = Gf.head[arc];
				if (!tree[head])
				{
					tree[head] = 1; parent[head] = arc;
					if (head == Gf.sink) { goto FOUND_PATH; }
					enqueueQStack(&A, head);
					/* update "dist" and "ts" */
					dist[head] = next_dist; ts[head] = ts[tail];
				}
				else if ((ts[head] <= ts[tail]) && (dist[head] > dist[tail]))
				{
					parent[head] = arc;
					dist[head] = next_dist; ts[head] = ts[tail];
				}
			}
		}
		break;

	FOUND_PATH:
		/* Augmentation Stage */
		timer++;

		/* 流す流量を決める */
		flow = INF;
		for (arc = parent[Gf.sink]; arc; arc = parent[Gf.tail[arc]])
		{
			if (flow > Gf.capa[arc]) { flow = Gf.capa[arc]; }
		}
		maxflow += flow;

		/* フローを流す */
		head = Gf.sink; O_top = 0;
		for (arc = parent[head]; arc; arc = parent[head = Gf.tail[arc]])
		{
			Gf.capa[arc] -= flow;
			if (Gf.capa[arc] == 0.0)
			{
				deleteAdjEdge(&Gf, arc); deleteRevEdge(&Gf, arc);
				/* シンクはorphanにしない */
				if (head != Gf.sink)
				{
					parent[head] = 0;
					if (!O_top) { O_ins = head; }
					O[head] = O_top; O_top = head;
				}
			}
			rev = inverse[arc];
			if ((Gf.capa[rev] == 0.0) 
			    && (Gf.head[rev] != Gf.src) && (head != Gf.sink))
			{
				appendAdjEdge(&Gf, rev); appendRevEdge(&Gf, rev);
			}
			Gf.capa[rev] += flow;
		}
		// シンクだけは最後に以下の処理が必要
		parent[Gf.sink] = 0; tree[Gf.sink] = 0;

		/* Adoption Stage */
		for (; O_top; O_top = O[O_top])
		{
			head = O_top; min_dist = INT_MAX;
			// 深さが小さい親を選ぶ
			for (arc = Gf.revf[head]; arc; arc = Gf.revn[arc])
			{
				tail = Gf.tail[arc];
				if (tree[tail])
				{
					if (ts[tail] != timer)
					{
						next_dist = 0;
						node = tail; edge = parent[node];
						while (edge && (ts[node] != timer))
						{
							next_dist++;
							node = Gf.tail[edge]; edge = parent[node];
						}
						if ((node == Gf.src) || edge)
						{
							root = node;
							next_dist += dist[root];
							node = tail;
							while (node != root)
							{
								dist[node] = next_dist--;
								ts[node] = timer;
								edge = parent[node];
								node = Gf.tail[edge];
							}
							ts[root] = timer;
						}
					}
					if ((ts[tail] == timer) && (min_dist > dist[tail]))
					{
						min_arc = arc; min_dist = dist[tail];
					}
				}
			}

			/* 新しい親が見つかった */
			if (min_dist < INT_MAX)
			{
				parent[head] = min_arc;
				dist[head] = min_dist + 1; ts[head] = timer;
			}
			/* 親はいなかったのだ... */
			else
			{
				for (arc = Gf.revf[head]; arc; arc = Gf.revn[arc])
				{
					tail = Gf.tail[arc];
					/* Activeに入ってないようならenqueue */
					if (tree[tail] && !inQStack(A, tail))
					{
						enqueueQStack(&A, tail);
					}
				}
				/* 自分の子をOrphanに追加する */
				tail = head;
				for (arc = Gf.first[tail]; arc; arc = Gf.next[arc])
				{
					head = Gf.head[arc];
					if (parent[head] == arc)
					{
						O[O_ins] = head; O[O_ins = head] = 0;
						parent[head] = 0;
					}
				}
				tree[tail] = 0;
				/* Activeから削除 */
				if (inQStack(A, tail)) { pruneDataQStack(&A, tail); }
			}
		}
	}

	free(ts); free(dist); free(parent);
	free(O); delQStack(&A);
	for (i = G.m; i; i--) { f[i] = Gf.capa[inverse[i]]; }
	delGraph(&Gf); free(inverse);

	return (maxflow);
}

/* kolmogorovのアルゴリズムの木の成長両側バージョン */

double boykov_kolmogorov(Graph G, double f[], int t[])
{
	Graph Gf;
	int *p, *inverse;
	QStack A;
	int *O, O_top, O_ins;
	int tail, head, arc, rev;
	int i;
	double flow, maxflow = 0.0;
	int root, edge, node;
	int min_dist, min_edge;
	int *d, next_d, *ts, timer;
	int v, w, label, org;
	int *from, *to;
	int *first, *next, *revf, *revn;

	initAdjList(&G);
	for (arc = G.first[G.src]; arc; arc = G.next[arc]) { maxflow += f[arc]; }

	// parent 
	p = (int *) calloc(G.n + 1, sizeof(int));
	if (p == NULL)
	{
		fprintf(stderr, "boykov_kolmogorov(): ERROR [p = malloc()]\n");
		exit(EXIT_FAILURE);
	}
	// 逆向き枝の番号をメモしておく配列 
	inverse = (int *) malloc(((G.m << 1) + 1) * sizeof(int));
	if (inverse == NULL)
	{
		fprintf(stderr, "boykov_kolmogorov(): ERROR [inverse = malloc()]\n");
		exit(EXIT_FAILURE);
	}
	// Active 
	newQStack(&A, G.n + 1);
	// Orphan 
	O = (int *) malloc((G.n + 1) * sizeof(int));
	if (O == NULL)
	{
		fprintf(stderr, "boykov_kolmogorov(): ERROR [O = malloc()]\n");
		exit(EXIT_FAILURE);
	}
	// 木の根からの深さをメモする配列 
	d = (int *) malloc((G.n + 1) * sizeof(int));
	if (d == NULL)
	{
		fprintf(stderr, "boykov_kolmogorov(): ERROR [d = malloc()]\n");
		exit(EXIT_FAILURE);
	}
	// タイムスタンプ 
	ts = (int *) malloc((G.n + 1) * sizeof(int));
	if (ts == NULL)
	{
		fprintf(stderr, "boykov_kolmogorov(): ERROR [ts = malloc()]\n");
		exit(EXIT_FAILURE);
	}

	newAuxGraph(G, &Gf, f, inverse);

	for (i = G.n; i; i--) { t[i] = 0; }
	ts[Gf.src] = timer = 1; ts[Gf.sink] = 1;
	d[Gf.src] = 0; d[Gf.sink] = 0;
	t[Gf.src] = 1; t[Gf.sink] = 2;
	initQStack(&A);
	enqueueQStack(&A, Gf.src); enqueueQStack(&A, Gf.sink);
	label = 1, first = Gf.first, next = Gf.next, to = Gf.head;
	for (; ; )
	{
		// Growth Stage 
		for (; !isEmptyQStack(A); dequeueQStack(&A))
		{
			v = getQueueNextQStack(A);
			if (t[v] != label)
			{
				if (label == 2)
				{
					label = 1; to = Gf.head;
					first = Gf.first; next = Gf.next;
				}
				else
				{
					label = 2; to = Gf.tail;
					first = Gf.revf; next = Gf.revn;
				}
			}
			next_d = d[v] + 1;
			for (arc = first[v]; arc; arc = next[arc])
			{
				w = to[arc];
				if (!t[w])
				{
					t[w] = label; p[w] = arc;
					enqueueQStack(&A, w);
					d[w] = next_d; ts[w] = ts[v];
				}
				else if (t[w] == label)
				{
					if ((ts[w] <= ts[v]) && (d[w] > d[v]))
					{
						p[w] = arc;
						d[w] = next_d; ts[w] = ts[v];
					}
				}
				else
				{
					goto FOUND_PATH;
				}
			}
		}
		break;

	FOUND_PATH:
		// Augmentation Stage 
		timer++;

		// パスを作る 
		if (label == 1) { rev = p[w]; p[w] = arc; arc = rev; }
		while (arc) { rev = p[v = Gf.head[arc]]; p[v] = arc; arc = rev; }

		flow = INF;
		for (arc = p[Gf.sink]; arc; arc = p[Gf.tail[arc]])
		{
			if (flow > Gf.capa[arc]) { flow = Gf.capa[arc]; }
		}
		maxflow += flow;

		O_top = 0; O_ins = 0; edge = 0;
		for (arc = p[head = Gf.sink]; arc; arc = p[head = tail])
		{
			if (t[head] == 2) { p[head] = edge; edge = arc; }
			tail = Gf.tail[arc];
			Gf.capa[arc] -= flow;
			if (Gf.capa[arc] == 0.0)
			{
				deleteAdjEdge(&Gf, arc); deleteRevEdge(&Gf, arc);
				if ((t[head] == 1) && (t[tail] == 1))
				{
					p[head] = 0;
					// OrphanにStack的に積む 
					if (!O_top) { O_ins = head; }
					O[head] = O_top; O_top = head;
				}
				else if ((t[tail] == 2) && (t[head] == 2))
				{
					edge = 0;
					// OrphanにQueue的に積む 
					if (!O_ins) { O_top = tail; }
					else        { O[O_ins] = tail; }
					O[O_ins = tail] = 0;
				}
			}
			rev = inverse[arc];
			if ((Gf.capa[rev] == 0.0) && (tail != Gf.src) && (head != Gf.sink))
			{
				appendAdjEdge(&Gf, rev); appendRevEdge(&Gf, rev);
			}
			Gf.capa[rev] += flow;
		}

		if (t[O_top] == 1)
		{
			label = 1, org = Gf.src, first = Gf.first, next = Gf.next;
			revf = Gf.revf, revn = Gf.revn, from = Gf.tail, to = Gf.head;
		}
		else
		{
			label = 2, org = Gf.sink, first = Gf.revf, next = Gf.revn;
			revf = Gf.first; revn = Gf.next, from = Gf.head; to = Gf.tail;
		}
		for (; O_top; O_top = O[O_top])
		{
			if (t[O_top] != label)
			{
				if (label == 2)
				{
					label = 1; org = Gf.src;
					first = Gf.first; next = Gf.next;
					revf = Gf.revf; revn = Gf.revn;
					from = Gf.tail; to = Gf.head;
				}
				else
				{
					label = 2; org = Gf.sink;
					first = Gf.revf; next = Gf.revn;
					revf = Gf.first; revn = Gf.next;
					from = Gf.head; to = Gf.tail;
				}
			}
			// 深さの小さい親を探す 
			w = O_top; min_dist = INT_MAX;
			for (arc = revf[w]; arc; arc = revn[arc])
			{
				v = from[arc];
				if (t[v] == label)
				{
					if (ts[v] != timer)
					{
						next_d = 0;
						node = v; edge = p[node];
						while (edge && (ts[node] != timer))
						{
							next_d++;
							node = from[edge];
							edge = p[node];
						}
						if ((node == org) || edge)
						{
							root = node;
							next_d += d[root];
							node = v;
							while (node != root)
							{
								d[node] = next_d--;
								ts[node] = timer;
								edge = p[node];
								node = from[edge];
							}
							ts[root] = timer;
						}
					}
					if ((ts[v] == timer) && (min_dist > d[v]))
					{
						min_edge = arc; min_dist = d[v];
					}
				}
			}
			// 新しい親が見つかった 
			if (min_dist < INT_MAX)
			{
				p[w] = min_edge;
				d[w] = min_dist + 1; ts[w] = timer;
			}
			// 新しい親が見つからない 
			else
			{
				for (arc = revf[w]; arc; arc = revn[arc])
				{
					v = from[arc];
					if ((t[v] == label) && !inQStack(A, v))
					{
						enqueueQStack(&A, v);
					}
				}
				// 自分の子をOrphanに追加する 
				v = w;
				for (arc = first[v]; arc; arc = next[arc])
				{
					w = to[arc];
					if (p[w] == arc)
					{
						p[w] = 0;
						O[O_ins] = w; O[O_ins = w] = 0;
					}
				}
				t[v] = 0;
				if (inQStack(A, v)) { pruneDataQStack(&A, v); }
			}
		}
	}

	free(ts); free(d); free(p); free(O); delQStack(&A);
	for (i = G.m; i; i--) { f[i] = Gf.capa[inverse[i]]; }
	delGraph(&Gf);
	free(inverse);
	for (i = G.n; i; i--)
	{
		if (t[i] == 2) { t[i] = 0; }
	}

	return (maxflow);
}


/*************************************************************/
//
//			construct s_c用kolmogorov
//
/*************************************************************/

/* kolmogorovのアルゴリズムの木の成長両側バージョン */
/*
void boykov_kolmogorov(Graph G, Graph *Gf, double f[], int t[])
{
	//Graph Gf;
	int *p, *inverse;
	QStack A;
	int *O, O_top, O_ins;
	int tail, head, arc, rev;
	int i;
	double flow;// maxflow = 0.0;
	int root, edge, node;
	int min_dist, min_edge;
	int *d, next_d, *ts, timer;
	int v, w, label, org;
	int *from, *to;
	int *first, *next, *revf, *revn;

	initAdjList(&G);
	//for (arc = G.first[G.src]; arc; arc = G.next[arc]) { maxflow += f[arc]; }

//	 parent 
	p = (int *) calloc(G.n + 1, sizeof(int));
	if (p == NULL)
	{
		fprintf(stderr, "boykov_kolmogorov(): ERROR [p = malloc()]\n");
		exit(EXIT_FAILURE);
	}
//	 逆向き枝の番号をメモしておく配列 
	inverse = (int *) malloc(((G.m << 1) + 1) * sizeof(int));
	if (inverse == NULL)
	{
		fprintf(stderr, "boykov_kolmogorov(): ERROR [inverse = malloc()]\n");
		exit(EXIT_FAILURE);
	}
//	 Active 
	newQStack(&A, G.n + 1);
//	 Orphan 
	O = (int *) malloc((G.n + 1) * sizeof(int));
	if (O == NULL)
	{
		fprintf(stderr, "boykov_kolmogorov(): ERROR [O = malloc()]\n");
		exit(EXIT_FAILURE);
	}
//	木の根からの深さをメモする配列 
	d = (int *) malloc((G.n + 1) * sizeof(int));
	if (d == NULL)
	{
		fprintf(stderr, "boykov_kolmogorov(): ERROR [d = malloc()]\n");
		exit(EXIT_FAILURE);
	}
//	 タイムスタンプ 
	ts = (int *) malloc((G.n + 1) * sizeof(int));
	if (ts == NULL)
	{
		fprintf(stderr, "boykov_kolmogorov(): ERROR [ts = malloc()]\n");
		exit(EXIT_FAILURE);
	}

	newAuxGraph(G, Gf, f, inverse);

	for (i = G.n; i; i--) { t[i] = 0; }
	ts[Gf->src] = timer = 1; ts[Gf->sink] = 1;
	d[Gf->src] = 0; d[Gf->sink] = 0;
	t[Gf->src] = 1; t[Gf->sink] = 2;
	initQStack(&A);
	enqueueQStack(&A, Gf->src); enqueueQStack(&A, Gf->sink);
	label = 1, first = Gf->first, next = Gf->next, to = Gf->head;
	for (; ; )
	{
//		 Growth Stage 
		for (; !isEmptyQStack(A); dequeueQStack(&A))
		{
			v = getQueueNextQStack(A);
			if (t[v] != label)
			{
				if (label == 2)
				{
					label = 1; to = Gf->head;
					first = Gf->first; next = Gf->next;
				}
				else
				{
					label = 2; to = Gf->tail;
					first = Gf->revf; next = Gf->revn;
				}
			}
			next_d = d[v] + 1;
			for (arc = first[v]; arc; arc = next[arc])
			{
				w = to[arc];
				if (!t[w])
				{
					t[w] = label; p[w] = arc;
					enqueueQStack(&A, w);
					d[w] = next_d; ts[w] = ts[v];
				}
				else if (t[w] == label)
				{
					if ((ts[w] <= ts[v]) && (d[w] > d[v]))
					{
						p[w] = arc;
						d[w] = next_d; ts[w] = ts[v];
					}
				}
				else
				{
					goto FOUND_PATH;
				}
			}
		}
		break;

	FOUND_PATH:
//		 Augmentation Stage 
		timer++;

//		 パスを作る 
		if (label == 1) { rev = p[w]; p[w] = arc; arc = rev; }
		while (arc) { rev = p[v = Gf->head[arc]]; p[v] = arc; arc = rev; }

		flow = INF;
		for (arc = p[Gf->sink]; arc; arc = p[Gf->tail[arc]])
		{
			if (flow > Gf->capa[arc]) { flow = Gf->capa[arc]; }
		}
		//maxflow += flow;

		O_top = 0; O_ins = 0; edge = 0;
		for (arc = p[head = Gf->sink]; arc; arc = p[head = tail])
		{
			if (t[head] == 2) { p[head] = edge; edge = arc; }
			tail = Gf->tail[arc];
			Gf->capa[arc] -= flow;
			if (Gf->capa[arc] == 0.0)
			{
				deleteAdjEdge(Gf, arc); deleteRevEdge(Gf, arc);
				if ((t[head] == 1) && (t[tail] == 1))
				{
					p[head] = 0;
//					 OrphanにStack的に積む 
					if (!O_top) { O_ins = head; }
					O[head] = O_top; O_top = head;
				}
				else if ((t[tail] == 2) && (t[head] == 2))
				{
					edge = 0;
//					 OrphanにQueue的に積む 
					if (!O_ins) { O_top = tail; }
					else        { O[O_ins] = tail; }
					O[O_ins = tail] = 0;
				}
			}
			rev = inverse[arc];
			if ((Gf->capa[rev] == 0.0) && (tail != Gf->src) && (head != Gf->sink))
			{
				appendAdjEdge(Gf, rev); appendRevEdge(Gf, rev);
			}
			Gf->capa[rev] += flow;
		}

		if (t[O_top] == 1)
		{
			label = 1, org = Gf->src, first = Gf->first, next = Gf->next;
			revf = Gf->revf, revn = Gf->revn, from = Gf->tail, to = Gf->head;
		}
		else
		{
			label = 2, org = Gf->sink, first = Gf->revf, next = Gf->revn;
			revf = Gf->first; revn = Gf->next, from = Gf->head; to = Gf->tail;
		}
		for (; O_top; O_top = O[O_top])
		{
			if (t[O_top] != label)
			{
				if (label == 2)
				{
					label = 1; org = Gf->src;
					first = Gf->first; next = Gf->next;
					revf = Gf->revf; revn = Gf->revn;
					from = Gf->tail; to = Gf->head;
				}
				else
				{
					label = 2; org = Gf->sink;
					first = Gf->revf; next = Gf->revn;
					revf = Gf->first; revn = Gf->next;
					from = Gf->head; to = Gf->tail;
				}
			}
//			 深さの小さい親を探す 
			w = O_top; min_dist = INT_MAX;
			for (arc = revf[w]; arc; arc = revn[arc])
			{
				v = from[arc];
				if (t[v] == label)
				{
					if (ts[v] != timer)
					{
						next_d = 0;
						node = v; edge = p[node];
						while (edge && (ts[node] != timer))
						{
							next_d++;
							node = from[edge];
							edge = p[node];
						}
						if ((node == org) || edge)
						{
							root = node;
							next_d += d[root];
							node = v;
							while (node != root)
							{
								d[node] = next_d--;
								ts[node] = timer;
								edge = p[node];
								node = from[edge];
							}
							ts[root] = timer;
						}
					}
					if ((ts[v] == timer) && (min_dist > d[v]))
					{
						min_edge = arc; min_dist = d[v];
					}
				}
			}
//			 新しい親が見つかった 
			if (min_dist < INT_MAX)
			{
				p[w] = min_edge;
				d[w] = min_dist + 1; ts[w] = timer;
			}
//			 新しい親が見つからない 
			else
			{
				for (arc = revf[w]; arc; arc = revn[arc])
				{
					v = from[arc];
					if ((t[v] == label) && !inQStack(A, v))
					{
						enqueueQStack(&A, v);
					}
				}
//				 自分の子をOrphanに追加する 
				v = w;
				for (arc = first[v]; arc; arc = next[arc])
				{
					w = to[arc];
					if (p[w] == arc)
					{
						p[w] = 0;
						O[O_ins] = w; O[O_ins = w] = 0;
					}
				}
				t[v] = 0;
				if (inQStack(A, v)) { pruneDataQStack(&A, v); }
			}
		}
	}

	free(ts); free(d); free(p); free(O); delQStack(&A);
	for (i = G.m; i; i--) { f[i] = Gf->capa[inverse[i]]; }
	//delGraph(&Gf);
	free(inverse);
	for (i = G.n; i; i--)
	{
		if (t[i] == 2) { t[i] = 0; }
	}

	//return (maxflow);
}
*/

/* 最大フロー問題を解くための補助グラフを作成する関数(ford fulkerson系用) */

void newAuxGraph(Graph G, Graph *Gf, double f[], int inverse[])
{
	int i, j, tail, head;

	newGraph(Gf, G.n, G.m << 1);

	// 順向き枝と逆向き枝をセット
	for (i = G.m, j = Gf->m; i; i--, j--)
	{
		setEdge(Gf, i, G.tail[i], G.head[i], G.capa[i] - f[i]);
		setEdge(Gf, j, G.head[i], G.tail[i], f[i]);
		inverse[i] = j; inverse[j] = i;
	}
	initAdjList(Gf); newRevList(Gf); initRevList(Gf);
	setSource(Gf, G.src); setSink(Gf, G.sink);

	// 容量が0の枝と自己閉路枝,ソースに入る枝,シンクから出る枝を取り除く
	for (i = Gf->m; i; i--)
	{
		tail = Gf->tail[i]; head = Gf->head[i];
		if ((Gf->capa[i] <= 0.0) || (tail == head)
		    || (head == G.src) || (tail == G.sink))
		{
			deleteAdjEdge(Gf, i); deleteRevEdge(Gf, i);
		}
	}
}


/*************************************************************/
//
//			construct s_c用newAuxGraph
//
/*************************************************************/

/* 最大フロー問題を解くための補助グラフを作成する関数(ford fulkerson系用) */
/*
void newAuxGraph(Graph G, Graph *Gf, double f[], int inverse[])
{
	int i, j, tail, head;

	//newGraph(Gf, G.n, G.m << 1);

	// 順向き枝と逆向き枝をセット 
	for (i = G.m, j = Gf->m; i; i--, j--)
	{
		setEdge(Gf, i, G.tail[i], G.head[i], G.capa[i] - f[i]);
		setEdge(Gf, j, G.head[i], G.tail[i], f[i]);
		inverse[i] = j; inverse[j] = i;
	}
	initAdjList(Gf);
	//newRevList(Gf);
	initRevList(Gf);
	//setSource(Gf, G.src); setSink(Gf, G.sink);

	// 容量が0の枝と自己閉路枝,ソースに入る枝,シンクから出る枝を取り除く 
	for (i = Gf->m; i; i--)
	{
		tail = Gf->tail[i]; head = Gf->head[i];
		if ((Gf->capa[i] <= 0.0) || (tail == head)
		    || (head == G.src) || (tail == G.sink))
		{
			deleteAdjEdge(Gf, i); deleteRevEdge(Gf, i);
		}
	}
}
*/


