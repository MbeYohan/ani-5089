#include "NKImage/Core/NkImage.h"
#include "NKTime/NkChrono.h"
#include <cstdio>

using namespace nkentseu;

static const int kRepetitions = 20;

struct Essai
{
    const char *nom;
    NkResizeFilter filtre;
    const char *fichier;
};

static long long CompterDifferences(const NkImage &a, const NkImage &b)
{
    long long n = 0;
    for (int y = 0; y < a.Height(); ++y)
    {
        for (int x = 0; x < a.Width(); ++x)
        {
            math::NkColor p = a.GetPixel(x, y);
            math::NkColor q = b.GetPixel(x, y);
            if (p.r != q.r || p.g != q.g || p.b != q.b || p.a != q.a)
            {
                ++n;
            }
        }
    }
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
    NkImage source = photo.Resize(2048, 2048);
    if (!source.IsValid())
    {
        std::printf("reduction en 2048 impossible\n");
        return 1;
    }
    std::printf("source : %d x %d\n", source.Width(), source.Height());

    const Essai essais[4] = {
        {"NK_NEAREST", NkResizeFilter::NK_NEAREST, "nearest.png"},
        {"NK_BILINEAR", NkResizeFilter::NK_BILINEAR, "bilinear.png"},
        {"NK_BICUBIC", NkResizeFilter::NK_BICUBIC, "bicubic.png"},
        {"NK_LANCZOS3", NkResizeFilter::NK_LANCZOS3, "lanczos3.png"},
    };

    NkImage resultat[4];
    double moyenne[4];
    for (int i = 0; i < 4; ++i)
    {
        double total = 0.0;
        for (int r = 0; r < kRepetitions; ++r)
        {
            NkChrono chrono;
            NkImage petite = source.Resize(256, 256, essais[i].filtre);
            total += chrono.Elapsed().ToMicroseconds();
            if (r == 0)
            {
                resultat[i] = static_cast<NkImage &&>(petite);
            }
        }
        moyenne[i] = total / kRepetitions;
        if (!resultat[i].IsValid() || !resultat[i].SavePNG(essais[i].fichier))
        {
            std::printf("%s : ecriture impossible\n", essais[i].fichier);
            return 1;
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        long long d = CompterDifferences(resultat[i], resultat[1]);
        std::printf("%s : %.1f us, differences : %lld\n", essais[i].nom, moyenne[i], d);
    }
    std::printf("taille des resultats : %d x %d\n", resultat[0].Width(), resultat[0].Height());
    return 0;
}
