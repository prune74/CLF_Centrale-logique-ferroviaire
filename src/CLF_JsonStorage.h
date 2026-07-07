#pragma once
#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "CLF_TrainState.h"
#include "CLF_Config.h"
#include "CLF_Variables.h"

#include "Debug.h"

/*
 * CLF_JsonStorage.h
 * Gestion du fichier JSON contenant les trains validés.
 * Fichier : /clf_trains.json
 */

class CLF_JsonStorage
{
public:
    static void begin();  // Montage SPIFFS
    static void load(TrainState trains[]);  // Lecture JSON → tableau TrainState
    static void saveTrain(const TrainState &t); // Mise à jour d’un train
    static void saveAll(const TrainState trains[]); // Sauvegarde complète
    static void saveProfile(TrackProfile p); // Sauvegarde du profil de voie (N / HO)

private:
    static void ensureFileExists(); // Crée un fichier vide si absent
};
