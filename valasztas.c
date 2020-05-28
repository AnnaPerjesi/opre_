//VALASZTAS
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

//elnök - szülő
//ellenőrző - gyerek1
//pecsételő - gyerek2
typedef struct Szavazatok
{
    int szavazoszam;
    int azonositok[100];
    int szavazhat[100];
    int szavazo_kepes;
    int nem_szavazo_kepes;
    int partok[100];
} Szavazatok;

void handler(int signumber)
{
    printf("Signal with number %i has arrived\n", signumber);
}
//handler 2 bugos, mármint a fg jó, csak nem akkor hívódik meg, amikor szeretném
void handler2(int signumber)
{
    printf("JELZES GYEREK2-BE %i\n", signumber);
}

int main(int argc, char const *argv[])
{
    srand(time(0));

    pid_t pid;  //ellenörző
    pid_t pid2; //pecsetelő
    int status;
    int pipefd[2];
    int pipefd2[2];
    int pipefd3[2];
    int pipefd4[2];

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

    /*SIGNALS*/
    signal(SIGUSR1, handler);
    signal(SIGUSR2, handler);
    signal(SIGTERM, handler2);
    /**/

    pid = fork();

    if (pid == -1)
    {
        perror("Fork hiba");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    { //gyerek1

        kill(getppid(), SIGUSR1); //jelzés a szülőnek

        Szavazatok szavazas;

        close(pipefd[1]);
        read(pipefd[0], &szavazas, sizeof(szavazas));
        close(pipefd[0]);

        printf("\nEllenőr kiírja az adatokat\n\n");
        for (int i = 0; i < szavazas.szavazoszam; i++)
        {
            printf("\n%i -szavazo %d - azon\n", i + 1, szavazas.azonositok[i]);
        }

        int validalas;
        for (int i = 0; i < szavazas.szavazoszam; i++)
        {
            validalas = rand() % 100;
            if (validalas < 20)
            {
                szavazas.szavazhat[i] = 0;
            }
            else
            {
                szavazas.szavazhat[i] = 1;
            }
        }
        /*TESZTELNI 
        for (int i = 0; i < szavazas.szavazoszam; i++)
        {
            printf("\n%i -szavazo %d - azon %i - szavazhat\n", i + 1, szavazas.azonositok[i], szavazas.szavazhat[i]);
        }
        */

        close(pipefd2[0]);
        write(pipefd2[1], &szavazas, sizeof(szavazas)); //visszaírom a csőbe a structot kiegészítve a szavazhat/nemszavazhattal
        close(pipefd2[1]);

        int kapott_pidszam;
        close(pipefd4[1]);
        read(pipefd4[0], &kapott_pidszam, sizeof(int));
        close(pipefd4[0]);

        kill(kapott_pidszam, SIGTERM); //jelzes gyerek2-nek
    }
    else
    { //szülő
        pid2 = fork();

        if (pid2 == -1)
        {
            perror("Fork hiba");
            exit(EXIT_FAILURE);
        }

        if (pid2 == 0)
        {                             //gyerek2
            kill(getppid(), SIGUSR2); //jelzés a szülőnek

            int pid2szam = getpid();
            close(pipefd4[0]);
            write(pipefd4[1], &pid2szam, sizeof(int));
            close(pipefd4[1]);

            Szavazatok szavazas;
            close(pipefd2[1]);
            read(pipefd2[0], &szavazas, sizeof(szavazas));
            close(pipefd2[0]);
            sleep(2);
            printf("\nPECSÉTELŐ kiírja az adatokat\n\n");
            for (int i = 0; i < szavazas.szavazoszam; i++)
            {
                printf("\n%i -szavazo %d - azon %s - szavazhat\n", i + 1, szavazas.azonositok[i], szavazas.szavazhat[i] == 1 ? "igen" : "nem");
            }
            /*ÖSSZESíTÉS*/
            int szavazhat = 0;
            int nem_szavazhat = 0;
            //int leadando_szavazat;
            for (int i = 0; i < szavazas.szavazoszam; i++)
            {
                int leadando_szavazat = rand() % 4; //0,1,2,3
                if (szavazas.szavazhat[i] == 1)
                {
                    szavazhat = szavazhat + 1;
                    szavazas.partok[i] = leadando_szavazat; //leadjuk a szavazatot is (3mas feladat, nem használok 4edik csövet)
                }
                else
                {
                    nem_szavazhat = nem_szavazhat + 1;
                }
            }
            szavazas.szavazo_kepes = szavazhat;
            szavazas.nem_szavazo_kepes = nem_szavazhat;
            /*CSORO megoldas, mindig structot kuldom, akkor is, ha nem kell, csak nyilván felveszem bele az új adatokat*/
            close(pipefd3[0]);
            write(pipefd3[1], &szavazas, sizeof(szavazas));
            close(pipefd3[1]);
        }
        else
        { //szülő
            pause();
            printf("\nMEGÉRKEZETT MIND A KÉT JELZÉS GOOO\n");

            if (argc < 2)
            {
                puts("Hiányzó argumentum!");
                exit(-1);
            }
            Szavazatok szavazas;

            szavazas.szavazoszam = atoi(argv[1]); //itt gyűjtöm be a parancssorról a szavazószámot ./valasztas.out 5
            printf("\n%d a szavazók száma\n", szavazas.szavazoszam);

            for (int i = 0; i < szavazas.szavazoszam; i++)
            {
                szavazas.azonositok[i] = rand() % 100;
            }
            /*TESZT? HOGY MINDEN OKÉS
            for (int i = 0; i < szavazas.szavazoszam; i++)
            {
                printf("\n%i -szavazo %d - azon\n", i + 1, szavazas.azonositok[i]);
            }*/
            close(pipefd[0]);
            write(pipefd[1], &szavazas, sizeof(szavazas));
            close(pipefd[1]);

            close(pipefd3[1]);
            read(pipefd3[0], &szavazas, sizeof(szavazas));
            close(pipefd3[0]);
            waitpid(pid, &status, 0);
            waitpid(pid2, &status, 0);

            int igen = szavazas.szavazo_kepes;
            int nem = szavazas.nem_szavazo_kepes;
            int sz = szavazas.szavazoszam;
            //printf("%0.2f", ((float)igen / (float)sz) * 100);
            printf("\nELNÖK hírdet, szavazhat %i fő, nem szavazhat %i fő", szavazas.szavazo_kepes, szavazas.nem_szavazo_kepes);
            printf("\nELNÖK hírdet, szavazhat %0.2f százalék, nem szavazhat %0.2f százalék", ((float)igen / (float)sz) * 100, ((float)nem / (float)sz) * 100);
            /**/
            int egy = 0;
            int ketto = 0;
            int harom = 0;
            int negy = 0;
            for (int i = 0; i < szavazas.szavazo_kepes; i++)
            {
                if (szavazas.partok[i] == 0)
                {
                    egy = egy + 1;
                }
                else if (szavazas.partok[i] == 1)
                {
                    ketto = ketto + 1;
                }
                else if (szavazas.partok[i] == 2)
                {
                    ketto = ketto + 1;
                }
                else
                {
                    harom = harom + 1;
                }
            }
            printf("\negyes part: %i\nkettespart: %i\nharmas part: %i\nnegyes part: %i\n", egy, ketto, harom, negy);
            waitpid(pid, &status, 0);
            waitpid(pid2, &status, 0);
            printf("\n\nELNÖK: Vége is\n");
        }
    }

    exit(EXIT_SUCCESS);
}