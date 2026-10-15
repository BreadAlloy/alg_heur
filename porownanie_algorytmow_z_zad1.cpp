#include <cassert>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <random>
#include <chrono>

#include "imgui.h"
#include "imgui_setup.h"
#include "implot.h"
#include "implot3d.h"
#include "zad1_algorytmy.h"

std::mt19937 zrodlo_losowosci;

void wylosuj_przedmioty_normal(){
    std::normal_distribution<float> dystrybucja(rozmiar_pudelek / 2.0f, 0.25f);

    przedmioty.resize(0);
    przedmioty.reserve(liczba_przedmiotow);

    for(int_t i = 0; i < liczba_przedmiotow; i++){
        rozmiar_t rozmiar = rozmiar_t(0);

        while((rozmiar <= rozmiar_t(0)) || (rozmiar > rozmiar_pudelek)){
            rozmiar = rozmiar_t(dystrybucja(zrodlo_losowosci));
        } 

        przedmioty.emplace_back(i, rozmiar);
    }
}

void wylosuj_przedmioty_uniform(){
    std::uniform_real_distribution<float> dystrybucja(0, rozmiar_pudelek);

    przedmioty.resize(0);
    przedmioty.reserve(liczba_przedmiotow);

    for(int_t i = 0; i < liczba_przedmiotow; i++){
        rozmiar_t rozmiar = rozmiar_t(0);

        while((rozmiar <= rozmiar_t(0)) || (rozmiar > rozmiar_pudelek)){
            rozmiar = rozmiar_t(dystrybucja(zrodlo_losowosci));
        } 

        przedmioty.emplace_back(i, rozmiar);
    }
}

struct miary_jakosci_t {
    float liczba_uzytych_pudelek = -1.0f;
    float odchylenie_od_ograniczenia = -1.0f;
    float czas = -1.0f;

    miary_jakosci_t(){}

    miary_jakosci_t(float liczba_uzytych_pudelek, float odchylenie_od_ograniczenia, float czas)
    : liczba_uzytych_pudelek(liczba_uzytych_pudelek), odchylenie_od_ograniczenia(odchylenie_od_ograniczenia), czas(czas)
    {
        assert(liczba_uzytych_pudelek >= 0.0f);
        assert(odchylenie_od_ograniczenia >= 0.0f);
        assert(czas >= 0.0f);
    }

    miary_jakosci_t(const pudelka_t& pudelka, float czas)
        : czas(czas) {
        policz_liczba_uzytych_pudelek(pudelka);
        policz_odchylenie_od_ograniczenia();
    }

    void policz_liczba_uzytych_pudelek(const pudelka_t& pudelka){
        liczba_uzytych_pudelek = (float)pudelka.size();
    }

    void policz_odchylenie_od_ograniczenia(){
        odchylenie_od_ograniczenia = (liczba_uzytych_pudelek - (float)przewidywana_ilosc_pudelek(przedmioty))/
                (float)przewidywana_ilosc_pudelek(przedmioty);
    }

    void operator+=(const miary_jakosci_t& prawy){
        liczba_uzytych_pudelek += prawy.liczba_uzytych_pudelek;
        odchylenie_od_ograniczenia += prawy.odchylenie_od_ograniczenia;
        czas += prawy.czas;
    }

    void operator/=(const float skalar){
        liczba_uzytych_pudelek /= skalar;
        odchylenie_od_ograniczenia /= skalar;
        czas /= skalar;
    }

    friend miary_jakosci_t operator+(const miary_jakosci_t& lewy, const miary_jakosci_t& prawy){
        return miary_jakosci_t(
            lewy.liczba_uzytych_pudelek + prawy.liczba_uzytych_pudelek,
            lewy.odchylenie_od_ograniczenia + prawy.odchylenie_od_ograniczenia,
            lewy.czas + prawy.czas);
    }

};

constexpr bool przejdz_do_prezentacji = true;
constexpr int_t ile_testow_liczby_przedmiotow = 11;
constexpr int_t liczby_przedmiotow_testowane[] = 
    {10, 20, 50, 100, 500, 1000, 5000, 10000, 20000, 50000, 100000};

constexpr int_t ile_losowan_instancji = 15;

struct czasomierz_t{

    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    long int diff; //ys

    float pomiar_czasu = 0.0f;

    void start(){
        printf("0.0 - start\n");
        begin = std::chrono::steady_clock::now();
    }

    void update_teraz(){
        end = std::chrono::steady_clock::now();
        diff = (long int)std::chrono::duration_cast<std::chrono::microseconds> (end - begin).count();
        pomiar_czasu = float(diff)/1000000.0f; // konwersja do sekund
    }

    void stop(){
        update_teraz();
        printf("%f - stop\n", pomiar_czasu);
    }
};


#define test(nazwa_algorytmu, dane_testowe, idx_algorytmu){ \
    if(at_idx(rezultaty_na_##dane_testowe, idx_algorytmu, P, i).czas < 0.0f) {\
    printf("Test "#dane_testowe": "#nazwa_algorytmu", instancja: %d, liczba_przedmiotow: %d\n", i, (int)przedmioty.size());\
    czasomierz.start();\
    pudelka_t pudelka = nazwa_algorytmu(przedmioty);\
    czasomierz.stop();\
    miara_jakosci = miary_jakosci_t(pudelka, czasomierz.pomiar_czasu);\
    at_idx(rezultaty_na_##dane_testowe, idx_algorytmu, P, i) = miara_jakosci;\
    fseek(fd_##dane_testowe, get_idx(idx_algorytmu, P, i) * sizeof(miary_jakosci_t), SEEK_SET);\
    fwrite(&at_idx(rezultaty_na_##dane_testowe, idx_algorytmu, P, i), sizeof(miary_jakosci_t), 1, fd_##dane_testowe);\
    printf("\n"); }\
}

std::vector<miary_jakosci_t> rezultaty_na_uniform(6*ile_testow_liczby_przedmiotow*ile_losowan_instancji);
std::vector<miary_jakosci_t> rezultaty_na_normal(6*ile_testow_liczby_przedmiotow*ile_losowan_instancji);

size_t get_idx(size_t idx_algorytmu, size_t idx_liczby_przedmiotow, size_t idx_instancji){
    return idx_algorytmu * ile_testow_liczby_przedmiotow * ile_losowan_instancji +
                (idx_liczby_przedmiotow * ile_losowan_instancji) + 
                 idx_instancji;
}

miary_jakosci_t& at_idx(std::vector<miary_jakosci_t>& dane, 
    size_t idx_algorytmu, size_t idx_liczby_przedmiotow, size_t idx_instancji){
    size_t idx = get_idx(idx_algorytmu, idx_liczby_przedmiotow, idx_instancji);
    return dane[idx];
}

// dana liczba przedmiotow | losowa instancja | algorytm | miara jakosci

// agregowane:
// algorytm | dana liczba przedmiotow | miara jakosci

// całe
// algorytm | dana liczba przedmiotow | losowa instancja | miara jakosci
// 3 | ile_testow_liczby_przedmiotow | ile_losowych_instancji | ...

int main(){
    
    FILE* fd_uniform = fopen("rezultaty_na_uniform", "rb");
    FILE* fd_normal = fopen("rezultaty_na_normal", "rb");

    if(fd_uniform != nullptr) {
        fread(rezultaty_na_uniform.data(), sizeof(miary_jakosci_t), rezultaty_na_uniform.size(), fd_uniform);
        fclose(fd_uniform);
    } else {
        assert(!przejdz_do_prezentacji);
    }
    
    if(fd_normal != nullptr) {
        fread(rezultaty_na_normal.data(), sizeof(miary_jakosci_t), rezultaty_na_normal.size(), fd_normal);
        fclose(fd_normal);
    } else {
        assert(!przejdz_do_prezentacji);
    }

    if(!przejdz_do_prezentacji){
    fd_uniform = fopen("rezultaty_na_uniform", "wb");
    fd_normal = fopen("rezultaty_na_normal", "wb");

    assert(fd_uniform != nullptr);
    assert(fd_normal != nullptr);

    fwrite(rezultaty_na_uniform.data(), sizeof(miary_jakosci_t), rezultaty_na_uniform.size(), fd_uniform);
    fwrite(rezultaty_na_normal.data(), sizeof(miary_jakosci_t), rezultaty_na_normal.size(), fd_normal);


    miary_jakosci_t miara_jakosci;
    czasomierz_t czasomierz;

    for(int_t P = 0; P < ile_testow_liczby_przedmiotow; P++){
        liczba_przedmiotow = liczby_przedmiotow_testowane[P];

        for(int_t i = 0; i < ile_losowan_instancji; i++){

            wylosuj_przedmioty_uniform();

            test(pierwsze_dopasowanie, uniform, 0);

            test(pierwsze_dopasowanie_malejaco, uniform, 1);

            test(cos_wlasnego<8>, uniform, 2);

            test(cos_wlasnego<32>, uniform, 3);

            test(cos_wlasnego<64>, uniform, 4);

            test(cos_wlasnego<256>, uniform, 5);

            
            wylosuj_przedmioty_normal();

            test(pierwsze_dopasowanie, normal, 0);

            test(pierwsze_dopasowanie_malejaco, normal, 1);

            test(cos_wlasnego<8>, normal, 2);

            test(cos_wlasnego<32>, normal, 3);

            test(cos_wlasnego<64>, normal, 4);

            test(cos_wlasnego<256>, normal, 5);

        }
    }

    fclose(fd_uniform);
    fclose(fd_normal);
    }

    // usrednione dla instancji (rozmiar instancji,  usrednione miary-3)
    // (instancja, losowe starty, miary - 3)


    std::vector<std::vector<miary_jakosci_t>> usrednione_uniform(6);
    std::vector<std::vector<miary_jakosci_t>> usrednione_normal(6);

    for(int_t A = 0; A < 6; A++){
        usrednione_uniform[A].reserve(ile_testow_liczby_przedmiotow);
        usrednione_normal[A].reserve(ile_testow_liczby_przedmiotow);

    for(int_t P = 0; P < ile_testow_liczby_przedmiotow; P++){
        
        usrednione_uniform[A].emplace_back(0.0f, 0.0f, 0.0f);
        usrednione_normal[A].emplace_back(0.0f, 0.0f, 0.0f);

        for(int_t i = 0; i < ile_losowan_instancji; i++){
            usrednione_uniform[A][P] += at_idx(rezultaty_na_uniform, A, P, i);
            usrednione_normal[A][P] += at_idx(rezultaty_na_normal, A, P, i);
        }

        usrednione_uniform[A][P] /= (float)ile_losowan_instancji;
        usrednione_normal[A][P] /= (float)ile_losowan_instancji;
    }}

    constexpr size_t rozmiar_przeskoku = sizeof(miary_jakosci_t) / sizeof(float);
    std::vector<float> Ny(ile_testow_liczby_przedmiotow * rozmiar_przeskoku);
    for(int_t i = 0; i < ile_testow_liczby_przedmiotow; i++){
        Ny[i * rozmiar_przeskoku] = (float)liczby_przedmiotow_testowane[i];
    }

    GLFWwindow* window; ImGuiIO* io = new ImGuiIO;
    int ret = imgui_i_grafika_setup(window, io, "Porownanie algorytmow");
    if (ret != 0) return ret;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Demo windows
        #if _DEBUG
        ImGui::ShowDemoWindow();
        ImPlot::ShowDemoWindow();
        ImPlot3D::ShowDemoWindow();
        #endif

        if(ImGui::Begin("Wykresy dla uniform")){

        if(ImPlot::BeginPlot("LICZBA UZYTYCH PUDELEK", ImVec2(500.0f, 500.0f))){
            ImPlot::SetupAxes("liczba przedmiotow", "liczba uzytych pudelek");
            ImPlot::PlotLine("pierwsze dopasowanie", Ny.data(), &usrednione_uniform[0].front().liczba_uzytych_pudelek, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("pierwsze dopasowanie malejaco", Ny.data(), &usrednione_uniform[1].front().liczba_uzytych_pudelek, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<8>", Ny.data(), &usrednione_uniform[2].front().liczba_uzytych_pudelek, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<32>", Ny.data(), &usrednione_uniform[3].front().liczba_uzytych_pudelek, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<64>", Ny.data(), &usrednione_uniform[4].front().liczba_uzytych_pudelek, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<256>", Ny.data(), &usrednione_uniform[5].front().liczba_uzytych_pudelek, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));

            ImPlot::EndPlot();
        }
        ImGui::SameLine();

        if(ImPlot::BeginPlot("ODCHYLENIE OD OGRANICZENIA", ImVec2(500.0f, 500.0f))){
            ImPlot::SetupAxes("liczba przedmiotow", "odchylenie od ograniczenia");
            ImPlot::PlotLine("pierwsze dopasowanie", Ny.data(), &usrednione_uniform[0].front().odchylenie_od_ograniczenia, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("pierwsze dopasowanie malejaco", Ny.data(), &usrednione_uniform[1].front().odchylenie_od_ograniczenia, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<8>", Ny.data(), &usrednione_uniform[2].front().odchylenie_od_ograniczenia, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<32>", Ny.data(), &usrednione_uniform[3].front().odchylenie_od_ograniczenia, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<64>", Ny.data(), &usrednione_uniform[4].front().odchylenie_od_ograniczenia, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<256>", Ny.data(), &usrednione_uniform[5].front().odchylenie_od_ograniczenia, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            
            ImPlot::EndPlot();
        }
        ImGui::SameLine();

        if(ImPlot::BeginPlot("CZAS", ImVec2(500.0f, 500.0f))){
            ImPlot::SetupAxes("liczba przedmiotow", "czas liczenia");
            ImPlot::PlotLine("pierwsze dopasowanie", Ny.data(), &usrednione_uniform[0].front().czas, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("pierwsze dopasowanie malejaco", Ny.data(), &usrednione_uniform[1].front().czas, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<8>", Ny.data(), &usrednione_uniform[2].front().czas, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<32>", Ny.data(), &usrednione_uniform[3].front().czas, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<64>", Ny.data(), &usrednione_uniform[4].front().czas, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<256>", Ny.data(), &usrednione_uniform[5].front().czas, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            
            ImPlot::EndPlot();
        }

        if(ImPlot3D::BeginPlot("Wszystkie testy", ImVec2(1400.0f, 800.0f))){
            ImPlot3D::SetupAxes("liczba uzytych pudelek", "odchylenie od ograniczenia", "czas");
            ImPlot3DSpec spec;
            spec.Stride = sizeof(miary_jakosci_t);
            spec.MarkerSize = 6.0f;

            miary_jakosci_t* pierwsze = &at_idx(rezultaty_na_uniform, 0, 0, 0);
            ImPlot3D::PlotScatter("pierwsze dopasowanie", 
                &(pierwsze->liczba_uzytych_pudelek), &(pierwsze->odchylenie_od_ograniczenia), &(pierwsze->czas)
                , ile_losowan_instancji * ile_testow_liczby_przedmiotow, spec);
            
            pierwsze = &at_idx(rezultaty_na_uniform, 1, 0, 0);
            ImPlot3D::PlotScatter("pierwsze dopasowanie malejaco", 
                &(pierwsze->liczba_uzytych_pudelek), &(pierwsze->odchylenie_od_ograniczenia), &(pierwsze->czas)
                , ile_losowan_instancji * ile_testow_liczby_przedmiotow, spec);

            pierwsze = &at_idx(rezultaty_na_uniform, 2, 0, 0);
            ImPlot3D::PlotScatter("cos_wlasnego<8>", 
                &(pierwsze->liczba_uzytych_pudelek), &(pierwsze->odchylenie_od_ograniczenia), &(pierwsze->czas)
                , ile_losowan_instancji * ile_testow_liczby_przedmiotow, spec);

            pierwsze = &at_idx(rezultaty_na_uniform, 3, 0, 0);
            ImPlot3D::PlotScatter("cos_wlasnego<32>", 
                &(pierwsze->liczba_uzytych_pudelek), &(pierwsze->odchylenie_od_ograniczenia), &(pierwsze->czas)
                , ile_losowan_instancji * ile_testow_liczby_przedmiotow, spec);

            pierwsze = &at_idx(rezultaty_na_uniform, 4, 0, 0);
            ImPlot3D::PlotScatter("cos_wlasnego<64>", 
                &(pierwsze->liczba_uzytych_pudelek), &(pierwsze->odchylenie_od_ograniczenia), &(pierwsze->czas)
                , ile_losowan_instancji * ile_testow_liczby_przedmiotow, spec);

            pierwsze = &at_idx(rezultaty_na_uniform, 5, 0, 0);
            ImPlot3D::PlotScatter("cos_wlasnego<256>", 
                &(pierwsze->liczba_uzytych_pudelek), &(pierwsze->odchylenie_od_ograniczenia), &(pierwsze->czas)
                , ile_losowan_instancji * ile_testow_liczby_przedmiotow, spec);

            ImPlot3D::EndPlot();
        }

        ImGui::End();
        }

        if(ImGui::Begin("Wykresy dla normal")){

        if(ImPlot::BeginPlot("LICZBA UZYTYCH PUDELEK", ImVec2(500.0f, 500.0f))){
            ImPlot::SetupAxes("liczba przedmiotow", "liczba uzytych pudelek");
            ImPlot::PlotLine("pierwsze dopasowanie", Ny.data(), &usrednione_normal[0].front().liczba_uzytych_pudelek, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("pierwsze dopasowanie malejaco", Ny.data(), &usrednione_normal[1].front().liczba_uzytych_pudelek, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<8>", Ny.data(), &usrednione_normal[2].front().liczba_uzytych_pudelek, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<32>", Ny.data(), &usrednione_normal[3].front().liczba_uzytych_pudelek, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<64>", Ny.data(), &usrednione_normal[4].front().liczba_uzytych_pudelek, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<256>", Ny.data(), &usrednione_normal[5].front().liczba_uzytych_pudelek, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            
            ImPlot::EndPlot();
        }
        ImGui::SameLine();

        if(ImPlot::BeginPlot("ODCHYLENIE OD OGRANICZENIA", ImVec2(500.0f, 500.0f))){
            ImPlot::SetupAxes("liczba przedmiotow", "odchylenie od ograniczenia");
            ImPlot::PlotLine("pierwsze dopasowanie", Ny.data(), &usrednione_normal[0].front().odchylenie_od_ograniczenia, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("pierwsze dopasowanie malejaco", Ny.data(), &usrednione_normal[1].front().odchylenie_od_ograniczenia, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<8>", Ny.data(), &usrednione_normal[2].front().odchylenie_od_ograniczenia, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<32>", Ny.data(), &usrednione_normal[3].front().odchylenie_od_ograniczenia, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<64>", Ny.data(), &usrednione_normal[4].front().odchylenie_od_ograniczenia, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<256>", Ny.data(), &usrednione_normal[5].front().odchylenie_od_ograniczenia, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            
            ImPlot::EndPlot();
        }
        ImGui::SameLine();

        if(ImPlot::BeginPlot("CZAS", ImVec2(500.0f, 500.0f))){
            ImPlot::SetupAxes("liczba przedmiotow", "czas liczenia");
            ImPlot::PlotLine("pierwsze dopasowanie", Ny.data(), &usrednione_normal[0].front().czas, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("pierwsze dopasowanie malejaco", Ny.data(), &usrednione_normal[1].front().czas, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<8>", Ny.data(), &usrednione_normal[2].front().czas, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<32>", Ny.data(), &usrednione_normal[3].front().czas, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<64>", Ny.data(), &usrednione_normal[4].front().czas, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            ImPlot::PlotLine("cos wlasnego<256>", Ny.data(), &usrednione_normal[5].front().czas, ile_testow_liczby_przedmiotow, 0, 0, sizeof(miary_jakosci_t));
            
            ImPlot::EndPlot();
        }

        if(ImPlot3D::BeginPlot("Wszystkie testy", ImVec2(1400.0f, 800.0f))){
            ImPlot3D::SetupAxes("liczba uzytych pudelek", "odchylenie od ograniczenia", "czas");
            ImPlot3DSpec spec;
            spec.Stride = sizeof(miary_jakosci_t);
            spec.MarkerSize = 6.0f;

            miary_jakosci_t* pierwsze = &at_idx(rezultaty_na_normal, 0, 0, 0);
            ImPlot3D::PlotScatter("pierwsze dopasowanie", 
                &(pierwsze->liczba_uzytych_pudelek), &(pierwsze->odchylenie_od_ograniczenia), &(pierwsze->czas)
                , ile_losowan_instancji * ile_testow_liczby_przedmiotow, spec);
            
            pierwsze = &at_idx(rezultaty_na_normal, 1, 0, 0);
            ImPlot3D::PlotScatter("pierwsze dopasowanie malejaco", 
                &(pierwsze->liczba_uzytych_pudelek), &(pierwsze->odchylenie_od_ograniczenia), &(pierwsze->czas)
                , ile_losowan_instancji * ile_testow_liczby_przedmiotow, spec);

            pierwsze = &at_idx(rezultaty_na_normal, 2, 0, 0);
            ImPlot3D::PlotScatter("cos_wlasnego<8>", 
                &(pierwsze->liczba_uzytych_pudelek), &(pierwsze->odchylenie_od_ograniczenia), &(pierwsze->czas)
                , ile_losowan_instancji * ile_testow_liczby_przedmiotow, spec);

            pierwsze = &at_idx(rezultaty_na_normal, 3, 0, 0);
            ImPlot3D::PlotScatter("cos_wlasnego<32>", 
                &(pierwsze->liczba_uzytych_pudelek), &(pierwsze->odchylenie_od_ograniczenia), &(pierwsze->czas)
                , ile_losowan_instancji * ile_testow_liczby_przedmiotow, spec);

            pierwsze = &at_idx(rezultaty_na_normal, 4, 0, 0);
            ImPlot3D::PlotScatter("cos_wlasnego<64>", 
                &(pierwsze->liczba_uzytych_pudelek), &(pierwsze->odchylenie_od_ograniczenia), &(pierwsze->czas)
                , ile_losowan_instancji * ile_testow_liczby_przedmiotow, spec);

            pierwsze = &at_idx(rezultaty_na_normal, 5, 0, 0);
            ImPlot3D::PlotScatter("cos_wlasnego<256>", 
                &(pierwsze->liczba_uzytych_pudelek), &(pierwsze->odchylenie_od_ograniczenia), &(pierwsze->czas)
                , ile_losowan_instancji * ile_testow_liczby_przedmiotow, spec);

            ImPlot3D::EndPlot();
        }

        ImGui::End();
        }

        // Render
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot3D::DestroyContext();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    delete io;

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;

}






