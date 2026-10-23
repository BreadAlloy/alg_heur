#pragma once
#include <chrono>

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