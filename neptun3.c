//2019.12.12
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <sys/msg.h>
#include <mqueue.h> // POSIX, modernebb üzenetsor
//hello -> NE ADJ MEG NEKIK RANDOM SZÁMOKAT
#define SIG_ACCEPTED SIGUSR1
#define SIG_NOT_ACCEPTED SIGUSR2

pid_t pid;     //hallgato
pid_t pid2;    //temavezeto
int elfogadas; //1 elfogadva; 2 elutasítva

typedef struct TemaBejelento
{
    char nev[50];
    char dolgozat[50];
    char beadas[10];
    char tema_vez_neve[50];
} TemaBejelento;

void handler(int signumber)
{
    if (signumber == SIGUSR1)
    {
        printf("\nHallgatóhoz megérkezet: ELFOGADVA\n");
        elfogadas = 1;
        // printf("\nSignal with number %i has arrived\n", signumber);
    }
    else
    {
        printf("\nHallgatóhoz megérkezett: ELUTASÍTVA\n");
        elfogadas = 2;
        // printf("\nSignal with number %i has arrived\n", signumber);
    }
}

void handler2(int signumber)
{
    printf("\nhandler2 signum: %d\n", signumber);
    printf("\n pid handkler2 %d\n", pid);
    fflush(stdout);
    if (signumber == SIGUSR1)
    {
        kill(pid, SIG_ACCEPTED);
    }
    else
    {
        kill(pid, SIG_NOT_ACCEPTED);
    }
}

int main(int argc, char *argv[])
{
    srand(time(0));
    //gyerek-hallgato
    //szulo -neptun
    //gyerek -temavezeto

    int pipefd[2];
    int pipefd2[2];
    int status;
    int itelet;

    //EZ ITT A SIGNAL HELYE
    signal(SIG_NOT_ACCEPTED, handler2);
    signal(SIG_ACCEPTED, handler2);
    signal(SIG_ACCEPTED, handler);
    signal(SIG_NOT_ACCEPTED, handler);

    if (pipe(pipefd) == -1)
    {
        perror("Hiba a pipe nyitaskor!");
        exit(EXIT_FAILURE);
    }

    if (pipe(pipefd2) == -1)
    {
        perror("Hiba a pipe nyitaskor!");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    //8732->
    printf("pid %d\n", pid);
    if (pid == -1)
    {
        perror("Fork hiba");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {

        //hallgato
        TemaBejelento kuldendo_info;

        printf("\nA HALLGATO (gyerek1) eküldi a bejelntőt a neptunba");
        close(pipefd[0]); //bezárjuk, ne csináljon rossz dolgokat

        strcpy(kuldendo_info.nev, "ANNA");
        strcpy(kuldendo_info.dolgozat, "OpreShit");
        strcpy(kuldendo_info.beadas, "2020");
        strcpy(kuldendo_info.tema_vez_neve, "Anyuuuuuuu");

        write(pipefd[1], &kuldendo_info, sizeof(kuldendo_info));
        close(pipefd[1]);
        printf("\nA HALLGATO elküldte az üzenetet\n");
    }
    else
    { //neptun
        printf("Szülő: %i\n", getpid());
        pid2 = fork();
        if (pid2 == -1)
        {
            perror("Fork hiba");
            exit(EXIT_FAILURE);
        }
        if (pid2 == 0)
        {
            //temavez
            sleep(1);
            waitpid(pid, &status, 0);
            TemaBejelento kapott_info;
            printf("\nA TÉMAVEZETŐHÖZ (gyerek2) megérkezik a felkérés\n");
            close(pipefd2[1]);
            read(pipefd2[0], &kapott_info, sizeof(kapott_info)); //sizeof(kapott_info)
            printf("\nA neptunba a hallgato által érkezett bejelentés:");
            printf("\nNEV: \t\t\t%s", kapott_info.nev);
            printf("\nDOLGOZAT: \t\t%s", kapott_info.dolgozat);
            printf("\nBEADÁS ÉVE: \t\t%s", kapott_info.beadas);
            printf("\nKÍVÁNT TÉMAVEZETŐ: \t%s", kapott_info.tema_vez_neve);
            printf("\n");
            close(pipefd2[0]);

            //60%os esély (???) -SIG_NOT_ACCEPTED, SIG_ACCEPTED neptunnak
            itelet = rand() % 10;
            if (itelet <= 3)
            {
                kill(getppid(), SIG_ACCEPTED);
            }
            else if (itelet > 3 && itelet <= 9)
            {

                kill(getppid(), SIG_NOT_ACCEPTED);
            }
        }
        else
        {
            //neptun
            waitpid(pid, &status, 0);
            /*********************************/
            TemaBejelento kapott_info;
            printf("\nA neptun (szulo) nyit! Megérkezik a HALLGATO (gyerek1) uzenete, melyet továbbít TÁMA-VEZNEK\n");
            close(pipefd[1]);
            read(pipefd[0], &kapott_info, sizeof(kapott_info));
            printf("\n");
            close(pipefd[0]);
            /*********************************/
            close(pipefd2[0]); //bezárjuk, ne csináljon rossz dolgokat
            write(pipefd2[1], &kapott_info, sizeof(kapott_info));
            close(pipefd2[1]);

            wait(NULL);
        }
    }

    exit(EXIT_SUCCESS);
}