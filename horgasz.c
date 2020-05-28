//2019.12.19 Ho-ho-ho-horgász
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
//horgász - SZÜLŐ
//főcsali - GYEREK
//hal -gyerek
#define SIG_KAPAS_VAN SIGUSR1
#define SIG_UJ_KAPAS SIGUSR2

void handler(int signumber)
{
    sleep(1);
    printf("A %i számú hal SIKERESEN KIFOGVA\n", signumber);
}

void handler2(int signumber)
{
    sleep(1);
    printf("\nA %i számú halra +10 százalékos az esély\n", signumber);
}

struct uzenet
{
    long mtype; //ez egy szabadon hasznalhato ertek, pl uzenetek osztalyozasara
    char mtext[1024];
};

int kuld(int uzenetsor, int talalat)
{ //5 üzenet kategóriája/osztálya
    struct uzenet uz;
    if (talalat < 50)
    {
        uz.mtype = 5;
        strcpy(uz.mtext, "HALAT LÁTOK FŐNI");
    }
    else
    {
        uz.mtype = 5;
        strcpy(uz.mtext, "NINCS HAL FŐNI");
    }

    int status;

    status = msgsnd(uzenetsor, &uz, strlen(uz.mtext) + 1, 0); //elküldted, utána bármi másra jó a status/uzenet stb
    if (status < 0)
    {
        perror("msgsnd");
    }

    status = msgrcv(uzenetsor, &uz, 1024, 3, 0);

    if (status < 0)
        perror("msgsnd");
    else
        printf("A kapott uzenet kodja a Horgásztól: %ld, szovege:  %s\n", uz.mtype, uz.mtext);

    return 0;
}

int fogad(int uzenetsor)
{
    struct uzenet uz;
    int status;

    status = msgrcv(uzenetsor, &uz, 1024, 5, 0);

    if (status < 0)
        perror("msgsnd");
    else
        printf("A kapott uzenet kodja a CSALITÓL: %ld, szovege:  %s\n", uz.mtype, uz.mtext);

    if (strcmp(uz.mtext, "HALAT LÁTOK FŐNI") == 0)
    {
        struct uzenet uz = {3, "Kapásra fel"};
        status = msgsnd(uzenetsor, &uz, strlen(uz.mtext) + 1, 0); //elküldted, utána bármi másra jó a status/uzenet stb
        if (status < 0)
        {
            perror("msgsnd");
        }
    }
    else
    {
        struct uzenet uz = {3, "Kapaszkodj, új pozícióra felkészülni!"};
        status = msgsnd(uzenetsor, &uz, strlen(uz.mtext) + 1, 0); //elküldted, utána bármi másra jó a status/uzenet stb
        if (status < 0)
        {
            perror("msgsnd");
        }
    }
    return 0;
}

int talal; //csövezés miatt, mertamúgy beakad

int main(int argc, char const *argv[])
{
    srand(time(0));

    pid_t pid;
    int gy_status;

    /*Második feladathoz*/
    pid_t pid2;
    int pipefd[2];
    int pipefd2[2];
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

    signal(SIG_KAPAS_VAN, handler);
    /**************************************/
    /**HÁRMAS*/
    int pipefd3[2];
    int pipefd4[2];
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
    signal(SIG_UJ_KAPAS, handler2);
    /***************************************/
    /*ÜZENETHEZ*/
    int uzenetsor, status;
    key_t kulcs;
    kulcs = ftok(argv[0], 1);
    //printf("A kulcs: %d\n", kulcs);
    uzenetsor = msgget(kulcs, 0600 | IPC_CREAT);
    /**/
    pid = fork();

    if (pid == -1)
    {
        perror("Fork hiba");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    {
        printf("\nÉn a főcsali vagyok, 50 százalékos esélyem van, hogy halat látok (szám<50)\n");
        //főcsali
        talal = rand() % 100;
        if (talal < 50)
        {
            printf("\n%d\n", talal);
            printf("\nCsali: talalt\n");
        }
        else
        {
            printf("\n%d\n", talal);
            printf("Csali: nem talalt\n");
        }
        printf("Megüzenem a Főninek, hogy látok halat, vagy sem\n\n");
        kuld(uzenetsor, talal);
        sleep(2);

        int num2 = 4;
        int num3 = 5;
        if (talal < 50)
        {
            char msg[50] = "Van-e kedve a Főnök kondérjába végezni?";
            close(pipefd[0]);
            write(pipefd[1], msg, strlen(msg) + 1);
            close(pipefd[1]);
            int num;
            close(pipefd2[1]);
            read(pipefd2[0], &num, sizeof(int));
            close(pipefd2[0]);
            // printf("\n\t\t %d\n", num);
            num2 = num;

            close(pipefd2[1]);
            read(pipefd2[0], &num, sizeof(int));
            close(pipefd2[0]);
            num3 = num;
        }
        else
        {
            printf("NEM VOLT KAPAS\n");
        }

        if (num2 < 3)
        {
            // printf("\n\t\t %d\n", num2);
            kill(getppid(), SIG_KAPAS_VAN);
        }
        else
        {
            printf("Halunknak NEM volt kedve a főnök kondérjába végezni");
            char msg[50] = "Gyere HAL, jó hely a kondér";
            close(pipefd3[0]);
            write(pipefd3[1], msg, strlen(msg) + 1);
            close(pipefd3[1]);

            kill(getppid(), SIG_UJ_KAPAS);
        }
        if (num3 > 5)
        {
            kill(getppid(), SIG_KAPAS_VAN);
        }
        else
        {
            printf("\nÚjabb kudarc\n");
        }
    }
    else
    { //horgász

        pid2 = fork();
        if (pid == -1)
        {
            perror("Fork hiba");
            exit(EXIT_FAILURE);
        }

        if (pid2 == 0)
        {
            //hal
            if (talal < 50)
            {
                char msg[50];
                close(pipefd[1]);
                read(pipefd[0], msg, 50);
                close(pipefd[0]);
                printf("\n%s\n", msg); //kiolvastuk az üzenetet

                int dontes = rand() % 10; //döntés, van e kedvünk kondétozni
                close(pipefd2[0]);
                write(pipefd2[1], &dontes, sizeof(int));
                close(pipefd2[1]);
                sleep(2);
                char msg2[50];
                close(pipefd3[1]);
                read(pipefd3[0], msg2, 50);
                close(pipefd3[0]);
                printf("\n%s\n", msg2); //kiolvastuk az üzenetet

                dontes = rand() % 10; //döntés, van e kedvünk kondétozni
                close(pipefd2[0]);
                write(pipefd2[1], &dontes, sizeof(int));
                close(pipefd2[1]);
            }
        }
        else
        {
            //horgász
            printf("\nHorgász a tónál, bedobja a csalit\n");
            fogad(uzenetsor);
            sleep(1);

            printf("\n\n\t GY2 SZÜLŐ \n\n");

            wait(NULL);
            //ÜZENETSOR TÖRLÉSE MIUTÁN TERMINÁLT MINDEN IS SZÜLŐBEN!!!
            status = msgctl(uzenetsor, IPC_RMID, NULL);
            if (status < 0)
            {
                perror("msgctl");
            }
            sleep(2);
            printf("\nHorgász dönt: Indulás haza\n\n");
        }
    }

    exit(EXIT_SUCCESS);
}