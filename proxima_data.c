#include <stdio.h>

int dias_mes[] = {
    31, 28, 31, 30, 31, 30,
    31, 31, 30, 31, 30, 31};

int bissexto(int ano)
{
    return (ano % 4 == 0) && (ano % 100 != 0 || ano % 400 == 0);
}

void proximo_dia(int *dia, int *mes, int *ano)
{
    int limite = dias_mes[*mes - 1];

    if (*mes == 2 && bissexto(*ano))
        limite = 29;
    (*dia)++;

    if (*dia > limite)
    {
        *dia = 1;
        (*mes)++;

        if (*mes > 12)
        {
            *mes = 1;
            (*ano)++;
        }
    }
}

int main(void)
{
    int dia = 28;
    int mes = 2;
    int ano = 2024;

    proximo_dia(&dia, &mes, &ano);

    printf("%d/%d/%d\n", dia, mes, ano);

    return 0;
}