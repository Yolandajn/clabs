
# include <stdio.h>
# include <conio.h>
# define MAX 200
int xx[MAX];
int totnum=0;
int totcnt=0;
double totpjz=0.0;
int readdat(void);
void writedat(void);
void calvalue(void)
 {
   int i,j;
   long val=0;
   for(i=0;i<MAX;i++) if(xx[i]) totnum++;
    for(i=0;i<totnum;i++)
    {
     j=(xx[i]>>1);
     if(j%2==0){
     totcnt++;
     val+=xx[i];
     }
	 }
	 totpjz=(double)val/totcnt;
	 }
     main()
      {
	clrscr();
	if (readdat()){
	  printf("the file cann't open!\007\n");
	  return;}
	  calvalue();
	  printf("the totnum is %d\n",totnum);
	  printf("the totcnt is %d\n",totcnt);
	  printf("the totpjz is %.2lf\n",totpjz);
	  writedat();
	  }
	int readdat(void)
	 { FILE *fp;
	   int i=0;
	   if((fp=fopen("in.dat","r"))==NULL)  return 1;
	   while(!feof(fp)){
	   fscanf(fp,"%d",&xx[i++]);
	}
	   fclose(fp);
	   return 0;
	   }
	   void writedat(void)
	    { FILE *fp;
	      fp=fopen("out10.dat","w");
	      fprintf(fp,"%d\n%d\n%.2lf\n",totnum,totcnt,totpjz);
	      fclose(fp);
	      }
