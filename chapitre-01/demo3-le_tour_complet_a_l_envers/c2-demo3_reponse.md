## Consigne

Montrer ma vitesse angulaire sans le forçage du chemin court, sur un delta minuscule qui se lit comme un tour presque complet. Puis ajouter les trois lignes et remontrer.


## L'exécution de exercice 9 de la série 5

Entrée, orientation de départ identité, orientation d'arrivée l'opposé d'une rotation de deux degrés, sur dix millisecondes :
0 0 0 1 0 -0.017452406437 0 -0.999847695156 0.01

Sortie:
avec forcage   -0.0000 3.4907 -0.0000  soit 200.0000 deg/s
sans forcage   0.0000 -624.8279 0.0000  soit 35800.0000 deg/s

## Constat

« Deux degrés. La tête a tourné de deux degrés, et le programme vous annonce trente-cinq mille huit cents degrés par seconde, dans l'autre sens. Il ne s'est pas trompé dans le calcul, il a calculé exactement ce qu'on lui a demandé. Le quaternion qu'on lui a donné est le bon, c'est son signe qui a changé, et un quaternion et son opposé décrivent la même rotation. Trois lignes séparent une valeur correcte d'une valeur qui envoie la tête n'importe où. »
