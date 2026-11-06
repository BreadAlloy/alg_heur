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

// Dwa legalnie różne obiekty
struct idxy_kolejnosci : std::vector<int_t>{
    using wektor = std::vector<int_t>;
    using wektor::wektor;

    void printf_przebyte() const {
        for(int_t i = 0; i < size(); i++){
            printf("%d ", operator[](i));
        }
        printf("\n");
    }
};
struct idxy_nastepnych : std::vector<int_t>{
    using wektor = std::vector<int_t>;
    using wektor::wektor;

    void printf_przebyte() const {
        int_t biezacy = 0;
        for(int_t i = 0; i < size(); i++){
            printf("%d ", biezacy);
            biezacy = operator[](biezacy);
        }
        assert(biezacy == 0);
        printf("\n");
    }
};

dystans_t dystans_objechania(idxy_kolejnosci& kolejnosc){
    dystans_t suma = 0;
    for(int_t i = 0; i < (liczba_punktow - 1); i++){
        suma += dystans_function(punkty[kolejnosc[i]], punkty[kolejnosc[i + 1]]);
    }
    suma += dystans_function(punkty[kolejnosc[liczba_punktow - 1]], punkty[kolejnosc[0]]);
    return suma;
}

dystans_t dystans_objechania(idxy_nastepnych& nastepni){
    dystans_t suma = 0;
    int_t biezacy = 0;
    for(int_t i = 0; i < liczba_punktow; i++){
        int_t nastepny = nastepni[biezacy];
        suma += dystans_function(punkty[biezacy], punkty[nastepny]);
        biezacy = nastepny;
    }
    assert(biezacy == 0);
    return suma;
}

struct opt_2{
    idxy_kolejnosci start;

    int_t i[2] = {0, 0};

    opt_2(idxy_kolejnosci& start)
    :start(start) {}

    bool nastepny(idxy_kolejnosci& rezultat){
        // zwraca czy już obejrzał wszystko
        assert(rezultat.size() == start.size());

        // zamien z powrotem
        pozamieniaj(rezultat);

        for(int_t k = 1; k < 2; k--){
            i[k]++;
            if(i[k] >= liczba_punktow){
                i[k] = 0;
                if(k == 0) return true;
            } else {
                break;
            }
        }

        pozamieniaj(rezultat);
        return false;
    }

    // samoodwrotna
    template<bool przod_tyl = true>
    void pozamieniaj(idxy_kolejnosci& permutacja){
        // j != i, j + 1 != i, i + 1 != j

        int_t temp;
        if(i[1] > i[0]){
            int_t ile_obrocic = i[1] - i[0];
            ile_obrocic = (ile_obrocic >> 1) + 1;
            for(int_t w = 0; w < ile_obrocic; w++){
                int_t w0 = i[0] + 1 + w;
                int_t w1 = i[1] - w;

                temp = permutacja[w0];
                permutacja[w0] = permutacja[w1];
                permutacja[w1] = temp;
            }
        } else {
            int_t ile_obrocic = i[0] - i[1];
            ile_obrocic = (ile_obrocic >> 1) + 1;
            for(int_t w = 0; w < ile_obrocic; w++){
                int_t w0 = i[0] + 1 + w;
                int_t w1 = i[1] - w;

                if(w0 >= liczba_punktow) w0 -= liczba_punktow;
                if(w1 >= liczba_punktow) w1 += liczba_punktow;

                temp = permutacja[w0];
                permutacja[w0] = permutacja[w1];
                permutacja[w1] = temp;
            }
        }
    }
};

struct opt_3{
    idxy_nastepnych start;

    int_t i[3] = {0, 0, 0};

    opt_3(idxy_nastepnych& start)
    :start(start) {}

    bool nastepny(idxy_nastepnych& rezultat){
        // zwraca czy już obejrzał wszystko
        assert(rezultat.size() == start.size());

        // zamien z powrotem
        pozamieniaj<false>(rezultat);

        while(true){
            for(int_t k = 2; k < 3; k--){
                i[k]++;
                if(i[k] >= liczba_punktow){
                    i[k] = 0;
                    if(k == 0) return true;
                } else {
                    break;
                }
            }

            int_t counter = 0;
            int_t biezacy = 0;
            for(int_t w = 0; w < liczba_punktow; w++){
                if(i[counter] == biezacy) counter++;
                biezacy = rezultat[biezacy];
            }
            assert(biezacy == 0);

            if(counter == 3) break;
        }
        pozamieniaj<true>(rezultat);
        
        return false;
    }

    template<bool przod_tyl>
    void pozamieniaj(idxy_nastepnych& permutacja){
        // i < j < k
        int_t temp;

        if constexpr(!przod_tyl){
            temp = permutacja[i[2]];
            permutacja[i[2]] = permutacja[i[1]];
            permutacja[i[1]] = permutacja[i[0]];
            permutacja[i[0]] = temp;
        } else {
            temp = permutacja[i[2]];
            permutacja[i[2]] = permutacja[i[0]];
            permutacja[i[0]] = permutacja[i[1]];
            permutacja[i[1]] = temp;
        }
    }
};

template<typename permutacja_t>
struct spacerowicz_t{
    permutacja_t jest_tu;
    dystans_t koszt_jest;

    permutacja_t chce_isc;
    dystans_t koszt_chcianego;

    spacerowicz_t(permutacja_t& start)
    : jest_tu(start), chce_isc(start){
        koszt_jest = dystans_objechania(jest_tu);
        koszt_chcianego = dystans_objechania(chce_isc);
    }

    template<bool(kiedy_konczyc)(spacerowicz_t&)>
    bool obejrz(permutacja_t& ogladana){
        // zwraca czy już się zdecydował
        assert(ogladana.size() == liczba_punktow);
        dystans_t koszt_ogladanego = dystans_objechania(ogladana);

        if(koszt_ogladanego < koszt_chcianego){
            chce_isc = ogladana;
            koszt_chcianego = koszt_ogladanego;
        }

        return kiedy_konczyc(*this);
    }

    static bool idz_na_pierwszym_lepszym(spacerowicz_t& spacerowicz){
        return spacerowicz.koszt_jest > spacerowicz.koszt_chcianego;
    }

    static bool wiecznie_niezdecydowany(spacerowicz_t& spacerowicz){
        return false;
    }

    bool czy_jest_powod_isc(){
        return koszt_jest > koszt_chcianego;
    }

    void idz(){
        jest_tu = chce_isc;
        koszt_jest = koszt_chcianego;
    }
};

template<typename permutacja_t, typename pokazywacz_sasiedztwa_t, 
        bool(kiedy_isc)(spacerowicz_t<permutacja_t>&)>
permutacja_t przeszukiwanie_lokalne(permutacja_t start){
    spacerowicz_t<permutacja_t> spacerowicz(start);

    while(true){
        permutacja_t pokazywana(spacerowicz.jest_tu);

        pokazywacz_sasiedztwa_t pokazywacz(spacerowicz.jest_tu);
        pokazywacz.template pozamieniaj<true>(pokazywana);

        while(!pokazywacz.nastepny(pokazywana)){
            if(spacerowicz.template obejrz<kiedy_isc>(pokazywana)) break;
        }

        if(spacerowicz.czy_jest_powod_isc()){
            spacerowicz.idz();
        } else {
            break;
        }
    }
    return spacerowicz.jest_tu;
}

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

    idxy_kolejnosci start_kolejnosc(liczba_punktow);
    for(int_t i = 0; i < liczba_punktow; i++){
        start_kolejnosc[i] = i;
    }

    idxy_nastepnych start_nastepni(liczba_punktow);
    for(int_t i = 0; i < liczba_punktow; i++){
        start_nastepni[i] = i + 1;
    }
    start_nastepni.back() = 0;

    {
        idxy_kolejnosci res = przeszukiwanie_lokalne<idxy_kolejnosci, opt_2, spacerowicz_t<idxy_kolejnosci>::idz_na_pierwszym_lepszym>(start_kolejnosc);
        printf("2-opt-first. %d\n", dystans_objechania(res));
        res.printf_przebyte(); printf("\n");
    }
    {    
        idxy_nastepnych res = przeszukiwanie_lokalne<idxy_nastepnych, opt_3, spacerowicz_t<idxy_nastepnych>::idz_na_pierwszym_lepszym>(start_nastepni);
        printf("3-opt-first. %d\n", dystans_objechania(res));
        res.printf_przebyte(); printf("\n");
    }
    {    
        idxy_kolejnosci res = przeszukiwanie_lokalne<idxy_kolejnosci, opt_2, spacerowicz_t<idxy_kolejnosci>::wiecznie_niezdecydowany>(start_kolejnosc);
        printf("2-opt-best. %d\n", dystans_objechania(res));
        res.printf_przebyte(); printf("\n");
    }
    {    
        idxy_nastepnych res = przeszukiwanie_lokalne<idxy_nastepnych, opt_3, spacerowicz_t<idxy_nastepnych>::wiecznie_niezdecydowany>(start_nastepni);
        printf("3-opt-best. %d\n", dystans_objechania(res));
        res.printf_przebyte(); printf("\n");
    }

    return 0;
}


