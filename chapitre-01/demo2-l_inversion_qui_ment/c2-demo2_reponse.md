## Consigne

Montrer mon inversion générale rendant l'identité sur une matrice dégénérée, sans le moindre message. Faire imaginer à la classe ce que cela donnerait dans un casque, puis le dire : la caméra revient à l'origine, sans rotation, et rien ne l'explique.


## L'exécution de l'exercice 7 de la serie 5

Entrée, une pose saine, oeil à un mètre soixante, tourné de soixante degrés :
0.2 1.6 -0.5 0 0.5000000000 0 0.8660254038

Sortie :
inversion generale
0.500000 0.000000 -0.866025 -0.533013
0.000000 1.000000 0.000000 -1.600000
0.866025 0.000000 0.500000 0.076795
0.000000 0.000000 0.000000 1.000000
construction directe
0.500000 0.000000 -0.866025 -0.533013
0.000000 1.000000 0.000000 -1.600000
0.866025 0.000000 0.500000 0.076795
0.000000 0.000000 0.000000 1.000000
ecart maximal sur les seize coefficients 0.000000000023
inversion generale sur matrice degeneree
1.000000 0.000000 0.000000 0.000000
0.000000 1.000000 0.000000 0.000000
0.000000 0.000000 1.000000 0.000000
0.000000 0.000000 0.000000 1.000000
aucun message d'erreur n'a ete emis


## Constat

« Aucune erreur. Aucun message dans le journal. Le programme continue, l'image sort à l'heure, la cadence est bonne. Simplement, la caméra est revenue à l'origine du monde, sans rotation, et le porteur du casque se retrouve quelque part dans le décor sans que rien ne lui explique pourquoi. Voilà pourquoi le module écrit l'inverse à la main : ce n'est pas une question de vitesse, c'est une question de silence. »
