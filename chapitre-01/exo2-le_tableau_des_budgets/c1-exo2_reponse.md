## Le tableau avec sources

| Étape du chapitre                  | Ordre de grandeur du cours | Valeur trouvée dans une source | Source |
|------------------------------------|----------------------------|--------------------------------|--------|
| Les capteurs mesurent le mouvement | 1 à 2 ms                   | 1 à 2 ms                       | 1.     |
| Le système transmet la mesure      | 1 à 3 ms                   | 1 à 4 ms                       | 2.     |
| Votre application décide et dessine| 5 à 11 ms                  | 5 à 10 ms                      | 3.     |
| Le compositeur assemble            | 1 à 2 ms                   | 1 à 2 ms                       | 4.     |
| L'écran affiche la ligne           | 2 à 5 ms                   | ras                            |        |


## Ce que je n'ai pas trouvé

Aucune source ne donne les cinq étapes mesurées séparément sur un même casque du commerce. Les tableaux publiés sont des budgets indicatifs, construits à partir de sources différentes pour chaque ligne, pas des relevés faits sur un seul appareil avec un même instrument. Le tableau ci-dessus est donc un assemblage, exactement comme celui du chapitre.

Je n'ai trouvé aucune valeur mesurée et publiée pour la transmission seule. Le chiffre de 1 à 4 millisecondes provient d'un tableau de synthèse dont la référence renvoie à une discussion de forum, ce qui est une source faible. Je la donne quand même en signalant sa qualité.


## Sources

Détails de la valeur trouvé : pour une centrale inertielle échantillonnée entre 500 et 1000 Hz. 15 à 33 ms si le suivi passe par des caméras, qui tournent entre 30 et 90 Hz. 
1. DAQRI, *Motion to Photon Latency in Mobile AR and VR*, 2018. https://medium.com/@DAQRI/motion-to-photon-latency-in-mobile-ar-and-vr-99f82c480926

Détails de la valeur trouvé : pour l'envoi des données du casque vers l'hôte et le calcul de la pose à six degrés de liberté
2. VR & AR Wiki, *Motion-to-photon latency*, tableau du budget de latence. https://vrarwiki.com/wiki/Motion-to-photon_latency

Détails de la valeur trouvé : Meta vise moins de 5 à 10 ms pour la partie CPU, et impose au rendu GPU de tenir sous 11,1 ms pour une cible à 90 Hz
3. Meta, *Understanding Gameplay Latency for Oculus Quest, Oculus Go, and Gear VR*. https://developers.meta.com/horizon/blog/understanding-gameplay-latency-for-oculus-quest-oculus-go-and-gear-vr/

Détails de la valeur trouvé : pour la correction de distorsion et le timewarp asynchrone
4. Meta, *Asynchronous Timewarp Examined*. https://developers.meta.com/horizon/blog/asynchronous-timewarp-examined/
