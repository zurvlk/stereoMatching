#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue_stack.h"

void newQStack(QStack *QS, const size_t size)
{
	if ((!size) < (size > INT_MAX))
	{
		fprintf(stderr, "newQStack(): ERROR [2nd arg]\n");
		exit(EXIT_FAILURE);
	}

	QS->next = (int *) calloc(size, sizeof(int));
	if (QS->next == NULL)
	{
		fprintf(stderr, "newQStack(): ERROR [QS->next = calloc()]\n");
		exit(EXIT_FAILURE);
	}
	QS->prev = (int *) malloc(size * sizeof(int));
	if (QS->prev == NULL)
	{
		fprintf(stderr, "newQStack(): ERROR [QS->prev = malloc()]\n");
		exit(EXIT_FAILURE);
	}
	memset(QS->prev, 255, size * sizeof(int));
	QS->lmax = size;
	QS->top = -2; QS->end = -2; QS->len = 0;
}

void initQStack(QStack *QS)
{
	QS->top = -2; QS->end = -2; QS->len = 0;
	memset(QS->prev, 255, QS->lmax * sizeof(int));
}

void delQStack(QStack *QS)
{
	if (QS->next != NULL) { free(QS->next); QS->next = NULL; }
	if (QS->prev != NULL) { free(QS->prev); QS->prev = NULL; }
}

int isEmptyQStack(QStack QS)
{
	return ((QS.len) ? 0 : 1);
}

int isFullQStack(QStack QS)
{
	return ((QS.len == QS.lmax) ? 1 : 0);
}

int inQStack(QStack QS, const unsigned data)
{
	return ((QS.prev[data] == -1) ? 0 : 1);
}

void showQStack(QStack QS)
{
	int x;

	for (x = QS.top; x != -2; x = QS.next[x]) { printf("%d ", x); }
	putchar('\n');
}

void pushQStack(QStack *QS, const unsigned int data)
{
	//if (QS->prev[data] != -1) {
	//	fprintf(stderr, "pushQStack(): ERROR [inQStack()]\n");
	//	exit(EXIT_FAILURE);
	//}

	(QS->len)++;
	if (QS->top == -2) { QS->end = data; }
	else               { QS->prev[QS->top] = data; }
	QS->next[data] = QS->top;
	QS->top = data;
	QS->prev[data] = -2;
}

void enqueueQStack(QStack *QS, const unsigned int data)
{
	//if (QS->prev[data] != -1) {
	//	fprintf(stderr, "enqueueQStack(): ERROR [inQStack()]\n");
	//	exit(EXIT_FAILURE);
	//}

	(QS->len)++;
	if (QS->end == -2) { QS->top = data; }
	else               { QS->next[QS->end] = data; }
	QS->prev[data] = QS->end;
	QS->end = data;
	QS->next[data] = -2;
}

int getStackNextQStack(QStack QS)
{
	//if (!QS.len) {
	//	fprintf(stderr, "getStackNextQStack(): ERROR [isEmptyQStack()]\n");
	//	exit(EXIT_FAILURE);
	//}
	return (QS.top);
}

/* constents equal to getStackNextQStack() */
int getQueueNextQStack(QStack QS)
{
	//if (!QS.len) {
	//	fprintf(stderr, "getQueueNextQStack(): ERROR [isEmptyQStack()]\n");
	//	exit(EXIT_FAILURE);
	//}
	return (QS.top);
}

/* constents equal to getStackNextQStack() */
int getTopQStack(QStack QS)
{
	//if (!QS.len) {
	//	fprintf(stderr, "getTopQStack(): ERROR [isEmptyQStack()]\n");
	//	exit(EXIT_FAILURE);
	//}
	return (QS.top);
}

int getEndQStack(QStack QS)
{
	//if (!QS.len) {
	//	fprintf(stderr, "getEndQStack(): ERROR [isEmptyQStack()]\n");
	//	exit(EXIT_FAILURE);
	//}
	return (QS.end);
}

void popQStack(QStack *QS)
{
	//if (!QS->len) {
	//	fprintf(stderr, "popQStack(): ERROR [isEmptyQStack()]\n");
	//	exit(EXIT_FAILURE);
	//}

	(QS->len)--;
	if (QS->top == QS->end) { QS->end = -2; }
	else                    { QS->prev[QS->next[QS->top]] = -2; }
	QS->prev[QS->top] = -1;
	QS->top = QS->next[QS->top];
}

/* constents equal to popQStack() */
void dequeueQStack(QStack *QS)
{
	//if (!QS->len) {
	//	fprintf(stderr, "dequeueQStack(): ERROR [isEmptyQStack()]\n");
	//	exit(EXIT_FAILURE);
	//}

	(QS->len)--;
	if (QS->top == QS->end) { QS->end = -2; }
	else                    { QS->prev[QS->next[QS->top]] = -2; }
	QS->prev[QS->top] = -1;
	QS->top = QS->next[QS->top];
}

void emitQStack(QStack *QS)
{
	//if (!QS->len) {
	//	fprintf(stderr, "emitQStack(): ERROR [isEmptyQStack()]\n");
	//	exit(EXIT_FAILURE);
	//}

	(QS->len)--;
	QS->end = QS->prev[QS->end];
	if (QS->end == -2)
	{
		QS->prev[QS->top] = -1; QS->top = -2;
	}
	else
	{
		QS->prev[QS->next[QS->end]] = -1; QS->next[QS->end] = -2;
	}
}

void pruneDataQStack(QStack *QS, const unsigned int data)
{
	//if (QS->prev[data] == -1) {
	//	fprintf(stderr, "pruneDataQStack(): ERROR [!inQStack()]\n");
	//	exit(EXIT_FAILURE);
	//}

	(QS->len)--;
	if (QS->prev[data] == -2) { QS->top = QS->next[data]; }
	else                      { QS->next[QS->prev[data]] = QS->next[data]; }
	if (QS->next[data] == -2) { QS->end = QS->prev[data]; }
	else                      { QS->prev[QS->next[data]] = QS->prev[data]; }
	QS->prev[data] = -1;
}

/*
void showQStack4Debug(QStack QS)
{
	int i;

	puts("----- QStack Data Structure -----");
	printf("QS.top: %d, QS.end: %d\n", QS.top, QS.end);
	printf("QS.len: %d, QS.lmax: %d\n", QS.len, QS.lmax);
	puts("/--- Q.next ---/");
	for (i = 0; i < QS.lmax; i++) { printf("%2d ", i); }
	putchar('\n');
	for (i = 0; i < QS.lmax; i++) { printf("%2d ", QS.next[i]); }
	puts("\n/--- Q.prev ---/");
	for (i = 0; i < QS.lmax; i++) { printf("%2d ", i); }
	putchar('\n');
	for (i = 0; i < QS.lmax; i++) { printf("%2d ", QS.prev[i]); }
	puts("\n/--- Queue Stack ---/");
	for (i = QS.top; i != -2; i = QS.next[i]) { printf("%d ", i); }
	putchar('\n');
}
*/

