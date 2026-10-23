#include <cassert>
#include <random>
#include "imgui.h"
#include "implot.h"
#include "implot3d.h"
#include "zad2_algorytmy.h"

#include "imgui_setup.h"

constexpr int_t ile_algorytmow = 3;

constexpr int_t testowane_ilosci_punktow[] = {100, 200, 500, 1000, 2000};
constexpr int_t ile_ilosci_punktow = sizeof(testowane_ilosci_punktow) / sizeof(int_t);

constexpr int_t testowane_ilosci_klastrow[] = {5, 10, 20, 50, 100};
constexpr int_t ile_ilosci_klastrow = sizeof(testowane_ilosci_klastrow) / sizeof(int_t);

constexpr int_t ile_losowan = 5;

std::mt19937 zrodlo_losowosci;
std::vector<std::vector<punkty_t>> instancje_testowane(ile_ilosci_punktow);

void wylosuj_dane(){
    std::uniform_real_distribution<float> dystrybucja(0.0f, 1.0f);

    for(int_t n = 0; n < ile_ilosci_punktow; n++){
        instancje_testowane[n].resize(ile_losowan);

        for(int_t i = 0; i < ile_losowan; i++){
            punkty_t& instancja = instancje_testowane[n][i];
            instancja.reserve(testowane_ilosci_punktow[n]);

            for(int_t j = 0; j < testowane_ilosci_punktow[n]; j++){
                punkt_t punkt;
                for(int_t d = 0; d < ilosc_wymiarow; d++){
                    static_assert(sizeof(punkt_t) == sizeof(float) * ilosc_wymiarow);
                    ((float*)&punkt)[d] = dystrybucja(zrodlo_losowosci);
                }
                instancja.push_back(punkt);
    }}}
}

void punkty_startowe_dowolne(){
    punkty_startowe.resize(liczba_klastrow);
    for(int_t i = 0; i < liczba_klastrow; i++){
        punkty_startowe[i] = i;
    }
}

void punkty_startowe_PAM(){
    PAM(&punkty_startowe);
}

struct miara_t{
    int_t liczba_punktow;
    int_t liczba_klastrow;
    int_t czas;
    int_t liczba_iteracji;
    std::vector<punkty_t> rozwiazanie;

    void daj_dane(przynaleznosci_t& przynaleznosci){
        assert(rozwiazanie.empty());

        czas = int_t(czasomierz.pomiar_czasu * 1000.0f); 
        this->liczba_iteracji = ::liczba_iteracji;
        this->liczba_klastrow = ::liczba_klastrow;
        this->liczba_punktow = ::liczba_punktow;

        rozwiazanie.resize(liczba_klastrow);
        for(int_t i = 0; i < liczba_klastrow; i++){
            rozwiazanie[i].reserve(liczba_punktow / liczba_klastrow);
        }

        for(int_t i = 0; i < liczba_punktow; i++){
            rozwiazanie[przynaleznosci[i]].emplace_back(punkty[i]);
        }
    }

    void plot_rozwiazanie(){
        for(int_t i = 0; i < liczba_klastrow; i++){
            ImPlot::PlotScatter(std::to_string(i).c_str(), 
            &(rozwiazanie[i][0].x), &(rozwiazanie[i][0].y), rozwiazanie[i].size(), 0, 0, sizeof(punkt_t));
        }
    }
};

// algorytm
// liczba punktow
// liczba klastrow
// instancja
// liczba_punktow, liczba_klastrow, czas, liczba iteracji, rozwiazanie
std::vector<miara_t> miary(ile_algorytmow * ile_ilosci_punktow * ile_ilosci_klastrow * ile_losowan);

// liczba punktow | liczba klastrow | czas
// liczba punktow | liczba klastrow | liczba_iteracji
// dla każdego algorytmu: wybrać instancje i pokazać rozwiazania

constexpr miara_t& at_index(int_t algorytm_idx, int_t punkty_idx, int_t klastry_idx, int_t instancja_idx){
    miara_t& miara = miary[
        algorytm_idx * ile_ilosci_punktow * ile_ilosci_klastrow * ile_losowan + 
        punkty_idx * ile_ilosci_klastrow * ile_losowan + 
        klastry_idx * ile_losowan +
        instancja_idx
    ];
    return miara;
}

int main(){
    wylosuj_dane();
    przynaleznosci_t rozwiazanie;

    for(int_t n = 0; n < ile_ilosci_punktow; n++){
        for(int_t i = 0; i < ile_losowan; i++){
            for(int_t k = 0; k < ile_ilosci_klastrow; k++){
                liczba_punktow = testowane_ilosci_punktow[n];
                liczba_klastrow = testowane_ilosci_klastrow[k];
                punkty = instancje_testowane[n][i];
                printf("liczba punktow: %d, liczba_klastrow: %d, instancja: %d\n", liczba_punktow, liczba_klastrow, i);

                printf("K-MEANS\n");
                punkty_startowe_dowolne();
                rozwiazanie =  k_means();
                at_index(0, n, k, i).daj_dane(rozwiazanie);
                printf("\n");

                printf("PAM\n");
                punkty_startowe_dowolne();
                rozwiazanie = PAM(&punkty_startowe);
                at_index(1, n, k, i).daj_dane(rozwiazanie);
                printf("\n");

                printf("K-MEANS OD PAM\n");
                rozwiazanie = k_means();
                at_index(2, n, k, i).daj_dane(rozwiazanie);
                printf("\n");
    }}}
    GLFWwindow* window; ImGuiIO* io = new ImGuiIO;
    int ret = imgui_i_grafika_setup(window, io, "Porownanie algorytmow");
    if (ret != 0) return ret;

    {
    ImGuiStyle& style = ImGui::GetStyle();
    style.FontScaleDpi = 1.5f;
    }

    {
    ImPlotStyle& style = ImPlot::GetStyle();
    style.MarkerSize = 7.0f;
    }

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Demo windows
        // ImGui::ShowDemoWindow();
        // ImPlot::ShowDemoWindow();
        // ImPlot3D::ShowDemoWindow();

        ImPlot3DSpec spec;
        spec.Stride = sizeof(miara_t);
        spec.MarkerSize = 6.0f;

        if(ImGui::Begin("Szybkosc liczenia")){

            if(ImPlot3D::BeginPlot("Czas wykonania", ImVec2(1300.0f, 1300.0f))){
                ImPlot3D::SetupAxes("liczba przedmiotow", "liczba klastrow", "czas wykonania[ms]");

                ImPlot3D::PlotScatter("K-MEANS", 
                    &at_index(0, 0, 0, 0).liczba_punktow, 
                    &at_index(0, 0, 0, 0).liczba_klastrow,
                    &at_index(0, 0, 0, 0).czas, 
                    ile_ilosci_punktow * ile_ilosci_klastrow * ile_losowan, spec);
                    
                ImPlot3D::PlotScatter("PAM", 
                    &at_index(1, 0, 0, 0).liczba_punktow, 
                    &at_index(1, 0, 0, 0).liczba_klastrow,
                    &at_index(1, 0, 0, 0).czas, 
                    ile_ilosci_punktow * ile_ilosci_klastrow * ile_losowan, spec);  

                ImPlot3D::PlotScatter("K-MEANS PO PAM", 
                    &at_index(2, 0, 0, 0).liczba_punktow, 
                    &at_index(2, 0, 0, 0).liczba_klastrow,
                    &at_index(2, 0, 0, 0).czas, 
                    ile_ilosci_punktow * ile_ilosci_klastrow * ile_losowan, spec);  

                ImPlot3D::EndPlot();
            }

            ImGui::SameLine();
            if(ImPlot3D::BeginPlot("Liczba iteracji wykonana", ImVec2(1300.0f, 1300.0f))){
                ImPlot3D::SetupAxes("liczba przedmiotow", "liczba klastrow", "liczba iteracji");

                ImPlot3D::PlotScatter("K-MEANS ", 
                    &at_index(0, 0, 0, 0).liczba_punktow, 
                    &at_index(0, 0, 0, 0).liczba_klastrow,
                    &at_index(0, 0, 0, 0).liczba_iteracji, 
                    ile_ilosci_punktow * ile_ilosci_klastrow * ile_losowan, spec);    
                    
                ImPlot3D::PlotScatter("PAM ", 
                    &at_index(1, 0, 0, 0).liczba_punktow, 
                    &at_index(1, 0, 0, 0).liczba_klastrow,
                    &at_index(1, 0, 0, 0).liczba_iteracji, 
                    ile_ilosci_punktow * ile_ilosci_klastrow * ile_losowan, spec);
                    
                ImPlot3D::PlotScatter("K-MEANS PO PAM ", 
                    &at_index(2, 0, 0, 0).liczba_punktow, 
                    &at_index(2, 0, 0, 0).liczba_klastrow,
                    &at_index(2, 0, 0, 0).liczba_iteracji, 
                    ile_ilosci_punktow * ile_ilosci_klastrow * ile_losowan, spec);

                ImPlot3D::EndPlot();
            }
        }
        ImGui::End();

        if(ImGui::Begin("Kolorowania")){
            static int idx_ogladane_punkty = 0;
            static int idx_ogladane_klastry = 0;
            static int idx_ogladane_losowanie = 0;

            ImGui::SliderInt("##idx_ogladane_punkty", &idx_ogladane_punkty, 0, ile_ilosci_punktow - 1);
            ImGui::SameLine(); ImGui::Text("Ilosc punktow = %d", testowane_ilosci_punktow[idx_ogladane_punkty]);

            ImGui::SliderInt("##idx_ogladane_klastry", &idx_ogladane_klastry, 0, ile_ilosci_klastrow - 1);
            ImGui::SameLine(); ImGui::Text("Ilosc klastrow = %d", testowane_ilosci_klastrow[idx_ogladane_klastry]);

            ImGui::SliderInt("Instancja", &idx_ogladane_losowanie, 0, ile_losowan - 1);

            if(ImPlot::BeginPlot("Klasteryzowanie K-means", ImVec2(900.0, 900.0f), ImPlotFlags_NoLegend)){
                at_index(0, idx_ogladane_punkty, idx_ogladane_klastry, idx_ogladane_losowanie)
                .plot_rozwiazanie();
                ImPlot::EndPlot();
            }

            ImGui::SameLine();
            if(ImPlot::BeginPlot("Klasteryzowanie PAM", ImVec2(900.0, 900.0f), ImPlotFlags_NoLegend)){
                at_index(1, idx_ogladane_punkty, idx_ogladane_klastry, idx_ogladane_losowanie)
                .plot_rozwiazanie();
                ImPlot::EndPlot();
            }

            ImGui::SameLine();
            if(ImPlot::BeginPlot("Klasteryzowanie K-means od PAM", ImVec2(900.0, 900.0f), ImPlotFlags_NoLegend)){
                at_index(2, idx_ogladane_punkty, idx_ogladane_klastry, idx_ogladane_losowanie)
                .plot_rozwiazanie();
                ImPlot::EndPlot();
            }

        }
        ImGui::End();

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