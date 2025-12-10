# IOT-Temperature-Controller
Projet IoT - Contrôleur intelligent de température
# Projet C : Moniteur de Données Série Arduino (Windows)

## Table des Matières

1.  Aperçu du Projet\
2.  Fonctionnalités\
3.  Prérequis\
4.  Configuration de l'Environnement\
5.  Utilisation du Projet\
6.  Structure du Code

------------------------------------------------------------------------

## 1. Aperçu du Projet

Ce projet est une application console écrite en langage C utilisant
l'API Windows (WinAPI) pour lire et traiter des données envoyées par un
microcontrôleur Arduino via le port série.

L'objectif principal est d'assurer une communication fiable sur Windows,
d'analyser la température reçue et d'enregistrer les données avec un
horodatage dans un fichier CSV.

------------------------------------------------------------------------

## 2. Fonctionnalités

-   **Communication Série (WinAPI)** : Établit la connexion et configure
    le port COM (Baud Rate, Timeouts).\
-   **Lecture et Enregistrement** : Lit les données brutes (température)
    du port série.\
-   **Horodatage Système** : Ajoute l'heure et la date réelles (format
    `[AAAA-MM-JJ HH:MM:SS]`) à chaque mesure.\
-   **Analyse de Données** : Utilise `sscanf` pour extraire la valeur
    numérique de la température.\
-   **Alerte Conditionnelle** : Affiche un message d'avertissement dans
    la console si la température dépasse un seuil défini (actuellement
    23.5°C).\
-   **Logging CSV** : Enregistre les données formatées avec l'horodatage
    dans un fichier `sensor_data.csv`.

------------------------------------------------------------------------

## 3. Prérequis

### Matériel

-   Un microcontrôleur Arduino (ou équivalent)\
-   Un capteur de température/humidité (ex : DHT11, DHT22)\
-   Un câble USB pour la connexion à Windows

### Logiciel et Outils

-   Windows 10/11\
-   GNU Compiler Collection (GCC) via MinGW-w64 (obligatoire pour
    compiler le code C sous Windows)\
-   Visual Studio Code (VS Code) recommandé pour l'édition

------------------------------------------------------------------------

## 4. Configuration de l'Environnement

### A. Installation de MinGW/GCC

Assurez-vous que MinGW est installé et que le chemin du dossier `bin`
est ajouté à la variable d'environnement `PATH`.

### B. Code Arduino

Votre programme Arduino doit envoyer uniquement la valeur numérique de
la température suivie d'un retour à la ligne.

**Exemple de format d'envoi :**

    Serial.println(temperature_float_value);

### C. Configuration du Port

Dans votre fichier C (ex : `temp.c`), définir le port COM correct :

    #define COM_PORT "\\.\COM3"    // !! CHANGEZ CE NUMÉRO (ex: COM4) !!
    #define BAUD_RATE 9600            // Doit correspondre à Serial.begin()

------------------------------------------------------------------------

## 5. Utilisation du Projet

### A. Compilation

Depuis le terminal de VS Code :

    gcc "temp.c" -o serial_read.exe -lkernel32 -mconsole

### B. Exécution

    ./serial_read.exe

Pour arrêter le programme : `Ctrl + C`.

------------------------------------------------------------------------

## 6. Structure du Code

Le fichier `temp.c` utilise les fonctions WinAPI pour gérer les
ressources du port série.

  -----------------------------------------------------------------------
  Composant                                 Description
  ----------------------------------------- -----------------------------
  `CreateFile()`                            Ouvre le port COM et obtient
                                            un handle (hSerial).

  `SetCommState()`                          Configure le Baud Rate, la
                                            parité, etc. via la structure
                                            DCB.

  `ReadFile()`                              Lit les données du port
                                            série.

  `sscanf()`                                Analyse la chaîne reçue pour
                                            extraire la température.

  `time()` / `strftime()`                   Récupère et formate
                                            l'horodatage.

  `fprintf()`                               Écrit les données et
                                            l'horodatage dans
                                            `sensor_data.csv`.
  -----------------------------------------------------------------------

------------------------------------------------------------------------

## Annexes (Contenu Répété du PDF)

### Projet C : Moniteur de Données Série Arduino (Windows)

### Table des Matières

1.  Aperçu du Projet\
2.  Fonctionnalités\
3.  Prérequis
    -   Matériel\
    -   Logiciel et Outils\
4.  Configuration de l'Environnement
    -   A. Installation de MinGW/GCC\
    -   B. Code Arduino\
    -   C. Configuration du Port\
5.  Utilisation du Projet
    -   A. Compilation\
    -   B. Exécution\
6.  Structure du Code
