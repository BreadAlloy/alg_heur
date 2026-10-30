#include <cassert>
#include <cstdio>
#include <utility>
#include <vector>
#include <iostream>
#include <inttypes.h>
#include <limits>

typedef float fp_t; 
typedef uint8_t dystans_t;
typedef uint8_t int_t;
typedef std::vector<int_t> przynaleznosci_t;

#define dystans_function graf_pelen

typedef int_t punkt_t;
typedef std::vector<punkt_t> punkty_t;

int_t liczba_punktow = 0; // też jako liczba wierzchołków
punkty_t punkty;

std::vector<dystans_t> grafowe_dystanse;

template<int_t a0>
int_t inline suma_arytmetycznego(int n){
    constexpr int r = 1;
    int_t ret = (n * (2 * a0 + r * (n - 1))) / 2;
    return ret;
}

dystans_t graf_pelen(punkt_t punkt1, punkt_t punkt2){
    if(punkt1 == punkt2) return 0;
    if(punkt1 < punkt2) {
        punkt_t temp = punkt1;
        punkt1 = punkt2;
        punkt2 = temp;
    }

    return grafowe_dystanse[punkt1 + liczba_punktow * punkt2 - suma_arytmetycznego<1>(punkt2 + 1)];
}

struct galaz_t{
    int_t idx_punktu;
    dystans_t dystans_od_drzewa = std::numeric_limits<dystans_t>::max();
    int_t punkt_przyczepienia = std::numeric_limits<int_t>::max();;

    // aby chodzic po drzewie
    std::vector<int_t> doczepione;
};

struct drzewo_t{
    std::vector<galaz_t> galezie;
    std::vector<galaz_t*> niedoczepione_galezie;

    int_t idx_korzenia;

    drzewo_t()
    : galezie(liczba_punktow)
    , niedoczepione_galezie(liczba_punktow){

        for(int_t i = 0; i < liczba_punktow; i++){
            galezie[i].idx_punktu = i;
            niedoczepione_galezie[i] = &(galezie[i]);
        }
    }

    void rozgalezij(int_t idx_korzenia_ = 0){
        idx_korzenia = idx_korzenia_;

        doczep_galaz<true>(0);
        while(!niedoczepione_galezie.empty()){
            int_t idx_niedoczepionej = znajdz_najblizsza_do_drzewa();
            doczep_galaz<false>(idx_niedoczepionej);
        }
    }

    template<bool korzen>
    void doczep_galaz(int_t idx_niedoczepionej_galezi){

        const galaz_t& podpinana = *(niedoczepione_galezie[idx_niedoczepionej_galezi]);

        niedoczepione_galezie[idx_niedoczepionej_galezi] = niedoczepione_galezie.back();
        niedoczepione_galezie.pop_back();

        if constexpr (!korzen){
            galezie[podpinana.punkt_przyczepienia].doczepione.emplace_back(podpinana.idx_punktu);
        }

        for(const auto G_ptr : niedoczepione_galezie){
            galaz_t& G = *G_ptr;

            dystans_t nowy_dystans_od_drzewa = dystans_function(punkty[G.idx_punktu], punkty[podpinana.idx_punktu]);
            if(nowy_dystans_od_drzewa < G.dystans_od_drzewa){
                G.dystans_od_drzewa = nowy_dystans_od_drzewa;
                G.punkt_przyczepienia = podpinana.idx_punktu;
            }
        }
    }

    int_t znajdz_najblizsza_do_drzewa() const {
        int_t najblizsza = 0;
        galaz_t* najblizsza_ptr = niedoczepione_galezie.front();

        for(int_t i = 1; i < niedoczepione_galezie.size(); i++){
            galaz_t* testowana_ptr = niedoczepione_galezie[i];

            if(testowana_ptr->dystans_od_drzewa < najblizsza_ptr->dystans_od_drzewa){
                najblizsza_ptr = testowana_ptr;
                najblizsza = i;
            }
        }
        return najblizsza;
    }
};

struct przechadzacz_drzewa_t{
    drzewo_t& drzewo;

    galaz_t* biezaca;
    std::vector<int_t> rozgalezienia;

    przechadzacz_drzewa_t(drzewo_t& drzewo)
    : drzewo(drzewo)
    , biezaca(&drzewo.galezie[drzewo.idx_korzenia])
    {
        rozgalezienia.emplace_back(0);
    }

    bool nastepna(){
        bool koniec = false;
        if(biezaca->doczepione.empty()){
            koniec = cofaj_sie();
        }
        if(koniec) return !koniec;
        idz_glebiej();
        return !koniec;
    }

    void idz_glebiej(){
        biezaca = &drzewo.galezie[biezaca->doczepione[rozgalezienia.back()]];
        rozgalezienia.emplace_back(0);
    }

    bool cofaj_sie(){
        do {            
            if(rozgalezienia.size() == 1){
                rozgalezienia[0] = 0;
                return true;
            }

            rozgalezienia.pop_back();
            rozgalezienia.back()++;
            biezaca = &drzewo.galezie[biezaca->punkt_przyczepienia];

        } while(biezaca->doczepione.size() <= rozgalezienia.back());
        return false;
    }
};



int main(){
    { // wczytywanie
    std::scanf("%hhd", &liczba_punktow);

    punkty.resize(liczba_punktow);

    for(int_t i = 0; i < liczba_punktow; i++){
        // Dla grafów dziwne, ale dla punktów będzie łatwo
        punkty[i] = i;
    }

    grafowe_dystanse.reserve(suma_arytmetycznego<0>(liczba_punktow));

    for(int_t j = 0; j < liczba_punktow; j++){
    for(int_t i = 0; i < liczba_punktow; i++){
        dystans_t dystans;
        std::scanf("%hhd", &dystans);

        if(j < i){
            grafowe_dystanse.emplace_back(dystans);
        } else {
            // nie wczytuj
        }
    }}
    } // wczytywanie

    drzewo_t drzewo;
    drzewo.rozgalezij();

    przechadzacz_drzewa_t przechadzacz(drzewo);

    dystans_t suma_przebyta = 0;
    galaz_t* poprzednia = przechadzacz.biezaca;
    galaz_t* biezaca = przechadzacz.biezaca;
    printf("%hhd ", biezaca->idx_punktu + 1);
    while(przechadzacz.nastepna()){
        biezaca = przechadzacz.biezaca;
        printf("%hhd ", biezaca->idx_punktu + 1);

        suma_przebyta += dystans_function(punkty[biezaca->idx_punktu], punkty[poprzednia->idx_punktu]);
        poprzednia = biezaca;
    }

    biezaca = przechadzacz.biezaca;
    suma_przebyta += dystans_function(punkty[biezaca->idx_punktu], punkty[poprzednia->idx_punktu]);
    printf("\n");
    printf("%hhd\n", suma_przebyta);

    return 0;
}


