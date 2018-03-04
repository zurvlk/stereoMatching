#include "bmp.h"
#include "grsa.h"
#include "graph.h"
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include <math.h>

// #define INF DBL_MAX

#define INF 10000000
#define _CALC_INFTY_ 1 // エネルギー計算時に取り得ない値を無限大で計算する 0:しない 1:する
#define _CONSIDE_ALL_SECTIONS_ 0 //全ての候補区間から劣モジュラ集合を取得する 0:しない 1:する

double dabs(double a, double b) {
    return a - b > 0 ? a - b : b - a;
}

double abss(double a) {
    return a > 0 ? a : - a;
}
int nc2(int n) {
    return n * (n - 1) / 2;
}

double fmin(double i, double j) {
    return i < j ? i : j;
}

double fmin3(double i, double j, double k) {
    return fmin(fmin(i, j), k);
}

double fmax(double i, double j) {
    return i > j ? i : j;
}
double theta(double n, double T) {

    if(function == 0) return fmin(n > 0 ? n : -n, T);
    else if (function == 1) return fmin(n * n, T);
    else {
        n = abss(n);
        if (n <= T) return n;
        else return n / la + (la - 1) / la * T;
    }
}

// リスト内にtargetが存在 1
int isin_array(int *ls, int target) {
    for (int i = 1; i <= ls[0]; i++) {
        if (ls[i] > target) break;
        if (ls[i] == target) return 1;
    }
    return 0;
}

int is_convex(int i, int j, double T) {
    if (theta(j, T) - theta(j - 1, T) >= (theta(j - 1, T) - theta(i, T)) / (j - 1 - i) &&
        i * (theta(i + 1, T) - theta(i, T)) >= theta(i, T) - theta(0, T) &&
        theta(i, T) - theta(0, T) >= 0) {
            return 1;
        }

    else return 0;
}

// 2つの配列の値がすべて同一 0
// 2つの配列の値が異なる 1
int cmparray(int *array1, int *array2, int size) {
    int i;
    for (i = 1; i <= size; i++) {
        if(array1[i] != array2[i]) return 1;
    }
    return 0;
}

void cpyarray(int *terget, int *source, int size) {
    int i;

    for (i = 1; i <= size; i++) {
        terget[i] = source[i];
    }
}

int mpair(int i, int j, int label_size) {
    int res = 0, c;
    int temp = i;
    if (i > j) {
        i = j;
        j = temp;
    }
    if(i == 0) return label_size - j;
    c = 0;
    while (c < i) {
        res += label_size - c;
        c++;
    }
    // printf("res: %d j: %d\n", res, j);
    res += label_size - j - 2 * c;
    return res;
}




int gen_submodular_subsets(int label_size, int range_size, Subsets *ss) {
    int i, j, k, l, m, n, prev, rs2, large_array, size;

    int ccvex = 0, label_max = label_size - 1;
    int convex[10][2];

    int **temp;


    int current_array;

    ss->number = 0;

    // 候補区間抽出
    i = 0;
    j = 1;
    while (j < label_size) {
        j++;
        // printf("i: %d, j: %d isc: %f %f\n", i, j, theta(j, T) - theta(j - 1, T) , (theta(j - 1, T) - theta(i, T)) / (j - 1 - i));
        if (j > 1  && is_convex(i, j, ss->T)) {
            prev = 1;
            if (j == label_size) {
                convex[ccvex][0] = i;
                convex[ccvex][1] = j - 1;
                ccvex++;
            }
            // printf("%d - > %d\n", i, j);
        } else if (i != j - 1 && prev) {
            // printf("%d %d\n", i, j);
            convex[ccvex][0] = i;
            convex[ccvex][1] = j - 1;
            i = j - 1;
            ccvex++;
            prev = 0;
        } else {
            i = j - 1;
            prev = 0;
        }
        if (ccvex > 9) break;

    }
    for (i = 0; i < ccvex; i++) {
        printf("T : %.0d 候補区間: %d --> %d\n", ss->T, convex[i][0], convex[i][1]);
    }

    // 全てのラベルのペアを列挙
    if ((ss->pairs = (int **)malloc(sizeof(int*) * (nc2(label_size) + 1))) == NULL) {
        fprintf(stderr, "Error!:malloc[main()->pairs]\n");
        exit(EXIT_FAILURE);
    }

    k = 1;
    for (i = 0; i < label_max; i++) {
        for (j = label_max; j > i; j--) {
            if ((ss->pairs[k] = (int*)malloc(sizeof(int) * 3)) == NULL) {
                fprintf(stderr, "Error!:malloc[main()-=>pairs]\n");
                exit(EXIT_FAILURE);
            }
            ss->pairs[k][0] = 0;
            ss->pairs[k][1] = i;
            ss->pairs[k][2] = j;
            k++;
        }
    }

    if ((ss->ls = (int **)malloc(sizeof(int*) * (nc2(label_size) + 1))) == NULL) {
        fprintf(stderr, "Error!:malloc[main()->ls]\n");
        exit(EXIT_FAILURE);
    }

    ss->number = 0;
    if (subset) {
        ss->number = label_size - range_size + 1;
        if ((ss->ls = (int **)malloc(sizeof(int*) * (ss->number + 1))) == NULL) {
            fprintf(stderr, "Error!:malloc[main()->ls]\n");
            exit(EXIT_FAILURE);
        }
        for (i = 1; i <= ss->number; i++) {
            if ((ss->ls[i] = (int *)malloc(sizeof(int) * (range_size + 1))) == NULL) {
                fprintf(stderr, "Error!:malloc[main()->ls]\n");
                exit(EXIT_FAILURE);
            }
            ss->ls[i][0] = range_size;
            ss->ls[i][1] = i - 1;
            for (j = 2; j <= range_size; j++) {
                ss->ls[i][j] = ss->ls[i][j - 1] + 1;
            }
        }
        if (allpairs) {
            large_array = ss->number;
            for (current_array = 1; current_array <= large_array; current_array++) {
                for (k = 0; k < ss->ls[current_array][0]; k++) {
                    for (l = ss->ls[current_array][0]; l > k; l--) {
                        for (m = 1; m <= nc2(label_size); m++) {
                            if (ss->pairs[m][1] == ss->ls[current_array][k] && ss->pairs[m][2] == ss->ls[current_array][l]) {
                                ss->pairs[m][0] = 1;
                                break;
                            }
                        }
                    }
                }
            }
        }
        for (i = 1; i <= ss->number; i++) {
            printf("submodular subsets: ");
            printf("%d, (%d) ",i, ss->ls[i][0]);
            for (j = 1; j <= ss->ls[i][0]; j++) {
                printf("%d ", ss->ls[i][j]);
            }
            printf(" end\n");
        }
        if ((temp = (int **)realloc(ss->ls, (ss->number + 1) * sizeof(int *))) == NULL) {
            fprintf(stderr, "Error!:malloc[main()->ls]\n");
            exit(EXIT_FAILURE);
        } else {
            ss->ls = temp;
            temp = NULL;
        }

    } else if (label_size == range_size) {
        large_array = 1;
        ss->number = 1;
        if ((temp = (int **)realloc(ss->ls, 2 * sizeof(int *))) == NULL) {
            fprintf(stderr, "Error!:malloc[main()->ls]\n");
            exit(EXIT_FAILURE);
        } else {
            ss->ls = temp;
            temp = NULL;
        }

        if ((ss->ls[1] = (int *)malloc(sizeof(int) * (range_size + 1))) == NULL) {
            fprintf(stderr, "Error!:malloc[main()->ls]\n");
            exit(EXIT_FAILURE);
        }
        ss->ls[1][0] = range_size;
        for (i = 1; i <= range_size; i++) ss->ls[1][i] = i - 1;
        ss->number = 1;
    } else if (range_size == 2) {
        ss->ls = ss->pairs;
        for (i = 1; i <= nc2(label_size); i++) ss->ls[i][0] = 2;
        ss->number = nc2(label_size);
    } else {
        printf("segm %d\n", segm);
        for (i = 0; i < ccvex; i++) {
            if (convex[i][0] == 0) {
                if (convex[i][1] > range_size) rs2 = range_size;
                else rs2 = convex[i][1];

                large_array = 1;

                if (segm != 0) {
                    j = rs2 - (rs2 / segm + 1);
                    do {
                        j += rs2 - (rs2 / segm + 1);
                        large_array++;
                    } while (j + rs2 - (rs2 / segm + 1) < label_size);
                    large_array++;
                    printf("large_array = %d\n", large_array);
                    ss->number = large_array;

                    for (j = 1; j < large_array; j++) {
                        if ((ss->ls[j] = (int *)malloc(sizeof(int) * (rs2 + 1))) == NULL) {
                            fprintf(stderr, "Error!:malloc[main()->ls]\n");
                            exit(EXIT_FAILURE);
                        }
                        ss->ls[j][0] = rs2;
                        if(j != 1) ss->ls[j][1] = ss->ls[j - 1][rs2 - (rs2 / segm)];
                        else ss->ls[j][1] = 0;
                        for (k = 2; k <= rs2; k++) {
                            //segmentation fault
                            // printf("%d, %d\n", j, k);
                            ss->ls[j][k] = ss->ls[j][k - 1] + 1;
                            n = ss->ls[j][k] - (rs2 / segm);
                        }
                    }
                    size = label_size - n;
                    if ((ss->ls[large_array] = (int *)malloc(sizeof(int) * size)) == NULL) {
                        fprintf(stderr, "Error!:malloc[main()->ls]\n");
                        exit(EXIT_FAILURE);
                    }
                    ss->ls[large_array][0] = size;
                    ss->ls[large_array][1] = n;

                    for (j = 2; ss->ls[large_array][j - 1] + 1 <= label_max; j++) {
                        ss->ls[large_array][j] = ss->ls[large_array][j - 1] + 1;
                        n = ss->ls[large_array][j];
                    }

                } else {
                
                    j = rs2 - 1;
                    do {
                        j += rs2 - 1;
                        large_array++;
                    } while (j + rs2 - 1 < label_size);
                    large_array++;
                    ss->number = large_array;

                    for (j = 1; j < large_array; j++) {
                        if ((ss->ls[j] = (int *)malloc(sizeof(int) * (rs2 + 1))) == NULL) {
                            fprintf(stderr, "Error!:malloc[main()->ls]\n");
                            exit(EXIT_FAILURE);
                        }
                        ss->ls[j][0] = rs2;
                        if(j != 1) ss->ls[j][1] = ss->ls[j - 1][rs2];
                        else ss->ls[j][1] = 0;
                        for (k = 2; k <= rs2; k++) {
                            //segmentation faultT, lambda);
                            // printf("%d, %d\n", j, k);
                            ss->ls[j][k] = ss->ls[j][k - 1] + 1;
                            n = ss->ls[j][k];
                        }
                    }
                    size = label_size - n;
                    if ((ss->ls[large_array] = (int *)malloc(sizeof(int) * size)) == NULL) {
                        fprintf(stderr, "Error!:malloc[main()->ls]\n");
                        exit(EXIT_FAILURE);
                    }
                    ss->ls[large_array][0] = size;
                    ss->ls[large_array][1] = n;

                    for (j = 2; ss->ls[large_array][j - 1] + 1 <= label_max; j++) {
                        ss->ls[large_array][j] = ss->ls[large_array][j - 1] + 1;
                        n = ss->ls[large_array][j];
                    }

                    if (harf) {
                        for (j = large_array + 1; j < 2 * large_array - 1; j++) {
                            if ((ss->ls[j] = (int *)malloc(sizeof(int) * (rs2 + 1))) == NULL) {
                                fprintf(stderr, "Error!:malloc[main()->ls(harf1)]\n");
                                exit(EXIT_FAILURE);
                            }
                            ss->ls[j][0] = rs2;
                            if(j == large_array + 1) ss->ls[j][1] = range_size / 2;
                            else ss->ls[j][1] = ss->ls[j - 1][range_size];
                            for (k = 2; k <= rs2; k++) {
                                // printf("%d, %d\n", j, k);
                                ss->ls[j][k] = ss->ls[j][k - 1] + 1;
                                n = ss->ls[j][k];
                                // printf("%d ", ss->ls[j][k]);
                            }
                            // printf("\n");
                        }

                        large_array = 2 * large_array - 2;
                        
                        ss->number = large_array;

                    }
                }
                
                if (allpairs) {
                    for (current_array = 1; current_array <= large_array; current_array++) {
                        for (k = 0; k < ss->ls[current_array][0]; k++) {
                            for (l = ss->ls[current_array][0]; l > k; l--) {
                                for (m = 1; m <= nc2(label_size); m++) {
                                    if (ss->pairs[m][1] == ss->ls[current_array][k] && ss->pairs[m][2] == ss->ls[current_array][l]) {
                                        ss->pairs[m][0] = 1;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }

            } else {
                if (allpairs) {
                    large_array = 0;
                    n = convex[i][1] / convex[i][0];
                    if (n + 1 > range_size) n = range_size - 1;
                    if (n >= 2) {
                        for (j = 0; j < convex[i][0]; j++) {
                            if (j + n * convex[i][0] <= label_max) large_array++;
                        }
                        for (j = 1; j <= large_array; j++) {
                            if ((ss->ls[ss->number + j] = (int *)malloc(sizeof(int) * (n + 2))) == NULL) {
                                fprintf(stderr, "Error!:malloc[main()->ls]\n");
                                exit(EXIT_FAILURE);
                            }
                            ss->ls[ss->number + j][0] = n + 1;
                            ss->ls[ss->number + j][1] = j - 1;
                            for (k = 2; k <= n + 1; k++) {
                                // printf("%d, %d\n",  ls[ss->number + j][k - 1] , convex[i][0]);
                                ss->ls[ss->number + j][k] = ss->ls[ss->number + j][k - 1] + convex[i][0];
                            }
                        }    
                    }
                    ss->number += large_array;

                    for (current_array = 1; current_array <= large_array; current_array++) {
                        for (k = 0; k < ss->ls[current_array][0]; k++) {
                            for (l = ss->ls[current_array][0]; l > k; l--) {
                                for (m = 1; m <= nc2(label_size); m++) {
                                    if (ss->pairs[m][1] == ss->ls[current_array][k] && ss->pairs[m][2] == ss->ls[current_array][l]) {
                                        ss->pairs[m][0] = 1;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }

        }
        printf("%d\n", ss->number);
        if (allpairs) {
            for (i = 1; i <= nc2(label_size); i++) {
                // printf("pairs[%d] = (%d, %d, %d)\n", i, pairs[i][0], pairs[i][1], pairs[i][2]);

                if (ss->pairs[i][0] == 0) {
                    ss->number++;
                    // printf("size: %d\n", size);
                    if ((ss->ls[ss->number] = (int *)malloc(sizeof(int) * (3))) == NULL) {
                        fprintf(stderr, "Error!:malloc[main()->ls]\n");
                        exit(EXIT_FAILURE);
                    }

                    ss->ls[ss->number][0] = 2;
                    ss->ls[ss->number][1] = ss->pairs[i][1];
                    ss->ls[ss->number][2] = ss->pairs[i][2];

                }
            }
        }
        printf("%d\n", ss->number);

        if ((temp = (int **)realloc(ss->ls, (ss->number + 1) * sizeof(int *))) == NULL) {
            fprintf(stderr, "Error!:malloc[main()->ls]\n");
            exit(EXIT_FAILURE);
        } else {
            ss->ls = temp;
            temp = NULL;
        }

    }
    return ss->number;
}

void readStrBitmap(Image *image, char filename[], int scale) {
    int i, j, grids_node;
    char imgleft[100];
    char imgright[100];
    char imgtruth[100];

    strcpy(imgleft, filename);
    strcpy(imgright, filename);
    strcpy(imgtruth, filename);

    strcat(imgleft, "left.bmp");
    strcat(imgright, "right.bmp");
    strcat(imgtruth, "truth.bmp");
    
    ReadBmp(imgleft, (image->raw_left));
    ReadBmp(imgright, (image->raw_right));
    ReadBmp(imgtruth, (image->truth));
    ReadBmp(imgtruth, (image->output));

    image->width = image->raw_left->width;
    image->height = image->raw_left->height;
    image->scale = scale;
    grids_node = image->height * image->width;

    if(image->width != image->raw_right->width || image->height != image->raw_right->height) {
        fprintf(stderr, "Error %s と %s の解像度が異なります\n", imgleft, imgright);
        exit(EXIT_FAILURE);
    }

    Gray((image->raw_left), (image->raw_left));
    Gray((image->raw_right), (image->raw_right));
    Gray((image->truth), (image->truth));


    if ((image->left = (int *)malloc(sizeof(int) * (grids_node + 1))) == NULL) {
        fprintf(stderr, "Error!:malloc[main()->image.left]\n");
        exit(EXIT_FAILURE);
    }
    if ((image->right = (int *)malloc(sizeof(int) * (grids_node + 1))) == NULL) {
        fprintf(stderr, "Error!:malloc[main()->image.right]\n");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i <  image->height; i++) {
        for (j = 0; j < image->width; j++) {
            image->left[i * image->width + j + 1] = image->raw_left->data[i][j].r / scale;
        }
    }
    for (i = 0; i <  image->height; i++) {
        for (j = 0; j < image->width; j++) {
            image->right[i * image->width + j + 1] = image->raw_right->data[i][j].r / scale;
        }
    }

}

double pairwise(double i, double j, double T, int lambda) {
    return lambda * theta(i - j, T);
}

double V_function(int x, int y, int T, double w) //平滑化項の関数
{
    return  w * theta(x - y, T);
}


double V_energy(int *label, int height, int width, int T, double w) //エネルギー関数の平滑化項のエネルギー 
{

    // int grids_node = height * width; 
	double v = 0;
    int i, j;
    int number = 0;
    for(i = 0; i < height - 1; i++)
	{
		for(j = 0; j < width - 1; j++)
		{
			number++;	                                 
			v += V_function(label[number], label[number + 1], T, w);     
			//printf("V_yoko : ans = %lf\n",ans);
			v += V_function(label[number], label[number + width], T, w);
			//printf("V_tate : ans = %lf\n",ans);
		}
		number++;
		v += V_function(label[number], label[number + width], T, w);
		//printf("V_tate : ans = %lf\n",ans);
	}
	for(i = 0; i < width - 1; i++)
	{
		number++;
		v += V_function(label[number], label[number + 1], T, w);
		//printf("V_tate : ans = %lf\n",ans);		
	}
	return v;	
}

double data(int i, int label) {
    return 1.0 * dabs(label, i);
}


int between(double a, double b, double c) {
    if (( isgreater(b, a) && isgreater(b, c) ) || ( isless(b, a) && isless(b, c))) return 0;
    else return 1;
}

double D_function(int x, img *tmpL, img *tmpR, int h, int w, int maxLabel) {

	int q = w - x;
	double Ip, Iq, Ip_l, Ip_r, Iq_r, Iq_l, fwd, rev;

    if(x < 0 || x > maxLabel) {
        return INF;
    }

	if(q < 0 && flag_infty) {
        return INF;
    }

	Ip = tmpL->data[h][w].r;
	Iq = tmpR->data[h][q].r;

	if ( !q )
	{
		Iq_l = INF;
		Iq_r = (Iq + tmpR->data[h][q + 1].r) / 2.0;
		if( between(Iq, Ip, Iq_r) ) { return 0; }
	}
	else if ( (q + 1) == tmpR->width )
	{
		Iq_l = (Iq + tmpR->data[h][q - 1].r) / 2.0;
		Iq_r = INF;
		if( between(Iq_l, Ip, Iq) ) { return 0; }
	}
	else
	{
		Iq_r = (Iq + tmpR->data[h][q + 1].r) / 2.0;
		Iq_l = (Iq + tmpR->data[h][q - 1].r) / 2.0;
		if( between(Iq_r, Ip, Iq) ) { return 0; }
		if( between(Iq_l, Ip, Iq) ) { return 0; }
	}

	fwd = fmin3( fabs(Ip - Iq_l), fabs(Ip - Iq), fabs(Ip - Iq_r) );

	if( !w )
	{
		Ip_l = INF;
		Ip_r = (Ip + tmpL->data[h][w + 1].r) / 2.0;
		if( between(Ip, Iq, Ip_r) ) { return 0; }
	}
	else if ( (w + 1) == tmpL->width )
	{
		Ip_l = (Ip + tmpL->data[h][w - 1].r) / 2.0;
		Ip_r = INF;
		if( between(Ip_l, Iq, Ip) ) { return 0; }
	}
	else
	{
		Ip_l = (Ip + tmpL->data[h][w - 1].r) / 2.0;
		Ip_r = (Ip + tmpL->data[h][w + 1].r) / 2.0;
		if( between(Ip_l, Iq, Ip) ) { return 0; }
		if( between(Ip_r, Iq, Ip) ) { return 0; }
	}

	rev = fmin3( fabs(Ip_l - Iq), fabs(Ip - Iq), fabs(Ip_r - Iq) );
    if (fmin3(fwd, rev, 20) < 0) printf("error\n");
	return pow( fmin3(fwd, rev, 20), 1 ); // const = 20
}

double D(int x, img *tmpL, img *tmpR, int h, int w, int maxLabel) {

	int q = w - x;
	double Ip, Iq, Ip_l, Ip_r, Iq_r, Iq_l, fwd, rev;

	if ( (x < 0) || (x > maxLabel)) { return INF; }

	Ip = tmpL->data[h][w].r;
	Iq = tmpR->data[h][q].r;

	if ( !q )
	{
		Iq_l = INF;
		Iq_r = (Iq + tmpR->data[h][q + 1].r) / 2.0;
		if( between(Iq, Ip, Iq_r) ) { return 0; }
	}
	else if ( (q + 1) == tmpR->width )
	{
		Iq_l = (Iq + tmpR->data[h][q - 1].r) / 2.0;
		Iq_r = INF;
		if( between(Iq_l, Ip, Iq) ) { return 0; }
	}
	else
	{
		Iq_r = (Iq + tmpR->data[h][q + 1].r) / 2.0;
		Iq_l = (Iq + tmpR->data[h][q - 1].r) / 2.0;
		if( between(Iq_r, Ip, Iq) ) { return 0; }
		if( between(Iq_l, Ip, Iq) ) { return 0; }
	}

	fwd = fmin3( fabs(Ip - Iq_l), fabs(Ip - Iq), fabs(Ip - Iq_r) );

	if( !w )
	{
		Ip_l = INF;
		Ip_r = (Ip + tmpL->data[h][w + 1].r) / 2.0;
		if( between(Ip, Iq, Ip_r) ) { return 0; }
	}
	else if ( (w + 1) == tmpL->width )
	{
		Ip_l = (Ip + tmpL->data[h][w - 1].r) / 2.0;
		Ip_r = INF;
		if( between(Ip_l, Iq, Ip) ) { return 0; }
	}
	else
	{
		Ip_l = (Ip + tmpL->data[h][w - 1].r) / 2.0;
		Ip_r = (Ip + tmpL->data[h][w + 1].r) / 2.0;
		if( between(Ip_l, Iq, Ip) ) { return 0; }
		if( between(Ip_r, Iq, Ip) ) { return 0; }
	}

	rev = fmin3( fabs(Ip_l - Iq), fabs(Ip - Iq), fabs(Ip_r - Iq) );
    if (fmin3(fwd, rev, 20) < 0) printf("error\n");
	return pow( fmin3(fwd, rev, 20), 1 ); // const = 20
}



double D_single(int x, img *tmp_l2, img *tmp_r2, int n, int m) {
    double D_s; 
    D_s = (tmp_l2->data[n][m].r - tmp_r2->data[n][m - x].r) * (tmp_l2->data[n][m].r - tmp_r2->data[n][m - x].r);
    return D_s;
}

double Dt_9n(int x, img *tmp_l2, img *tmp_r2, int i, int j, int max_label){
	double sum = 0;
	int n, m;
	// if(x == -1 || x == max_label) return INF;
    if(x < 0 || x > max_label) {
        flag_infty = 1;
        return INF;
    }

	if(j - x < 0) {
        flag_infty = 1;
        return INF;
    }
	
	if(i - 1 < 1){
		if(j - 1 < 1){
			for(n = i; n <= i + 1; n++){
				for(m = j; m <= j + 1; m++){
                    sum += D_single(x, tmp_l2, tmp_r2, n, m);
				}
			}
		}else if(j + 1 > tmp_l2->width){
			for(n = i; n <= i + 1; n++){
				for(m = j - 1 ; m <= j; m++){
                    sum += D_single(x, tmp_l2, tmp_r2, n, m);
				}
			}
		}else{
			for(n = i; n <= i + 1; n++){
				for(m = j - 1; m <= j + 1; m++){
                    sum += D_single(x, tmp_l2, tmp_r2, n, m);
				}
			}
		}
	}else if(i + 1 > tmp_l2->height){
		if(j - 1 < 1){
			for(n = i - 1; n <= i; n++){
				for(m = j; m <= j + 1; m++){
                    sum += D_single(x, tmp_l2, tmp_r2, n, m);
				}
			}
		}else if(j + 1 > tmp_l2->width){
			for(n = i - 1; n <= i; n++){
				for(m = j - 1; m <= j; m++){
                    sum += D_single(x, tmp_l2, tmp_r2, n, m);
				}
			}
		}else {
			for(n = i - 1; n <= i; n++){
				for(m = j - 1; m <= j + 1; m++){
                    sum += D_single(x, tmp_l2, tmp_r2, n, m);
				}
			}
		}
	}else{
		if(j - 1 < 1){
			for(n = i - 1; n <= i + 1; n++){
				for(m = j; m <= j + 1; m++){
                    sum += D_single(x, tmp_l2, tmp_r2, n, m);
				}
			}
		}else if (j + 1 > tmp_l2->width){
			for(n = i - 1; n <= i + 1; n++){
				for(m = j - 1; m <= j; m++){
                    sum += D_single(x, tmp_l2, tmp_r2, n, m);
				}
			}
		}else {
			for(n = i - 1; n <= i + 1; n++){
				for(m = j - 1; m <= j + 1; m++){
                    sum += D_single(x, tmp_l2, tmp_r2, n, m);
				}
			}
		}
	}
	//sum = sum * sum;
	return sqrt(sum);
}

int calheight(int width, int node) {
	// if (node % width) return node / width;
	// else return node / width - 1;
    return (node - 1) / width;
}

int calwidth(int width, int node) {
	// if (node % width) return node % width - 1;
	// else return width - 1;
    return (node - 1) % width;
}

double energy(Graph *G, int *label, int *I, double T, int lambda) {
    int i;
    double energy = 0;
    //* Dterm
    for (i = 1; i <= G->n - 2; i++) {
        energy += data(I[i], label[i]);
    }
    // */
    // Vterm
    for (i = 1; i <= G->m - 2 * (G->n - 2); i++) {
        energy += pairwise(label[G->tail[i]], label[G->head[i]], T, lambda);
    }
    return energy;
}

double energy_str(Graph *G, int *label,  double T, int lambda, int height, int width, int label_max, int *left, int *right, img *raw_left, img *raw_right) {
    int i;
    double energy = 0;
    flag_infty = 0;
    // Dterm
    for(i = 1; i <= G->n - 2; i++) {
        // energy += Dt(label[i], &image, i / image.width, i % image.width);
        if (dterm == 1) energy += Dt_9n(label[i], raw_left, raw_right, calheight(width, i), calwidth(width, i), label_max);
        else
            #if _CALC_INFTY_            
            energy += D_function(label[i], raw_left, raw_right, calheight(width, i), calwidth(width, i), label_max);
            #else
            energy += D(label[i], raw_left, raw_right, calheight(width, i), calwidth(width, i), label_max);
            #endif
    }
    
    
    // Vterm
    // for (i = 1; i <= G->m - 2 * (G->n - 2); i++) {
    //     energy += pairwise(label[G->tail[i]], label[G->head[i]], T, lambda);
    // }
    energy += V_energy(label, height, width, T, lambda);
    return energy;
}

double e_cost(int i, int j, double T) {
    double cost = 0.5 * (theta(i - j + 1, T)- 2 * theta(i - j, T) + theta(i - j - 1, T));
    return cost;
}

//枝を作る関数makeedge(グラフ,高さ,幅)
void set_single_edges(Graph *G, int height, int width) {
    int i, j, edge_count;
    int tail, head, source, sink;

    source = G->n - 1;
    sink = source + 1;
    setSource(G, source);
    setSink(G, sink);

    edge_count = 1;
    //点と点の間の枝（横）
    for (i = 1; i < height + 1; i++) {
        for (j = 1; j < width; j++) {
            tail =  (i - 1) * width + j;
            head =  tail + 1;
            setEdge(G, edge_count, tail, head, 0);
            edge_count++;
        }
    }

    //点と点の間の枝（縦）
    for (i = 1; i < height ; i++){
        for (j = 1; j < width + 1; j++) {
            tail = (i - 1) * width + j;
            head = tail + width;
            setEdge(G, edge_count, tail, head, 0);
            edge_count++;
        }
    }

    //sourceと点の間の枝
    for (i = 1; i < height * width + 1; i++){
       setEdge(G, edge_count, G->src, i, 0);
       edge_count++;
    }

    //点とsinkの間の枝
    for (i = 1; i < height * width + 1; i++){
       setEdge(G, edge_count, i, G->sink, 0);
       edge_count++;
    }
    return;
}

int make_label_index(Graph *G, int *label, int *label_index, int alpha, int beta) {
    int i, arraysize;
    for (i = 1; i <= G->n - 2; i++) label_index[i] = 0;
    arraysize = 1;
    for (i = 1; i <= G->n - 2; i++) {
        if (label[i] <= beta && label[i] >= alpha) {
            label_index[arraysize] = i;
            arraysize++;
        }
    }
    return arraysize;
}

double phi (int i, int j, int *ls, double T, int lambda) {
    double p = 0;
    if(j > i) return 0;
    if(1 < j && j <= i) {
        p = lambda * (theta(ls[i] - ls[j - 1], T) - theta(ls[i] - ls[j], T) - theta(ls[i - 1] - ls[j - 1], T) + theta(ls[i - 1] - ls[j], T));
        if(i == j) p *= 0.5;
    }
    return p;
}

double near_nodes(int i, int j, int height, int width, int *label, int *isin, double T, int lambda) {
    int grids_node = height * width;

    double nnp_total = 0;

    if (i >= width + 1) {
        // 画素が一番上の行に存在しないとき(iの上が空白でないとき)
        if (!isin[i - width]){
            // iの上の点がLs内に含まれない
            // nnp_total += pairwise(ls[j], label[i - width], T, lambda) ;
            nnp_total += V_function(j, label[i - width], T, lambda) ;
        }
    }

    if (i <= grids_node - width) {
        // 画素が一番下の行に存在しないとき(iの下が空白でないとき)
        if (!isin[i + width]){
            // iの下の点がLs内に含まれない
            // nnp_total += pairwise(ls[j], label[i + width], T, lambda) ;
            nnp_total += V_function(j, label[i + width], T, lambda) ;
        }
    }

    if ((i % width) != 1) {
        // 画素が一番左の列に存在しないとき(iの左が空白でないとき)
        if (!isin[i - 1]){
            // iの左の点がLs内に含まれない
            // nnp_total += pairwise(ls[j], label[i - 1], T, lambda) ;
            nnp_total += V_function(j, label[i - 1], T, lambda) ;
        }
    }

    if ((i % width) != 0) {
        // 画素が一番右の列に存在しないとき(iの右が空白でないとき)
        if (!isin[i + 1]){
            // iの右の点がLs内に含まれない
            // nnp_total += pairwise(ls[j], label[i + 1], T, lambda) ;
            nnp_total += V_function(j, label[i + 1], T, lambda) ;
        }
    }
    return nnp_total;
}

// set_edge for grsa
int set_edge(Graph *G, int *ls, int *label, double T, int lambda, int label_max, int *left, int *right, img *raw_left, img *raw_right) {
    int i, j, k, l, height, width;
    int tail, head, t_base, h_base, grids_node, source, sink, edge_count, current_edge;
    int s2i_begin, i2t_begin, depth_begin;
    int *isin;
    double *min;

    // 格子部分1階層分の点数合計
    grids_node = raw_left->height * raw_left->width;
    height = raw_left->height;
    width = raw_left->width;

    if (((min = (double *) malloc(sizeof(double) * G->n))) == NULL) {
        fprintf(stderr, "set_all_edge(): ERROR [min = malloc()]\n");
        exit (EXIT_FAILURE);
    }
    // min[i]の全てにINFを設定
    for (i = 0; i < G->n; i++) min[i] = INF;

    if (((isin = (int *) malloc(sizeof(int) * (grids_node + 1)))) == NULL) {
        fprintf(stderr, "set_all_edge(): ERROR [isin = malloc()]\n");
        exit (EXIT_FAILURE);
    }
    for (i = 1; i <= grids_node; i++) isin[i] = isin_array(ls, label[i]);

    for (i = 1; i < G->n; i++) G->capa[i] = 0;
    source = grids_node * ls[0] + 1;
    sink = source + 1;

    setSource(G, source);
    setSink(G, sink);

    edge_count = 1;
    // source->i1
    s2i_begin = edge_count;
    for (i = 1; i <= grids_node; i++) {
        setEdge(G, edge_count, source, i, 0);
        if(isin[i]) {
            G->capa[edge_count] = INF;
        }
        if (min[i] > G->capa[edge_count]) min[i] = G->capa[edge_count];
        edge_count++;
    }

    // depth
    depth_begin = edge_count;
    for (i = 1; i <= grids_node; i++) {
        tail = i;
        head = i;
        for (j = 1; j < ls[0]; j++) {
            head = head + grids_node;
            setEdge(G, edge_count, tail, head, 0);
            if(isin[i]) {
                // これまでの
                // if (dterm == 0) G->capa[edge_count] = Dt(ls[j], &image, i / width, i % width) + near_nodes(i, j, height, width, ls, label, isin, T, lambda);
                if (dterm == 1) G->capa[edge_count] = Dt_9n(ls[j], raw_left, raw_right, calheight(width, i), calwidth(width, i), label_max);
                else    G->capa[edge_count] = D_function(ls[j], raw_left, raw_right, calheight(width, i), calwidth(width, i), label_max);
                
                G->capa[edge_count] += near_nodes(i, ls[j], height, width, label, isin, T, lambda);               
            }
            if (min[i] > G->capa[edge_count]) min[i] = G->capa[edge_count];
            edge_count++;
            tail = head;
        }
    }
    // ik->sink
    i2t_begin = edge_count;
    // rk = r(beta , label_size, grids_edge);
    for (i = 1; i <= grids_node; i++) {
        setEdge(G, edge_count, i + grids_node * (ls[0] - 1), sink, 0);
        if(isin[i]) {
            // G->capa[edge_count] = data_str(i, ls[ls[0]], image.width, image.left, image.right) + near_nodes(i, j, image.height, image.width , ls, label, isin, T, lambda);
            // if (dterm == 0) G->capa[edge_count] = Dt(ls[ls[0]], &image, i / width, i % width) + near_nodes(i, j, height, width , ls, label, isin, T, lambda);
            if (dterm == 1) G->capa[edge_count] = Dt_9n(ls[ls[0]], raw_left, raw_right, calheight(width, i), calwidth(width, i), label_max);
            else G->capa[edge_count] = D_function(ls[ls[0]], raw_left, raw_right, calheight(width, i), calwidth(width, i), label_max);
            
            G->capa[edge_count] += near_nodes(i, ls[j], height, width, label, isin, T, lambda);    
        }

        if (min[i] > G->capa[edge_count]) min[i] = G->capa[edge_count];
        edge_count++;
    }


    // reverce edge
    // depth
    for (i = 1; i <= grids_node; i++) {
        tail = i;
        head = i;
        for (j = 1; j < ls[0]; j++) {
            head = head + grids_node;
            setEdge(G, edge_count, head, tail, 0);
            if(isin[i]) {
                G->capa[edge_count] = INF;
            }
            edge_count++;
            tail = head;
        }
    }

    // new codes
    // horizonal
    for (i = 1; i <= height; i++) {
        for (j = 1; j < width; j++) {
            t_base =  (i - 1) * width + j;
            h_base =  t_base + 1;
            for (k = 1; k < ls[0]; k++) {
                tail = t_base + k * grids_node;
                for (l = 1; l < ls[0]; l++) {
                    head = h_base + l * grids_node;
                    setEdge(G, edge_count, tail, head, 0);
                    if(isin[t_base] && isin[h_base]) {
                        // head, tail in label_index
                        G->capa[edge_count] = phi(k + 1, l + 1, ls, T, lambda);
                    }
                    edge_count++;
                }
            }
        }
    }

    // vertical
    for (i = 1; i < height ; i++){
        for (j = 1; j < width + 1; j++) {
            t_base = (i - 1) * width + j;
            h_base = t_base + width;
            for (k = 1; k < ls[0]; k++) {
                tail = t_base + k * grids_node;
                for (l = 1; l < ls[0]; l++) {
                    head = h_base + l * grids_node;
                    setEdge(G, edge_count, tail, head, 0);
                    if(isin[t_base] && isin[h_base]) {
                        G->capa[edge_count] = phi(k + 1, l + 1, ls, T, lambda);
                        // head, tail in label_index
                    }
                    edge_count++;
                }
            }
        }
    }

    // new codes
    // horizonal
    for (i = 1; i <= height; i++) {
        for (j = 1; j < width; j++) {
            t_base =  (i - 1) * width + j;
            h_base =  t_base + 1;
            for (k = 1; k < ls[0]; k++) {
                tail = t_base + k * grids_node;
                for (l = 1; l < ls[0]; l++) {
                    head = h_base + l * grids_node;
                    setEdge(G, edge_count, head, tail, 0);
                    if(isin[t_base] && isin[h_base]) {
                        G->capa[edge_count] = phi(l + 1, k + 1, ls, T, lambda);
                    }
                    edge_count++;
                }
            }
        }
    }

    // vertical
    for (i = 1; i < height ; i++){
        for (j = 1; j < width + 1; j++) {
            t_base = (i - 1) * width + j;
            h_base = t_base + width;
            for (k = 1; k < ls[0]; k++) {
                tail = t_base + k * grids_node;
                for (l = 1; l < ls[0]; l++) {
                    head = h_base + l * grids_node;
                    setEdge(G, edge_count, head, tail, 0);
                    if(isin_array(ls, label[t_base]) && isin_array(ls, label[h_base])) {
                        G->capa[edge_count] = phi(l + 1, k + 1, ls, T, lambda);
                    }
                    edge_count++;
                }
            }
        }
    }

    //  s->tの一連の枝から定数値を引く処理
    for (i = s2i_begin; i <= grids_node; i++) {
        G->capa[i] -= min[i];
    }
    for (i = 1; i <= grids_node; i++) {
        G->capa[i + i2t_begin - 1] -= min[i];
    }
    current_edge = depth_begin;
    for (i = 1; i <= grids_node; i++) {
        for (j = 1; j < ls[0]; j++) {
            G->capa[current_edge] -=min[i];
            current_edge++;
        }
    }

    free(min);
    free(isin);

    // printf("total edge : %d\n", edge_count - 1);
    return edge_count - 1;
}

double err_rate(img *output, img *truth, int scale) {
    int i, error_count = 0;
    double err;
    //     // Gray(&truth, &truth);
    //     Gray(&(image.output), &(image.output));

    if (truth->data[0][0].r) {
        for(i = 1; i <= (output->height) * (output->width); i++) {
            if (abs(output->data[(i - 1) / output->width][(i - 1) % output->width].r - truth->data[(i - 1) / truth->width][(i - 1) % truth->width].r )
                >= scale + 1) {
                error_count++;
            }
        }
    } else {
        for(i = 1; i <= (output->height) * (output->width); i++) {
            if ((i - 1) / output->width >= scale && (i - 1) % output->width >= scale &&
                (i - 1) / output->width <= output->height - scale && (i - 1) % output->width <= output->width - scale) {
                if (abs(output->data[(i - 1) / output->width][(i - 1) % output->width].r - truth->data[(i - 1) / truth->width][(i - 1) % truth->width].r )
                    >= scale + 1) {
                    error_count++;
                }
            }
        }
    }

    err = 100 * error_count / (double)(truth->height * truth->width);
    return err;
}

double data_str(int i, int label, int width, int *I_left, int *I_right) {
    double data = 0;
    // return 1.0 * dabs(label, I_left[i]);
    //leftの中のものがどこにあるか
    if ((i - 1) / width == (i - label - 1) / width) {
        // data = (I_left[i] - I_right[i - label]) * (I_left[i] - I_right[i - label]);
        data = (I_left[i] - I_right[i - label]);
    }else data = INF;

    return abss(data);
    // return sqrt(data);
}

void set_capacity_abswap(Graph *G, int *label, int alpha, int beta, double T, int lambda, img *raw_left, img *raw_right, int label_max) {
    int i, s2i_begin, i2t_begin, grids_node;
    // int height = raw_left->height;
    int width = raw_left->width;
    // int alpha = pairs[0];
    // int beta = pairs[1];
    double A, B, C, D, temp;
    //　source->各ノード　を示す枝の開始位置
    
    s2i_begin = G->m - 2 * (G->n - 2) + 1;
    
    //　各ノード->sink　を示す枝の開始位置
    i2t_begin = G->m - (G->n - 2) + 1;

    // G中のノードのうちsource, sinkを除く画像部分のノードの総数
    grids_node = G->n - 2;

    // set Dterm
    for(i = 1; i < G->n - 1; i++){
        if (label[i] == alpha || label[i] == beta) {
            if(dterm == 0) {
                temp =  D_function(alpha, raw_left, raw_right, calheight(width, i), calwidth(width, i), label_max) -  D_function(beta, raw_left, raw_right, calheight(width, i), calwidth(width, i), label_max);
                // temp = Dt(label[i], image, calheight(width, i), calwidth(width, i)) - Dt(alpha, image, calheight(width, i), calwidth(width, i));
            } else {
                temp = Dt_9n(alpha, raw_left, raw_right, calheight(width, i), calwidth(width, i), label_max) - Dt_9n(beta, raw_left, raw_right, calheight(width, i), calwidth(width, i), label_max);
                // temp = data(i, label[i], image.height, image.width, image.left, image.right) - data(i, alpha, image.height, image.width, image.left, image.right);
            }
            // temp += near_nodes(i, alpha, image.height, image.width, pairs, label, T, lambda) - near_nodes(i, beta, image.height, image.width, pairs, label, T, lambda);

            if(temp > 0){
                G->capa[s2i_begin - 1 + i] += temp;
            } else {
                G->capa[i2t_begin - 1 + i] -= temp;
            }
        }
    }
    // set Vterm
    for(i = 1; i < s2i_begin; i++) {
        //head,tailが共にalpha->枝を削除
        if ((label[G->tail[i]] == alpha || label[G->tail[i]] == beta) && (label[G->head[i]] == alpha || label[G->head[i]] == beta)) {
            A = pairwise(alpha, alpha, T, lambda);
            B = pairwise(alpha, beta, T, lambda);
            C = pairwise(beta, alpha, T, lambda);
            D = pairwise(beta, beta, T, lambda);
            //打ち切り
            A = fmin(A, B + C - D);
            // A = fmin(A, B + C - D);
            G->capa[i] = B + C - A - D;

            if(G->capa[i] < 0) {
                printf("劣モジュラではない\n");
                exit(EXIT_FAILURE);
            }

            // source->i
            temp = B - D;
            if (temp > 0) {
                G->capa[s2i_begin - 1 + G->tail[i]] += temp;
            } else {
                G->capa[i2t_begin - 1 + G->tail[i]] -= temp;
            }
            
            // i->sink
            temp = B - A;
            if(temp > 0) {
                G->capa[i2t_begin - 1 + G->head[i]] += temp;
            } else {
                G->capa[s2i_begin - 1 + G->head[i]] -= temp;
            }

        } else {
            if ((label[G->tail[i]] == alpha || label[G->tail[i]] == beta) && (label[G->head[i]] != alpha && label[G->head[i]] != beta)) {
                //tail側がαもしくはβに含まれるとき
                temp = pairwise(alpha, label[G->head[i]], T, lambda) - pairwise(beta, label[G->head[i]], T, lambda);
                if (temp > 0) {
                    G->capa[s2i_begin - 1 + G->tail[i]] += temp;
                } else {
                    G->capa[i2t_begin - 1 + G->tail[i]] -= temp;
                }

            } else if ((label[G->tail[i]] != alpha && label[G->tail[i]] != beta) && (label[G->head[i]] == alpha || label[G->head[i]] == beta)) {
                //ｈｅａｄ側がαもしくはβに含まれるとき
                temp = pairwise(label[G->tail[i]], alpha, T, lambda) - pairwise(label[G->tail[i]], beta, T, lambda);
                if (temp > 0) {
                    G->capa[s2i_begin - 1 + G->head[i]] += temp;
                } else {
                    G->capa[i2t_begin - 1 + G->head[i]] -= temp;
                }

            }
            deleteAdjEdge(G, i);
        }
    }
    // 各 s->i->tに対し、capacityが小さいEdgeのcapacityを、両方のcapacityから引く(高速化処理)
    for (i = s2i_begin; i < i2t_begin; i++) {
        temp = fmin(G->capa[i], G->capa[i + grids_node]);
        G->capa[i] -= temp;
        G->capa[i + grids_node] -= temp;
    }
    return;
}

void set_abswap_edge(Graph *G, int height, int width) {
    int i, j, tail, head, grids_node, source, sink, edge_count;

    grids_node = height * width;
    edge_count = 1;

    // horizontal
    for (i = 1; i < height + 1; i++) {
        for (j = 1; j < width; j++) {
            tail = (i - 1) * width + j;
            head = (i - 1) * width + j + 1;
            setEdge(G, edge_count, tail, head, 0);
            edge_count++;
        }
    }

    // vertical
    for (i = 1; i < height ; i++) {
        for (j = 1; j < width + 1; j++) {
            tail = (i - 1) * width + j;
            head = i * width + j;
            setEdge(G, edge_count, tail, head, 0);
            edge_count++;
        }
    }

    source = grids_node + 1;
    sink = source + 1;

    setSource(G, source);
    setSink(G, sink);
    for (i = 1; i <= grids_node; i++) {
        setEdge(G, edge_count, source, i, 0);
        setEdge(G, edge_count + G->n - 2, i, sink, 0);
        edge_count++;
    }
    return;
}
