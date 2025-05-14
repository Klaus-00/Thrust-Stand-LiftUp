# 📝 Spiegazione Dettagliata del Codice - Thrust Stand May 2025

Questo documento fornisce una spiegazione approfondita e commentata del funzionamento del file `thrust_stand_may2025.ino`, presente nella cartella `firmware/` della repository. Lo scopo è aiutare qualsiasi sviluppatore o studente a comprendere e modificare il codice con consapevolezza.

---

## ⚖️ Scopo del Codice

Il firmware permette di:

* Controllare un motore tramite un ESC (Electronic Speed Controller);
* Rilevare spinta, coppia, tensione, corrente e potenza;
* Effettuare test ciclici configurabili in termini di step;
* Interrompere il test se la corrente media supera 38 A (per protezione);
* Stampare tutti i dati in tempo reale sul monitor seriale.

---

## ⚙️ Inizializzazione e Setup

### Librerie incluse:

* `Servo.h`: per controllare l'ESC;
* `HX711.h`: per leggere i valori dalle celle di carico;
* `Wire.h`: per comunicare via I2C con INA226;
* `INA226_WE.h`: per misurare la tensione tramite INA226.

### Pin definiti:

Tutti i pin utilizzati sono dichiarati con `#define` per facilitare le modifiche future. I nomi sono parlanti:

* LED, pulsanti, celle di carico (LEFT, RIGHT, THRUST), ESC, ACS758 (corrente), INA226 (tensione).

### Costanti:

* Fattori di calibrazione per le celle;
* Parametri test: PWM start, end, step, tempo di misura e stabilizzazione;
* Limite di corrente a 38 A;
* Conversioni e fattori empirici per la lettura del sensore ACS758 e del INA226.

---

## 📄 Funzione `setup()`

* Inizializza la comunicazione seriale e tutti i pin;
* Attende l'inizializzazione dell'ESC con segnale di 1000 µs;
* Inizializza le tre celle di carico con la funzione `configureScale()`;
* Esegue la calibrazione a vuoto del sensore ACS758 con `calibrateBaseline()`;
* Inizializza il sensore INA226.

---

## 📊 Funzione `loop()`

1. Legge i due pulsanti:

   * `ARM_BUTTON_PIN`: arma o disarma il sistema
   * `START_BUTTON_PIN`: avvia il test se armato, altrimenti lampeggia il LED rosso

2. Se `is_test_running` diventa `true`, chiama la funzione `runTest()` e alla fine disarma il sistema.

---

## 🔄 Funzione `configureScale()`

Inizializza una singola cella di carico:

* Chiama `scale.begin()` con i pin;
* Aspetta che la cella sia pronta;
* Applica il fattore di calibrazione;
* Esegue la tara (azzeramento).

---

## ✏️ Funzione `calibrateBaseline()`

Calcola la tensione a riposo del sensore ACS758:

* Esegue 50 letture analogiche su `A0`;
* Media i valori e li converte in Volt;
* Questo valore serve come riferimento (0 A).

---

## 🔒 Funzioni `armSystem()` e `disarmSystem()`

* Attivano o disattivano i LED;
* Cambiano il flag `is_armed`;
* Stampano stato sul monitor seriale.

---

## ✔️ Funzione `tareBalances()`

Tenta di effettuare la tara per tutte e 3 le celle:

* Riprova fino a 10 volte (ogni 500 ms);
* Restituisce `true` se tutte le celle sono pronte.

---

## 🚀 Funzione `runTest()`

È il cuore del sistema.

1. Cicla da `test_start_percent` a `test_end_percent` con passi di `test_step_percent`.

2. Per ogni step:

   * Invia PWM al motore
   * Attende `stabilizationTime`
   * Per `measurementDuration`, raccoglie:

     * Spinta (da cella THRUST)
     * Coppia (differenza tra LEFT e RIGHT moltiplicata per distanza in metri)
     * Corrente (da ACS758, calibrata)
     * Tensione (da INA226, scalata)
     * Potenza (tensione \* corrente)
   * LED lampeggiano alternati (rosso/verde)

3. Se corrente media ≥ 38 A:

   * Il test viene interrotto e segnalato sul monitor seriale

4. A fine test:

   * Viene disattivato il motore
   * LED tornano verdi
   * Sistema si disarma automaticamente

---

## 🔦 Funzione `flashLedFast()`

Serve a far lampeggiare velocemente un LED per segnalare errore di avvio test.

---

## 🔧 Parametri Modificabili nel Codice

Nel file `thrust_stand_may2025.ino`, puoi modificare facilmente:

```cpp
const int test_start_percent = 10;     // Percentuale iniziale (es. 30%)
const int test_step_percent  = 10;     // Incremento (es. 10%)
const int test_end_percent   = 100;    // Percentuale finale (es. 80%)
const unsigned long stabilizationTime = 2000; // Tempo per stabilizzarsi tra uno step e l'altro
const unsigned long measurementDuration = 5000; // Durata di ogni step
```

⚠️ **Non modificare `CURRENT_LIMIT = 38.0`!** Questo valore protegge il sensore ACS758 da danni, poiché in realtà la corrente effettiva può superare i 45 A in quel momento.

---

## 📄 Conclusione

Questo codice è progettato per essere modulare, chiaro e facilmente estendibile. Le misure sono affidabili e protette da controlli di sicurezza in tempo reale. Chiunque voglia effettuare test su sistemi di propulsione elettrici può utilizzarlo come base per le proprie analisi. Il progetto è stato sviluppato da **Claudio Di Bartolo** per il **Progetto LiftUp** dell'**Università di Padova**.
