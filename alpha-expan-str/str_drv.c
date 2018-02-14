#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bmp.h"
#include "a_estr.h"
#include "graph.h"
#include "ford_fulkerson.h"

#define _CHK_ENERGY_ 0      // ラベル更新時にエネルギーの減少を確認す　0:確認しない 1:確認する
#define _OUTPUT_INFO_ 0     // デバッグ情報出力 0:出力しない 1:出力する
#define _OUTPUT_GRAPH_ 0    // グラフ情報出力 　0:出力しない 1:出力する
#define _OUTPUT_PROGRESS_ 0 // 処理過程ファイル出力 　0:出力しない 1:出力する

int main(int argc, char *argv[]) {
    long int i, j, node, edge, grids_node;
    int scale = 16, alpha, label_size, last, chk;
    int *t, *label;
    int lambda = 1;
    int init_label = 5;
    // I->入力画像の輝度, t->2値変数, label->ラベル付け
    double flag, temp, error, T = 9;
    double *f;
    char output_file[100];
    clock_t start;
    Image image;
    Graph G;

#if _OUTPUT_INFO_
    double maxflow;
#endif

    dterm = 0;
    vterm = 0;
    calinf = 1;
    trancation = 0;

#if _OUTPUT_PROGRESS_
    int k = 0;
    char pf[100];
    system("rm output/*.bmp &> /dev/null");
#endif


    if (argc != 2 && argc != 3 && (argc < 5 && argc > 10)) {
        printf("argc: %d\n", argc);
        printf("Usage: %s <input_file> <output_file> <scale> <lambda> <T> <dterm> <vterm> <init_label> <truncation>\n", argv[0]);
        return 1;
    }

    if (argc == 2) strcpy(output_file, "/dev/null");
    else strcpy(output_file, argv[2]);
    if (argc >= 5 && argc <= 10) {
        scale = atoi(argv[3]);
        lambda = atoi(argv[4]);
        if (argc >= 6) T = atof(argv[5]);
        if (argc >= 7) dterm = atoi(argv[6]);
        if (argc >= 8) vterm = atoi(argv[7]);
        if (argc >= 9) init_label = atoi(argv[8]);
        if (argc >= 10) trancation = atoi(argv[9]);
    }


    label_size = 256 / scale;
    image.scale = scale;
    image.label_max = label_size - 1;

    if (argc == 2) strcpy(output_file, "/dev/null");
    else strcpy(output_file, argv[2]);
    grids_node = readStrBmp(&image, argv[1], scale);

    printf("----------------------------------------------\n");
    printf("alpha-expansion %s\n", trancation == 1 ? "(truncation)" : "(CH)");
    printf("input_file: %s\n", argv[1]);
    printf("output_file: %s\n", output_file);
    printf("label_size: %d\n", label_size);
    printf("lambda: %d\n", lambda);
    printf("T: %.2f\n", pairwise(T, 0, 1000000, 1));
    printf("Dterm: ");
    if(dterm == 0) printf("Birchfield and Tomasi’s function\n");
    else if(dterm == 1) printf("normal\n");
    else {
        fprintf(stderr, "error! dterm is 0 or 1: %d\n", dterm);
        exit(EXIT_FAILURE);
    }
    printf("Vterm: ");
    if(vterm == 0) printf("Vpq(fp, fq) = (fp - fq)^2\n");
    else if(vterm == 1) printf("piecewise linear function\n");
    else {
        fprintf(stderr, "error! vterm is 0 or 1 : %d\n", vterm);
        exit(EXIT_FAILURE);
    }

    node = grids_node + 2;
    edge = (image.height - 1) * image.width + image.height * (image.width - 1) + 2 * grids_node;

    // グラフ初期設定
    newGraph(&G, node, edge);
    set_all_edge(&G, image.height, image.width);
    initAdjList(&G);

    if ((f = (double *) malloc(sizeof(double) * (G.m + 1))) == NULL) {
        fprintf(stderr, "main(): ERROR [f = malloc()]\n");
        return (EXIT_FAILURE);
    }
    if ((t = (int *) malloc(sizeof(int) * (G.n + 1))) == NULL) {
        fprintf(stderr, "main(): ERROR [t = malloc()]\n");
        return (EXIT_FAILURE);
    }
    if ((label = (int *) malloc(sizeof(int) * (G.n - 1))) == NULL) {
        fprintf(stderr, "main(): ERROR [label = malloc()]\n");
        return (EXIT_FAILURE);
    }
    // 初期ラベル設定
    for (i = 1; i <= G.n - 2; i++) label[i] = init_label;
    printf("Energy (before): %lf\n", energy(&G, label, T, lambda, image));

    start = clock();
    last = label_size;
    flag = 1;
    int c = 0;
    while (flag > 0) {
        temp = energy(&G, label, T, lambda, image);
        for (alpha = 0; alpha <= label_size; alpha++) {
            if(last == alpha) break;
            for (i = 0; i <= G.m + 1; i++) {
                f[i] = 0;
                G.capa[i] = 0;
            }

            // capacity設定
            set_capacity(&G, label, alpha, T, lambda, image);
            // capacity(&G, label, I, alpha);
            c++;
#if _OUTPUT_INFO_
            printf("Maximum Flow: %5.2lf\n", boykov_kolmogorov(G, f, t));
            printf("after bk-max-f\n");
            showAdjList(&G);
#else
            boykov_kolmogorov(G, f, t);
#endif
            //削除された枝はvoykov_kolmogorov(G, f, t)で再度リストに追加されている
            if (trancation) {
                // tを基にラベル更新
                for (i = 1; i <= G.n - 2; i++) {
                    if(t[i] == 1) {
                        label[i] = alpha;
                        last = alpha;
                    }
                }
            } else {
                for (i = 1; i <= G.n - 2; i++) {
                    chk = label[i];
                    if(t[i] == 1) {
                        label[i] = alpha > label[i] ? alpha : label[i] ;
                        last = alpha;
                    } else {
                        label[i] = alpha < label[i] ? alpha : label[i];
                    }
                    if (chk != label[i]) last = alpha;
                }
            }

#if _OUTPUT_PROGRESS_
            for (i = 0; i <  image.height; i++) {
                for (j = 0; j < image.width; j++) {
                    image.output->data[i][j].r = label[i * image.width + j + 1] * scale;
                    image.output->data[i][j].g = image.output->data[i][j].r;
                    image.output->data[i][j].b = image.output->data[i][j].r;
                }
            }
            sprintf(pf, "output/image_%04d.bmp", k);
            WriteBmp(pf, image.output);
            k++;
#endif
        }
        flag = temp - energy(&G, label, T, lambda, image);
        // printf("Energy %lf\n", temp - flag);
    }

    printf("ita: %d\n", c);
    calinf = 0;
    printf("Energy (after): %lf\n", energy(&G, label, T, lambda, image));
    printf("Run time[%.2lf]\n", (double) (clock() - start) / CLOCKS_PER_SEC);

#if _OUTPUT_GRAPH_
    printf("---Graph information---\n");
    showGraph(&G);
#endif

    // output to bitmap file
    for (i = 0; i <  image.height; i++) {
        for (j = 0; j < image.width; j++) {
            image.output->data[i][j].r = label[i * image.width + j + 1] * scale;
            image.output->data[i][j].g = image.output->data[i][j].r;
            image.output->data[i][j].b = image.output->data[i][j].r;
        }
    }
    WriteBmp(output_file, image.output);

#if _OUTPUT_PROGRESS_
    if (system("cd output && yes |./mkmovie.sh") == -1) {
      printf("動画の出力に失敗しました\n");
    }
#endif

    if (strcmp(output_file, "/dev/null") != 0){
        ReadBmp(output_file, (image.output));
        // Gray(&truth, &truth);
        Gray((image.output), (image.output));
        error = err_rate(*image.output, image);
        printf("Error rate : %lf\n", error);
    }

    // プログラム終了時のfreeは必要ないかも
    delGraph(&G);
    free(image.left);
    free(image.right);
    free(f);
    free(t);
    free(label);

    return 0;
}
