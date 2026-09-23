## La mesure, un seul oeil

| Grandeur                             | Moyenne  | Médiane  |
|--------------------------------------|----------|----------|
| Rendu seul                           | 0,863 ms | 0,740 ms |
| Image complète                       | 0,994 ms | 0,873 ms |
| Soustraction                         | 0,131 ms | 0,133 ms |


## L'estimation

Seul le rendu de la scène se double, parce que lui seul dépend du point de vue. La logique, la physique et le chargement des ressources ne dépendent pas de l'oeil et se font une fois par image.

Estimation sur les moyennes : 0,131 + 2 × 0,863 = 1,857 ms

Estimation sur les médianes : 0,133 + 2 × 0,740 = 1,613 ms

## La vérification

Mesure réelle avec `--double` :

```
mode : double rendu
image complete, logique plus rendu
  moyenne 1.637 ms
  mediane 1.622 ms
  centile 99 2.745 ms
  pire image 11.909 ms
  au-dela de 11.000 ms : 1 sur 1000
rendu seul, sans la logique
  moyenne 1.499 ms
  mediane 1.480 ms
  centile 99 2.513 ms
  pire image 11.785 ms
  au-dela de 11.000 ms : 1 sur 1000
```

Les deux estimations ne sont pas egales, et c'est le résultat le plus instructif de l'exercice.

Sur les moyennes, l'estimation annonçait 1,857 ms et la mesure donne 1,637 ms. L'estimation dépasse la réalité de 0,220 ms, soit 12% de trop.

Sur les médianes, l'estimation annonçait 1,613 ms et la mesure donne 1,622 ms. L'écart est de 0,009 ms.

Le rendu seul confirme la même chose. Sa médiane passe de 0,740 à 1,480 ms, exactement le double au millième près, alors que sa moyenne passe de 0,863 à 1,499 ms, soit un facteur 1,74 seulement.


## Ce qu'il faudrait réduire

Le rendu, pas la logique, et la raison est arithmétique plutôt qu'esthétique.

Le rendu est la seule partie qui paie deux fois. Une milliseconde gagnée sur le rendu en fait gagner deux sur l'image complète ; une milliseconde gagnée sur la logique n'en rapporte qu'une. À effort égal, l'optimisation du rendu a un rendement double de celle du reste.

Les chiffres le disent plus brutalement encore. Sur les médianes, le rendu représente 85% de l'image à un oeil et 91% à deux yeux. La logique, elle, pèse 0,133 ms, soit 1.5% du budget : on peut la diviser par dix sans que personne ne s'en aperçoive. Le simple fait de passer en stéréoscopie déplace le centre de gravité du coût vers le rendu, sans qu'une ligne de code ait changé.

Mais mes relevés montrent qu'il faut distinguer deux travaux d'optimisation, parce qu'ils ne visent pas la même chose. Réduire la médiane du rendu fait baisser le coût de toutes les images, et c'est ce que double la stéréoscopie. Supprimer les pics isolés est une autre enquête, qui ne porte ni sur le rendu ni sur la logique mais sur ce qui bloque le programme au mauvais moment, appels système, allocations mémoire, ordonnancement. Or en réalité virtuelle, c'est le second travail qui décide si l'expérience est tenable, puisqu'une expérience en casque se juge à sa pire image.

Ordre de priorité qui en découle : d'abord faire disparaître les pics, ensuite réduire la médiane du rendu, et la logique en dernier, voire jamais.