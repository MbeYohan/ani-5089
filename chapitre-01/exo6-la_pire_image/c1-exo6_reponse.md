## Le programme de mesure

N'ayant pas de programme graphique sous la main, j'en ai écrit un. Il dessine dans le terminal une sphère éclairée, case par case, et met à jour quarante mille particules à chaque image, ce qui lui donne une charge de calcul réaliste plutôt qu'un temps quasi nul qui rendrait la mesure sans intérêt.

Le point important est la méthode. Je chronomètre chaque image séparément avec `std::chrono::steady_clock` et je garde les mille durées, au lieu de diviser le temps total par mille. Une moyenne cache précisément ce que l'exercice cherche : une image sur cent qui prend le double ne déplace presque pas la moyenne, et se voit dans un casque.

Les statistiques sortent sur la sortie d'erreur et le dessin sur la sortie standard, ce qui permet d'envoyer l'un à l'écran et de lire l'autre tranquillement.


```cpp
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <string>
#include <vector>

namespace {
constexpr int kImages = 1000;
constexpr int kColonnes = 120;
constexpr int kLignes = 40;
constexpr double kBudget = 11.0;
const char kNuances[] = " .:-=+*#%@";
}

struct Particule {
    double x;
    double y;
    double vx;
    double vy;
};

struct Scene {
    double angle = 0.0;
    double rayon = 0.75;
    std::vector<Particule> particules;
};

void MettreAJourLaLogique(Scene& scene) {
    scene.angle += 0.01;
    scene.rayon = 0.75 + 0.15 * std::sin(scene.angle);
    for (Particule& p : scene.particules) {
        const double dx = -p.x;
        const double dy = -p.y;
        const double d = std::sqrt(dx * dx + dy * dy) + 1e-3;
        p.vx += dx / (d * d * d) * 0.001;
        p.vy += dy / (d * d * d) * 0.001;
        p.x += p.vx * 0.016;
        p.y += p.vy * 0.016;
    }
}

// fonction qui se fait deux fois quand on dessine deux yeux.
void RendreUnePasse(const Scene& scene, double decalageOeil, std::string& tampon) {
    for (int ligne = 0; ligne < kLignes; ++ligne) {
        for (int colonne = 0; colonne < kColonnes; ++colonne) {
            const double x = (colonne - kColonnes / 2.0) / (kColonnes / 2.0) * 2.0 + decalageOeil;
            const double y = (ligne - kLignes / 2.0) / (kLignes / 2.0);
            double intensite = 0.0;
            const double distance = std::sqrt(x * x + y * y);
            if (distance < scene.rayon) {
                const double z = std::sqrt(scene.rayon * scene.rayon - distance * distance);
                const double lx = std::cos(scene.angle);
                const double lz = std::sin(scene.angle);
                intensite = std::fmax(0.0, (x * lx + z * lz) / scene.rayon);
                for (int passe = 0; passe < 8; ++passe) {
                    intensite = intensite * 0.5 + std::fabs(std::sin(intensite * 3.0)) * 0.5;
                }
            }
            const int niveau = static_cast<int>(intensite * 9.0);
            tampon[ligne * (kColonnes + 1) + colonne] = kNuances[std::min(9, std::max(0, niveau))];
        }
        tampon[ligne * (kColonnes + 1) + kColonnes] = '\n';
    }
    std::cout.write(tampon.data(), static_cast<std::streamsize>(tampon.size()));
    std::cout.flush();
}

void Statistiques(const char* nom, std::vector<double> valeurs) {
    std::sort(valeurs.begin(), valeurs.end());
    double total = 0.0;
    int depassements = 0;
    for (double v : valeurs) {
        total += v;
        if (v > kBudget) {
            ++depassements;
        }
    }
    const size_t n = valeurs.size();
    std::cerr << std::fixed << std::setprecision(3);
    std::cerr << nom << '\n';
    std::cerr << "  moyenne " << total / n << " ms\n";
    std::cerr << "  mediane " << valeurs[n / 2] << " ms\n";
    std::cerr << "  centile 99 " << valeurs[static_cast<size_t>(n * 0.99)] << " ms\n";
    std::cerr << "  pire image " << valeurs.back() << " ms\n";
    std::cerr << "  au-dela de " << kBudget << " ms : " << depassements
              << " sur " << n << '\n';
}

int main(int argc, char** argv) {
    // --double rend deux fois par image, un oeil gauche et un oeil droit.
    const bool doubler = (argc > 1 && std::string(argv[1]) == std::string("--double"));

    Scene scene;
    scene.particules.reserve(40000);
    for (int i = 0; i < 40000; ++i) {
        const double t = i * 0.017;
        scene.particules.push_back(Particule{std::cos(t) * (1.0 + t * 0.01),
                                             std::sin(t) * (1.0 + t * 0.01), 0.0, 0.0});
    }
    std::string tampon(static_cast<size_t>(kLignes) * (kColonnes + 1), ' ');
    std::vector<double> imagesCompletes;
    std::vector<double> renduSeul;
    imagesCompletes.reserve(kImages);
    renduSeul.reserve(kImages);

    for (int image = 0; image < kImages; ++image) {
        const auto debutImage = std::chrono::steady_clock::now();

        MettreAJourLaLogique(scene);

        const auto debutRendu = std::chrono::steady_clock::now();
        std::cout << "\033[H";
        RendreUnePasse(scene, 0.0, tampon);
        if (doubler) {
            std::cout << "\033[H";
            RendreUnePasse(scene, 0.065, tampon);
        }
        const auto finRendu = std::chrono::steady_clock::now();

        imagesCompletes.push_back(
            std::chrono::duration<double, std::milli>(finRendu - debutImage).count());
        renduSeul.push_back(
            std::chrono::duration<double, std::milli>(finRendu - debutRendu).count());
    }

    std::cerr << "\nmode : " << (doubler ? "double rendu" : "rendu simple") << '\n';
    Statistiques("image complete, logique plus rendu", imagesCompletes);
    Statistiques("rendu seul, sans la logique", renduSeul);
    return 0;
}
```

## Les mesures

Relevé sur mille images consécutives :

```
mode : rendu simple
image complete, logique plus rendu
  moyenne 0.994 ms
  mediane 0.873 ms
  centile 99 1.859 ms
  pire image 36.132 ms
  au-dela de 11.000 ms : 4 sur 1000
rendu seul, sans la logique
  moyenne 0.863 ms
  mediane 0.740 ms
  centile 99 1.731 ms
  pire image 35.999 ms
  au-dela de 11.000 ms : 4 sur 1000
```

D'où, la pire image est à 36,132 ms et 4 images sur 1000 au-delà de onze millisecondes.

## Est-ce que ce programme tiendrait dans un casque

Non. 
La moyenne est de 0,994 ms et la médiane de 0,873 ms, soit moins d'un dixième du budget de dix millisecondes dont dispose le code à 90 hertz. Le centile 99 lui-même n'est qu'à 1,859 ms. Si je m'étais arrêté à ces trois nombres, j'aurais conclu que le programme a une marge confortable, et j'aurais eu tort.

La pire image prend 36,132 ms. C'est 41 fois la médiane et plus 3 fois le budget complet d'une image à 90 hertz. Quand elle arrive, le compositeur n'a rien de neuf à afficher pendant environ trois cycles d'écran et se rabat sur l'image précédente, déformée selon le mouvement de la tête.

