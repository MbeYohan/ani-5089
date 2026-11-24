#include <cstdio>
#include <iomanip>
#include <iostream>
#include <vector>

const int LARGEUR = 800;
const int HAUTEUR = 240;
const double PIXELS_PAR_DEGRE = 20.0;

double lacetTete(double tMs) { return 5.0 + 0.1 * tMs; }

struct Boite
{
    double azimut;
    double largeur;
    int haut;
    unsigned char r, g, b;
};

std::vector<unsigned char> image(LARGEUR * HAUTEUR * 3);

void rect(int x0, int y0, int x1, int y1, unsigned char r, unsigned char g, unsigned char b)
{
    for (int y = y0; y < y1; y++)
        for (int x = x0; x < x1; x++)
            if (x >= 0 && x < LARGEUR && y >= 0 && y < HAUTEUR)
            {
                unsigned char *p = &image[(y * LARGEUR + x) * 3];
                p[0] = r;
                p[1] = g;
                p[2] = b;
            }
}

int pixelX(double azimut, double lacet)
{
    return LARGEUR / 2 + static_cast<int>((azimut - lacet) * PIXELS_PAR_DEGRE);
}

int main(int argc, char **argv)
{
    const char *fichier = argc > 1 ? argv[1] : "capture.ppm";

    double lacetRendu = lacetTete(0);
    double lacetReel = lacetTete(14);

    double soumis = lacetRendu;
    // soumis = lacetTete(14);

    double correction = lacetReel - soumis;
    double lacetVu = lacetRendu + correction;
    double ecart = lacetVu - lacetReel + 0.0;

    std::cout << std::fixed << std::setprecision(1);
    std::cout << "tete au rendu : " << lacetRendu << " deg\n";
    std::cout << "tete a l'affichage : " << lacetReel << " deg\n";
    std::cout << "pose soumise : " << soumis << " deg\n";
    std::cout << std::scientific << std::setprecision(3);
    std::cout << "correction du compositeur : " << correction << " deg\n";
    std::cout << "ecart restant a l'ecran : " << ecart << " deg\n";

    Boite scene[] = {
        {-8.0, 3.0, 90, 200, 70, 60},
        {-1.0, 4.0, 140, 70, 160, 90},
        {7.0, 3.0, 110, 70, 100, 200},
        {15.0, 5.0, 80, 220, 180, 60},
    };
    int sol = 190;

    rect(0, 0, LARGEUR, sol, 235, 235, 240);
    rect(0, sol, LARGEUR, HAUTEUR, 150, 150, 150);
    for (double a = -20; a <= 30; a += 5)
        rect(pixelX(a, lacetReel), 0, pixelX(a, lacetReel) + 1, sol, 215, 215, 220);

    for (const Boite &b : scene)
    {
        int vx0 = pixelX(b.azimut, lacetReel), vx1 = pixelX(b.azimut + b.largeur, lacetReel);
        int x0 = pixelX(b.azimut, lacetVu), x1 = pixelX(b.azimut + b.largeur, lacetVu);
        rect(x0, sol - b.haut, x1, sol, b.r, b.g, b.b);
        rect(vx0, sol - b.haut, vx1, sol - b.haut + 3, 0, 0, 0);
        rect(vx0, sol - 3, vx1, sol, 0, 0, 0);
        rect(vx0, sol - b.haut, vx0 + 3, sol, 0, 0, 0);
        rect(vx1 - 3, sol - b.haut, vx1, sol, 0, 0, 0);
    }
    rect(LARGEUR / 2 - 1, 0, LARGEUR / 2 + 1, 12, 220, 0, 0);

    FILE *f = std::fopen(fichier, "wb");
    if (!f)
        return 1;
    std::fprintf(f, "P6\n%d %d\n255\n", LARGEUR, HAUTEUR);
    std::fwrite(image.data(), 1, image.size(), f);
    std::fclose(f);
}
