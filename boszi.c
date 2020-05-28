//2018.12.06 Perjési Kisasszony
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
#define SIGTERM1 SIGTERM
#define SIGTERM2 SIGTERM

struct uzenet
{
    long mtype;
    char mtext[1024];
};

// sendig a message
int kuld(int uzenetsor)
{
    struct uzenet uz = {5, "zárja be a boszorkányt a fáskamrába HÖLGYEM"};
    int status;

    status = msgsnd(uzenetsor, &uz, strlen(uz.mtext) + 1, 0);
    if (status < 0)
        perror("msgsnd");

    struct uzenet uz2 = {6, "Keresd meg a kulcsot"};
    status = msgsnd(uzenetsor, &uz2, strlen(uz2.mtext) + 1, 0);
    if (status < 0)
        perror("msgsnd");

    status = msgrcv(uzenetsor, &uz, 1024, 7, 0);

    if (status < 0)
        perror("msgsnd");
    else
        printf("JULCSItól kapott uzenet kodja: %ld, szovege:  %s\n", uz.mtype, uz.mtext);

    return 0;
}

// receiving a message.
int fogad(int uzenetsor)
{
    struct uzenet uz;
    int status;

    status = msgrcv(uzenetsor, &uz, 1024, 5, 0);

    if (status < 0)
        perror("msgsnd");
    else
        printf("JANCSItól kapott  uzenet kodja: %ld, szovege:  %s\n", uz.mtype, uz.mtext);

    status = msgrcv(uzenetsor, &uz, 1024, 6, 0);

    if (status < 0)
        perror("msgsnd");
    else
        printf("JANCSItól kapott uzenet kodja: %ld, szovege:  %s\n", uz.mtype, uz.mtext);

    struct uzenet uz2 = {7, "Nem találom"};
    status = msgsnd(uzenetsor, &uz2, strlen(uz2.mtext) + 1, 0);
    if (status < 0)
        perror("msgsnd");

    return 0;
}

void handler(int signumber)
{
    printf("Signal with number %i has arrived\n", signumber);
}
int pipefd3[2];

void handlerMunka(int signumber)
{
    char *munkak[] = {"körömvágás", "bibircsókpuszi", "asd", "dsa", "felmosás"};
    int num = rand() % 5;
    ///char msg[10] = munkak[num];
    close(pipefd3[0]);
    write(pipefd3[1], munkak[num], 10);
    close(pipefd3[1]);
}

int main(int argc, char const *argv[])
{
    srand(time(0));

    pid_t pid;
    pid_t pid2;
    int status;

    int pipefd[2];  //küld julcsinak
    int pipefd2[2]; //küld jancsinak
    int pipefd4[2]; //küld jancsinak
    /*ÜZISOR*/
    int uzenetsor, status2;
    key_t kulcs;
    kulcs = ftok(argv[0], 1);
    uzenetsor = msgget(kulcs, 0600 | IPC_CREAT);
    if (uzenetsor < 0)
    {
        perror("msgget");
        return 1;
    }
    /*************************************/
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
    if (pipe(pipefd3) == -1)
    {
        perror("Hiba a pipe nyitaskor!");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipefd4) == -1)
    {
        perror("Hiba a pipe nyitaskor!");
        exit(EXIT_FAILURE);
    }

    signal(SIGUSR1, handler);
    signal(SIGUSR2, handler);
    signal(SIGTERM, handlerMunka);

    pid = fork();

    if (pid == -1)
    {
        perror("Fork hiba");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        //julcsi
        kill(getppid(), SIGUSR1);
        int kapott;
        close(pipefd[1]);
        read(pipefd[0], &kapott, sizeof(int));
        close(pipefd[0]);

        if (kapott == 1)
        {
            printf("\nJULCSI \t\t KALACS\n");
        }
        else
        {
            printf("\nJULCSI \t\t SÜTI\n");
        }
        /*********************************/
        char msg[15];
        sleep(1);

        kill(getppid(), SIGTERM);
        close(pipefd3[1]);
        read(pipefd3[0], msg, 15);
        close(pipefd3[0]);
        printf("\n BANYA MONDTA: \t %s\n", msg);
        sleep(1);

        kill(getppid(), SIGTERM);
        close(pipefd3[1]);
        read(pipefd3[0], msg, 15);
        close(pipefd3[0]);
        printf("\n BANYA MONDTA: \t %s\n", msg);
        sleep(1);

        kill(getppid(), SIGTERM);
        close(pipefd3[1]);
        read(pipefd3[0], msg, 15);
        close(pipefd3[0]);
        printf("\n BANYA MONDTA: \t %s\n\n", msg);

        fogad(uzenetsor);
    }
    else
    {
        //banya
        pid2 = fork();

        if (pid2 == -1)
        {
            perror("Fork hiba");
            exit(EXIT_FAILURE);
        }

        if (pid2 == 0)
        {
            //jancsi
            kill(getppid(), SIGUSR2);
            int kapott;
            close(pipefd2[1]);
            read(pipefd2[0], &kapott, sizeof(int));
            close(pipefd2[0]);

            if (kapott == 1)
            {
                printf("\nJANCSI \t\t KALACS\n");
            }
            else
            {
                printf("\nJANCSI \t\t SÜTI\n");
            }
            sleep(3);
            int kover_vok_e = rand() % 3;
            if (kover_vok_e == 2)
            {
                kover_vok_e = -1;
            }
            close(pipefd4[0]);
            write(pipefd4[1], &kover_vok_e, sizeof(int));
            close(pipefd4[1]);

            kuld(uzenetsor);
        }
        else
        {
            //banya
            sleep(2);
            int finomsag = rand() % 2; //0,1
            close(pipefd[0]);
            write(pipefd[1], &finomsag, sizeof(int));
            close(pipefd[1]);

            int finomsag2 = rand() % 2;
            close(pipefd2[0]);
            write(pipefd2[1], &finomsag2, sizeof(int));
            close(pipefd2[1]);

            int kapott_koverseg;
            close(pipefd4[1]);
            read(pipefd4[0], &kapott_koverseg, sizeof(int));
            close(pipefd4[0]);
            sleep(1);
            printf("\nBANYA konstallálciója : %d  JNACSI kövésrége\n", kapott_koverseg);

            waitpid(pid, &status, 0);
            waitpid(pid2, &status, 0);
            status = msgctl(uzenetsor, IPC_RMID, NULL);
            if (status < 0)
                perror("msgctl");
            printf("\n\nPÁPÁ\n\n");
        }
    }

    exit(EXIT_SUCCESS);
}