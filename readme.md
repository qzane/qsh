[INCLUDE=presentation]
Title         : qsh
Sub Title     : a simple shell
Author        : qzane
Email         : qzane@live.com
Reveal Theme  : sky
Beamer Theme  : singapore

[TITLE]

# Content

[TOC]

# What is qsh

[qsh] serve as a shell interface that accepts user commands and then executes each command in a separate
process. <br>
Here are some of the features:

* run in the foreground/background

* show history and rerun

* do multiple jobs in a single line

* redirection of stdin/stdout/stderr

[qsh]: http://github.com/qzane/qsh

## Install


``` python
  gcc -o qsh qsh.c # the main program
  gcc -o delay delay.c # a simple program to test
  gcc -o delay_stdin delay_stdin.c  # a simple program to test
```

RUN

``` python
./qsh
```

# demo

## UI
you can run some small executable file on your machine.<br>
notice that a single ```cd```  will take you where you started.

``` python
root@qzanenet:~/qsh/git# ./qsh
qsh[0]>>pwd
/root/qsh/git
qsh[1]>>ls
delay  delay.c  delay_1000.in  delay_stdin  delay_stdin.c  qsh  qsh.c
qsh[2]>>cd /
qsh[3]>>ls
bin   dev  fastboot  lib         media  opt   root  sbin  sys  usr
boot  etc  home      lost+found  mnt    proc  run   srv   tmp  var
qsh[4]>>cd
qsh[5]>>pwd
/root/qsh/git
qsh[6]>>ls
delay  delay.c  delay_1000.in  delay_stdin  delay_stdin.c  qsh  qsh.c
```


## run in the background
``` python
qsh[8]>>cat delay.c
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
qsh[9]>>gcc -o delay delay.c
```

## run in the background

you can run different commands in different ways
``` python
qsh[10]>>./delay 1000 a ; ./delay 1005 b & ./delay 998 c & ./delay 800 d ;
Delay-a: begin
Delay-a: end after 1000
Delay-d: begin
Delay-b: begin
Delay-c: begin
Delay-d: end after 800
qsh[11]>>Delay-c: end after 998
Delay-b: end after 1005
```
Timeline:

```
                  --b(begin)--
a(begin)--a(end)--            --d(begin)--d(end)--c(end)--b(end)
                  --c(begin)--
```
notice that the output of b and c occupied the screen when the shell are going to read a new line
## redirection of stdin/stdout/stderr

``` python
qsh[12]>>./delay 1000 a ; ./delay 1005 b >b.out & ./delay 998 c >c.out & ./delay 800 d ;
Delay-a: begin
Delay-a: end after 1000
Delay-d: begin
Delay-d: end after 800
qsh[13]>>ls
b.out  c.out  delay  delay.c  delay_1000.in  delay_stdin  delay_stdin.c  qsh  qsh.c
qsh[14]>>cat b.out
Delay-b: begin
Delay-b: end after 1005
qsh[15]>>cat c.out
Delay-c: begin
Delay-c: end after 998
```
## redirection of stdin/stdout/stderr

``` python
qsh[16]>>cat delay_stdin.c
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
qsh[17]>>cat delay_1000.in
1000 ina
qsh[18]>>gcc -o delay_stdin delay_stdin.c
```
## redirection of stdin/stdout/stderr
``` python
qsh[19]>>./delay_stdin <delay_1000.in 1>stdout.out 2>stderr.out
qsh[20]>>cat stdout.out
Delay-ina: begin
Delay-ina: end after 1000
qsh[21]>>./delay 200 append >>stdout.out
qsh[22]>>cat stdout.out
Delay-ina: begin
Delay-ina: end after 1000
Delay-append: begin
Delay-append: end after 200
```
```1>``` stands for stdout 
(1 can be omitted)
```2>``` stands for stderr
```>>``` means append

## about history

``` python
qsh[23]>>history
History.4 :cd
History.5 :pwd
History.6 :ls
History.7 :
History.8 :cat delay.c
History.9 :gcc -o delay delay.c
History.10:./delay 1000 a ; ./delay 1005 b & ./delay 998 c & ./delay 800 d ;
History.11:
History.12:./delay 1000 a ; ./delay 1005 b >b.out & ./delay 998 c >c.out & ./delay 800 d ;
History.13:ls
History.14:cat b.out
History.15:cat c.out
History.16:cat delay_stdin.c
History.17:cat delay_1000.in
History.18:gcc -o delay_stdin delay_stdin.c
History.19:./delay_stdin <delay_1000.in 1>stdout.out 2>stderr.out
History.20:cat stdout.out
History.21:./delay 200 append >>stdout.out
History.22:cat stdout.out
History.23:history
qsh[24]>>!12
Delay-a: begin
Delay-a: end after 1000
Delay-d: begin
Delay-d: end after 800
qsh[25]>>!!
Delay-a: begin
Delay-a: end after 1000
Delay-d: begin
Delay-d: end after 800
```
## After that
``` python
qsh[24]>>!12
Delay-a: begin
Delay-a: end after 1000
Delay-d: begin
Delay-d: end after 800
qsh[25]>>!!
Delay-a: begin
Delay-a: end after 1000
Delay-d: begin
Delay-d: end after 800
qsh[26]>>history
History.7 :
History.8 :cat delay.c
History.9 :gcc -o delay delay.c
History.10:./delay 1000 a ; ./delay 1005 b & ./delay 998 c & ./delay 800 d ;
History.11:
History.12:./delay 1000 a ; ./delay 1005 b >b.out & ./delay 998 c >c.out & ./delay 800 d ;
History.13:ls
History.14:cat b.out
History.15:cat c.out
History.16:cat delay_stdin.c
History.17:cat delay_1000.in
History.18:gcc -o delay_stdin delay_stdin.c
History.19:./delay_stdin <delay_1000.in 1>stdout.out 2>stderr.out
History.20:cat stdout.out
History.21:./delay 200 append >>stdout.out
History.22:cat stdout.out
History.23:history
History.24:./delay 1000 a ; ./delay 1005 b >b.out & ./delay 998 c >c.out & ./delay 800 d ;
History.25:./delay 1000 a ; ./delay 1005 b >b.out & ./delay 998 c >c.out & ./delay 800 d ;
History.26:history
```
# design

``` cpp
  int main(int argc,char *argv[]){
    getcwd(DIRS[0],MAX_LINE);//save the init-dir
    while(1){
        print_Hellow();
        get_Line();
        if(run_Lines()!=0)
            break;
    }
    return 0;
  }
```
``` cpp
  int Argc;
  char *Argv[MAX_Argc];//Argv to be exe
  char Argvv[MAX_Argc][MAX_Argv];//place to store argv
  char CMD_OPT[4][MAX_Argv];//|stdin|stdout|stderr|foreground/background|...
  char Line[MAX_LINE];//store a new line
  char CMDS[MAX_CMDS][MAX_LINE];//store commands
  int CMDN;
  char History[MAX_HISTORY][MAX_LINE];//store history
  int History_Num=0;
  char DIRS[MAX_DIR][MAX_LINE];//store dirs
```
## print_Hellow

``` cpp
  void print_Hellow(){//done
    printf("qsh[%d]>>",History_Num);
  }
```

## get_Line

1. use fgets for safety
2. replace !x with corresponding command
3. story the command in History
4. split the line into commands

## run_Lines
1. call put_Argv to split command into argvs
2. set CMD_OPT:stdin|stdout|stderr|foreground/background|...
3. pid=fork()
4. (child) set stdin/stdout/stderr
4. (parent) waitpid(pid)

# Thanks for watching!
