# Projet-Labyrinthe
## Binôme : Alexandre l'Heritier et Damien Merret
Jeu labyrinthe en C++.

Projet POA : https://www.lri.fr/~pa/PROGCXX/laby-hante.html

# Derniers changements :
## [Build 200405.1] - 2020-04-05

### Added

- (A/D) Ajout d'une version initial d'un lecteur de carte.

### Fixed

- (A) Multiplication des positions des persos par 10 (pour avoir les bonnes positions).
- (A) Remplissage des tableaux finaux manuellement car c'est peut-être la source de plantages (A voir).
- (A) Correction de la taille de la grille (erreur lors de la création de _data).
- (A) Dans char data(), inversion des i et j (car mon tableau est \_data\[height]\[width]).
