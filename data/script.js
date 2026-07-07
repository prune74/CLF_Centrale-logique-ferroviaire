// -------------------------------------------------------------
// WebSocket – Base moderne Discovery 2026
// -------------------------------------------------------------

let ws = null;
let wsConnected = false;

// Connexion automatique au WebSocket
function wsConnect() {
  ws = new WebSocket(`ws://${location.host}/ws`);

  ws.onopen = () => {
    wsConnected = true;
    console.log("WS → connecté");
  };

  ws.onclose = () => {
    wsConnected = false;
    console.warn("WS → déconnecté, reconnexion dans 1s...");
    setTimeout(wsConnect, 1000);
  };

  ws.onerror = (err) => {
    console.error("WS → erreur", err);
  };

  ws.onmessage = (event) => {
    try {
      const msg = JSON.parse(event.data);
      handleMessage(msg);
    } catch (e) {
      console.error("WS → JSON invalide :", event.data);
    }
  };
}

// -------------------------------------------------------------
// Gestion des messages reçus
// -------------------------------------------------------------
function handleMessage(msg) {
  console.log("WS → message reçu :", msg);

  // Exemple : affichage des états système
  if (msg.wifi_on !== undefined) {
    console.log("WiFi :", msg.wifi_on ? "ON" : "OFF");
  }

  if (msg.can_ok !== undefined) {
    console.log("CAN OK :", msg.can_ok);
  }

  if (msg.last_rx !== undefined) {
    console.log("Dernière trame CAN reçue :", msg.last_rx, "ms");
  }

  // Tu ajouteras ici :
  // - états ferroviaires
  // - TCO
  // - signaux
  // - aiguilles
  // - profils train
  // - supervision Discovery
}

// -------------------------------------------------------------
// Envoi d'une commande JSON au WebSocket
// -------------------------------------------------------------
function sendCommand(obj) {
  if (!wsConnected) {
    console.warn("WS → non connecté, commande ignorée");
    return;
  }

  ws.send(JSON.stringify(obj));
}

// -------------------------------------------------------------
// Commandes simples (exemples)
// -------------------------------------------------------------
function ping() {
  sendCommand({ cmd: "ping" });
}

function wifiToggle(enable) {
  sendCommand({ cmd: "wifi", enable });
}

function sendCAN(id, data) {
  sendCommand({ cmd: "send_can", id, data });
}

// -------------------------------------------------------------
// Démarrage automatique
// -------------------------------------------------------------
window.addEventListener("load", () => {
  wsConnect();
});
