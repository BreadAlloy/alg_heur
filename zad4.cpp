#include "zad4_algorytmy.h"

int main(){
    algorytmy_t<przestrzen_graf_t> algorytmy;

    { // wczytywanie
    std::scanf("%d", &algorytmy.liczba_punktow);

    algorytmy.grafowe_dystanse.reserve(suma_arytmetycznego<0>(algorytmy.liczba_punktow));

    for(int_t j = 0; j < algorytmy.liczba_punktow; j++){
    for(int_t i = 0; i < algorytmy.liczba_punktow; i++){
        int_t dystans_int;
        std::scanf("%d", &dystans_int);

        if(j < i){
            algorytmy.grafowe_dystanse.emplace_back(fp_t(dystans_int));
        } else {
            // nie wczytuj
        }
    }}
    } // wczytywanie

    idxy_kolejnosci start_kolejnosc;
    std::mt19937_64 losowosc(14112026);
    start_kolejnosc.random(algorytmy.liczba_punktow, losowosc);
    #if 0
    {
        idxy_kolejnosci res = algorytmy.LOCAL_SEARCH_OPT_2_FIRST(start_kolejnosc);
        printf("2-opt-first. %f\n", algorytmy.dystans_objechania(res));
        res.printf_przebyte(); printf("\n");
    }
    {    
        idxy_nastepnych res = algorytmy.LOCAL_SEARCH_OPT_3_FIRST(start_kolejnosc);
        printf("3-opt-first. %f\n", algorytmy.dystans_objechania(res));
        res.printf_przebyte(); printf("\n");
    }
    {    
        idxy_kolejnosci res = algorytmy.LOCAL_SEARCH_OPT_2_BEST(start_kolejnosc);
        printf("2-opt-best. %f\n", algorytmy.dystans_objechania(res));
        res.printf_przebyte(); printf("\n");
    }
    {    
        idxy_nastepnych res = algorytmy.LOCAL_SEARCH_OPT_3_BEST(start_kolejnosc);
        printf("3-opt-best. %f\n", algorytmy.dystans_objechania(res));
        res.printf_przebyte(); printf("\n");
    }
    #endif
    {    
        idxy_nastepnych res = algorytmy.SIMULATED_ANNEALING_RANDOM_OPT_3(start_kolejnosc);
        printf("3-opt-best. %f\n", algorytmy.dystans_objechania(res));
        res.printf_przebyte(); printf("\n");
    }

    return 0;
}


