//2017.12.11
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
#include <sys/msg.h>
#include <mqueue.h> // POSIX, modernebb üzenetsor
#include <sys/ipc.h>
#include <sys/shm.h>

#define felderito1 SIGUSR1
#define felderito2 SIGUSR2
//KápóVadász szülő
//felderítő(k) gyerek1
//fejvadász(ok) gyerek2

typedef struct HolAKocsi
{
    char *varosok[100];  //dunakeszi vac godollo
    char *eredmeny[100]; //van/nincs
    int varosok_szama;
} HolAKocsi;

void handler(int signumber)
{
    printf("\nAz %i azonosítójú feldírtő elindítja a keresést\n", signumber);
}

int main(int argc, char *argv[])
{
    /*
    Szülő parancssorból olvas: ./kocsi.out dunakeszi vac godollo
    -->OSZTOTT MEMÓRIÁBA ÍR
    felderítők???? mi a csuda*/

    pid_t pid;  //felderítő1
    pid_t pid2; //felderítő2
    pid_t pid3; //fejvadász
    int pipefd[2];
    int pipefd2[2];
    int status;

    signal(felderito1, handler);
    signal(felderito2, handler);

    /*OSZTOTTMEMO*/
    key_t kulcs;
    int oszt_mem_id;
    char *s;
    kulcs = ftok(argv[0], 1);
    oszt_mem_id = shmget(kulcs, 500, IPC_CREAT | S_IRUSR | S_IWUSR);
    s = shmat(oszt_mem_id, NULL, 0);
    /*OSZTOTT MEMO*/

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
    if (pid == -1)
    {
        perror("Fork hiba");
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {
        //felderítő1
        sleep(1);
        printf("\nFELDERÍTŐ1 jelentkezik, azonosítója: %d\n", getpid());
        char varos[100];
        strcpy(varos, s);
        printf("\nA FELDERÍTŐ1 ezt olvasta az osztott memoriabol: %s\n", s);
        shmdt(s); // gyerek1 elengedi
        HolAKocsi kocsi;

        int i = 0;
        char *p = strtok(varos, " ");
        while (p != NULL)
        {
            kocsi.varosok[i] = p;
            p = strtok(NULL, " ");
            i++;
        }

        for (int j = 0; j < i; j++)
        {
            printf("\nKOCSI.VAROSOK[%d] %s", j, kocsi.varosok[j]);
        }
        kill(getppid(), felderito1); //jelzés a szülőbe
        printf("\n");
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
            //felderítő2
            sleep(2);
            printf("\nFELDERÍTŐ2 jelentkezik, azonosítója: %d\n", getpid());
            char varos[100];
            strcpy(varos, s);
            printf("\nA FELDERÍTŐ2 ezt olvasta az osztott memoriabol: %s\n", s);
            shmdt(s); // gyerek1 elengedi
            HolAKocsi kocsi;

            int i = 0;
            char *p = strtok(varos, " ");
            while (p != NULL)
            {
                kocsi.varosok[i] = p;
                p = strtok(NULL, " ");
                i++;
            }

            for (int j = 0; j < i; j++)
            {
                printf("\nKOCSI.VAROSOK[%d] %s", j, kocsi.varosok[j]);
            }
            kill(getppid(), felderito2); //jelzés a szülőbe
            printf("\n");
        }
        else
        {
            //KápóVadász
            /*HASZNOS DEBUG RÉSZ
            char a[30] = "CICA";
            char b[30] = "Macska";
            char c[30];
            strcpy(c, a); //CICA
            strcpy(a, b); //Macska
            printf("%s\n", c);
            printf("%s\n", a);*/

            int meret = 1;
            char varos[100];
            strcpy(varos, argv[1]);
            // strcpy(kocsi.varosok[0], argv[1]);

            for (int i = 2; i < argc; i++)
            {
                strcat(varos, " ");
                strcat(varos, argv[i]);
                meret++;
            }

            // printf("\nMERET %d", meret);
            // printf("\nVAROSOK %s", varos);
            strcpy(s, varos); // beirunk a memoriaba
            shmdt(s);         // elengedjuk az osztott memoriat
            /************************************************************/
            HolAKocsi kocsi;
            kocsi.varosok_szama = meret;
            int i = 0;
            char *p = strtok(varos, " ");
            char *arr[3];

            while (p != NULL)
            {
                kocsi.varosok[i] = p;
                p = strtok(NULL, " ");
                i++;
            }

            /* for (int i = 0; i < meret; i++)
            {
                printf("\nKOCSI.VAROSOK[%d] %s", i, kocsi.varosok[i]);
            }*/

            waitpid(pid, &status, 0);
            waitpid(pid2, &status, 0);
            shmctl(oszt_mem_id, IPC_RMID, NULL); // SHARED MEMORY TÖRLÉSE
            printf("\nSHAREDMEMORY törölve, csácsá\n");
        }
    }

    exit(EXIT_SUCCESS);
}