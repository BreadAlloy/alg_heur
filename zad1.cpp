#include <iostream>
#include "zad1_algorytmy.h"

int main(){
    { // wczytywanie
    std::scanf("%hhd %hhd", &liczba_przedmiotow, &rozmiar_pudelek);

    przedmioty.resize(liczba_przedmiotow);
    int temp = 0;

    for(int_t i = 0; i < liczba_przedmiotow; i++){
        przedmiot& p = przedmioty[i];
        std::cin >> temp;
        p.rozmiar = int_t(temp);
        p.idx = i;
    }
    } // wczytywanie

    printf("FF\n");
    print_rozklad(pierwsze_dopasowanie(przedmioty));
    printf("\n");

    printf("FFD\n");
    print_rozklad(pierwsze_dopasowanie_malejaco(przedmioty));
    printf("\n");

    printf("Wlasne\n");
    print_rozklad(cos_wlasnego(przedmioty));
    printf("\n");

    return 0;
}



