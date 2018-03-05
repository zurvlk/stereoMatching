#include "bmp.h"
#include "a_estr.h"
#include "graph.h"
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <string.h>
#define INF 10000000


int calheight(int width, int node) {
	if (node % width) return node / width;
	else return node / width - 1;
}

int calwidth(int width, int node) {
	if (node % width) return node % width - 1;
	else return width - 1;
}


double readStrBmp(Image *image, char filename[], int scale) {
    int i, j, grids_node;
    char imgleft[100];
    char imgright[100];
    char imgtruth[100];

    strcpy(imgleft, filename);
    strcpy(imgright, filename);
    strcpy(imgtruth, filename);

    strcat(imgleft, "_left.bmp");
    strcat(imgright, "_right.bmp");
    strcat(imgtruth, "_truth.bmp");

    // Read Bitmap
    image->raw_left = (img *)malloc(sizeof(img));
    image->raw_right = (img *)malloc(sizeof(img));
    image->output = (img *)malloc(sizeof(img));
    image->truth = (img *)malloc(sizeof(img));

    ReadBmp(imgleft, (image->raw_left));
    ReadBmp(imgright, (image->raw_right));
    ReadBmp(imgtruth, (image->truth));
    ReadBmp(imgtruth, (image->output));

    image->width = image->raw_left->width;
    image->height = image->raw_left->height;
    grids_node = image->height * image->width;

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
    return grids_node;
}

double dabs(double a, double b) {
    return a - b > 0 ? a - b : b - a;
}
double fmin(double i, double j) {
    return i < j  ? i : j;
}

double fmin3(double i, double j, double k) {
    return fmin(fmin(i, j), k);
}

double between(double a, double b, double c){
    // if((a <= b && b <= c) || (c <= b && b <= a))    return 0;
    if (( isgreater(b, a) && isgreater(b, c) ) || ( isless(b, a) && isless(b, c))) return 0;
    else return 1;
}

double pairwise(double i, double j, double T, int lambda) {
    if (vterm == 0) return lambda * fmin((i - j) * (i - j), T * T);
    else {
        if (dabs(i, j) <= T) return lambda  * dabs(i, j);
        else return dabs(i, j) + (lambda - 1) * T;
    }   
}


double data_single(int i, int label, int height, int width, int *I_left, int *I_right) {
    double data = 0;
    int p_height = calheight(width, i);

    if (p_height == calheight(width, i - label)) {
        data += (I_left[i] - I_right[i - label]) * (I_left[i] - I_right[i - label]);
    }else data += INF;
    return data;
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
    if(x < 0 || x > max_label) return INF;
	if(j - x < 0) return INF;
	
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

double data(int i, int label, int height, int width, int *I_left, int *I_right) {
    double data = 0;
    int p_height, p_width;

    p_height = calheight(width, i);
    p_width = calwidth(width, i);
    // return 1.0 * dabs(label, I_left[i]);
    // leftの中のものがどこにあるか

    data += data_single(i, label, height, width, I_left, I_right);

    if (p_height == 0) {
        if (p_width != 0) {
            data += data_single(i - 1, label, height, width, I_left, I_right);
            data += data_single(i - 1 + width, label, height, width, I_left, I_right);
        } else if (p_width != width - 1) {
            data += data_single(i + 1, label, height, width, I_left, I_right);
            data += data_single(i + 1 + width, label, height, width, I_left, I_right);
        }
        data += data_single(i + width, label, height, width, I_left, I_right);
    } else if (p_height == height - 1) {
        if (p_width != 0) {
            data += data_single(i - 1, label, height, width, I_left, I_right);
            data += data_single(i - 1 - width, label, height, width, I_left, I_right);
        } else if (p_width != width - 1) {
            data += data_single(i + 1, label, height, width, I_left, I_right);
            data += data_single(i + 1 - width, label, height, width, I_left, I_right);
        }
        data += data_single(i - width, label, height, width, I_left, I_right);
    } else {
        if (p_width != 0) {
            data += data_single(i - 1, label, height, width, I_left, I_right);
            data += data_single(i - 1 - width, label, height, width, I_left, I_right);
            data += data_single(i - 1 + width, label, height, width, I_left, I_right);
        } else if (p_width != width - 1) {
            data += data_single(i + 1, label, height, width, I_left, I_right);
            data += data_single(i + 1 - width, label, height, width, I_left, I_right);
            data += data_single(i + 1 + width, label, height, width, I_left, I_right);
        }
        data += data_single(i - width, label, height, width, I_left, I_right);
        data += data_single(i + width, label, height, width, I_left, I_right);
    }

    return sqrt(data);
}

double D_term(int x, img *tmp_l2, img *tmp_r2, int i, int j, int max_label){
	double sum = 0;
	int n, m;
	
	// if(x == -1 || x == max_label) return INF;
    if(x < 0 || x > max_label) return INF;
	if(j - x < 0) return INF;
	
	if(i - 1 < 1){
		if(j - 1 < 1){
			for(n = i; n <= i + 1; n++){
				for(m = j; m <= j + 1; m++){
					sum += fabs(tmp_l2->data[n][m].r - tmp_r2->data[n][m - x].r) * fabs(tmp_l2->data[n][m].r - tmp_r2->data[n][m - x].r);
				}
			}
		}else if(j + 1 > tmp_l2->width){
			for(n = i; n <= i + 1; n++){
				for(m = j - 1 ; m <= j; m++){
					sum += fabs(tmp_l2->data[n][m].r - tmp_r2->data[n][m - x].r) * fabs(tmp_l2->data[n][m].r - tmp_r2->data[n][m - x].r);
				}
			}
		}else{
			for(n = i; n <= i + 1; n++){
				for(m = j - 1; m <= j + 1; m++){
					sum += fabs(tmp_l2->data[n][m].r - tmp_r2->data[n][m - x].r) * fabs(tmp_l2->data[n][m].r - tmp_r2->data[n][m - x].r);
				}
			}
		}
	}else if(i + 1 > tmp_l2->height){
		if(j - 1 < 1){
			for(n = i - 1; n <= i; n++){
				for(m = j; m <= j + 1; m++){
					sum += fabs(tmp_l2->data[n][m].r - tmp_r2->data[n][m - x].r) * fabs(tmp_l2->data[n][m].r - tmp_r2->data[n][m - x].r);
				}
			}
		}else if(j + 1 > tmp_l2->width){
			for(n = i - 1; n <= i; n++){
				for(m = j - 1; m <= j; m++){
					sum += fabs(tmp_l2->data[n][m].r - tmp_r2->data[n][m - x].r) * fabs(tmp_l2->data[n][m].r - tmp_r2->data[n][m - x].r);
				}
			}
		}else {
			for(n = i - 1; n <= i; n++){
				for(m = j - 1; m <= j + 1; m++){
					sum += fabs(tmp_l2->data[n][m].r - tmp_r2->data[n][m - x].r) * fabs(tmp_l2->data[n][m].r - tmp_r2->data[n][m - x].r);
				}
			}
		}
	}else{
		if(j - 1 < 1){
			for(n = i - 1; n <= i + 1; n++){
				for(m = j; m <= j + 1; m++){
					sum += fabs(tmp_l2->data[n][m].r - tmp_r2->data[n][m - x].r) * fabs(tmp_l2->data[n][m].r - tmp_r2->data[n][m - x].r);
				}
			}
		}else if (j + 1 > tmp_l2->width){
			for(n = i - 1; n <= i + 1; n++){
				for(m = j - 1; m <= j; m++){
					sum += fabs(tmp_l2->data[n][m].r - tmp_r2->data[n][m - x].r) * fabs(tmp_l2->data[n][m].r - tmp_r2->data[n][m - x].r);
				}
			}
		}else {
			for(n = i - 1; n <= i + 1; n++){
				for(m = j - 1; m <= j + 1; m++){
					sum += fabs(tmp_l2->data[n][m].r - tmp_r2->data[n][m - x].r) * fabs(tmp_l2->data[n][m].r - tmp_r2->data[n][m - x].r);
				}
			}
		}
	}
	//sum = sum * sum;
	return sqrt(sum);

}
double Dt(int x, Image *image, int i, int j) {
    double d_m, d_l, d_r, d_m2, d_l2, d_r2;
    double  I, I_1 = 0, I_2 = 0;

	if(x < 0 || x > image->label_max ) return INF;
    if (j - x < 0 && calinf) return INF;

    int flag = 1;
    while(flag){

	    d_m = image->raw_left->data[i][j].r;
	    d_m2 =  image->raw_right->data[i][j - x].r;
        // D_fwd
	    if((j - x - 1 >= 0) && (j - x + 1 <= image->raw_right->width - 1)){

	        d_l2 = (image->raw_right->data[i][j - x].r + image->raw_right->data[i][j - x - 1].r) / 2.0;
            d_r2 = (image->raw_right->data[i][j - x].r + image->raw_right->data[i][j - x + 1].r) / 2.0;
	        if(between(d_l2, d_m, d_m2) == 0)    break;
	        if(between(d_r2, d_m, d_m2) == 0)    break;
	        I_1 = fmin3(fabs(d_m - d_l2), fabs(d_m - d_m2), fabs(d_m - d_r2));              /*特に制約なし*/

	    }else if(j - x - 1 < 0){

	        d_r2 = (image->raw_right->data[i][j - x].r + image->raw_right->data[i][j - x + 1].r) / 2.0;
	        if(between(d_m2, d_m, d_r2) == 0)    break;
	        I_1 = fmin(fabs(d_m - d_m2), fabs(d_m - d_r2));                                /*Rの左端が出る*/

	    }else{

	        d_l2 = (image->raw_right->data[i][j - x].r + image->raw_right->data[i][j - x - 1].r) / 2.0;
	        if(between(d_l2, d_m, d_m2) == 0)    break;
	        I_1 = fmin(fabs(d_m - d_l2), fabs(d_m - d_m2));                                /*Rの右端が出る*/
	    }

        // D_rev
	    if((j != 0) && (j != image->raw_left->width - 1)){

	        d_l = (image->raw_left->data[i][j].r + image->raw_left->data[i][j - 1].r) / 2.0;
	        d_r = (image->raw_left->data[i][j].r + image->raw_left->data[i][j + 1].r) / 2.0;
	        if(between(d_l, d_m2, d_m) == 0)    break;
	        if(between(d_r, d_m2, d_m) == 0)    break;
	        I_2 = fmin3(fabs(d_l - d_m2), fabs(d_m - d_m2), fabs(d_r - d_m2));              /*特に制約なし*/

	    }else if(j == 0){

	        d_r = (image->raw_left->data[i][j].r + image->raw_left->data[i][j + 1].r) / 2.0;
	        if(between(d_m, d_m2, d_r) == 0)    break;
	        I_2 = fmin(fabs(d_m - d_m2), fabs(d_r - d_m2));                                /*Lの左端が出る*/

	    }else{

	        d_l = (image->raw_left->data[i][j].r + image->raw_left->data[i][j - 1].r) / 2.0;
	        if(between(d_l, d_m2, d_m) == 0)    break;
	        I_2 = fmin(fabs(d_l - d_m2), fabs(d_m - d_m2));                                /*Lの右端が出る*/
	    }
	    flag = 0;
	}
    I = fmin(I_1 / (double)image->scale, I_2 / (double)image->scale);
    I = fmin(I, 20);
    return I;
    //I *= I;
    //return I * I;                                   /*Dを2乗する*/
    //return C_D * (I);

}

double D_function(int x, img *tmpL, img *tmpR, int h, int w, int maxLabel) {

	int q = w - x;
	double Ip, Iq, Ip_l, Ip_r, Iq_r, Iq_l, fwd, rev;

	if ( (x < 0) || (x > maxLabel) || q < 0 ) { return INF; }

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


double energy(Graph *G, int *label,  double T, int lambda, Image image) {

    double energy = 0;
    //* Dterm
    if (dterm == 1) {
        for (int i = 1; i <= G->n - 2; i++) {
            energy += Dt_9n(label[i], image.raw_left, image.raw_right, calheight(image.width, i), calwidth(image.width, i), image.label_max);
            
            // energy += data(i, label[i], image.height, image.width, image.left, image.right);
        // energy += data(I_left[i], label[i]);
        }
    } else {
        for(int i = 1; i <= G->n - 2; i++) {
            // energy += Dt(label[i], &image, calheight(image.width, i), calwidth(image.width, i));
            energy += D_function(label[i], image.raw_left, image.raw_right, calheight(image.width, i), calwidth(image.width, i), image.label_max);
        }
    }
    // Vterm
    for (int i = 1; i <= G->m - 2 * (G->n - 2); i++) {
        energy += pairwise(label[G->tail[i]], label[G->head[i]], T, lambda);
    }
    return energy;
}

void set_capacity(Graph *G, int *label, int alpha, double T, int lambda, Image image) {
    int i, s2i_begin, i2t_begin, grids_node, min1, max1, min2, max2;
    double A, B, C, D, temp;
    //　source->各ノード　を示す枝の開始位置
    s2i_begin = G->m - 2 * (G->n - 2) + 1;
    //　各ノード->sink　を示す枝の開始位置
    i2t_begin = G->m - (G->n - 2) + 1;
    // G中のノードのうちsource, sinkを除く画像部分のノードの総数
    grids_node = G->n - 2;

    for (i = 0; i <= G->m + 1; i++) G->capa[i] = 0;
    for (i = s2i_begin; i < i2t_begin; i++) {
        //labelがalphaでない->capacity設定s2i
        //labelがalpha->枝を削除
        if (label[G->head[i]] == alpha) {
            deleteAdjEdge(G, i);
            deleteAdjEdge(G, i + grids_node);
        }
    }

    // set Dterm
    for(i = 1; i < G->n - 1; i++){
        if (trancation) {
            min1 = label[i];
            max1 = alpha;
        } else {
            min1 = alpha > label[i] ? label[i] : alpha;
            max1 = alpha < label[i] ? label[i] : alpha;
        }
        if(dterm == 0) {
            temp =  D_function(min1, image.raw_left, image.raw_right, calheight(image.width, i), calwidth(image.width, i), image.label_max) -  D_function(max1, image.raw_left, image.raw_right, calheight(image.width, i), calwidth(image.width, i), image.label_max);
            // temp =  D_function(label[i], &image.raw_left, &image.raw_right, calheight(image.width, i), calwidth(image.width, i), image.label_max) -  D_function(alpha, &image.raw_left, &image.raw_right, calheight(image.width, i), calwidth(image.width, i), image.label_max);
            
            // temp = Dt(label[i], &image, calheight(image.width, i), calwidth(image.width, i)) - Dt(alpha, &image, calheight(image.width, i), calwidth(image.width, i));
        } else {
            temp = Dt_9n(min1, image.raw_left, image.raw_right, calheight(image.width, i), calwidth(image.width, i), image.label_max) - Dt_9n(max1, image.raw_left, image.raw_right, calheight(image.width, i), calwidth(image.width, i), image.label_max);
            // temp = Dt_9n(label[i], &image.raw_left, &image.raw_right, calheight(image.width, i), calwidth(image.width, i), image.label_max) - Dt_9n(alpha, &image.raw_left, &image.raw_right, calheight(image.width, i), calwidth(image.width, i), image.label_max);
            
            // temp = data(i, label[i], image.height, image.width, image.left, image.right) - data(i, alpha, image.height, image.width, image.left, image.right);
        }

        if(temp > 0){
            G->capa[s2i_begin - 1 + i] += temp;
        }else{
            G->capa[i2t_begin - 1 + i] -= temp;
        }
    }
    // set Vterm
    for(i = 1; i < s2i_begin; i++) {
        //head,tailが共にalpha->枝を削除
        if (label[G->tail[i]] != alpha && label[G->head[i]] != alpha) {
            if (trancation) {
                A = pairwise(label[G->tail[i]], label[G->head[i]], T, lambda);
                B = pairwise(label[G->tail[i]], alpha, T, lambda);
                C = pairwise(alpha, label[G->head[i]], T, lambda);
                D = pairwise(alpha, alpha, T, lambda);

                A = fmin(A, B + C - D);
                G->capa[i] = B + C - A - D;

                if(G->capa[i] < 0) {
                    printf("劣モジュラではない\n");
                    exit(EXIT_FAILURE);
                }
            } else {

                min1 = alpha > label[G->tail[i]] ? label[G->tail[i]] : alpha;
                min2 = alpha > label[G->head[i]] ? label[G->head[i]] : alpha;
                max1 = alpha < label[G->tail[i]] ? label[G->tail[i]] : alpha;
                max2 = alpha < label[G->head[i]] ? label[G->head[i]] : alpha;

                A = pairwise(min1, min2, T, lambda);
                B = pairwise(min1, max2, T, lambda);
                C = pairwise(max1, min2, T, lambda);
                D = pairwise(max1, max2, T, lambda);

                

                if ((min1 == alpha && min2 == alpha)) {
                    A = fmin(A, B + C - D);
                } else if (max1 == alpha && max2 == alpha) {
                    D = fmin(D, B + C - A);
                } else if (min1 == alpha && max2 == alpha) {
                    B = fmax(B, A + D - C);
                } else {
                    C = fmax(C, A + D - B);
                }
                //     A = pairwise(min1, min2, T, lambda);
                //     B = pairwise(min1, max2, T, lambda);
                //     C = pairwise(max1, min2, T, lambda);
                //     D = pairwise(max1, max2, T, lambda);
                //     if(A + D < B + C)
                //     D = fmax(D, B + C - A);
                // } 
                G->capa[i] = B + C - A - D;

                if(G->capa[i] < 0) {
                    printf("劣モジュラではない\n");
                    exit(EXIT_FAILURE);
                }
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

void set_all_edge(Graph *G, int height, int width) {
    int i, j, tail, head, grids_node, source, sink, edge_count;

    grids_node = height * width;
    edge_count = 1;

    // horizontal
    for (i = 1; i < height + 1; i++){
        for (j = 1; j < width; j++){
            tail = (i - 1) * width + j;
            head = (i - 1) * width + j + 1;
            setEdge(G, edge_count, tail, head, 0);
            edge_count++;
        }
    }

    // vertical
    for (i = 1; i < height ; i++){
        for (j = 1; j < width + 1; j++){
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

double err_rate(img output, Image image) {
    int i, error_count = 0;
    double err;
    if (image.truth->data[0][0].r) {
        for(i = 1; i <= (image.output->height) * (image.output->width); i++) {
            if (abs(image.output->data[(i - 1) / image.output->width][(i - 1) % image.output->width].r - image.truth->data[(i - 1) / image.truth->width][(i - 1) % image.truth->width].r )
                >= image.scale + 1) {
                error_count++;
            }
        }
    } else {
        for(i = 1; i <= (image.output->height) * (image.output->width); i++) {
            if ((i - 1) / image.output->width >= image.scale && (i - 1) % image.output->width >= image.scale &&
                (i - 1) / image.output->width <= image.output->height - image.scale && (i - 1) % image.output->width <= image.output->width - image.scale) {
                if (abs(image.output->data[(i - 1) / image.output->width][(i - 1) % image.output->width].r - image.truth->data[(i - 1) / image.truth->width][(i - 1) % image.truth->width].r )
                    >= image.scale + 1) {
                    error_count++;
                }
            }
        }
    }

    err = 100 * error_count / (double)(image.height * image.width);
    return err;
}
