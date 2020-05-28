//2018.12.6
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

struct uzenet
{
    long mtype;
    char mtext[1024];
};
int tipp_gy2 = 0;
// sendig a message
int kuld(int uzenetsor)
{
    int tipp = rand() % 2;
    struct uzenet uz;
    if (tipp == 0)
    {
        tipp_gy2 = -1;
        uz.mtype = 5;
        strcpy(uz.mtext, "NEM találja el");
    }
    else
    {
        tipp_gy2 = 1;
        uz.mtype = 5;
        strcpy(uz.mtext, "eltalálja");
    }

    int status;

    status = msgsnd(uzenetsor, &uz, strlen(uz.mtext) + 1, 0);
    if (status < 0)
        perror("msgsnd");

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
        printf("KETTES játékostól kapott tipp kodja: %ld, szovege:  %s\n", uz.mtype, uz.mtext);

    return 0;
}

void handler(int signumber)
{
    printf("A %i számú versenyző KÉSZEN ÁLL\n", signumber);
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    pid_t pid;
    pid_t pid2;
    int status;

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

    int pipefd[2];
    int pipefd2[2];
    int pipefd3[2];
    int pipefd4[2];
    int pipefd5[2];
    int pipefd6[2];
    int pipefd7[2];
    int pipefd8[2];

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
    if (pipe(pipefd5) == -1)
    {
        perror("Hiba a pipe nyitaskor!");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipefd6) == -1)
    {
        perror("Hiba a pipe nyitaskor!");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipefd7) == -1)
    {
        perror("Hiba a pipe nyitaskor!");
        exit(EXIT_FAILURE);
    }
    if (pipe(pipefd8) == -1)
    {
        perror("Hiba a pipe nyitaskor!");
        exit(EXIT_FAILURE);
    }

    char *nevek[] = {"Bencus", "Dániel", "Anna", "Marci", "Fanni"};
    int idx1 = rand() % 4;
    int idx2 = rand() % 4;
    int gy1_v = rand() % 5 + 1;
    int gy2_v = rand() % 5 + 1;
    int gy1_v2 = rand() % 5 + 1;

    signal(SIGUSR1, handler);
    signal(SIGUSR2, handler);

    pid = fork();

    if (pid == -1)
    {
        perror("Fork hiba");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        //gyerek1
        kill(getppid(), SIGUSR1);
        sleep(1);
        printf("Az én nevem: %s\n", nevek[idx1]);
        close(pipefd[0]);
        write(pipefd[1], nevek[idx1], strlen(nevek[idx1]) + 1);
        close(pipefd[1]);

        char msg[50];
        close(pipefd3[1]);
        read(pipefd3[0], msg, 50);
        close(pipefd3[0]);
        printf("\nKÉRDÉS GY1: %s\n", msg);
        printf("\nGY1 VÁLASZOM: %d\n", gy1_v);

        close(pipefd5[0]);
        write(pipefd5[1], &gy1_v, sizeof(int));
        close(pipefd5[1]);

        close(pipefd7[1]);
        read(pipefd7[0], msg, 50);
        close(pipefd7[0]);
        printf("\nMÁSODIK KÉRDÉS GY1: %s\n", msg);
        printf("\nGY1 VÁLASZOM: %d\n", gy1_v2);

        close(pipefd8[0]);
        write(pipefd8[1], &gy1_v2, sizeof(int));
        close(pipefd8[1]);
    }
    else
    {
        //szülő
        pid2 = fork();

        if (pid2 == -1)
        {
            perror("Fork hiba");
            exit(EXIT_FAILURE);
        }
        if (pid2 == 0)
        {
            //gyerek2
            kill(getppid(), SIGUSR2);
            sleep(1);
            printf("\nAz én nevem: %s\n", nevek[idx2]);
            close(pipefd2[0]);
            write(pipefd2[1], nevek[idx2], strlen(nevek[idx2]) + 1);
            close(pipefd2[1]);

            char msg[50];
            close(pipefd4[1]);
            read(pipefd4[0], msg, 50);
            close(pipefd4[0]);
            printf("\nKÉRDÉS GY2: %s\n", msg);
            printf("\nGY2 VÁLASZOM: %d\n", gy2_v);

            close(pipefd6[0]);
            write(pipefd6[1], &gy2_v, sizeof(int));
            close(pipefd6[1]);
            kuld(uzenetsor);
        }
        else
        {
            //szülő
            int cnt1 = 0;
            int cnt2 = 0;
            sleep(2);
            char msg[50];
            close(pipefd[1]);
            read(pipefd[0], msg, 50);
            close(pipefd[0]);
            printf("\nEGYES VERSENYZŐ: %s\n", msg); //kiolvastuk az üzenetet

            char msg2[50];
            close(pipefd2[1]);
            read(pipefd2[0], msg2, 50);
            close(pipefd2[0]);
            printf("\nKETTES VERSENYZŐ: %s\n", msg2); //kiolvastuk az üzenetet

            char *kerdesek[] = {"kerdes1", "kerdes2", "kerdes3", "kerdes4", "kerdes5"};
            int valaszok[] = {1, 2, 3, 4, 5};
            sleep(2);
            close(pipefd3[0]);
            write(pipefd3[1], kerdesek[idx1], strlen(kerdesek[idx1]) + 1);
            close(pipefd3[1]);

            close(pipefd4[0]);
            write(pipefd4[1], kerdesek[idx1], strlen(kerdesek[idx1]) + 1);
            close(pipefd4[1]);

            int gyerek1_valasz;
            close(pipefd5[1]);
            read(pipefd5[0], &gyerek1_valasz, sizeof(int));
            close(pipefd5[0]);

            int gyerek2_valasz;
            close(pipefd6[1]);
            read(pipefd6[0], &gyerek2_valasz, sizeof(int));
            close(pipefd6[0]);
            sleep(2);

            printf("\nEGYES VERSENYZŐ VÁLASZ: %d\n", gyerek1_valasz);
            printf("KETTES VERSENYZŐ VÁLASZ: %d\n", gyerek2_valasz);
            sleep(1);
            if (valaszok[idx1] == gyerek1_valasz)
            {
                printf("\nGY1 mikulást kap\n");
                cnt1++;
            }
            else
            {
                printf("\nGY1 virgácsot kap\n");
            }

            if (valaszok[idx1] == gyerek2_valasz)
            {

                printf("\nGY2 mikulást kap\n");
                cnt2++;
            }
            else
            {
                printf("\nGY2 virgácsot kap\n");
            }
            sleep(1);
            printf("\n\t\tMÁSODIK FORDULÓ\n");

            close(pipefd7[0]);
            write(pipefd7[1], kerdesek[idx2], strlen(kerdesek[idx2]) + 1);
            close(pipefd7[1]);

            sleep(1);
            int gyerek1_valasz2;
            close(pipefd5[1]);
            read(pipefd5[0], &gyerek1_valasz2, sizeof(int));
            close(pipefd5[0]);

            fogad(uzenetsor);

            if (valaszok[idx2] == gyerek1_valasz2)
            {
                printf("\nGY1 mikulást kap\n");
                cnt1++;
                if (tipp_gy2 == 1)
                {
                    cnt2++;
                }
            }
            else
            {
                printf("\nGY1 virgácsot kap\n");
            }

            sleep(1);
            printf("\n\tÁllás: Gy2\t %d \t Gy1\t %d", cnt1, cnt2);

            waitpid(pid, &status, 0);
            waitpid(pid2, &status, 0);
            printf("\n\nKÖSZI A FIGYELMET\n\n");
            status2 = msgctl(uzenetsor, IPC_RMID, NULL);
            if (status < 0)
                perror("msgctl");
        }
    }

    exit(EXIT_SUCCESS);
}