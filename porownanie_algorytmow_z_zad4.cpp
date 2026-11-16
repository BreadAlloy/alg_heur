#include <cassert>
#include <cstdio>
#include <random>

#include "czasomierz.h"
#include "zad4_algorytmy.h"
#include "wczytaj_TSP.h"
#include "imgui_setup.h"
#include "imgui_dodatki/implot_dodatki.h"
#include "imgui_dodatki/implot3d_dodatki.h"

constexpr bool skip_liczenie = true;

constexpr std::string algorytmy[4] = {
    "OPT_2_FIRST",
    "OPT_3_FIRST",
    "OPT_2_BEST",
    "OPT_3_BEST"
};

constexpr std::string pliki_tsp[10] = {
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

constexpr dystans_t optymalne[10] = {
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

int main(){
    std::vector<algorytmy_t<przestrzen_graf_t>> instancje(10);
    std::array<std::array<idxy_kolejnosci, ile_startow>, 10> starty;

    // instancja, start, algorytm
    std::array<std::array<std::array<miara_t, 4>, ile_startow>, 10> miary;

    

    for(int_t i = 0; i < 10; i++){
        wczytywacz_TSP wczytywacz(pliki_tsp[i]);
        wczytywacz.wczytaj(instancje[i]);
    }

    {
    std::mt19937_64 losowosc(14112026);
    for(int_t i = 0; i < 10; i++){
    for(int_t j = 0; j < ile_startow; j++){
        starty[i][j].random(instancje[i].liczba_punktow, losowosc);
    }}}

    FILE* fd_checkpointy = fopen("checkpointy", "rb");
    if(fd_checkpointy != nullptr) {
    fread(miary.data(), 1, sizeof(miary), fd_checkpointy);
    fclose(fd_checkpointy);
    }

    if(!skip_liczenie) {
    fd_checkpointy = fopen("checkpointy", "ab");
    assert(fd_checkpointy != nullptr);

    czasomierz_t czasomierz;

    for(int_t i = 0; i < 10; i++){
        algorytmy_t<przestrzen_graf_t>& instancja = instancje[i];
        dystans_t optimum = optymalne[i];
    for(int_t s = 0; s < ile_startow; s++){
        idxy_kolejnosci& start = starty[i][s];

        {
        miara_t& miara = miary[i][s][0];
        if(miara.czas < 0.0f){

            printf("Performing: instancja: %s, start: %d, algorytm: %s\n", instancja.nazwa.c_str(), s, algorytmy[0].c_str());
            czasomierz.start();
            auto trasa = instancja.LOCAL_SEARCH_OPT_2_FIRST(start);
            czasomierz.stop();

            dystans_t rezultat = instancja.dystans_objechania(trasa);
            miara.przypisz(i, s, 0, czasomierz.pomiar_czasu, rezultat, optimum);
            size_t ret = fwrite(&miara, 1, sizeof(miara_t), fd_checkpointy);
            assert(ret == sizeof(miara_t));
            fflush(fd_checkpointy);
        } else {
            assert(miara.instancja_idx == i);
            assert(miara.start_idx == s);
            assert(miara.alg_idx == 0);
            printf("Skipping: instancja: %s, start: %d, algorytm: %s\n", instancja.nazwa.c_str(), s, algorytmy[0].c_str());
        }
            printf("\n");
        }

        {
        miara_t& miara = miary[i][s][1];
        if(miara.czas < 0.0f){

            printf("Performing: instancja: %s, start: %d, algorytm: %s\n", instancja.nazwa.c_str(), s, algorytmy[1].c_str());
            czasomierz.start();
            auto trasa = instancja.LOCAL_SEARCH_OPT_3_FIRST(start);
            czasomierz.stop();

            dystans_t rezultat = instancja.dystans_objechania(trasa);
            miara.przypisz(i, s, 1, czasomierz.pomiar_czasu, rezultat, optimum);
            size_t ret = fwrite(&miara, 1, sizeof(miara_t), fd_checkpointy);
            assert(ret == sizeof(miara_t));
            fflush(fd_checkpointy);
        } else {
            assert(miara.instancja_idx == i);
            assert(miara.start_idx == s);
            assert(miara.alg_idx == 1);
            printf("Skipping: instancja: %s, start: %d, algorytm: %s\n", instancja.nazwa.c_str(), s, algorytmy[1].c_str());
        }
            printf("\n");
        }

        {
        miara_t& miara = miary[i][s][2];
        if(miara.czas < 0.0f){

            printf("Performing: instancja: %s, start: %d, algorytm: %s\n", instancja.nazwa.c_str(), s, algorytmy[2].c_str());
            czasomierz.start();
            auto trasa = instancja.LOCAL_SEARCH_OPT_2_BEST(start);
            czasomierz.stop();

            dystans_t rezultat = instancja.dystans_objechania(trasa);
            miara.przypisz(i, s, 2, czasomierz.pomiar_czasu, rezultat, optimum);
            size_t ret = fwrite(&miara, 1, sizeof(miara_t), fd_checkpointy);
            assert(ret == sizeof(miara_t));
            fflush(fd_checkpointy);
        } else {
            assert(miara.instancja_idx == i);
            assert(miara.start_idx == s);
            assert(miara.alg_idx == 2);
            printf("Skipping: instancja: %s, start: %d, algorytm: %s\n", instancja.nazwa.c_str(), s, algorytmy[2].c_str());
        }
            printf("\n");
        }

        {
        miara_t& miara = miary[i][s][3];
        if(miara.czas < 0.0f){

            printf("Performing: instancja: %s, start: %d, algorytm: %s\n", instancja.nazwa.c_str(), s, algorytmy[3].c_str());
            czasomierz.start();
            auto trasa = instancja.LOCAL_SEARCH_OPT_3_BEST(start);
            czasomierz.stop();

            dystans_t rezultat = instancja.dystans_objechania(trasa);
            miara.przypisz(i, s, 3, czasomierz.pomiar_czasu, rezultat, optimum);
            size_t ret = fwrite(&miara, 1, sizeof(miara_t), fd_checkpointy);
            assert(ret == sizeof(miara_t));
            fflush(fd_checkpointy);
        } else {
            assert(miara.instancja_idx == i);
            assert(miara.start_idx == s);
            assert(miara.alg_idx == 3);
            printf("Skipping: instancja: %s, start: %d, algorytm: %s\n", instancja.nazwa.c_str(), s, algorytmy[3].c_str());
        }
            printf("\n");
        }
    }}

    fclose(fd_checkpointy);
    }

    std::array<std::array<miara_t, 4>, 10> miary_usrednione;
    std::array<fp_t, 10> rozmiary_instancji;

    for(int_t i = 0; i < 10; i++){
    for(int_t a = 0; a < 4; a++){
        miara_t& usedniana = miary_usrednione[i][a];
        usedniana.przypisz(i, -1, a, 0.0f, 0.0f, 0.0f);
        usedniana.odchylenie_od_optimum = 0.0f;
        usedniana.optimum = optymalne[i];
        int_t ile_policzono = 0;
    for(int_t s = 0; s < ile_startow; s++){
        if(miary[i][s][a].czas >= 0.0f){
        if(miary[i][s][a].rezultat > 0.0f){
            usedniana += miary[i][s][a];
            ile_policzono ++;
        }
    }}
        usedniana /= ile_policzono;
    }}

    for(int_t i = 0; i < 10; i++){
        rozmiary_instancji[i] = instancje[i].liczba_punktow;
    }

    printf("ALGORYTM | INSTANCJA | CZAS\n");
    for(int_t i = 0; i < 10; i++){
    for(int_t a = 0; a < 4; a++){
        printf("%s | %s | %fs\n", algorytmy[a].c_str(), instancje[i].nazwa.c_str(), miary_usrednione[i][a].czas);
    }}
    printf("\n");

    printf("ALGORYTM | INSTANCJA | ODCHYLENIE OD OPTIMUM\n");
    for(int_t i = 0; i < 10; i++){
    for(int_t a = 0; a < 4; a++){
        printf("%s | %s | %f\n", algorytmy[a].c_str(), instancje[i].nazwa.c_str(), miary_usrednione[i][a].odchylenie_od_optimum);
    }}
    printf("\n");

    printf("INSTANCJA | ALGORYTM | CZAS\n");
    for(int_t i = 0; i < 10; i++){
    for(int_t a = 0; a < 4; a++){
        printf("%s | %s | %fs\n", instancje[i].nazwa.c_str(), algorytmy[a].c_str(), miary_usrednione[i][a].czas);
    }}
    printf("\n");

        printf("INSTANCJA | ALGORYTM | ODCHYLENIE OD OPTIMUM\n");
    for(int_t i = 0; i < 10; i++){
    for(int_t a = 0; a < 4; a++){
        printf("%s | %s | %f\n", instancje[i].nazwa.c_str(), algorytmy[a].c_str(), miary_usrednione[i][a].odchylenie_od_optimum);
    }}
    printf("\n");

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

        if(ImGui::Begin("Rezultaty usrednione")){

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




