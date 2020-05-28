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
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <wait.h>

#define MEMSIZE 1024

int szemafor_letrehozas(const char *pathname, int szemafor_ertek)
{
    int semid;
    key_t kulcs;

    kulcs = ftok(pathname, 1);
    if ((semid = semget(kulcs, 1, IPC_CREAT | S_IRUSR | S_IWUSR)) < 0)
        perror("semget");
    // semget 2. parameter is the number of semaphores
    if (semctl(semid, 0, SETVAL, szemafor_ertek) < 0) //0= first semaphores
        perror("semctl");

    return semid;
}

void szemafor_muvelet(int semid, int op)
{
    struct sembuf muvelet;

    muvelet.sem_num = 0;
    muvelet.sem_op = op; // op=1 up, op=-1 down
    muvelet.sem_flg = 0;

    if (semop(semid, &muvelet, 1) < 0) // 1 number of sem. operations
        perror("semop");
}

void szemafor_torles(int semid)
{
    semctl(semid, 0, IPC_RMID);
}

void handler(int signumber)
{
    printf("%i számú személy jelzez TEAMSEN\n", signumber);
}

struct uzenet
{
    long mtype;
    char mtext[1024];
};
int tipp_gy2 = 0;
// sendig a message
int kuld(int uzenetsor)
{

    struct uzenet uz = {5, "Jó napot"};

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
    {
        perror("msgsnd");
    }
    else
    {
        printf("\nELSŐ feladat\n");
        printf("\nOktatónak köszön gyerek : %ld, szovege:  %s\n", uz.mtype, uz.mtext);
    }
    return 0;
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));

    pid_t pid;  //rendőr
    pid_t pid2; //orvos
    int status;
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

    /*OSZTOTTMEMO*/
    key_t kulcs2;
    int oszt_mem_id;
    char *s;
    kulcs2 = ftok(argv[0], 2);
    oszt_mem_id = shmget(kulcs2, 500, IPC_CREAT | S_IRUSR | S_IWUSR);
    s = shmat(oszt_mem_id, NULL, 0);
    /*OSZTOTT MEMO*/

    /*SZEMAFOR*/
    int semid = szemafor_letrehozas(argv[0], 0); // sem state is down!!!
    /*************/

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
        //gyerek1 oktató
        kill(getppid(), SIGUSR1);
        sleep(1);
        fogad(uzenetsor);
        /*2es feladat*/
        char msg[50] = "Melyik a default ütemező a Linux rendszerben?";
        close(pipefd[0]);
        write(pipefd[1], msg, strlen(msg) + 1);
        close(pipefd[1]);
        //sleep(2);
        szemafor_muvelet(semid, -1);
        printf("\n\n\t HARMAS FELADAT ÉS NÉGYES\n");
        printf("\nAz OKTATÓ kiolvassa a választ az osztott memoriabol: %s\n", s);
        szemafor_muvelet(semid, 1);
        shmdt(s); // gyerek1 elengedi
    }
    else
    {
        pid2 = fork();
        if (pid2 == -1)
        {
            perror("Fork hiba");
            exit(EXIT_FAILURE);
        }
        if (pid2 == 0)
        {
            //gyerek2 hallgató
            kill(getppid(), SIGUSR2);
            sleep(1);
            kuld(uzenetsor);
            /*2es feladat*/

            char msg2[50];
            close(pipefd2[1]);
            read(pipefd2[0], msg2, 50);
            close(pipefd2[0]);
            printf("\nDIÁKUNKnak feltett kérdés az anyagból, melyet TEAMS közvetített: %s\n", msg2);
            printf("\nDIÁKUNK osztott memóriába írja a választ:\n");
            /*OSZTOTT MEMO*/
            char valasz[5] = "CFS";
            strcpy(s, valasz); // beirunk a memoriaba
            szemafor_muvelet(semid, 1);
            shmdt(s);
        }
        else
        {
            //szülő
            char msg[50];
            close(pipefd[1]);
            read(pipefd[0], msg, 50);
            close(pipefd[0]);
            printf("\n\tMÁSODIK FELADAT\n");
            // printf("\nDIÁKUNKnak feltett kérdés az anyagból: %s\n", msg);

            close(pipefd2[0]);
            write(pipefd2[1], msg, strlen(msg) + 1);
            close(pipefd2[1]);

            waitpid(pid, &status, 0);
            waitpid(pid2, &status, 0);
            printf("\nAz órának vége. A program bezár\n");
            szemafor_torles(semid);
            shmctl(oszt_mem_id, IPC_RMID, NULL); // SHARED MEMORY TÖRLÉSE
            status2 = msgctl(uzenetsor, IPC_RMID, NULL);
            if (status2 < 0)
                perror("msgctl");
        }
    }
    exit(EXIT_SUCCESS);
}