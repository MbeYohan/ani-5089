#include <cstdio>
#include <vector>

const int TRAMES = 20;
const int CELLULE = 40;
const int HAUTEUR = 120;

struct Session
{
    bool ouverte = false;

    void pomperEvenements() {}
    bool attendreTrame() { return !ouverte; }
    void commencerTrame() { ouverte = true; }
    void synchroniserActions() {}
    bool devraitRendre(int trame) const { return trame < 6 || trame > 10; }
    void finirTrame() { ouverte = false; }
};

int ecran[TRAMES + 1];
int debuts[TRAMES + 1];
int fins[TRAMES + 1];

void enregistrer(int trame, int rend, int debut, int fin, int dernier)
{
    std::printf("trame : %d, rend : %d, debut : %d, fin : %d\n", trame, rend, debut, fin);
    ecran[trame] = dernier;
    debuts[trame] = debut;
    fins[trame] = fin;
}

void ecrireImage(const char *fichier);

int main(int argc, char **argv)
{
    Session session;
    int dernier = 0;

    for (int trame = 1; trame <= TRAMES; trame++)
    {
        int rend = 0, debut = 0, fin = 0;

        session.pomperEvenements();
        if (!session.attendreTrame())
        {
            enregistrer(trame, rend, debut, fin, dernier);
            continue;
        }
        session.commencerTrame();
        debut = 1;
        session.synchroniserActions();
        if (session.devraitRendre(trame))
        {
            rend = 1;
            dernier = trame;
            // session.finirTrame(); fin = 1;
        }
        session.finirTrame();
        fin = 1;

        enregistrer(trame, rend, debut, fin, dernier);
    }
    ecrireImage(argc > 1 ? argv[1] : "ecran.ppm");
}

void ecrireImage(const char *fichier)
{
    const int largeur = TRAMES * CELLULE;
    std::vector<unsigned char> img(largeur * HAUTEUR * 3, 255);
    auto rect = [&](int x0, int y0, int x1, int y1, int r, int g, int b)
    {
        for (int y = y0; y < y1; y++)
            for (int x = x0; x < x1; x++)
            {
                unsigned char *p = &img[(y * largeur + x) * 3];
                p[0] = r;
                p[1] = g;
                p[2] = b;
            }
    };
    for (int t = 1; t <= TRAMES; t++)
    {
        int x = (t - 1) * CELLULE;
        bool bloquee = debuts[t] == 0;
        rect(x + 1, 2, x + CELLULE - 1, 72, bloquee ? 200 : 60, bloquee ? 40 : 60, bloquee ? 40 : 70);
        rect(x + 3, 4, x + CELLULE - 3, 70, 30, 30, 40);
        int d = ecran[t];
        if (d > 0)
            rect(x + 4 + (d % 5) * 6, 12 + (d / 5) * 12, x + 12 + (d % 5) * 6, 20 + (d / 5) * 12, 240, 200, 60);
        if (debuts[t])
            rect(x + 1, 78, x + CELLULE - 1, 92, 60, 120, 230);
        else
            rect(x + 1, 78, x + CELLULE - 1, 92, 90, 90, 90);
        if (fins[t])
            rect(x + 1, 98, x + CELLULE - 1, 112, 60, 180, 90);
        else if (debuts[t])
            rect(x + 1, 98, x + CELLULE - 1, 112, 220, 40, 40);
        else
            rect(x + 1, 98, x + CELLULE - 1, 112, 90, 90, 90);
    }
    FILE *f = std::fopen(fichier, "wb");
    if (!f)
        return;
    std::fprintf(f, "P6\n%d %d\n255\n", largeur, HAUTEUR);
    std::fwrite(img.data(), 1, img.size(), f);
    std::fclose(f);
}
