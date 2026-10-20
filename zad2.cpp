#include <cassert>
#include <cstdio>
#include <utility>
#include <vector>
#include <iostream>
#include <inttypes.h>
#include <limits>

typedef float fp_t; 
typedef fp_t dystans_t;
typedef uint8_t dim_t;
typedef uint8_t int_t;
typedef std::vector<int_t> przynaleznosci_t;

constexpr dim_t ilosc_wymiarow = 2; 

#define dystans_function euklides

struct punkt_t{
    fp_t x;
    fp_t y;

    punkt_t() = default;

    punkt_t(const fp_t x, const fp_t y)
    : x(x), y(y){}

    void operator+=( const punkt_t& prawy){
        x += prawy.x;
        y += prawy.y;
    }

    void operator/=( const fp_t& skalar){
        x /= skalar;
        y /= skalar;
    }

    friend punkt_t operator-(const punkt_t& lewy, const punkt_t& prawy){
        return punkt_t(lewy.x - prawy.x, lewy.y - prawy.y);
    }

    friend punkt_t operator/(const punkt_t& lewy, const fp_t& skalar){
        return punkt_t(lewy.x / skalar, lewy.y / skalar);
    }
};
typedef std::vector<punkt_t> punkty_t;

static_assert(ilosc_wymiarow * sizeof(fp_t) == sizeof(punkt_t));

struct znajdywacz_najmniejszego_t{
    fp_t wartosc_min = std::numeric_limits<float>::max();
    int_t idx_min = 0;

    void nastepne(const fp_t wartosc, const int_t idx){
        if(wartosc < wartosc_min){
            wartosc_min = wartosc;
            idx_min = idx;
        }
    }
};

struct znajdywacz_srodka_t{
    punkt_t punkt_srodka;
    int_t liczba_punktow = 0;

    znajdywacz_srodka_t()
    : punkt_srodka(0.0f, 0.0f)
    {}

    void nastepny(const punkt_t& kolejny){
        punkt_srodka += kolejny;
        liczba_punktow++;
    }

    punkt_t rezultat(){
        return punkt_srodka / ((float)liczba_punktow);
    }
};

dystans_t euklides(const punkt_t& punkt1, const punkt_t& punkt2){
    punkt_t roznica = punkt1 - punkt2;
    return roznica.x * roznica.x + roznica.y * roznica.y;
}

void printf_przynaleznosci(const przynaleznosci_t& przynaleznosci){
    for(int_t i = 0; i < przynaleznosci.size(); i++){
        printf("%hhd", przynaleznosci[i]);

        if(i < (przynaleznosci.size() - 1)){
            printf(" ");
        } else {
            printf("\n");
        }
    }
}

int_t liczba_klastrow = 0;
int_t liczba_punktow = 0;
punkty_t punkty;

przynaleznosci_t k_means(){
    przynaleznosci_t przynaleznosci(punkty.size());
    punkty_t centroidy(liczba_klastrow);

    for(int_t i = 0; i < liczba_klastrow; i++){
        centroidy[i] = punkty[i];
    }

    bool kontynuuj = true;
    while (kontynuuj) {
        kontynuuj = false;

        for(int_t i = 0; i < liczba_punktow; i++){
            znajdywacz_najmniejszego_t znajdywacz_najmniejszego;

            for(int_t k = 0; k < liczba_klastrow; k++){
                znajdywacz_najmniejszego.nastepne(
                    dystans_function(punkty[i], centroidy[k]), k);
            }

            if(przynaleznosci[i] != znajdywacz_najmniejszego.idx_min) kontynuuj = true;
            przynaleznosci[i] = znajdywacz_najmniejszego.idx_min;

        }

        std::vector<znajdywacz_srodka_t> nastepne_centroidy(liczba_klastrow);
        for(int_t i = 0; i < liczba_punktow; i++){
            nastepne_centroidy[przynaleznosci[i]].nastepny(punkty[i]);
        }

        for(int_t k = 0; k < liczba_klastrow; k++){
            centroidy[k] = nastepne_centroidy[k].rezultat();
        }
    }

    return przynaleznosci;
}

przynaleznosci_t PAM(){
    // Pisałem wersje prostą ze złożonością podobno O(n^2k^2t) a wyszło O(n^2kt) i tego nie rozumiem.
    // Rozwiązanie też inne produkuje, ale podobne do kmeans więc raczej jest sensowne
    std::vector<std::vector<int_t>> klastry(liczba_klastrow);
    przynaleznosci_t medoidy(liczba_klastrow);

    for(int_t i = 0; i < liczba_klastrow; i++){
        medoidy[i] = i;
    }

    while (true) {

        // opróżnij klaster przed przypisanie punktów na nowo
        for(int_t k = 0; k < liczba_klastrow; k++){
            klastry[k].clear();
        }

        // wszystkie punkty przypisz do klastra z najbliższym centroidem
        for(int_t i = 0; i < liczba_punktow; i++){
            znajdywacz_najmniejszego_t znajdywacz_najmniejszego;

            for(int_t k = 0; k < liczba_klastrow; k++){
                znajdywacz_najmniejszego.nastepne(
                    dystans_function(punkty[i], punkty[medoidy[k]]), k);
            }

            // do klastra znajdywacz_najmniejszego.idx_min dodaj punkt i
            klastry[znajdywacz_najmniejszego.idx_min].emplace_back(i);
        }

        // znajdz najmniejsza wartość funkcji celu po przepisaniu medoidu
        //                             idx_klastra, idx_punktu
        std::vector<znajdywacz_najmniejszego_t> znajdywacze_najmniejszego_nowego(liczba_klastrow);
        for(int_t k = 0; k < liczba_klastrow; k++){
            for(int_t i = 0; i < liczba_punktow; i++){

                fp_t suma_dystansow = 0.0f;

                for(int_t kk = 0; kk < liczba_klastrow; kk++){
                    // testowany na medoid jest już medoidem, pomiń
                    if(i == medoidy[kk]) goto koniec;
                }

                // dystans nowego medoidu do wszystkich punktów w klastrze(funkcja celu)
                for(int_t j = 0; j < klastry[k].size(); j++){
                    suma_dystansow += dystans_function(punkty[i], punkty[klastry[k][j]]);
                }
                znajdywacze_najmniejszego_nowego[k].nastepne(suma_dystansow, i);
                
                koniec:;
            }

        }

        // policz już osągniętą funkcję celu
        znajdywacz_najmniejszego_t znajdywacz_najwiekszego_ulepszenia;
        for(int_t k = 0; k < liczba_klastrow; k++){
            fp_t biezaca_funkcja_celu = 0.0f;

            for(int_t i = 0; i < klastry[k].size(); i++){
                biezaca_funkcja_celu += dystans_function(punkty[medoidy[k]], punkty[klastry[k][i]]);
            }

            znajdywacz_najwiekszego_ulepszenia.nastepne(
                znajdywacze_najmniejszego_nowego[k].wartosc_min - biezaca_funkcja_celu, k);
        }

        int_t k = znajdywacz_najwiekszego_ulepszenia.idx_min;

        // najlepsze ulepszenie nie ulepsza
        if(znajdywacz_najwiekszego_ulepszenia.wartosc_min >= 0.0f) break;

        // najlepsze jest już użyte
        if(medoidy[k] == znajdywacze_najmniejszego_nowego[k].idx_min) break;
        
        // użyj nowego najlepszego
        medoidy[k] = znajdywacze_najmniejszego_nowego[k].idx_min;
    }

    przynaleznosci_t przynaleznosci(liczba_punktow);

    for(int_t k = 0; k < liczba_klastrow; k++){
    for(int_t i = 0; i < klastry[k].size(); i++){
        przynaleznosci[klastry[k][i]] = k;
    }}

    return przynaleznosci;
}

int main(){
    { // wczytywanie
    std::scanf("%hhd %hhd", &liczba_punktow, &liczba_klastrow);

    punkty.resize(liczba_punktow);

    for(int_t i = 0; i < liczba_punktow; i++){
        punkt_t& p = punkty[i];
        std::cin >> p.x;
        std::cin >> p.y;
    }
    } // wczytywanie

    printf("K-MEANS\n");
    printf_przynaleznosci(k_means());
    printf("\n");

    printf("PAM\n");
    printf_przynaleznosci(PAM());
    printf("\n");

    return 0;
}


