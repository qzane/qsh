#include<stdio.h>

int main(int argc,char *argv[]){
    int count,i,j,g;
    char *name;
    if(argc<=1||!strcmp(argv[1],"help)")){
        printf("Usage: %s [time] [name]\n",argv[0]);
        fflush(stdout);
        return 0;
    }
    if(argc==2)
        name = "";
    else
        name = argv[2];
    sscanf(argv[1],"%d",&count);
    printf("Delay-%s: begin\n",name);
    fflush(stdout);
    for(g=1,i=0;i<count;++i)
        for(j=0;j<10000;++j)
            g^=j;
    printf("Delay-%s: end after %d\n",name,count);
    fflush(stdout);
    return 0;
}
