## Le casque choisi

Valve Index, en mode natif, à 144 hertz. Je le choisis parce que c'est l'un des rares casques pour lesquels les quatre angles sont publiés séparément, relevés directement dans le runtime OpenVR avec l'outil `hmdq` et déposés dans une base ouverte, plutôt que résumés en un seul chiffre de champ de vision total comme le font les fiches commerciales.

## Les quatre angles de l'oeil gauche

Valeurs brutes de la matrice de projection, telles que le runtime les rend :

| Direction | Angle         |
|-----------|---------------|
| Gauche    | -49,00 degrés |
| Droite    | 47,98 degrés  |
| Bas       | -54,63 degrés |
| Haut      | 54,73 degrés  |

La même base donne aussi ces angles ramenés dans le repère de la tête, ce qui est la mesure la plus parlante puisque les écrans de l'Index sont inclinés de cinq degrés vers l'extérieur :

| Direction | Angle dans le repère de la tête |
|-----------|---------------------------------|
| Gauche    | -54,00 degrés                   |
| Droite    | 42,98 degrés                    |
| Bas       | -54,52 degrés                   |
| Haut      | 54,63 degrés                    |

Écart interpupillaire au moment du relevé : 58 mm. Champ total des deux yeux : 108,06 degrés horizontalement, 109,16 verticalement, avec un recouvrement stéréo de 85,93 degrés.

Source : HMD Geometry Database, Richard Musil, configuration Valve Index 144 Hz, relevé contribué par *jojon*, données produites par l'outil `hmdq`. https://risa2000.github.io/hmdgdb/hmd_cfgs/Index_Native_R144.html

## L'asymétrie, en chiffres

Dans le repère de la tête, l'oeil gauche voit cinquante-quatre degrés vers l'extérieur et seulement quarante-trois vers l'intérieur, soit onze degrés d'écart. La raison est physique et le chapitre la donne : la lentille n'est pas centrée sur l'oeil, et le nez occupe le côté intérieur. L'axe vertical est presque symétrique, cinquante-quatre en haut comme en bas, parce que rien n'obstrue ces deux directions.

Valve écrit d'ailleurs, dans sa propre note technique sur le champ de vision de l'Index, que les troncs de projection décalés et la forme non circulaire de la zone rendue rendent le champ asymétrique, et que ce champ n'est même pas rigoureusement constant d'une image à l'autre à cause du masquage dynamique du compositeur. Source : https://www.valvesoftware.com/en/index/deep-dive/fov

## Si l'on employait un champ symétrique de même surface

On perdrait de la vision périphérique vers l'extérieur, là où l'oeil voit réellement quelque chose, pour dessiner des pixels vers l'intérieur, du côté du nez, où l'oeil ne voit rien : même coût de rendu, moins de champ utile.
