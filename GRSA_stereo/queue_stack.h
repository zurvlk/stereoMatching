#ifndef __QUEUE_STACK__INCLUDED__
#define __QUEUE_STACK__INCLUDED__

typedef struct __QUEUE_STACK__STRUCTURE__
{
	int *next, *prev, top, end;
	size_t len, lmax;
}
QStack;

void newQStack(QStack *QS, const size_t size);
void initQStack(QStack *QS);
void delQStack(QStack *QS);
int isEmptyQStack(QStack QS);
int isFullQStack(QStack QS);
int inQStack(QStack QS, const unsigned int data);
void showQStack(QStack QS);
void pushQStack(QStack *QS, const unsigned int data);
void enqueueQStack(QStack *QS, const unsigned int data);
int getStackNextQStack(QStack QS);
int getQueueNextQStack(QStack QS);
int getTopQStack(QStack QS);
int getEndQStack(QStack QS);
void popQStack(QStack *QS);
void dequeueQStack(QStack *QS);
void emitQStack(QStack *QS);
void pruneDataQStack(QStack *QS, const unsigned int data);
void showQStack4Debug(QStack QS);

#endif


