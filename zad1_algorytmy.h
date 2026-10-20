#pragma once

#include <cassert>
#include <cstdio>
#include <inttypes.h>
#include <limits>
#include <vector>
#include <algorithm>
#include <random>
#include <limits.h>

typedef int32_t int_t;
typedef float rozmiar_t;

int_t liczba_przedmiotow = 0;
rozmiar_t rozmiar_pudelek = 1.0f;

struct przedmiot{
    int_t idx = 0;
    rozmiar_t rozmiar = 0;

    bool operator<(const przedmiot& prawy){
        return rozmiar > prawy.rozmiar;
    }
};

typedef std::vector<przedmiot> przedmioty_t;
przedmioty_t przedmioty;

template<bool sprawdza = true>
struct pudelko{
    przedmioty_t przedmioty_brane;
    rozmiar_t ladunek = 0;

    bool czy_przepelnione() const {
        return ladunek > rozmiar_pudelek;
    }

    bool czy_sie_zmiesci(const przedmiot& przedmiot_dobierany){
        return rozmiar_pudelek >= (ladunek + przedmiot_dobierany.rozmiar);
    }

    void dorzuc_przedmiot(const przedmiot& przedmiot_dobierany){
        if(sprawdza) assert(czy_sie_zmiesci(przedmiot_dobierany));
        ladunek += przedmiot_dobierany.rozmiar;
        przedmioty_brane.push_back(przedmiot_dobierany);
    }

    void zabierz_przedmiot(int_t idx_w_pudelku){
        ladunek -= przedmioty_brane[idx_w_pudelku].rozmiar;
        przedmioty_brane[idx_w_pudelku] = przedmioty_brane.back();
        przedmioty_brane.pop_back();
    }

    void zamien_przedmiot(int_t idx_w_pudelku, const przedmiot P){
        ladunek -= przedmioty_brane[idx_w_pudelku].rozmiar;
        if(sprawdza) assert(czy_sie_zmiesci(P));
        przedmioty_brane[idx_w_pudelku] = P;
        ladunek += P.rozmiar;
    }

    friend void przenies_przedmiot(pudelko& Z, int_t idx_z, pudelko& DO){
        DO.dorzuc_przedmiot(Z[idx_z]);
        Z.zabierz_przedmiot(idx_z);
    }

    przedmiot& operator[](int_t idx) { return przedmioty_brane[idx]; }

    friend void zamien_przedmiot(pudelko& P1, int_t idx1, pudelko& P2, int_t idx2){
        przedmiot temp = P1[idx1];
        P1.zamien_przedmiot(idx1, P2[idx2]);
        P2.zamien_przedmiot(idx2, temp);
    }

    void wypisz_brane() const {
        for(auto p : przedmioty_brane){
            printf("%hhd ", p.idx + 1);
        }
    }

    int_t znajdz_najblizszy_rozmiaru(rozmiar_t szukany){
        int_t najblizszy = 0;
        rozmiar_t najblizszy_rozmiar = std::numeric_limits<rozmiar_t>::max();
        for(int_t i = 0; i < (int_t)przedmioty_brane.size(); i++){
            rozmiar_t dystans = szukany - przedmioty_brane[i].rozmiar;
            dystans = abs(dystans);
            if(najblizszy_rozmiar > dystans){
                najblizszy_rozmiar = dystans;
                najblizszy = i;
            }
        }
        return najblizszy;
    }
}; 


typedef std::vector<pudelko<true>> pudelka_t;
typedef std::vector<pudelko<false>> pudelka_przepelnialne_t;

pudelka_t do_nieprzepelnialnych(pudelka_przepelnialne_t& pudelka){
    for(const auto& P : pudelka) assert(!P.czy_przepelnione());
    return *(pudelka_t*)(&pudelka);
}

pudelka_t pierwsze_dopasowanie(
    const przedmioty_t& przedmioty)
{
    pudelka_t pudelka;
    std::vector<bool> czy_wziete(liczba_przedmiotow, false);

    for(bool koniec = false; !koniec;){
        koniec = true;
    
        pudelka.push_back(pudelko<true>());
        pudelko<true>& napelniane_pudelko = pudelka.back();

        for(int_t idx_p = 0; idx_p < liczba_przedmiotow; idx_p++){
            const przedmiot& rozwazany_przedmiot = przedmioty[idx_p];

            if(!czy_wziete[idx_p]){
                if (napelniane_pudelko.czy_sie_zmiesci(rozwazany_przedmiot)) {
                    napelniane_pudelko.dorzuc_przedmiot(rozwazany_przedmiot);
                    czy_wziete[idx_p] = true;
                    koniec = false;
                }
            }
        }
    }

    pudelka.resize(pudelka.size() - 1);
    return pudelka;
}

pudelka_t pierwsze_dopasowanie_malejaco(
    przedmioty_t przedmioty)
{
    std::sort(przedmioty.begin(), przedmioty.end());
    return pierwsze_dopasowanie(przedmioty);
}


int_t przewidywana_ilosc_pudelek(const przedmioty_t& przedmioty);
pudelka_przepelnialne_t losowy_rozklad(const przedmioty_t& przedmioty);
template<int_t za_duzo_w_nadmiarze>
void napraw_rozklad(pudelka_przepelnialne_t& pudelka);
void wyrownaj(pudelko<false>& wyrownywane, pudelko<false>& nadmiar);

template<int_t za_duzo_w_nadmiarze>
pudelka_t cos_wlasnego(przedmioty_t& przedmioty){
    pudelka_przepelnialne_t pudelka_niepoprawne = losowy_rozklad(przedmioty);
    napraw_rozklad<za_duzo_w_nadmiarze>(pudelka_niepoprawne); // po tym powinny być poprawne
    return do_nieprzepelnialnych(pudelka_niepoprawne);
}

//?+1+5+?+1+3+4+5+1+5+1+1+4+3+5+3+3+4+3+2
//IK2026@UAM

template<int_t za_duzo_w_nadmiarze>
void napraw_rozklad(pudelka_przepelnialne_t& pudelka){
    pudelko<false> nadmiar;

    for(uint8_t k = 0; k < 2; k++){
    for(int_t i = 0; i < (int_t)pudelka.size(); i++){
        wyrownaj(pudelka[i], nadmiar);
        if (nadmiar.przedmioty_brane.size() >= za_duzo_w_nadmiarze) {
            pudelka.push_back(pudelko<false>());
            wyrownaj(pudelka.back(), nadmiar);
        }
    }}

    while (nadmiar.przedmioty_brane.size() > 0) {
        pudelka.push_back(pudelko<false>());
        wyrownaj(pudelka.back(), nadmiar);
    }

    // for(int_t i = 0; i < pudelka.size(); i++){
    //     pudelko<false>& dopelniane = pudelka[i];
    // for(int_t j = i; j < pudelka.size(); j++){
    //     if(i == j) continue;
    //     pudelko<false>& oprozniane = pudelka[i];

    // }}
}

void wyrownaj(pudelko<false>& wyrownywane, pudelko<false>& nadmiar){
    // wyrównywane zostawia wyrownywane z poprawnym ładunkiem
    if(wyrownywane.czy_przepelnione()){
        // za dużo
        while (wyrownywane.czy_przepelnione()) {
            rozmiar_t ile_za_duzo = wyrownywane.ladunek - rozmiar_pudelek;
            int_t przenoszone = wyrownywane.znajdz_najblizszy_rozmiaru(ile_za_duzo);
            przenies_przedmiot(wyrownywane, przenoszone, nadmiar);
        }
    } else {
        // za mało może
        for(int_t i = 0; i < (int_t)nadmiar.przedmioty_brane.size(); i++){
            if(wyrownywane.czy_sie_zmiesci(nadmiar[i])){
                przenies_przedmiot(nadmiar, i, wyrownywane);
            }   
        }
    }

    // dopełnij zamianami
    for(int_t i = 0; i < (int_t)wyrownywane.przedmioty_brane.size(); i++){
    for(int_t j = 0; j < (int_t)nadmiar.przedmioty_brane.size(); j++){
        przedmiot& P1 = wyrownywane[i];
        przedmiot& P2 = nadmiar[j];
        if(((wyrownywane.ladunek - P1.rozmiar + P2.rozmiar) <= rozmiar_pudelek) && (P1.rozmiar < P2.rozmiar)){
            zamien_przedmiot(wyrownywane, i, nadmiar, j);
        }
    }}
}

pudelka_przepelnialne_t losowy_rozklad(const przedmioty_t& przedmioty){
    
    int_t liczba_pudelek = przewidywana_ilosc_pudelek(przedmioty);

    std::mt19937 generator_losowosci;
    std::uniform_int_distribution<int_t> losowacz(0, liczba_pudelek-1);

    
    pudelka_przepelnialne_t pudelka(liczba_pudelek);

    for(const auto p : przedmioty){
        pudelka[losowacz(generator_losowosci)].dorzuc_przedmiot(p);
    }

    return pudelka;
}

int_t przewidywana_ilosc_pudelek(const przedmioty_t& przedmioty){
    rozmiar_t suma_rozmiarow = 0;
    for(auto p : przedmioty) suma_rozmiarow += p.rozmiar;
    return suma_rozmiarow / rozmiar_pudelek;
}

void print_rozklad(const pudelka_t& pudelka){
    for(int_t i = 0; i < (int_t)pudelka.size(); i++){
        const pudelko<true>& pudelko = pudelka[i];
        printf("%hhd: ", i);
        pudelko.wypisz_brane();
        printf(" - %f\n", pudelko.ladunek);
    }
}
