#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

// --- Configuration ---
#define COM_PORT "COM5" // !! CHANGE THIS to your Arduino's COM port !!
#define BAUD_RATE 9600
#define BUFFER_SIZE 256
// ---------------------

struct temperature {
    float seuil_min;
    float seuil_max;
    float intervalle_mesure; // temps entre deux mesures en secondes
};

struct alerte {
    int confirmed;       // nombre d'alertes confirmées
    int duree_confirme;  // durée totale en état d'alerte (en secondes)
};

int main() {
    char timestamp_buffer[50];
    HANDLE hSerial;
    DCB dcbSerialParams = { 0 };
    COMMTIMEOUTS timeouts = { 0 };
    DWORD dwBytesRead = 0;
    char lpBuffer[BUFFER_SIZE];
    FILE *dataFile = NULL;

    // 1. Open the Serial Port
    // FILE_ATTRIBUTE_NORMAL ensures it's opened for standard I/O
    // 0 is for not sharing the file
    // OPEN_EXISTING means the port must already be present
    hSerial = CreateFile(
        COM_PORT,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("Error opening serial port %s. Check port name and if it's in use.\n", COM_PORT);
        return 1;
    }

    // 2. Configure Communication Parameters (Baud Rate, Data Bits, etc.)
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        printf("Error getting state.\n");
        CloseHandle(hSerial);
        return 1;
    }

    dcbSerialParams.BaudRate = BAUD_RATE;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        printf("Error setting serial port state.\n");
        CloseHandle(hSerial);
        return 1;
    }

    // 3. Configure Timeouts (VITAL for ReadFile to not block forever)
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hSerial, &timeouts)) {
        printf("Error setting timeouts.\n");
        CloseHandle(hSerial);
        return 1;
    }

    // 4. Open file for saving data
    dataFile = fopen("sensor_data.csv", "w");
    if (dataFile == NULL) {
        printf("Error opening sensor_data.csv for writing.\n");
        CloseHandle(hSerial);
        return 1;
    }

    printf("Successfully opened %s at %d baud. Reading data...\n", COM_PORT, BAUD_RATE);
    printf("Press Ctrl+C to stop.\n");


    // ----------------------------

    int i, n;

    int confirmed1 = 0, confirmed2 = 0, confirmed3 = 0 , confirmed4 = 0, confirmed5 = 0, confirmed6 = 0 ;



    struct temperature temp;
    // --- Lecture des seuils depuis config.txt ---
    char ligne[100];
    FILE *f = fopen("config.txt", "r");
    if (!f) { 
        printf("Erreur ouverture fichier config.txt\n"); 
        return 1; 
    }

    fgets(ligne, sizeof(ligne), f); temp.seuil_min = atof(ligne);
    fgets(ligne, sizeof(ligne), f); temp.seuil_max = atoi(ligne);
    fgets(ligne, sizeof(ligne), f); temp.intervalle_mesure = atoi(ligne);
    fclose(f);

    // --- Initialisation des alertes ---
    struct alerte alertes[6]; // 0: niveau1 chaud, ..., 5: niveau3 froid
    for (int i = 0; i < 6; i++) {
        alertes[i].confirmed = 0;
        alertes[i].duree_confirme = 0;
    }
    int last_level = 0;         // dernier niveau d'alerte lu
    int consec_count = 0;       // compteur consécutif pour ce niveau
    int episode_confirme = 0;   // permet de confirmer l'alerte une seule fois par épisode

    // --- Statistiques par lot ---
    float somme = 0.0f, min = 1e9f, max = -1e9f;
    int compteur_mesures = 0;

    // --------------------------------

    FILE *rapport = fopen("rapport_journalier.txt", "w");
    if (!rapport) { 
        printf("Erreur ouverture fichier rapport_journalier.txt.\n"); 
        return 1; 
    }

    // 5. Read Data Loop
    while (1) {
        // ReadFile will attempt to read up to BUFFER_SIZE bytes
        if (ReadFile(hSerial, lpBuffer, BUFFER_SIZE - 1, &dwBytesRead, NULL)) {
            if (dwBytesRead > 0) {
                lpBuffer[dwBytesRead] = '\0'; // Null-terminate the string
                
                // Print and Save
                

                float t = 0.0;
            
                if (sscanf(lpBuffer, " %f", &t) == 1) { 
                    
                    time_t rawtime;
                    struct tm *timeinfo;
                    time(&rawtime);
                    timeinfo = localtime(&rawtime);
                    strftime(timestamp_buffer, 50, "[%Y-%m-%d %H:%M:%S]", timeinfo);

                    printf("Temperature = %.2f C\n", t);
                    fprintf(dataFile, "%s,Temperature = %.2f C\n", timestamp_buffer, t);
                    fflush(dataFile); // Écrit immédiatement sur le disque
                    
                    // ------------------------------------------------------------

                    int niveau = 0;

                    if (t > temp.seuil_max + 5) niveau = 3;
                    else if (t > temp.seuil_max + 2 && t <= temp.seuil_max + 5) niveau = 2;
                    else if (t > temp.seuil_max) niveau = 1;
                    else if (t < temp.seuil_min - 5) niveau = 6;
                    else if (t >= temp.seuil_min - 5 && t <= temp.seuil_min - 2) niveau = 5;
                    else if (t <= temp.seuil_min) niveau = 4;

                    if (niveau == 0) {
                        last_level = 0;
                        consec_count = 0;
                        episode_confirme = 0;
                    }
                    else {
                        if (niveau == last_level) {
                        consec_count++;
                        } else {
                            last_level = niveau;
                            consec_count = 1;
                            episode_confirme = 0;
                        }     
                    }
                    
                    

                    if (consec_count >= 3 && episode_confirme == 0) {
                        episode_confirme = 1;
                        int idx = niveau - 1; // niveau 1->0, 2->1, ..., 6->5
                        alertes[idx].confirmed++;
                        alertes[idx].duree_confirme += consec_count * temp.intervalle_mesure;
                        printf(">>> ALERTE CONFIRMEE NIVEAU %d ! (%d consecutives)\n", niveau, consec_count);
                    }

                    somme += t;
                    if (t < min) min = t;
                    if (t > max) max = t;
                    compteur_mesures++;

                    int MESURES_PAR_LOT = 15;
                    if (compteur_mesures >= MESURES_PAR_LOT) {
                        float moyenne = somme / compteur_mesures;
                        fprintf(rapport, "Rapport pour les %d dernières mesures (%s)\n", MESURES_PAR_LOT);
                        fprintf(rapport, "Temp moyenne : %.2f\n", moyenne);
                        fprintf(rapport, "Temp min : %.2f\n", min);
                        fprintf(rapport, "Temp max : %.2f\n", max);

                        const char* noms[6] = {"Niveau 1 chaud","Niveau 2 chaud","Niveau 3 chaud",
                                               "Niveau 1 froid","Niveau 2 froid","Niveau 3 froid"};
                        for (int j = 0; j < 6; j++) {
                            fprintf(rapport, "%s : %d alertes confirmées, duree totale %d sec\n",
                                    noms[j], alertes[j].confirmed, alertes[j].duree_confirme);
                        }
                        fprintf(rapport, "----------------------------------------\n");
                        fflush(rapport);

                        // --- Réinitialisation pour le prochain lot ---
                        somme = 0.0f; min = 1e9f; max = -1e9f;
                        compteur_mesures = 0;
                        for (int j = 0; j < 6; j++) {
                            alertes[j].confirmed = 0;
                            alertes[j].duree_confirme = 0;
                        }
                    }


                }         
            }
        } 
        Sleep(10); // Pause briefly to prevent high CPU usage
    }

    // 6. Cleanup (Note: loop is infinite, cleanup runs on exit)
    fclose(dataFile);
    CloseHandle(hSerial);
    return 0;
}