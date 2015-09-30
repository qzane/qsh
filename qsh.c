#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/wait.h>

#define MAX_Argcn 10
#define MAX_Argc 20
#define MAX_Argv 100
#define MAX_LINE 400
#define MAX_CMDS 10
#define MAX_HISTORY 20
#define MAX_DIR 20

pip_t Postpid;
char INPUT_PIPE[MAX_Argv];
char OUTPUT_PIPE[MAX_Argv];
int Argc,Argcn;//Argcn:管道把命令切成了多少段
int Argvn[MAX_Argcn];
char *Argv[MAX_Argcn][MAX_Argc];//Argv to be exe
char Argvv[MAX_Argc][MAX_Argv];//place to store argv
char CMD_OPT[MAX_Argcn][4][MAX_Argv];//|stdin|stdout|stderr|foreground/background|...
char Line[MAX_LINE];//store a new line
char CMDS[MAX_CMDS][MAX_LINE];//store commands
int CMDN;
char History[MAX_HISTORY][MAX_LINE];//store history
int History_Num=0;
char DIRS[MAX_DIR][MAX_LINE];//store dirs

void put_Argv(char *str){//split a line of command into argv
    char *point;
    int i,j;
    Argc = Argcn = 0;
    point = strtok(str," \n\r");
    while(point!=NULL){
        strcpy(Argvv[Argc],point);
        point = strtok(NULL," \n\r");
        ++Argc;
    }
    --Argc;
    for(j=0;j<MAX_Argcn;++j)
        for(i=0;i<4;++i)
            CMD_OPT[j][i][0]='\0';
    Argvn[0]=0;
    for(i=0;i<Argc;++i){
        if(Argvv[i][0]==';'){
            CMD_OPT[Argcn][3][0]=';';
            Argv[Argcn][Argvn[Argcn]]=NULL;
        }
        else if(Argvv[i][0]=='&'){
            CMD_OPT[Argcn][3][0]='&';
            Argv[Argcn][Argvn[Argcn]]=NULL;
        }
        else if(Argvv[i][0]=='<'){
            strcpy(CMD_OPT[Argcn][0],Argvv[i]);
        }
        else if(Argvv[i][0]=='>'){
            strcpy(CMD_OPT[Argcn][1],Argvv[i]);
        }
        else if(Argvv[i][0]=='1' && Argvv[i][1]=='>'){
            strcpy(CMD_OPT[Argcn][1],&Argvv[i][1]);
        }
        else if(Argvv[i][0]=='2' && Argvv[i][1]=='>'){
            strcpy(CMD_OPT[Argcn][2],&Argvv[i][1]);
        }
        else if(Argvv[i][0]=='|'){
            CMD_OPT[Argcn][3][0]=';';
            Argv[Argcn][Argvn[Argcn]]=NULL;
            ++Argcn;
        }
        else{
            Argv[Argcn][Argvn[Argcn]++]=Argvv[i];
        }
    
    }
    ///*/test:
    for(i=0;i<Argcn;++i)
        for(j=0;;++j){
            if(Argv[i][j]==NULL){
                printf("\n");
                break;
            }
            printf("#%s%, ",Argv[i][j]);            
        }
    
    /*
    if(Argvv[Argc][0]==';')
        CMD_OPT[3][0]=';';
    else
        CMD_OPT[3][0]='&';
    for(i=0;i<Argc;++i){
        if(Argvv[i][0]=='|'){
            //what to put here?
        
        }
        else if(Argvv[i][0]=='<'){
            strcpy(CMD_OPT[0],Argvv[i]);
            Argvv[i][0]='\0';
        }else if(Argvv[i][0]=='>'){
            strcpy(CMD_OPT[1],Argvv[i]);
            Argvv[i][0]='\0';
        }else if(Argvv[i][0]=='1' && Argvv[i][1]=='>'){
            strcpy(CMD_OPT[1],&Argvv[i][1]);
            Argvv[i][0]='\0';
        }else if(Argvv[i][0]=='2' && Argvv[i][1]=='>'){
            strcpy(CMD_OPT[2],&Argvv[i][1]);
            Argvv[i][0]='\0';
        }
    }
    
    for(i=j=0;i<Argc;++i)
        if(Argvv[i][0]!='\0' && Argvv[i][0]!=';' && Argvv[i][0] !='&')
            strcpy(Argvv[j++],Argvv[i]);
    Argv[Argc=j]=NULL;//del the last argv (; or &)
    ///*/
}


void print_Hellow(){//done
    printf("qsh[%d]>>",History_Num);
}

void split_Line(){//split a line into commands
    int begin,end;
    CMDN=0;
    begin=0;
    for(end=0;end<strlen(Line);++end){
        if(Line[end]=='&' || Line[end]==';'){
            strncpy(CMDS[CMDN],&Line[begin],end-begin+1);
            CMDS[CMDN++][end-begin+1]='\0';
            begin=end+1;
        }
    }
    if(begin<strlen(Line)){//the last cmd may not have a ';'
        strcpy(CMDS[CMDN],&Line[begin]);
        CMDS[CMDN][strlen(CMDS[CMDN])+1]='\0';
        CMDS[CMDN][strlen(CMDS[CMDN])]=';';//ensure every cmd end with ; or &
        ++CMDN;
    }
}

void get_Line(){//read a line
    int num;
    fgets(Line,MAX_LINE,stdin);
    if(Line[0]=='!' && Line[1]=='!')
        strcpy(Line,History[(History_Num-1)%MAX_HISTORY]);
    else if(Line[0]=='!'){
        sscanf(&Line[1],"%d",&num);
        if(History_Num - num > MAX_HISTORY || num>History_Num){
            printf("This commands doesn't exist.\n");
            Line[0]='\0';
            return ;
        }else{
            strcpy(Line,History[num%MAX_HISTORY]);
        }
    }
    strcpy(History[(History_Num++)%MAX_HISTORY],Line);
    split_Line();// split line into commands
}
int run_Lines(){//exe
    int no,fno,ftmp,i,pipen;
    pid_t pid;
    for(no=0;no<CMDN;++no){
        put_Argv(CMDS[no]);
        for(pipen=0;pipen<Argcn;++pipen){        
            if(Argv[pipen][0]]==NULL){
                ;
            }
            else if(!strcmp(Argv[pipen][0],"exit")){
               printf("bye!\n");
               return 1;
            }
            else if(!strcmp(Argv[pipen][0],"cd")){
                if(Argvn[pipen]>=2)
                    chdir(Argv[pipen][1]);
                else
                    chdir(DIRS[0]);
            }
            else if(!strcmp(Argv[pipen][0],"history")){
                for(i=History_Num - MAX_HISTORY;i<History_Num;++i){
                    if(i<0)continue;
                    printf("History.%-2d:%s",i,History[i%MAX_HISTORY]);
                }
            }
            else{
                pid = fork();
                if(pid==0){
                    for(fno=1;fno<3;++fno){
                        if(CMD_OPT[pipen][fno][0]=='>'){
                            if(CMD_OPT[pipen][fno][1]=='>'){//">>" means append
                                ftmp = open(&CMD_OPT[pipen][fno][2],O_CREAT|O_RDWR|O_FSYNC,S_IRUSR | S_IWUSR);
                                lseek(ftmp,0,SEEK_END);
                                dup2(ftmp,fno);
                            }else{
                                ftmp = open(&CMD_OPT[pipen][fno][1],O_CREAT|O_TRUNC|O_RDWR|O_FSYNC,S_IRUSR | S_IWUSR);
                                dup2(ftmp,fno);
                            }
                        }
                    }
                    if(CMD_OPT[0][0]=='<'){
                        ftmp = open(&CMD_OPT[0][1],O_RDONLY);
                        dup2(ftmp,0);
                    }
                    if(pipen!=Argcn-1){
                        sprintf(OUTPUT_PIPE,"qsh-%d.pipe",getpid());
                        ftmp = open(OUTPUT_PIPE,O_CREAT|O_TRUNC|O_RDWR|O_FSYNC,S_IRUSR | S_IWUSR);
                        dup2(ftmp,1);
                    }
                    if(pipen!=0){
                        sprintf(INPUT_PIPE,"qsh-%d.pipe",Postpid);
                        ftmp = open(INPUT_PIPE,O_RDONLY);
                        dup2(ftmp,0);                      
                    }
                    execvp(Argv[pipen][0],Argv);
                    fflush(stdout);
                    exit(0);
                }else{
                    Postpid = pid;
                    if(CMD_OPT[pipen][3][0]!='&'){
                        waitpid(pid,NULL,0);
                    }
                    fflush(stdout);
                }
            }
        }
    }
    return 0;
}


int main(int argc,char *argv[]){
    getcwd(DIRS[0],MAX_LINE);
    while(1){
        print_Hellow();
        get_Line();
        if(run_Lines()!=0)
            break;    
    }
    return 0;
}
