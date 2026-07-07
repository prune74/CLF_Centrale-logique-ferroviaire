#include "CLF_JsonStorage.h"

/* ---------------------------------------------------------------------------
 * Montage SPIFFS
 * --------------------------------------------------------------------------- */
void CLF_JsonStorage::begin()
{
    if (!SPIFFS.begin(true))
    {
        LOG_ERROR("SPIFFS → échec du montage");
        return;
    }

    LOG_INFO("SPIFFS monté avec succès");

    ensureFileExists();
}

/* ---------------------------------------------------------------------------
 * Crée le fichier JSON s'il n'existe pas
 * --------------------------------------------------------------------------- */
void CLF_JsonStorage::ensureFileExists()
{
    if (!SPIFFS.exists(CLF_Variables::JSON_FILENAME))
    {
        LOG_WARN("%s absent → création d’un fichier vide", CLF_Variables::JSON_FILENAME);

        JsonDocument doc;
        doc["trains"] = JsonArray();

        File file = SPIFFS.open(CLF_Variables::JSON_FILENAME, "w");
        serializeJsonPretty(doc, file);
        file.close();
    }
}

/* ---------------------------------------------------------------------------
 * Lecture du fichier JSON → tableau TrainState
 * --------------------------------------------------------------------------- */
void CLF_JsonStorage::load(TrainState trains[])
{
    File file = SPIFFS.open(CLF_Variables::JSON_FILENAME, "r");
    if (!file)
    {
        LOG_ERROR("Impossible d’ouvrir %s en lecture", CLF_Variables::JSON_FILENAME);
        return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error)
    {
        LOG_ERROR("Erreur JSON dans %s", CLF_Variables::JSON_FILENAME);
        return;
    }

    // Lecture du profil de voie (N / HO)
    if (!doc["trackProfile"].isNull())
    {
        uint8_t p = doc["trackProfile"] | 1; // HO par défaut
        CLF_TRACK_PROFILE = (p == 1 ? TrackProfile::HO : TrackProfile::N);

        LOG_INFO("CLF → profil chargé depuis JSON : %s",
                 (CLF_TRACK_PROFILE == TrackProfile::HO ? "HO" : "N"));
    }
    else
    {
        LOG_WARN("CLF → aucun profil dans JSON → HO par défaut");
        CLF_TRACK_PROFILE = TrackProfile::HO;
    }

    JsonArray arr = doc["trains"].to<JsonArray>();

    uint8_t index = 0;
    for (JsonObject obj : arr)
    {
        if (index >= CLF_Variables::MAX_TRAINS)
            break;

        trains[index].trainID = obj["trainID"] | 0;
        trains[index].vitesseFinale = obj["vitesseFinale"] | 0.0f;
        trains[index].essieuxFinal = obj["essieuxFinal"] | 0;
        trains[index].cv5 = obj["cv5"] | CLF_Variables::CV5_DEFAULT;
        trains[index].silenceActif = obj["silenceActif"] | false;
        trains[index].silenceExpireAt = obj["silenceExpireAt"] | 0;
        trains[index].timestampValidation = obj["timestampValidation"] | 0;

        // ⭐ Lecture cantonHistory
        JsonArray hist = obj["cantonHistory"].as<JsonArray>();
        for (uint8_t i = 0; i < CLF_Variables::NB_MESURES_VALIDATION; i++)
            trains[index].cantonHistory[i] = hist[i] | 0;

        index++;
    }

    LOG_INFO("%s → %u trains chargés", CLF_Variables::JSON_FILENAME, index);
}

/* ---------------------------------------------------------------------------
 * Sauvegarde d’un train dans le JSON
 * --------------------------------------------------------------------------- */
void CLF_JsonStorage::saveTrain(const TrainState &t)
{
    File file = SPIFFS.open(CLF_Variables::JSON_FILENAME, "r");
    if (!file)
    {
        LOG_ERROR("Impossible d’ouvrir %s en lecture", CLF_Variables::JSON_FILENAME);
        return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error)
    {
        LOG_ERROR("Erreur JSON → impossible de mettre à jour un train");
        return;
    }

    JsonArray arr = doc["trains"].as<JsonArray>();

    // Chercher le train dans le JSON
    bool found = false;
    for (JsonObject obj : arr)
    {
        if (obj["trainID"] == t.trainID)
        {
            obj["vitesseFinale"] = t.vitesseFinale;
            obj["essieuxFinal"] = t.essieuxFinal;
            obj["cv5"] = t.cv5;
            obj["silenceActif"] = t.silenceActif;
            obj["silenceExpireAt"] = t.silenceExpireAt;
            obj["timestampValidation"] = t.timestampValidation;

            // ⭐ Sauvegarde cantonHistory
            JsonArray hist = obj["cantonHistory"].to<JsonArray>();
            hist.clear();
            for (uint8_t i = 0; i < CLF_Variables::NB_MESURES_VALIDATION; i++)
                hist.add(t.cantonHistory[i]);

            found = true;
            break;
        }
    }

    // Si le train n'existe pas → ajout
    if (!found)
    {
        JsonObject obj = arr.add<JsonObject>();
        obj["trainID"] = t.trainID;
        obj["vitesseFinale"] = t.vitesseFinale;
        obj["essieuxFinal"] = t.essieuxFinal;
        obj["cv5"] = t.cv5;
        obj["silenceActif"] = t.silenceActif;
        obj["silenceExpireAt"] = t.silenceExpireAt;
        obj["timestampValidation"] = t.timestampValidation;

        JsonArray hist = obj["cantonHistory"].to<JsonArray>();
        for (uint8_t i = 0; i < CLF_Variables::NB_MESURES_VALIDATION; i++)
            hist.add(t.cantonHistory[i]);
    }

    // Écriture du fichier
    file = SPIFFS.open(CLF_Variables::JSON_FILENAME, "w");
    if (!file)
    {
        LOG_ERROR("Impossible d’ouvrir %s en écriture", CLF_Variables::JSON_FILENAME);
        return;
    }

    serializeJsonPretty(doc, file);
    file.close();

    LOG_INFO("Train %d mis à jour dans %s", t.trainID, CLF_Variables::JSON_FILENAME);
}

/* ---------------------------------------------------------------------------
 * Sauvegarde complète du tableau
 * --------------------------------------------------------------------------- */
void CLF_JsonStorage::saveAll(const TrainState trains[])
{
    JsonDocument doc;
    JsonArray arr = doc["trains"].to<JsonArray>();

    for (uint8_t i = 0; i < CLF_Variables::MAX_TRAINS; i++)
    {
        if (trains[i].trainID == 0)
            continue;

        JsonObject obj = arr.add<JsonObject>();
        obj["trainID"] = trains[i].trainID;
        obj["vitesseFinale"] = trains[i].vitesseFinale;
        obj["essieuxFinal"] = trains[i].essieuxFinal;
        obj["cv5"] = trains[i].cv5;
        obj["silenceActif"] = trains[i].silenceActif;
        obj["silenceExpireAt"] = trains[i].silenceExpireAt;
        obj["timestampValidation"] = trains[i].timestampValidation;

        // ⭐ Sauvegarde cantonHistory
        JsonArray hist = obj["cantonHistory"].to<JsonArray>();
        for (uint8_t j = 0; j < CLF_Variables::NB_MESURES_VALIDATION; j++)
            hist.add(trains[i].cantonHistory[j]);
    }

    File file = SPIFFS.open(CLF_Variables::JSON_FILENAME, "w");
    if (!file)
    {
        LOG_ERROR("Impossible d’ouvrir %s en écriture", CLF_Variables::JSON_FILENAME);
        return;
    }

    serializeJsonPretty(doc, file);
    file.close();

    LOG_INFO("%s sauvegardé entièrement", CLF_Variables::JSON_FILENAME);
}

/* ---------------------------------------------------------------------------
 * Sauvegarde du profil de voie (N / HO )
 * --------------------------------------------------------------------------- */
void CLF_JsonStorage::saveProfile(TrackProfile p)
{
    File file = SPIFFS.open(CLF_Variables::JSON_FILENAME, "r");
    if (!file)
    {
        LOG_ERROR("Impossible d’ouvrir %s pour lecture", CLF_Variables::JSON_FILENAME);
        return;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, file);
    file.close();

    if (err)
    {
        LOG_ERROR("Erreur JSON → impossible de sauvegarder le profil");
        return;
    }

    doc["trackProfile"] = (uint8_t)p;

    file = SPIFFS.open(CLF_Variables::JSON_FILENAME, "w");
    if (!file)
    {
        LOG_ERROR("Impossible d’ouvrir %s pour écriture", CLF_Variables::JSON_FILENAME);
        return;
    }

    serializeJsonPretty(doc, file);
    file.close();

    LOG_INFO("CLF → profil sauvegardé dans JSON : %s",
             (p == TrackProfile::HO ? "HO" : "N"));
}
