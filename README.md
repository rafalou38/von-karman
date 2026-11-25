# Projet matlab LBM - Von Karman

Le code a jour est disponible sur: [https://github.com/rafalou38/von-karman](https://github.com/rafalou38/von-karman)

Pour executer le code c++, il faut installer:
- la librarie raylib sur le système
- le repo eigen, version 5.0.1 dans le dossier portage.

Le code a été testé sur linux, compatibilité inconnue sur windows.

## Compilation

```sh
make -j3
```

## Execution
```sh
# Execution affichage double
./vonkarm

# Execution interactive (tab pour changer de visualisation)
./vonkarm -i

# Enregistrement dans le dossier out/
./vonkarm -r
```