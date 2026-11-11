#include "chaine.h"
#include <iostream>

int main()
{
    ChaineEchange chaine;
    ChaineDesc desc;
    desc.imageCount = 3;
    std::cout << "creation de 3 images : " << (chaine.Creer(desc) ? "accepte" : "refuse") << '\n';

    for (int tour = 1; tour <= 6; ++tour)
    {
        std::uint32_t emprunte = 0;
        chaine.AcquireImage(emprunte);
        chaine.ReleaseImage();
        std::cout << "tour " << tour << " : emprunte " << emprunte
                  << ", prochaine " << chaine.GetNextIndex()
                  << ", derniere rendue " << chaine.GetLastReleasedIndex() << '\n';
    }

    std::uint32_t index = 0;
    chaine.AcquireImage(index);
    bool deuxieme = chaine.AcquireImage(index);
    std::cout << "double emprunt : " << (deuxieme ? "accepte" : "refuse") << '\n';
    chaine.ReleaseImage();

    bool sansEmprunt = chaine.ReleaseImage();
    std::cout << "rendre sans avoir emprunte : " << (sansEmprunt ? "accepte" : "refuse") << '\n';

    ChaineEchange trop;
    ChaineDesc grande;
    grande.imageCount = ChaineEchange::MaxImages + 1;
    std::cout << "creation de 5 images : " << (trop.Creer(grande) ? "accepte" : "refuse") << '\n';

    ChaineEchange seule;
    desc.imageCount = 1;
    seule.Creer(desc);
    std::uint32_t unique = 0;
    seule.AcquireImage(unique);
    seule.ReleaseImage();
    std::cout << "une seule image : prochaine " << seule.GetNextIndex()
              << ", derniere rendue " << seule.GetLastReleasedIndex() << '\n';
    return 0;
}
