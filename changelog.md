# Projet POA

## Changelog

## [Build 200410.1] - 2020-04-10

### Fixed

- (A) Correction au niveau de la lecture du laby qui sortait une représentation mirroir.


## [Build 200405.1] - 2020-04-05

### Added

- (A/D) Ajout d'une version initial d'un lecteur de carte.

### Fixed

- (A) Multiplication des positions des persos par 10 (pour avoir les bonnes positions).
- (A) Remplissage des tableaux finaux manuellement car c'est peut-être la source de plantages (A voir).
- (A) Correction de la taille de la grille (erreur lors de la création de _data).
- (A) Dans char data(), inversion des i et j (car mon tableau est _data[height][width]).

### Added

- Marche aléatoire du gardien
- Ajout display_tab, afficher le contenu de _data
- Ajout des hitbox qui suivent le gardien et le chasseur

