/****************************************************************************/
/*                                                                          */
/*         bmp.c: bmp �ե����������Υ饤�֥���                                   */
/*                                                                          */
/*            Kazutoshi Ando (Shizuoka Univ.)                               */
/*                                                                          */
/*                 Ver. 2004.11.30                                          */
/*                     WriteBmp: �إå������η���������.                      */
/*                 Ver. 2004.11.26                                          */
/*                     Diminish: ��Ĵ(?)�롼�������ɲ�.                      */
/*                     PrintBmpInfo: bmp�ե������ʳ��Υե��������ɤ����Ȥ���,     */
/*                                   ���顼�����Ϥ����褦���ѹ�.                */
/*                 Ver. 2004.08.20                                          */
/*                     ReadBmp: 24bit����bmp�ե������ʳ��Υե�������             */
/*                              �ɤ����Ȥ���, ���顼�����Ϥ����褦���ѹ�.         */
/*                     PrintBmpInfo: ��ʿ, ��ľ�����٤����Ϥ����褦���ѹ�.      */
/*                     WriteBmp: �إå������η���������.                      */
/*                 Ver. 2004.08.18                                          */
/*                     Gray ���ɲ�.                                          */
/*                 Ver. 2004.08.17                                          */
/*                     4byte �����˹��碌�뤿���η׻���������,                 */
/*                     Rotate90, Shrink, Mosaic ���ɲ�.                      */
/*                     ���顼���å������ν�������ɸ�२�顼���Ϥ��ѹ�.            */
/*                 Ver. 2003.11.04                                          */
/*                 Ver. 2010.10.19                                          */
/*                     long int �� int �ǽ��Ϥ��Ƥ����ȷٹ𤬤Ǥ��Τǽ���        */
/*                                                                          */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"

unsigned char Bmp_headbuf[HEADERSIZE]; /* �إå�����Ǽ���뤿�����ѿ� */
unsigned char Bmp_Pallet[PALLETSIZE];  /* ���顼�ѥ��åȤ���Ǽ */

char Bmp_type[2];                      /* �ե����륿���� "BM" */
unsigned long Bmp_size;                /* bmp�ե������Υ�����(�Х���) */
unsigned int Bmp_info_header_size;     /* �����إå��Υ����� = 40 */
unsigned int Bmp_header_size;          /* �إå������� = 54 */
unsigned short Bmp_planes;             /* �ץ졼���� ����1 */
unsigned short Bmp_color;              /* �� (�ӥå�) 24 */
long Bmp_comp;                         /* ������ˡ 0 */
long Bmp_image_size;                   /* ������ʬ�Υե����륵����(�Х���) */
long Bmp_xppm;                         /* ��ʿ������ (ppm) */
long Bmp_yppm;                         /* ��ľ������ (ppm) */

/*
	�ؿ�̾: ReadBmp
	����  : char *filename, img *imgp
	�֤���: void
	ư��  :	bmp�����Υե����� filename �򳫤���, ���β����ǡ�����
	       	2�������� imgp->data �˳�Ǽ����. Ʊ����, �إå������ɤ߹��ޤ줿
	       	���������ȹ⤵�򥰥����Х��ѿ� Bmp_width ��Bmp_height �˥��åȤ���.
*/
void ReadBmp(char *filename, img *imgp) {
  int i,j;
  int Real_width;
  FILE *Bmp_Fp=fopen(filename,"rb");	/* �Х��ʥ��⡼���ɤ߹����Ѥ˥����ץ� */
  unsigned char *Bmp_Data;						/* �����ǡ�����1��ʬ��Ǽ */

  if(Bmp_Fp==NULL){
    fprintf(stderr,"Error: file %s couldn\'t open for read!.\n",filename);
    exit(1);
  }

  /* �إå��ɤ߹��� */
  fread(Bmp_headbuf,sizeof(unsigned char),HEADERSIZE,Bmp_Fp);

	memcpy(&Bmp_type, Bmp_headbuf, sizeof(Bmp_type));

	if (strncmp(Bmp_type, "BM", 2) != 0){
		fprintf(stderr,"Error: %s is not a bmp file.\n",filename);
		exit(1);
	}

	memcpy(&imgp->width, Bmp_headbuf+18, 4);
	memcpy(&imgp->height, Bmp_headbuf+22, 4);
	memcpy(&Bmp_color, Bmp_headbuf+28, 4);

	if ( (Bmp_color != 24) && (Bmp_color != 8) )
	{
		fprintf(stderr, "Error: Bmp_color = %d is not implemented in this program.\n", Bmp_color);
		exit(1);
	}

	if (imgp->width > MAXWIDTH)
	{
		fprintf(stderr, "Error: Bmp_width = %ld > %d = MAXWIDTH!\n", imgp->width, MAXWIDTH);
		exit(1);
	}

	if (imgp->height > MAXHEIGHT)
	{
		fprintf(stderr, "Error: Bmp_height = %ld > %d = MAXHEIGHT!\n", imgp->height, MAXHEIGHT);
		exit(1);
	}

	Real_width = imgp->width*3 + imgp->width%4; /* 4byte �����ˤ��碌�뤿���˼ºݤ����η׻� */

	/* �����ΰ���ưŪ����. ���Ԥ��������ϥ��顼���å����������Ϥ��ƽ�λ */
	if ((Bmp_Data = (unsigned char *)calloc(Real_width, sizeof(unsigned char))) == NULL)
	{
		fprintf(stderr,"Error: Memory allocation failed for Bmp_Data!\n");
		exit(1);
	}

  /* �����ǡ����ɤ߹��� */
	for(i=0;i<imgp->height;i++)
	{
		fread(Bmp_Data,1,Real_width,Bmp_Fp);
		for (j=0;j<imgp->width;j++)
		{
			imgp->data[imgp->height-i-1][j].b = Bmp_Data[j*3];
			imgp->data[imgp->height-i-1][j].g = Bmp_Data[j*3+1];
			imgp->data[imgp->height-i-1][j].r = Bmp_Data[j*3+2];
		}
	}

  /* ưŪ�˳��ݤ��������ΰ��β��� */
  free(Bmp_Data);

  /* �ե����륯������ */
  fclose(Bmp_Fp);
}

/*
   �ؿ�̾: WriteBmp
   ����  : char *filename, img *tp
   �֤���: void
   ư��  : 2�������� tp->data �����Ƥ������ǡ����Ȥ���, 24�ӥå�
           bmp�����Υե����� filename �˽񤭽Ф�.
*/
void WriteBmp(char *filename, img *tp) {

  int i,j;
  int Real_width;
  FILE *Out_Fp = fopen(filename,"wb");  /* �ե����륪���ץ� */
  unsigned char *Bmp_Data;     /* �����ǡ�����1��ʬ��Ǽ               */

  if(Out_Fp==NULL){
    fprintf(stderr,"Error: file %s couldn\'t open for write!\n",filename);
    exit(1);
  }

  Bmp_color=24;
  Bmp_header_size=HEADERSIZE;
  Bmp_info_header_size=40;
  Bmp_planes=1;

  Real_width = tp->width*3 + tp->width%4;  /* 4byte �����ˤ��碌�뤿���˼ºݤ����η׻� */

  /* �����ΰ���ưŪ����. ���Ԥ��������ϥ��顼���å����������Ϥ��ƽ�λ */
  if((Bmp_Data = (unsigned char *)calloc(Real_width,sizeof(unsigned char)))==NULL) {
   fprintf(stderr,"Error: Memory allocation failed for Bmp_Data!\n");
   exit(1);
 }

  /* �إå������ν��� */
  Bmp_xppm = Bmp_yppm = 0;
  Bmp_image_size = tp->height*Real_width;
  Bmp_size       = Bmp_image_size + HEADERSIZE;
  Bmp_headbuf[0] = 'B'; Bmp_headbuf[1] = 'M';
  memcpy(Bmp_headbuf+2, &Bmp_size, sizeof(Bmp_size));
  Bmp_headbuf[6] = Bmp_headbuf[7] = Bmp_headbuf[8] = Bmp_headbuf[9] = 0;
  memcpy(Bmp_headbuf+10, &Bmp_header_size, sizeof(Bmp_header_size));
  Bmp_headbuf[11] = Bmp_headbuf[12] = Bmp_headbuf[13] = 0;
  memcpy(Bmp_headbuf+14, &Bmp_info_header_size, 4);
  Bmp_headbuf[15] = Bmp_headbuf[16] = Bmp_headbuf[17] = 0;
  memcpy(Bmp_headbuf+18, &tp->width, 4);
  memcpy(Bmp_headbuf+22, &tp->height, 4);
  memcpy(Bmp_headbuf+26, &Bmp_planes, 2);
  memcpy(Bmp_headbuf+28, &Bmp_color, 2);
  memcpy(Bmp_headbuf+34, &Bmp_image_size, 4);
  memcpy(Bmp_headbuf+38, &Bmp_xppm, 4);
  memcpy(Bmp_headbuf+42, &Bmp_yppm, 4);
  Bmp_headbuf[46] = Bmp_headbuf[47] = Bmp_headbuf[48] = Bmp_headbuf[49] = 0;
  Bmp_headbuf[50] = Bmp_headbuf[51] = Bmp_headbuf[52] = Bmp_headbuf[53] = 0;

  /* �إå������񤭽Ф� */
  fwrite(Bmp_headbuf,sizeof(unsigned char),HEADERSIZE,Out_Fp);

  /* �����ǡ����񤭽Ф� */
  for (i=0;i<tp->height;i++) {
    for (j=0;j<tp->width;j++) {
      Bmp_Data[j*3]   = tp->data[tp->height-i-1][j].b;
      Bmp_Data[j*3+1] = tp->data[tp->height-i-1][j].g;
      Bmp_Data[j*3+2] = tp->data[tp->height-i-1][j].r;
    }
    for (j=tp->width*3;j<Real_width;j++) {
      Bmp_Data[j]=0;
    }
    fwrite(Bmp_Data,sizeof(unsigned char),Real_width,Out_Fp);
  }

  /* ưŪ�˳��ݤ��������ΰ��β��� */
  free(Bmp_Data);

  /* �ե����륯������ */
  fclose(Out_Fp);
}

/*
   �ؿ�̾: PrintBmpInfo
   ����  : char *filename
   �֤���: void
   ư��  : �����Ȥ���Ϳ���������ե�����̾������ bmp �����β����ե�����
           ��°�������̤˽��Ϥ���.
*/
void PrintBmpInfo(char *filename) {
	long Bmp_height; /* �⤵ (�ԥ�����) */
	long Bmp_width;  /* �� (�ԥ�����) */

	FILE *Bmp_Fp=fopen(filename,"rb"); /* �Х��ʥ��⡼���ɤ߹����Ѥ˥����ץ� */
	if (Bmp_Fp == NULL)
	{
		fprintf(stderr,"Error: file %s couldn\'t open for write!\n",filename);
		exit(1);
	}

	fread(Bmp_headbuf,sizeof(unsigned char),HEADERSIZE,Bmp_Fp);

	memcpy(&Bmp_type, Bmp_headbuf, sizeof(Bmp_type));

	if (strncmp(Bmp_type, "BM", 2) != 0)
	{
		fprintf(stderr,"Error: %s is not a bmp file.\n",filename);
    	exit(1);
	}

	memcpy(&Bmp_size, Bmp_headbuf+2, 4);
	memcpy(&Bmp_width, Bmp_headbuf+18, 4);
	memcpy(&Bmp_height, Bmp_headbuf+22, 4);
	memcpy(&Bmp_color, Bmp_headbuf+28, 2);
	memcpy(&Bmp_comp, Bmp_headbuf+30, 4);
	memcpy(&Bmp_image_size, Bmp_headbuf+34, 4);
	memcpy(&Bmp_xppm, Bmp_headbuf+38, 4);
	memcpy(&Bmp_yppm, Bmp_headbuf+42, 4);


	printf("file name             = %s \n",filename);
	printf("file type             = %c%c \n",Bmp_type[0],Bmp_type[1]);
	printf("file size             = %ld (byte)\n",Bmp_size);
	printf("width                 = %ld (pixel)\n",Bmp_width);
	printf("height                = %ld (pixel)\n",Bmp_height);
	printf("color                 = %d (bit)\n",Bmp_color);
	printf("compression           = %ld\n",Bmp_comp);
	printf("size of image part    = %ld (byte)\n",Bmp_image_size);
	printf("horizontal resolution = %ld (ppm)\n",Bmp_xppm);
	printf("vertical resolution   = %ld (ppm)\n",Bmp_yppm);

  fclose(Bmp_Fp);
}

/*
   �ؿ�̾: HMirror
   ����  : img *sp, img *tp
   �֤���: void
   ư��  : 2�������� tp->data �β�������ʿ�����ζ��Ǥ��Ȥä����Τ�
           2�������� sp->data �˳�Ǽ����.
*/
void HMirror(img* sp, img *tp) {
  int i,j;
//  long k;
  for(i=0;i<tp->height;i++)
    for(j=0;j<tp->width;j++)
      sp->data[tp->height-i-1][j]=tp->data[i][j];
  sp->height=tp->height;
  sp->width=tp->width;
}

/*
   �ؿ�̾: VMirror
   ����  : img *sp, img *tp
   �֤���: void
   ư��  : 2�������� tp->data �β�������ľ�����ζ��Ǥ��Ȥä����Τ�
           2�������� sp->data �˳�Ǽ����.
*/
void VMirror(img *sp, img *tp) {
  int i,j;
//  long k;
  for(i=0;i<tp->height;i++)
    for(j=0;j<tp->width;j++)
      sp->data[i][tp->width-j-1]=tp->data[i][j];
  sp->height=tp->height;
  sp->width=tp->width;
}


/*
   �ؿ�̾: Rotate90
   ����  : int a, img *sp, img *tp
   �֤���: void
   ư��  : 2�������� tp->data �β����� 90��a �ٻ��ײ����˲�ž�������Τ�
           2�������� sp->data �˳�Ǽ����. ������, ���������ȹ⤵��,
           sp->height �� sp->width ������.
*/
void Rotate90(int a, img *sp, img *tp) {
  int i,j;
  if ((a%4)==0) {
    for(i=0;i<tp->height;i++)
      for(j=0;j<tp->width;j++)
        sp->data[i][j]=tp->data[i][j];
    sp->width=tp->width;
    sp->height=tp->height;
  } else if ((a%4)==1) {
    for(i=0;i<tp->height;i++)
      for(j=0;j<tp->width;j++)
        sp->data[j][tp->height-i-1]=tp->data[i][j];
    sp->height=tp->width;
    sp->width=tp->height;
  } else if ((a%4)==2) {
    for(i=0;i<tp->height;i++)
      for(j=0;j<tp->width;j++)
        sp->data[tp->height-i-1][tp->width-j-1]=tp->data[i][j];
    sp->width=tp->width;
    sp->height=tp->height;
  } else {
    for(i=0;i<tp->height;i++)
      for(j=0;j<tp->width;j++)
        sp->data[tp->width-j-1][i]=tp->data[i][j];
    sp->height=tp->width;
    sp->width=tp->height;
  }
}


/*
   �ؿ�̾: Shrink
   ����  : int a, img *sp, img *tp
   �֤���: void
   ư��  : 2�������� tp->data �β��������ȹ⤵�� 1/a �ܤ���������
           2�������� sp->data �˳�Ǽ����.
*/
void Shrink(int a, img *sp, img *tp) {
  int i,j,k,l,w,h,count;
  unsigned long tmp_r,tmp_g,tmp_b;

  sp->height=tp->height/a + ((tp->height%a == 0)?0:1);
  sp->width=tp->width/a + ((tp->width%a == 0)?0:1);

  for(i=0;i<sp->height;i++) {
    h = (a*(i+1)<=tp->height)?a:tp->height-a*i;
    for(j=0;j<sp->width;j++) {
      w = (a*(j+1)<=tp->width)?a:tp->width-a*j;
      tmp_r =0; tmp_g=0; tmp_b =0; count=0;
      for(k=0;k<h;k++)
        for(l=0;l<w;l++) {
          tmp_r+=tp->data[i*a+k][j*a+l].r;
          tmp_g+=tp->data[i*a+k][j*a+l].g;
          tmp_b+=tp->data[i*a+k][j*a+l].b;
          count++;
        }
      sp->data[i][j].r = (unsigned char)(tmp_r/count);
      sp->data[i][j].g = (unsigned char)(tmp_g/count);
      sp->data[i][j].b = (unsigned char)(tmp_b/count);
    }
  }
}

/*
   �ؿ�̾: Mosaic
   ����  : int a, img *sp, img *tp
   �֤���: void
   ư��  : 2�������� tp->data �β����˥⥶�����򤫤���������
           2�������� sp->data �˳�Ǽ����. �⥶�������礭����
           a��a �Ǥ���.
*/
void Mosaic(int a, img *sp, img *tp) {
  int i,j,k,l,w,h,t_height,t_width,count;
  unsigned long tmp_r,tmp_g,tmp_b;

  sp->height=tp->height;
  sp->width=tp->width;

  t_height=tp->height/a + ((tp->height%a == 0)?0:1);
  t_width=tp->width/a + ((tp->width%a == 0)?0:1);

  for(i=0;i<t_height;i++) {
    h = (a*(i+1)<=tp->height)?a:tp->height-a*i;
    for(j=0;j<t_width;j++) {
      w = (a*(j+1)<=tp->width)?a:tp->width-a*j;
      tmp_r =0; tmp_g=0; tmp_b =0; count=0;
      for(k=0;k<h;k++)
        for(l=0;l<w;l++) {
          tmp_r+=tp->data[i*a+k][j*a+l].r;
          tmp_g+=tp->data[i*a+k][j*a+l].g;
          tmp_b+=tp->data[i*a+k][j*a+l].b;
          count++;
        }
      tmp_r = (unsigned char)(tmp_r/count);
      tmp_g = (unsigned char)(tmp_g/count);
      tmp_b = (unsigned char)(tmp_b/count);
      for(k=0;k<h;k++)
        for(l=0;l<w;l++) {
          sp->data[i*a+k][j*a+l].r = tmp_r;
          sp->data[i*a+k][j*a+l].g = tmp_g;
          sp->data[i*a+k][j*a+l].b = tmp_b;
        }
    }
  }

}

/*
   �ؿ�̾: Gray
   ����  : img *sp, img *tp
   �֤���: void
   ư��  : 2�������� tp->data �β����򥰥쥤���������Ѵ�����,
           2�������� sp->data �˳�Ǽ����.
*/
void Gray(img *sp, img *tp) {
  int i,j;
  unsigned char tmp;

  sp->height=tp->height;
  sp->width=tp->width;

  for(i=0;i<sp->height;i++) {
    for(j=0;j<sp->width;j++) {
      tmp=(unsigned char)((tp->data[i][j].r+tp->data[i][j].g+tp->data[i][j].b)/3);
      sp->data[i][j].r = sp->data[i][j].g = sp->data[i][j].b = tmp;
    }
  }

}

/*
   �ؿ�̾: Diminish
   ����  : img *sp, img *tp, unsigned char x
   �֤���: void
   ư��  : 2�������� tp->data �β�������Ĵ�� (��RGB�ε��٤ˤĤ���
           2^x �ȥޥ���������) ��, 2�������� sp->data �˳�Ǽ����.
*/
void Diminish(img *sp, img *tp, unsigned char x) {
  int i,j;
  unsigned char y;

  x = x % 8;
  y = 255 << x;

  sp->height=tp->height;
  sp->width=tp->width;

  for(i=0;i<sp->height;i++) {
    for(j=0;j<sp->width;j++) {
      sp->data[i][j].r = tp->data[i][j].r & y;
      sp->data[i][j].g = tp->data[i][j].g & y;
      sp->data[i][j].b = tp->data[i][j].b & y;
    }
  }
}
