#pragma once
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <string>
#include <string_view>

#include "zad4_algorytmy.h"


struct wczytywacz_TSP{
    static constexpr std::string_view folder = "../../../TSP_instancje/";

    FILE* fd = nullptr;
    
    std::string plik;

    wczytywacz_TSP(std::string nazwa_plik){
        fd = fopen((folder + nazwa_plik).c_str(), "rb");
        assert(fd != nullptr);

        int ret = fseek(fd, 0, SEEK_END);
        assert(ret == 0);

        size_t rozmiar_pliku = ftell(fd);
        plik.resize(rozmiar_pliku + 1);

        ret = fseek(fd, 0, SEEK_SET);
        assert(ret == 0);

        size_t przeczytano = fread(plik.data(), 1, rozmiar_pliku, fd);
        assert(przeczytano == rozmiar_pliku);

        plik.back() = '\0';
        fclose(fd);

        // for(size_t i = 0; i < rozmiar_pliku; i++){
        //     if(plik[i] == '\n') plik[i] = '\0';
        // }
    }

    std::string substr_do_entera(std::string::size_type gdzie){
        std::string::size_type gdzie_koniec = plik.find("\n", gdzie);
        return plik.substr(gdzie, gdzie_koniec - gdzie);
    }

    void wczytaj(algorytmy_t<przestrzen_graf_t>& tu_wczytac){
        {
        std::string::size_type gdzie = find_by_key("NAME");
        assert(gdzie != std::string::size_type(-1));
        tu_wczytac.nazwa = substr_do_entera(gdzie);
        }

        {
        std::string::size_type gdzie = find_by_key("DIMENSION");
        assert(gdzie != std::string::size_type(-1));
        tu_wczytac.set_liczba_punktow(std::stoi(substr_do_entera(gdzie)));
        }

        {
        std::string::size_type gdzie = find_by_key("EDGE_WEIGHT_TYPE");
        assert(gdzie != std::string::size_type(-1));
        tu_wczytac.typ_wag = substr_do_entera(gdzie);
        }

        if(tu_wczytac.typ_wag.starts_with("EUC_2D")) {
            std::string::size_type gdzie = find_by_key("NODE_COORD_SECTION");
            assert(gdzie != std::string::size_type(-1));

            wczytaj_2D_punkty(tu_wczytac, gdzie);
            tu_wczytac.policz_dystanse_z_punktow();
            return;
        }

        if(tu_wczytac.typ_wag.starts_with("EXPLICIT")) {
            {
            std::string::size_type gdzie = find_by_key("DISPLAY_DATA_SECTION");
            if(gdzie != std::string::size_type(-1)){
                wczytaj_2D_punkty(tu_wczytac, gdzie);
            }
            }

            std::string typ_macierzy;

            {
            std::string::size_type gdzie = find_by_key("EDGE_WEIGHT_FORMAT");
            assert(gdzie != std::string::size_type(-1));
            typ_macierzy = substr_do_entera(gdzie);
            }

            std::string::size_type gdzie = find_by_key("EDGE_WEIGHT_SECTION");
            assert(gdzie != std::string::size_type(-1));

            if(typ_macierzy.starts_with("LOWER_DIAG_ROW")) {
                wczytaj_graf_dolny_diag(tu_wczytac, gdzie);
                return;
            }

            if(typ_macierzy.starts_with("FULL_MATRIX")) {
                wczytaj_graf_matrix_full(tu_wczytac, gdzie);
                return;
            }

            assert(false);
        }
        assert(false);
    }

    void wczytaj_2D_punkty(algorytmy_t<przestrzen_graf_t>& tu_wczytac, std::string::size_type gdzie){
        int idx;

        char* linia = plik.data() + gdzie;
        for(int_t i = 0; i < tu_wczytac.liczba_punktow; i++){
            int ret = sscanf(linia, " %d %f %f ", &idx, &tu_wczytac.punkty[i].x, &tu_wczytac.punkty[i].y);
            assert(ret == 3);
            assert((i + 1) == int_t(idx));

            gdzie = plik.find("\n", gdzie) + 1;
            linia = plik.data() + gdzie;
        }
    }

    void wczytaj_graf_matrix_full(algorytmy_t<przestrzen_graf_t>& tu_wczytac, std::string::size_type gdzie){
        for(int_t y = 0; y < tu_wczytac.liczba_punktow; y++){
        for(int_t x = 0; x < tu_wczytac.liczba_punktow; x++){

            while(!((plik[gdzie] >= '0') && (plik[gdzie] <= '9'))){
                gdzie++;
            }

            char* dane = plik.data() + gdzie;
            dystans_t wczytane = std::stof(dane);

            if(x >= y){
            if(x == y){
                assert(wczytane == 0.0f);
            } else {
                tu_wczytac.grafowe_dystanse[x + tu_wczytac.liczba_punktow * y - suma_arytmetycznego<1>(y + 1)] = wczytane;
            }}

            //znajdź nastepna cyfre
            while(((plik[gdzie] >= '0') && (plik[gdzie] <= '9')) || plik[gdzie] == '.'){
                gdzie++;
            }

        }}
    }

    void wczytaj_graf_dolny_diag(algorytmy_t<przestrzen_graf_t>& tu_wczytac, std::string::size_type gdzie){
        for(int_t y = 0; y < tu_wczytac.liczba_punktow; y++){
        for(int_t x = 0; x <= y; x++){

            while(!((plik[gdzie] >= '0') && (plik[gdzie] <= '9'))){
                gdzie++;
            }

            char* dane = plik.data() + gdzie;
            dystans_t wczytane = std::stof(dane);

            if(x == y){
                assert(wczytane == 0.0f);
            } else {
                tu_wczytac.grafowe_dystanse[y + tu_wczytac.liczba_punktow * x - suma_arytmetycznego<1>(x + 1)] = wczytane;
            }

            //znajdź nastepna cyfre
            while(((plik[gdzie] >= '0') && (plik[gdzie] <= '9')) || plik[gdzie] == '.'){
                gdzie++;
            }

        }}
    }

    std::string::size_type find_by_key(std::string_view key){
        std::string::size_type gdzie = plik.find(key);
        if(gdzie == std::string::size_type(-1)) return gdzie;
        gdzie += key.length();
        
        std::string co_pominac[3] = {" : ", ": ", "\n"};

        for(auto pomin : co_pominac){
            if(plik.substr(gdzie, 4).starts_with(pomin)){
                gdzie += pomin.length();
                return gdzie;
            }
        }

        return gdzie;
    }

};



