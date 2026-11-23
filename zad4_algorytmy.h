#pragma once

#include <cassert>
#include <cmath>
#include <cstdio>
#include <random>
#include <utility>
#include <vector>
#include <iostream>
#include <inttypes.h>
#include <limits>

std::mt19937_64 los_eng(23112026);

typedef float fp_t; 
typedef fp_t dystans_t;
typedef uint32_t int_t;

template<int_t a0>
int_t inline suma_arytmetycznego(int n){
    constexpr int r = 1;
    int_t ret = (n * (2 * a0 + r * (n - 1))) / 2;
    return ret;
}

struct punkt_2D_t{
    fp_t x;
    fp_t y;

    punkt_2D_t() = default;

    punkt_2D_t(const fp_t x, const fp_t y)
    : x(x), y(y){}

    void operator+=( const punkt_2D_t& prawy){
        x += prawy.x;
        y += prawy.y;
    }

    void operator/=( const fp_t& skalar){
        x /= skalar;
        y /= skalar;
    }

    friend punkt_2D_t operator-(const punkt_2D_t& lewy, const punkt_2D_t& prawy){
        return punkt_2D_t(lewy.x - prawy.x, lewy.y - prawy.y);
    }

    friend punkt_2D_t operator/(const punkt_2D_t& lewy, const fp_t& skalar){
        return punkt_2D_t(lewy.x / skalar, lewy.y / skalar);
    }
};

struct instancja{
    std::string nazwa = "";
    int_t liczba_punktow = 0;
    std::string typ_wag = "";
    std::vector<dystans_t> grafowe_dystanse;
    std::vector<punkt_2D_t> punkty;

    void set_liczba_punktow(int_t nowa_liczba_punktow){
        liczba_punktow = nowa_liczba_punktow;
        punkty.resize(nowa_liczba_punktow);
        grafowe_dystanse.resize(suma_arytmetycznego<0>(nowa_liczba_punktow));
    }
};

struct przestrzen_graf_t : instancja{

    typedef int_t punkt_t;

    punkt_t get_punkt(int_t idx_punktu){
        return idx_punktu;
    }

    dystans_t dystans_function(punkt_t punkt1, punkt_t punkt2){
        if(punkt1 == punkt2) return 0;
        if(punkt1 < punkt2) {
            punkt_t temp = punkt1;
            punkt1 = punkt2;
            punkt2 = temp;
        }

        return grafowe_dystanse[punkt1 + liczba_punktow * punkt2 - suma_arytmetycznego<1>(punkt2 + 1)];
    }

    void policz_dystanse_z_punktow(){
        for(punkt_t p2 = 0; p2 < liczba_punktow; p2++){ // x
        for(punkt_t p1 = p2 + 1; p1 < liczba_punktow; p1++){ // y
            const punkt_2D_t& P1 = punkty[p1];
            const punkt_2D_t& P2 = punkty[p2];
            const punkt_2D_t roznica = P1 - P2;

            grafowe_dystanse[p1 + liczba_punktow * p2 - suma_arytmetycznego<1>(p2 + 1)] = 
            std::sqrt(roznica.x * roznica.x + roznica.y * roznica.y);
        }}

            
    }
};

struct przestrzen_2D_t : instancja{
    int_t liczba_punktow = 0;
    typedef punkt_2D_t punkt_t;

    punkt_t& get_punkt(int_t idx_punktu){
        return punkty[idx_punktu];
    }

    dystans_t dystans_function(const punkt_t punkt1, const punkt_t punkt2){
        punkt_t roznica = punkt1 - punkt2;
        return std::sqrt(roznica.x * roznica.x + roznica.y * roznica.y);
    }
};

// Dwa legalnie różne obiekty
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

struct idxy_kolejnosci : std::vector<int_t>{
    using wektor = std::vector<int_t>;
    using wektor::wektor;

    void printf_przebyte() const {
        for(int_t i = 0; i < size(); i++){
            printf("%d ", operator[](i));
        }
        printf("\n");
    }

    void random(int_t ile_punktow, std::mt19937_64& losowosc){
        resize(ile_punktow);
        for(int_t i = 0; i < ile_punktow; i++){
            operator[](i) = i;
        }

        std::uniform_int_distribution<int_t> losowa_pozycja(0, ile_punktow - 1);
        for(int_t i = 0; i < ile_punktow; i++){
            int_t los = losowa_pozycja(losowosc);

            int_t temp = operator[](i);
            operator[](i) = operator[](los);
            operator[](los) = temp;
        }     
    }

    operator idxy_nastepnych() const {
        idxy_nastepnych ret(size());

        for(int_t i = 0; i < (size() - 1); i++){
            int_t j = i + 1;
            ret[operator[](i)] = operator[](j);
        }
        ret[back()] = front();

        return ret;
    }
};

struct opt_2{
    typedef idxy_kolejnosci used_permutation_t;

    int_t i[2] = {0, 0};

    bool nastepny(idxy_kolejnosci& rezultat){
        // zwraca czy już obejrzał wszystko
        int_t liczba_punktow = int_t(rezultat.size());

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

        int_t liczba_punktow = int_t(permutacja.size());
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
    typedef idxy_nastepnych used_permutation_t;

    int_t i[3] = {0, 0, 0};

    bool nastepny(idxy_nastepnych& rezultat){
        // zwraca czy już obejrzał wszystko
        int_t liczba_punktow = int_t(rezultat.size());

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

struct opt_3_random : private opt_3{
    typedef opt_3::used_permutation_t used_permutation_t;
    using opt_3::pozamieniaj;

    bool nastepny(idxy_nastepnych& rezultat){
        // nigdy wszystkiego nie obejrzy
        int_t liczba_punktow = int_t(rezultat.size());
        std::uniform_int_distribution<int_t> los_idx(0, liczba_punktow - 1);

        // zamien z powrotem
        pozamieniaj<false>(rezultat);

        while(true){
            i[0] = los_idx(los_eng);
            i[1] = los_idx(los_eng);
            i[2] = los_idx(los_eng);

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
};

template<typename permutacja_t>
struct spacerowicz_ogolny_t{
    permutacja_t jest_tu;
    dystans_t koszt_jest;

    permutacja_t chce_isc;
    dystans_t koszt_chcianego;

    spacerowicz_ogolny_t(permutacja_t& start, dystans_t dystans_start)
    : jest_tu(start), koszt_jest(dystans_start), chce_isc(start), koszt_chcianego(dystans_start)
    {}

    bool obejrz(permutacja_t& ogladana, dystans_t koszt_ogladanego){
        // zwraca czy już się zdecydował

        if(koszt_ogladanego < koszt_chcianego){
            chce_isc = ogladana;
            koszt_chcianego = koszt_ogladanego;
        }

        return kiedy_isc();
    }

    // decyzja o zakonczeniu algorytmu
    virtual bool czy_kontynuowac_spacer() = 0;

    void idz(){
        jest_tu = chce_isc;
        koszt_jest = koszt_chcianego;
        gdy_idzie();
    }
    virtual void gdy_idzie() = 0;
    virtual bool kiedy_isc() = 0;
    virtual permutacja_t najlepsze() = 0;
};

template<typename permutacja_t>
struct spacerowicz_first_fit_t : public spacerowicz_ogolny_t<permutacja_t>{
    using spacerowicz_t = spacerowicz_ogolny_t<permutacja_t>;
    using spacerowicz_t::spacerowicz_t;

    bool czy_kontynuowac_spacer() final {
        return spacerowicz_t::koszt_jest > spacerowicz_t::koszt_chcianego;
    }

    bool kiedy_isc() final {
        return spacerowicz_t::koszt_jest > spacerowicz_t::koszt_chcianego;
    }

    void gdy_idzie() final {}

    permutacja_t najlepsze() override {
        return spacerowicz_t::jest_tu;
    }
};

template<typename permutacja_t>
struct spacerowicz_best_fit_t : public spacerowicz_ogolny_t<permutacja_t>{
    using spacerowicz_t = spacerowicz_ogolny_t<permutacja_t>;
    using spacerowicz_t::spacerowicz_t;

    bool czy_kontynuowac_spacer() final {
        return spacerowicz_t::koszt_jest > spacerowicz_t::koszt_chcianego;
    }

    bool kiedy_isc() final {
        return false;
    }

    void gdy_idzie() final {}

    permutacja_t najlepsze() override {
        return spacerowicz_t::jest_tu;
    }
};

template<typename permutacja_t>
struct spacerowicz_wyzarzacz_t : public spacerowicz_ogolny_t<permutacja_t>{
    using spacerowicz_t = spacerowicz_ogolny_t<permutacja_t>;

    permutacja_t najlepsze_odwiedzone;
    dystans_t koszt_najlepszego;

    spacerowicz_wyzarzacz_t(permutacja_t& start, dystans_t dystans_start)
    : spacerowicz_t(start, dystans_start)
    , najlepsze_odwiedzone(start), koszt_najlepszego(dystans_start)
    {}

    fp_t energia = 1.0f;
    static constexpr fp_t stala_chlodzenia = 0.7f;
    static constexpr fp_t energia_docelowa = 0.1f;
    std::uniform_real_distribution<fp_t> los_P = std::uniform_real_distribution<fp_t>(0.0f, 1.0f);

    bool czy_kontynuowac_spacer() final {
        energia *= stala_chlodzenia;
        return energia > energia_docelowa;
    }

    bool kiedy_isc() final {
        if(spacerowicz_t::koszt_jest > spacerowicz_t::koszt_chcianego){
            return true;
        } else {
            // spacerowicz_t::koszt_jest <= spacerowicz_t::koszt_chcianego
            dystans_t diff = (spacerowicz_t::koszt_jest - spacerowicz_t::koszt_chcianego);
            // diff < 0

            if(std::exp(diff/stala_chlodzenia) > los_P(los_eng)){
                return true;
            } else {
                return false;
            }
        }
    }

    void gdy_idzie() final {
        if(spacerowicz_t::koszt_jest < koszt_najlepszego){
            koszt_najlepszego = spacerowicz_t::koszt_jest;
            najlepsze_odwiedzone = spacerowicz_t::jest_tu;
        }
    }

    permutacja_t najlepsze() override {
        return najlepsze_odwiedzone;
    }
};

template<typename przestrzen_t>
struct algorytmy_t : przestrzen_t {
    using przestrzen_t::get_punkt;
    using przestrzen_t::dystans_function;
    using przestrzen_t::liczba_punktow;

    dystans_t dystans_objechania(idxy_kolejnosci& kolejnosc){
        dystans_t suma = 0;
        for(int_t i = 0; i < (liczba_punktow - 1); i++){
            suma += dystans_function(get_punkt(kolejnosc[i]), get_punkt(kolejnosc[i + 1]));
        }
        suma += dystans_function(get_punkt(kolejnosc[liczba_punktow - 1]), get_punkt(kolejnosc[0]));
        return suma;
    }

    dystans_t dystans_objechania(idxy_nastepnych& nastepni){
        dystans_t suma = 0;
        int_t biezacy = 0;
        for(int_t i = 0; i < liczba_punktow; i++){
            int_t nastepny = nastepni[biezacy];
            suma += dystans_function(get_punkt(biezacy), get_punkt(nastepny));
            biezacy = nastepny;
        }
        assert(biezacy == 0);
        return suma;
    }

    template<typename pokazywacz_sasiedztwa_t, typename spacerowicz_t>
    pokazywacz_sasiedztwa_t::used_permutation_t przeszukiwanie_lokalne(pokazywacz_sasiedztwa_t::used_permutation_t start){
        using permutacja_t = pokazywacz_sasiedztwa_t::used_permutation_t;
        spacerowicz_t spacerowicz(start, dystans_objechania(start));

        while(true){
            permutacja_t pokazywana(spacerowicz.jest_tu);

            pokazywacz_sasiedztwa_t pokazywacz;
            pokazywacz.template pozamieniaj<true>(pokazywana);

            while(!pokazywacz.nastepny(pokazywana)){
                if(spacerowicz.obejrz(pokazywana, dystans_objechania(pokazywana))) break;
            }

            if(spacerowicz.czy_kontynuowac_spacer()){
                spacerowicz.idz();
            } else {
                break;
            }
        }
        return spacerowicz.jest_tu;
    }


};

#define LOCAL_SEARCH_OPT_2_FIRST przeszukiwanie_lokalne<opt_2, spacerowicz_first_fit_t<opt_2::used_permutation_t>>
#define LOCAL_SEARCH_OPT_2_BEST przeszukiwanie_lokalne<opt_2, spacerowicz_best_fit_t<opt_2::used_permutation_t>>
#define LOCAL_SEARCH_OPT_3_FIRST przeszukiwanie_lokalne<opt_3, spacerowicz_first_fit_t<opt_3::used_permutation_t>>
#define LOCAL_SEARCH_OPT_3_BEST przeszukiwanie_lokalne<opt_3, spacerowicz_best_fit_t<opt_3::used_permutation_t>>
#define SIMULATED_ANNEALING_RANDOM_OPT_3 przeszukiwanie_lokalne<opt_3_random, spacerowicz_wyzarzacz_t<opt_3_random::used_permutation_t>>



