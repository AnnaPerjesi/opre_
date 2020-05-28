

void bead1()
{
    /*READING THE FILE AND MAKE A LIST ABOUT FREE PLACES/DAY*/
    int f = open("output.txt", O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (f < 0)
    {
        perror("Error 'cause of reading the file\n");
        exit(-1);
    }

    Person person;
    Person people_number[70]; //10 to every day
    int num = 1;              //first, second, third..etc

    while (read(f, &person, sizeof(Person)))
    {
        people_number[num].id = person.id;
        strcpy(people_number[num].name, person.name);
        strcpy(people_number[num].address, person.address);
        strcpy(people_number[num].days, person.days);
        num += 1; //num will be 1 anytime, so congrats I've to indexing from 1 not from 0
    }

    /*COUNTING.. until reach num-> number of people who had applied*/
    int left_places[7] = {10, 10, 10, 10, 10, 10, 10};
    char *denim = " ";
    char *token;
    char *copied_days = malloc(sizeof(char) * 70);

    for (int i = 1; i < num; i++)
    {

        strcpy(copied_days, people_number[i].days);
        token = strtok(copied_days, denim);
        /*KILLING SPACES -SEPARATE DAYS TO KNOW HOW MANY DAYS LEFT-*/
        while (token != NULL)
        {
            if (strcmp(token, "hetfo") == 0)
            {
                left_places[0] -= 1;
            }
            else if (strcmp(token, "kedd") == 0)
            {
                left_places[1] -= 1;
            }
            else if (strcmp(token, "szerda") == 0)
            {
                left_places[2] -= 1;
            }
            else if (strcmp(token, "csutortok") == 0)
            {
                left_places[3] -= 1;
            }
            else if (strcmp(token, "pentek") == 0)
            {
                left_places[4] -= 1;
            }
            else if (strcmp(token, "szombat") == 0)
            {
                left_places[5] -= 1;
            }
            else if (strcmp(token, "vasarnap") == 0)
            {
                left_places[6] -= 1;
            }
            token = strtok(NULL, denim);
        }
    }
    close(f);
    free(copied_days); //I'm not sure this is valid or not

    printOutLeftPlaces(left_places);

    int category;
    do
    {
        printf("\n\nAdja meg a kivant tevekenyseg kodjat\n");
        printf("Jelentkezes:\t\t 1\n");
        printf("Adat modositas:\t\t 2\n");
        printf("Adat torles:\t\t 3\n");
        printf("Adat lista napokra:\t 4\n");
        printf("Adat lista osszesre:\t 5\n");
        printf("Kilepes:\t\t 0\n");
        printf("A kod helye: ");
        scanf("%d", &category);
        // checkInput(category);

        switch (category)
        {
        case 1:
        {
            /*COMES TO APPLICATIONS*/
            if (num == 70) //1..70 = 70db
            {
                printf("\nThere is enough volunteer so thx, Come back next time\n");
                exit(70);
            }

            Person applicant; //people_number[num].name
            fgets(applicant.name, 100, stdin);
            while (strcmp(trim(applicant.name), "") == 0)
            {
                printf("Adja meg a nevet: ");
                fgets(applicant.name, 100, stdin);
                strcpy(applicant.name, trim(applicant.name));
            }
            strcpy(applicant.name, trim(applicant.name));

            printf("Adja meg a cimet: ");
            fgets(applicant.address, 100, stdin);
            while (strcmp(trim(applicant.address), "") == 0)
            {
                printf("Adja meg a cimet: ");
                fgets(applicant.address, 100, stdin);
                strcpy(applicant.address, trim(applicant.address));
            }
            strcpy(applicant.address, trim(applicant.address));

            printf("Adja meg a napokat kisbetuvel, ekezetek nelkul: ");
            fgets(applicant.days, 64, stdin);
            while (strcmp(trim(applicant.days), "") == 0)
            {
                printf("Adja meg a napokat, amikor dolgozni szeretne, KISBETUVEL + EKEZETEK NELKUL: ");
                fgets(applicant.days, 64, stdin);
                strcpy(applicant.days, trim(applicant.days));
            }
            strcpy(applicant.days, applicant.days);

            char *copied_days_two = malloc(sizeof(char) * 70);
            strcpy(copied_days_two, trim(applicant.days));

            char *denim = " ";
            char *token;
            token = strtok(copied_days_two, denim);
            bool unable = false;
            int mon = 0;
            int tue = 0;
            int wed = 0;
            int thu = 0;
            int fri = 0;
            int sat = 0;
            int sun = 0;
            while (token != NULL)
            {
                if (strstr(token, "\n"))
                {
                    token[strlen(token) - 1] = 0;
                }
                if (strcmp(token, "hetfo") == 0)
                {

                    mon += 1;
                    left_places[0] -= 1;
                    if (left_places[0] == -1)
                    {
                        unable = true;
                    }
                }
                else if (strcmp(token, "kedd") == 0)
                {

                    tue += 1;
                    left_places[1] -= 1;
                    if (left_places[1] == -1)
                    {
                        unable = true;
                    }
                }
                else if (strcmp(token, "szerda") == 0)
                {

                    wed += 1;
                    left_places[2] -= 1;
                    if (left_places[2] == -1)
                    {
                        unable = true;
                    }
                }
                else if (strcmp(token, "csutortok") == 0)
                {
                    thu += 1;
                    left_places[3] -= 1;
                    if (left_places[3] == -1)
                    {
                        unable = true;
                    }
                }
                else if (strcmp(token, "pentek") == 0)
                {

                    fri += 1;
                    left_places[4] -= 1;
                    if (left_places[4] == -1)
                    {
                        unable = true;
                    }
                }
                else if (strcmp(token, "szombat") == 0)
                {
                    sat += 1;
                    left_places[5] -= 1;
                    if (left_places[5] == -1)
                    {
                        unable = true;
                    }
                }
                else if (strcmp(token, "vasarnap") == 0)
                {
                    sun += 1;
                    left_places[6] -= 1;
                    if (left_places[6] == -1)
                    {
                        unable = true;
                    }
                }
                else
                {
                    printf("\nNincs olyan nap a szotaramban, amit megadtal\n");
                    exit(2);
                }
                token = strtok(NULL, denim);
            }

            checkDays(mon, tue, wed, thu, fri, sat, sun);

            if (unable)
            {
                printf("\nA jelentkezesben feltuntetett nap(ok) betelt(ek), javaslom megtekinteni a jelentkezes tetejen levo tablazatot\n");
                exit(15);
            }
            else
            {
                people_number[num] = applicant;
                num += 1;
            }
            printPeopleInToFile(num, people_number);
            free(copied_days_two);
            printOutLeftPlaces(left_places);
            break;
        }
        case 2:
        {
            int id_to_modify;
            if (num < 1)
            {
                printf("\nJelen allas szerint nincs jelentkezo, nem tudsz modositani\n");
                exit(5);
            }
            else
            {
                printf("\nAdd meg az alábbi listából azt a sorszámot, ahol az adatokat modositani szeretned!\n");
                listPeople(num, people_number);
                printf("Sorszam: ");
                printf("\n");
                scanf("%i", &id_to_modify);
            }

            if (id_to_modify > num - 1 || id_to_modify < 1)
            {
                printf("\nA modositani kivant sorszam nem szerepel a listan\n");
                exit(6);
            }

            bool succesfull = false;
            int searched = 1;

            while (searched < num - 1 && people_number[searched].id != id_to_modify)
            {
                searched += 1;
            }

            printf("\nMODOSÍTÁSNÁL ELLEŐRZÉS:\n");
            printf("\nid_to_modify: %d", id_to_modify);
            printf("\nsearched: %d ", searched);
            printf("\nnum: %d ", num);
            printf("\n");

            if (searched < num)
            {
                succesfull = true;
            }
            else
            {
                printf("Nem lesz ez jo foni");
                exit(12);
            }
            /*NAME*/
            if (succesfull)
            {
                char *days_to_free_up = people_number[searched].days;
                problemWithDays(days_to_free_up, people_number[searched], left_places);
                char name[100];
                fgets(name, 100, stdin);
                name[strlen(name) - 1] = 0;
                while (strcmp(trim(name), "") == 0)
                {
                    printf("Adja meg a nevet: ");
                    fgets(name, 100, stdin);
                    name[strlen(name) - 1] = 0;
                    strcpy(name, trim(name));
                }
                strcpy(people_number[searched].name, name);
                printf("\nSIKER\n");
                /*ADRESS*/
                printf("Adja meg a cimet: ");
                char address[100];
                fgets(address, 100, stdin);
                address[strlen(address) - 1] = 0;
                while (strcmp(trim(address), "") == 0)
                {
                    printf("Adja meg a cimet: ");
                    fgets(address, 100, stdin);
                    address[strlen(address) - 1] = 0;
                    strcpy(address, trim(address));
                }
                strcpy(people_number[searched].address, address);
                printf("\nSIKER\n");
                /*DAYS*/
                printf("Adja meg a napokat kisbetuvel, ekezetek nelkul: ");
                char days[64];
                fgets(days, 64, stdin);
                days[strlen(days) - 1] = 0;
                while (strcmp(trim(days), "") == 0)
                {
                    printf("Adja meg a napokat, amikor dolgozni szeretne, KISBETUVEL + EKEZETEK NELKUL: ");
                    fgets(days, 64, stdin);
                    strcpy(days, trim(days));
                }
                strcpy(people_number[searched].days, days);

                char *new_days = malloc(sizeof(char) * 70);
                strcpy(new_days, days);

                char *denim = " ";
                char *token;
                token = strtok(new_days, denim);
                bool unable = false;
                int mon = 0;
                int tue = 0;
                int wed = 0;
                int thu = 0;
                int fri = 0;
                int sat = 0;
                int sun = 0;
                while (token != NULL)
                {
                    if (strstr(token, "\n"))
                    {
                        token[strlen(token) - 1] = 0;
                    }
                    if (strcmp(token, "hetfo") == 0)
                    {

                        mon += 1;
                        left_places[0] -= 1;
                        if (left_places[0] == -1)
                        {
                            unable = true;
                        }
                    }
                    else if (strcmp(token, "kedd") == 0)
                    {

                        tue += 1;
                        left_places[1] -= 1;
                        if (left_places[1] == -1)
                        {
                            unable = true;
                        }
                    }
                    else if (strcmp(token, "szerda") == 0)
                    {

                        wed += 1;
                        left_places[2] -= 1;
                        if (left_places[2] == -1)
                        {
                            unable = true;
                        }
                    }
                    else if (strcmp(token, "csutortok") == 0)
                    {
                        thu += 1;
                        left_places[3] -= 1;
                        if (left_places[3] == -1)
                        {
                            unable = true;
                        }
                    }
                    else if (strcmp(token, "pentek") == 0)
                    {

                        fri += 1;
                        left_places[4] -= 1;
                        if (left_places[4] == -1)
                        {
                            unable = true;
                        }
                    }
                    else if (strcmp(token, "szombat") == 0)
                    {
                        sat += 1;
                        left_places[5] -= 1;
                        if (left_places[5] == -1)
                        {
                            unable = true;
                        }
                    }
                    else if (strcmp(token, "vasarnap") == 0)
                    {
                        sun += 1;
                        left_places[6] -= 1;
                        if (left_places[6] == -1)
                        {
                            unable = true;
                        }
                    }
                    else
                    {
                        printf("\nNincs olyan nap a szotaramban, amit megadtal\n");
                        exit(2);
                    }
                    token = strtok(NULL, denim);
                }
                checkDays(mon, tue, wed, thu, fri, sat, sun);

                if (unable)
                {
                    printf("\nA modositasban feltuntetett nap(ok) betelt(ek), javaslom megtekinteni a jelentkezes tetejen levo tablazatot\n");
                    exit(15);
                }
                strcpy(people_number[searched].days, new_days);
                printf("\nSIKER\n");
                free(new_days);
            }

            printPeopleInToFile(num, people_number);
            printOutLeftPlaces(left_places);
            break;
        }
        case 3:
        {
            /*DELETE*/
            int id_to_delete;

            if (num < 1)
            {
                printf("\nNINCS KIT TOROLNI\n");
            }
            else
            {
                printf("\nLista a jelentkezokrol: \n");
                listPeople(num, people_number);

                printf("\nAdd meg a torolni kivant jelentkezes sorszamat\n");
                scanf("%i", &id_to_delete);

                if (id_to_delete == 0)
                {
                    printf("\nNINCS KIT TOROLNI\n");
                }
            }

            int searched = 1;
            bool success = false;

            if (id_to_delete > num - 1 || id_to_delete < 1)
            {
                printf("\nTorles nem lehetseges, nincs megadott sorszamu jelentkezo\n");
                exit(11);
            }

            while (searched < num && people_number[searched].id != id_to_delete)
            {
                searched += 1;
            }

            printf("\nTORLESNEL ELLENORZES:\n");
            printf("\nid_to_modify: %d", id_to_delete);
            printf("\nsearched: %d ", searched);
            printf("\nnum: %d ", num);
            printf("\n");

            if (searched <= num)
            {
                success = true;
            }
            else
            {
                printf("Baj van foni, a listazando nincs a listan");
                exit(66);
            }

            if (success)
            {
                char *days_to_free_up = people_number[searched].days;
                problemWithDays(days_to_free_up, people_number[searched], left_places);
                for (int del = searched; del < num - 1; del++)
                {
                    people_number[del] = people_number[del + 1];
                }
                num -= 1;
            }
            /*VAN EGY SEARCHED-INDEXŰ ELEMEM, AMIT TÖRÖLNI AKAROK? NADE..
            https://www.programmingsimplified.com/c/source-code/c-program-delete-element-from-array*/
            printPeopleInToFile(num, people_number);
            printOutLeftPlaces(left_places);
            break;
        }
        case 4:
        {
            /*NAPOS LISTA*/

            char day[10]; //csütörtök =9
            fgets(day, 10, stdin);
            while (strcmp(trim(day), "") == 0)
            {
                printf("Adja meg a napot, ami szerint listázni szeretne, KISBETUVEL + EKEZETEK NELKUL: ");
                fgets(day, 10, stdin);
                strcpy(day, trim(day));
            }
            //  day[strlen(day) - 1];
            bool success = false;
            for (int i = 1; i < num; i++)
            {
                if (strstr(people_number[i].days, day))
                {
                    success = true;
                    printf("\njelentkezo sorszama: %d", i);
                    printf("\nNeve: %s", people_number[i].name);
                    printf("\nCime: %s", people_number[i].address);
                    printf("\nNapok, melyeket vallalt: %s", people_number[i].days);
                    printf("\n");
                }
            }
            if (!success)
            {
                printf("\nAz adott napon nem volt jelentkezo\n");
            }
            break;
        }
        case 5:
        {

            if (num == 1)
            {
                printf("\nJelen allas szerint nincs kit kilistaznunk, LEGY TE AZ ELSO\n");
                exit(5);
            }
            listPeople(num, people_number);
            break;
        }
        default:
        {
            printOutLeftPlaces(left_places);
            printf("\nKöszönjük, hogy minket választott\n");
            break;
        }
        }
    } while (category != 0);

}