#include<stdio.h>

int main(int argc,char *argv[]){
    int count,i,j,g;
    char name[100];
    scanf("%d %s",&count,name);
    printf("Delay-%s: begin\n",name);
    fflush(stdout);
    for(g=1,i=0;i<count;++i)
        for(j=0;j<10000;++j)
            g^=j;
    printf("Delay-%s: end after %d\n",name,count);
    fflush(stdout);
    return 0;
}
