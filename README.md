# Jeu de Gladiateurs en C avec GTK3

## Description
Ce projet est un jeu de gladiateurs développé en langage C en utilisant la bibliothèque GTK3 pour l'interface graphique. Les gladiateurs sont placés dans une arène et doivent combattre tous ceux qu'ils croisent. Chaque gladiateur possède la capacité de se cacher périodiquement derrière des objets dans l'arène pour éviter les attaques. Lorsqu'un gladiateur réussit à éliminer un adversaire, sa force augmente, améliorant ainsi ses chances de survie.

## Fonctionnalités
- **Déplacement des gladiateurs** : Les gladiateurs se déplacent librement dans l'arène.
- **Combat automatique** : Tout gladiateur qui croise un adversaire l'attaque immédiatement.
- **Système de cachette** : Les gladiateurs peuvent se cacher derrière des objets pour éviter les attaques.
- **Gain de force** : Lorsqu'un gladiateur élimine un adversaire, sa force augmente.
- **Interface graphique intuitive** : L'arène et les gladiateurs sont représentés visuellement grâce à GTK3.

## Prérequis
Pour exécuter ce jeu, vous devez installer GTK3 et un compilateur C compatible.

### Installation de GTK3 (Linux)
```sh
sudo apt update
sudo apt install libgtk-3-dev
```

### Installation de GTK3 (Windows)
Vous pouvez télécharger et installer GTK3 à partir du site officiel : [https://www.gtk.org/](https://www.gtk.org/)

## Compilation et exécution
1. Clonez le dépôt :
```sh
git clone https://github.com/adamaKomi/gladiator-game.git
gladiator-game
```

2. Compilez le projet avec `gcc` :
```sh
gcc -o gladiateurs main.c `pkg-config --cflags --libs gtk+-3.0`
```

3. Exécutez le jeu :
```sh
./gladiateurs
```


## Améliorations futures
- Ajout d'une intelligence artificielle pour rendre les combats plus stratégiques.
- Intégration d'un mode multijoueur.
- Ajout d'un système de niveaux et de progression.
- Effets visuels et sonores pour améliorer l'expérience de jeu.

## Contribution
Les contributions sont les bienvenues ! N'hésitez pas à ouvrir une issue ou à proposer une pull request pour améliorer le projet.

## Licence
Ce projet est sous licence MIT. Voir le fichier `LICENSE` pour plus d'informations.

