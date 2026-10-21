#include "NKImage/Core/NkImage.h"
#include <cstdio>

using namespace nkentseu;

static bool EcrirePNG(const NkImage &i, const char *f) { return i.SavePNG(f); }
static bool EcrireJPEG(const NkImage &i, const char *f) { return i.SaveJPEG(f, 90); }
static bool EcrireBMP(const NkImage &i, const char *f) { return i.SaveBMP(f); }
static bool EcrireTGA(const NkImage &i, const char *f) { return i.SaveTGA(f); }
static bool EcrireQOI(const NkImage &i, const char *f) { return i.SaveQOI(f); }

struct Essai
{
    const char *nom;
    const char *fichier;
    bool (*ecrire)(const NkImage &, const char *);
};

static long TailleFichier(const char *chemin)
{
    FILE *f = std::fopen(chemin, "rb");
    if (!f)
        return -1;
    std::fseek(f, 0, SEEK_END);
    long n = std::ftell(f);
    std::fclose(f);
    return n;
}

int main()
{
    NkImage photo;
    if (!photo.Load("photo.jpg") || !photo.IsValid())
    {
        std::printf("photo.jpg introuvable ou illisible\n");
        return 1;
    }
    std::printf("photo : %d x %d, %d canaux\n", photo.Width(), photo.Height(), photo.Channels());

    const Essai essais[5] = {
        {"png", "sortie.png", EcrirePNG},
        {"jpeg", "sortie.jpeg", EcrireJPEG},
        {"bmp", "sortie.bmp", EcrireBMP},
        {"tga", "sortie.tga", EcrireTGA},
        {"qoi", "sortie.qoi", EcrireQOI},
    };

    bool memesDimensions[5];
    for (int i = 0; i < 5; ++i)
    {
        bool ecrit = essais[i].ecrire(photo, essais[i].fichier);
        long octets = TailleFichier(essais[i].fichier);
        NkImage relue;
        bool lu = ecrit && relue.Load(essais[i].fichier) && relue.IsValid();
        memesDimensions[i] = lu && relue.Width() == photo.Width() && relue.Height() == photo.Height();
        if (lu)
        {
            std::printf("%s : %ld octets, %d x %d\n", essais[i].nom, octets, relue.Width(), relue.Height());
        }
        else
        {
            std::printf("%s : %ld octets, illisible\n", essais[i].nom, octets);
        }
    }
    std::printf("\ndimensions relues comparees a la photo (%d x %d) :", photo.Width(), photo.Height());
    for (int i = 0; i < 5; ++i)
    {
        std::printf(" %s %s%s", essais[i].nom, memesDimensions[i] ? "identiques" : "differentes", i < 4 ? "," : "\n");
    }
    return 0;
}
