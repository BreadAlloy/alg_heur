#include <cassert>
#include <cstdio>
#include <random>

#include "czasomierz.h"
#include "zad4_algorytmy.h"
#include "wczytaj_TSP.h"

constexpr bool skip_liczenie = true;

constexpr int_t ile_algorytmow = 5;
constexpr std::string_view algorytmy[ile_algorytmow] = {
    "OPT_2_FIRST              ",
    "OPT_3_FIRST              ",
    "OPT_2_BEST               ",
    "OPT_3_BEST               ",
    "OPT_3_SIMULATED_ANNEALING"
};

constexpr int_t ile_instancji = 10;
constexpr std::string pliki_tsp[ile_instancji] = {
    "dantzig42.tsp",
    "swiss42.tsp",
    "gr48.tsp",
    "hk48.tsp",
    "berlin52.tsp",
    "bier127.tsp",
    "a280.tsp",
    "fl417.tsp",
    "pa561.tsp",
    "u724.tsp"
};

constexpr dystans_t optymalne[ile_instancji] = {
    699.0f,
    1273.0f,
    5046.0f,
    11461.0f,
    7542.0f,
    118282.0f,
    2579.0f,
    11861.0f,
    2763.0f,
    41910.0f
};

struct miara_t{
    int_t instancja_idx = -1;
    int_t start_idx = -1;
    int_t alg_idx = -1;

    fp_t czas = -1.0f;
    fp_t odchylenie_od_optimum = -1.0f;

    dystans_t rezultat = -1.0f;
    dystans_t optimum = -1.0f;

    void przypisz(int_t instancja_idx_, int_t start_idx_, int_t alg_idx_, fp_t czas_, dystans_t rezultat_, dystans_t optimum_){
        czas = czas_;
        rezultat = rezultat_;
        optimum = optimum_;

        assert(optimum <= rezultat);

        odchylenie_od_optimum = rezultat_ / optimum_ - 1.0f;

        instancja_idx = instancja_idx_;
        start_idx = start_idx_;
        alg_idx = alg_idx_;
    }

    void operator+=(const miara_t& druga){
        czas += druga.czas;
        odchylenie_od_optimum += druga.odchylenie_od_optimum;
        rezultat += druga.rezultat;

        if(druga.instancja_idx != int_t(-1)){
            assert(instancja_idx == druga.instancja_idx);
        }

        if(druga.alg_idx != int_t(-1)){
            assert(alg_idx == druga.alg_idx);
        }
    }

    void operator/=(fp_t skalar){
        czas /= skalar;
        odchylenie_od_optimum /= skalar;
        rezultat /= skalar;
    }
};

constexpr int_t ile_startow = 10;
constexpr bool nie_pomijaj = false;
constexpr int_t ktory_algorytm_liczony = 3;
static_assert(ktory_algorytm_liczony < ile_algorytmow);

int main(){
    std::vector<algorytmy_t<przestrzen_graf_t>> instancje(ile_instancji);
    std::array<std::array<idxy_kolejnosci, ile_startow>, ile_instancji> starty;

    // algorytm, instancja, start
    std::array<std::array<std::array<miara_t, ile_startow>, ile_instancji>, ile_algorytmow> miary;

    for(int_t i = 0; i < ile_instancji; i++){
        wczytywacz_TSP wczytywacz(pliki_tsp[i]);
        wczytywacz.wczytaj(instancje[i]);
    }

    {
    std::mt19937_64 losowosc(14112026);
    for(int_t i = 0; i < ile_instancji; i++){
    for(int_t j = 0; j < ile_startow; j++){
        starty[i][j].random(instancje[i].liczba_punktow, losowosc);
    }}}

    std::array<FILE*, ile_algorytmow> checkpointy;
    for(int_t a = 0; a < ile_algorytmow; a++){
        checkpointy[a] = fopen((std::string("checkpointy_")+std::string(algorytmy[a])).c_str(), "rb");
        if(checkpointy[a] != nullptr) {
        fread(miary[a].data(), 1, sizeof(miary[0]), checkpointy[a]);
        fclose(checkpointy[a]);
        }
    }

    if(!skip_liczenie) {

    for(int_t a = 0; a < ile_algorytmow; a++){
        checkpointy[a] = fopen((std::string("checkpointy_")+std::string(algorytmy[a])).c_str(), "ab");
        assert(checkpointy[a] != nullptr);
    }

    czasomierz_t czasomierz;
    FILE* fd_checkpoint = checkpointy[ktory_algorytm_liczony];

    for(int_t i = 0; i < ile_instancji; i++){
        algorytmy_t<przestrzen_graf_t>& instancja = instancje[i];
        dystans_t optimum = optymalne[i];

    for(int_t s = 0; s < ile_startow; s++){
        idxy_kolejnosci& start = starty[i][s];

        {
        miara_t& miara = miary[ktory_algorytm_liczony][i][s];
        if((miara.czas < 0.0f) || nie_pomijaj){

            printf("Performing: instancja: %s, start: %d, algorytm: %s\n", instancja.nazwa.data(), s, algorytmy[ktory_algorytm_liczony].data());
            dystans_t rezultat;

            czasomierz.start();
            if constexpr (0 == ktory_algorytm_liczony) {
                auto trasa = instancja.LOCAL_SEARCH_OPT_2_FIRST(start);
                rezultat = instancja.dystans_objechania(trasa);
            }
            if constexpr (1 == ktory_algorytm_liczony) {
                auto trasa = instancja.LOCAL_SEARCH_OPT_3_FIRST(start);
                rezultat = instancja.dystans_objechania(trasa);
            }
            if constexpr (2 == ktory_algorytm_liczony) {
                auto trasa = instancja.LOCAL_SEARCH_OPT_2_BEST(start);
                rezultat = instancja.dystans_objechania(trasa);
            }
            if constexpr (3 == ktory_algorytm_liczony) {
                auto trasa = instancja.LOCAL_SEARCH_OPT_3_BEST(start);
                rezultat = instancja.dystans_objechania(trasa);
            }
            if constexpr (4 == ktory_algorytm_liczony) {
                auto trasa = instancja.SIMULATED_ANNEALING_RANDOM_OPT_3(start);
                rezultat = instancja.dystans_objechania(trasa);
            }
            czasomierz.stop();

            miara.przypisz(i, s, ktory_algorytm_liczony, czasomierz.pomiar_czasu, rezultat, optimum);
            size_t ret = fwrite(&miara, 1, sizeof(miara_t), fd_checkpoint);
            assert(ret == sizeof(miara_t));
            fflush(fd_checkpoint);
        } else {
            assert(miara.instancja_idx == i);
            assert(miara.start_idx == s);
            assert(miara.alg_idx == ktory_algorytm_liczony);
            printf("Skipping: instancja: %s, start: %d, algorytm: %s\n", instancja.nazwa.data(), s, algorytmy[ktory_algorytm_liczony].data());
        }
            printf("\n");
        }
    }}

    for(int_t a = 0; a < ile_algorytmow; a++){
        fclose(checkpointy[a]);
    }
    }

    std::array<std::array<miara_t, ile_algorytmow>, ile_instancji> miary_usrednione;

    for(int_t i = 0; i < ile_instancji; i++){
    for(int_t a = 0; a < ile_algorytmow; a++){
        miara_t& usedniana = miary_usrednione[i][a];
        usedniana.przypisz(i, -1, a, 0.0f, 0.0f, 0.0f);
        usedniana.odchylenie_od_optimum = 0.0f;
        usedniana.optimum = optymalne[i];
        int_t ile_policzono = 0;
    for(int_t s = 0; s < ile_startow; s++){
        if(miary[a][i][s].czas >= 0.0f){
        if(miary[a][i][s].rezultat > 0.0f){
            usedniana += miary[a][i][s];
            ile_policzono ++;
        }
    }}
        usedniana /= ile_policzono;
    }}

    printf("ALGORYTM | INSTANCJA | CZAS\n");
    for(int_t i = 0; i < ile_instancji; i++){
    for(int_t a = 0; a < ile_algorytmow; a++){
        printf("%s | %s | %fs\n", algorytmy[a].data(), instancje[i].nazwa.data(), miary_usrednione[i][a].czas);
    }}
    printf("\n");

    printf("ALGORYTM | INSTANCJA | ODCHYLENIE OD OPTIMUM\n");
    for(int_t i = 0; i < ile_instancji; i++){
    for(int_t a = 0; a < ile_algorytmow; a++){
        printf("%s | %s | %f\n", algorytmy[a].data(), instancje[i].nazwa.data(), miary_usrednione[i][a].odchylenie_od_optimum);
    }}
    printf("\n");

    printf("INSTANCJA | ALGORYTM | CZAS\n");
    for(int_t i = 0; i < ile_instancji; i++){
    for(int_t a = 0; a < ile_algorytmow; a++){
        printf("%s | %s | %fs\n", instancje[i].nazwa.data(), algorytmy[a].data(), miary_usrednione[i][a].czas);
    }}
    printf("\n");

        printf("INSTANCJA | ALGORYTM | ODCHYLENIE OD OPTIMUM\n");
    for(int_t i = 0; i < ile_instancji; i++){
    for(int_t a = 0; a < ile_algorytmow; a++){
        printf("%s | %s | %f\n", instancje[i].nazwa.data(), algorytmy[a].data(), miary_usrednione[i][a].odchylenie_od_optimum);
    }}
    printf("\n");
    return 0;
}




