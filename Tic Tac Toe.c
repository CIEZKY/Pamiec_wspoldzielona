#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

struct KIK {
    char mapa[9];
    int tura;         // 1 - tura X, 2 - tura O
    int oczekiwanie;  // 0 - oczekiwanie na drugiego gracza, 1 - gracz drugi dolaczyl
    int stop;         // 1 - zwyciestwo, 2 - remis
};

void wyswietlanie_mapy(char red_table[9])
{
    printf(" %c | %c | %c \n",red_table[6],red_table[7],red_table[8]);
    printf("---+---+---\n");
    printf(" %c | %c | %c \n",red_table[3],red_table[4],red_table[5]);
    printf("---+---+---\n");
    printf(" %c | %c | %c \n\n",red_table[0],red_table[1],red_table[2]);
}

void ruch(char red_table[9], int pole, char znak)
{
    red_table[pole-1] = znak;
}

int sprawdzenie_zwyciestwa(char red_table[9], char znak)
{
    for (int i=0; i<3; i++)
    {
        if (red_table[i*3] == znak && red_table[i*3+1] == znak && red_table[i*3+2] == znak)
        {
            //wiersze
            return 1;
        }
        if (red_table[i] == znak && red_table[3+i] == znak && red_table[6+i] == znak)
        {
            //kolumny
            return 1;
        }
    }
    //przekatne
    if (red_table[6] == znak && red_table[4] == znak && red_table[2] == znak)
    {
        return 1;
    }
    if (red_table[0] == znak && red_table[4] == znak && red_table[8] == znak)
    {
        return 1;
    }
    return 0;
}

int sprawdzenie_remisu(char red_table[9])
{
    for (inti =0; i<9; i++)
    {
        if (red_table[i] == ' ')
        {
            return 0;
        }
    }
    return 1;
}


int main(int argc, char* argv[]) {
    if (argc != 2)
    {
        printf("Wpisz jendą nazwę dla mapy pamięci\n");
        return 1;
    }

    HANDLE pamiec_wspoldzielona = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE,0, sizeof(struct KIK), argv[1]);
    if (pamiec_wspoldzielona == NULL)
    {
        printf("Błąd przy we współdzielni\n");
        return 1;
    }

    struct KIK* k_i_k = (struct KIK*)MapViewOfFile(pamiec_wspoldzielona,FILE_MAP_ALL_ACCESS,0,0,sizeof(struct KIK));
    if (k_i_k == NULL)
    {
        CloseHandle(pamiec_wspoldzielona);
        printf("Błąd dolaczania\n");
        return 1;
    }

    system("cls");
    int Gracz;
    k_i_k->tura = 1;

    if ((GetLastError() != ERROR_ALREADY_EXISTS)) //sprawdzenie gracza
    {
        Gracz = 1;
        k_i_k->oczekiwanie = 0;
        printf("Jestes graczem X!!\n");
        printf("\x1b[33m""Oczekiwanie na dolaczenie drugiego gracza...\n\n""\x1b[0m");
    }
    else
    {
        printf("Jestes graczem O!!\n\n");
        Sleep(1000);
        Gracz = 2;
        k_i_k->oczekiwanie = 1;
    }

    while (!k_i_k->oczekiwanie)
    {
    }

    //inicjalizacja mapy
    for (int i = 0; i < 9; i++)
    {
        k_i_k->mapa[i] = ' ';
    }
    system("cls");
    k_i_k->stop = 0;
    int glupota = 0;
    int pole;
    char znak;
    while(1)
    {
        if (k_i_k->tura == Gracz)
        {
            system("cls");
            if (Gracz == 1)
            {
                znak = 'X';
            }
            else {
                znak = 'O';
            }

            wyswietlanie_mapy(k_i_k->mapa);

            printf("\x1b[34m""Wykonaj Ruch 1-9 wedlug tej legendy:\n""\x1b[0m");
            printf("7|8|9\n");
            printf("4|5|6\n");
            printf("1|2|3\n");
            scanf("%d", &pole);
            printf("\n");

            if (pole<1 || pole>9)
            {
                printf("\x1b[31m""RUCH POZA SKALA! Sprobuj jeszcze raz!\n""\x1b[0m");
                Sleep(1000);
                continue;
            }
            if (k_i_k->mapa[pole-1] != ' ')
            {
                printf("\x1b[31m""ZAJETE POLE! Sprobuj jeszcze raz!\n""\x1b[0m");
                Sleep(1000);
                continue;
            }

            ruch(k_i_k->mapa, pole, znak);

            if (sprawdzenie_zwyciestwa(k_i_k->mapa, znak))
            {
                system("cls");
                wyswietlanie_mapy(k_i_k->mapa);
                printf("\x1b[32m""WYGRANA!!!\n""\x1b[0m");
                printf("Wygrywa Gracz %c!\n", znak);
                k_i_k->stop = 1;
                break;
            }
            if (sprawdzenie_remisu(k_i_k->mapa))
            {
                system("cls");
                wyswietlanie_mapy(k_i_k->mapa);
                printf("\x1b[33m""Remis!\n""\x1b[0m");
                k_i_k->stop = 2;
                break;
            }

            //zmiana gracza
            if (k_i_k->tura == 1)
            {
                k_i_k->tura = 2;
                glupota = 0;
                system("cls");
            }
            else
            {
                k_i_k->tura = 1;
                glupota = 0;
                system("cls");
            }
        }
        else
        {
            if (glupota == 0)
            {
                wyswietlanie_mapy(k_i_k->mapa);
                printf("\x1b[34m""Drugi gracz wykonuje swoj ruch...\n""\x1b[0m");
                glupota = 1;
            }
            if (k_i_k->stop == 1)
            {
                system("cls");
                if (Gracz == 1)
                {
                    znak = 'O';
                }
                else {
                    znak = 'X';
                }
                wyswietlanie_mapy(k_i_k->mapa);
                printf("\x1b[31m""PRZEGRANA\n""\x1b[0m");
                printf("Wygrywa Gracz %c!\n", znak);
                break;
            }
            if (k_i_k->stop == 2)
            {
                system("cls");
                wyswietlanie_mapy(k_i_k->mapa);
                printf("\x1b[33m""Remis!\n""\x1b[0m");
                break;
            }

        }
    }

    UnmapViewOfFile(k_i_k);
    CloseHandle(pamiec_wspoldzielona);

    return 0;
}