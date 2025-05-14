# ✈️ Thrust Test Stand - May 2025

![LiftUp Logo](./logo_liftup.png)

## 📖 Panoramica Generale

Il **Thrust Test Stand** è un dispositivo di misura di precisione progettato per testare ed analizzare le prestazioni di motori ed eliche impiegati in veicoli aerei. Sviluppato come parte del **Progetto LiftUp** presso l'**Università di Padova**, questo banco prova consente a studenti e ricercatori di raccogliere dati fondamentali come **spinta**, **coppia**, **tensione**, **corrente** e **potenza**, ottimizzando così i sistemi di propulsione.

Questo strumento è essenziale per l'ingegneria aeronautica sperimentale, in particolare per progetti condotti da studenti che vogliono progettare e testare veicoli aerei innovativi. Fornisce una piattaforma pratica per apprendere e applicare principi di aerodinamica e ingegneria della propulsione.

---

## ✨ Caratteristiche Principali

* **Sistema di sicurezza a doppio pulsante**:

  * Un pulsante arma/disarma il sistema
  * L'altro avvia il test
* **Misure in tempo reale**:

  * Spinta (grammi)
  * Coppia (Nm)
  * Tensione (V)
  * Corrente (A)
  * Potenza (W)
* **Test parametrico configurabile**:

  * Percentuale di avvio, fine, passo, durata dello step, tempo di stabilizzazione
* **Limite di corrente automatico**:

  * ⚠️ Se la corrente media supera i **38 A**, il test viene automaticamente interrotto per motivi di sicurezza. **Non modificare questo valore!** Questi 38 A si riferiscono alla media dello step precedente: ciò significa che nel momento in cui questa soglia viene superata, il sistema sta probabilmente già assorbendo **tra i 43 e i 45 A**. Questo valore si avvicina pericolosamente al limite massimo della scheda, che è di **50 A**. Superare tale soglia potrebbe causare **danni irreversibili al sensore di corrente (ACS758)** o all'intera PCB.
  * La PCB è dimensionata per correnti fino a **50 A**
* **LED di stato**:

  * Verde = sistema disarmato (SAFE)
  * Rosso = sistema armato (ATTENZIONE)

---

## ⚙️ Come Funziona

Il banco test avvia il motore tramite segnali PWM inviati all'ESC. Le celle di carico misurano la spinta e la coppia generata. Il sensore INA226 misura la tensione del sistema, mentre l'ACS758 misura la corrente assorbita. Tutti i dati vengono elaborati e stampati in tempo reale sul monitor seriale durante il test.

Il test procede in **step configurabili**, es. da 10% a 100% di PWM in passi da 10%. A ogni step:

* Il motore gira per 5 secondi
* Vengono raccolti i dati
* ⚠️ Se la corrente media supera i **38 A**, il test viene automaticamente interrotto. **Non modificare questo valore!** Questo limite è stato scelto perché i 38 A si riferiscono alla media dello step precedente. In pratica, mentre si raggiunge questa soglia, il sistema può già assorbire **tra i 43 e i 45 A** nello step successivo, avvicinandosi pericolosamente al **limite massimo di 50 A** imposto dal sensore di corrente ACS758 e dall’elettronica della PCB. Superare questo valore potrebbe **danneggiare permanentemente** il sensore o la scheda. Quindi il valore di 38 A rappresenta una soglia preventiva critica da rispettare.

Durante il test, i LED lampeggiano alternativamente per segnalare pericolo.

---

## 🧰 Hardware e Pinout

### Componenti principali:

* **Arduino Uno**
* **ESC** con segnale PWM (pin 11)
* **Motore brushless** con elica
* **3 celle di carico** con moduli HX711:

  * Torque Left (pin DT 3, SCK 2)
  * Torque Right (pin DT 7, SCK 6)
  * Thrust (pin DT 5, SCK 4)
* **ACS758 (50A)** per la corrente su pin A0
* **INA226** via I2C per la tensione
* **LED SAFE (verde)** su pin 10
* **LED ARMED (rosso)** su pin 8
* **Pulsante ARM** su pin 9
* **Pulsante START TEST** su pin 12

---

## 🧪 Guida all'Uso

### 🛠️ Setup di Montaggio

1. Posizionare il banco prova al **bordo di un tavolo**.
2. Bloccare le estremità laterali della base con due **clamp robusti** (tipo a pappagallo).
3. Collegare le celle di carico ai rispettivi connettori sulla PCB:

   * Left → "Load Cell Left"
   * Right → "Load Cell Right"
   * Thrust → "Load Cell Thrust"
   * ⚠️ La **freccia sul connettore** deve combaciare con il **pallino nero** serigrafato sulla scheda.
4. Collegare il connettore ESC alla PCB:

   * Il **cavo nero dell’ESC** deve combaciare con il **pallino nero** sul connettore.
5. Collegare l’ESC al motore, poi collegare la batteria tramite il connettore JST.
6. Collegare Arduino al computer tramite cavo USB.

---

## 🧮 Guida alla Calibrazione //se necessaria (vedi cartella calibrazione)

Per calibrare le celle di carico, utilizza lo sketch separato `cell_calibration.ino`:

1. Imposta `cellSelect` a:

   * `0` per la Thrust
   * `1` per Torque Left
   * `2` per Torque Right
2. Posiziona un peso noto (es. **1000g**) sulla cella
3. Apri il **Monitor Seriale** e leggi il valore misurato
4. Calcola il nuovo Calibration Factor:

```
NuovoCalib = CalibrazioneAttuale * (peso_noto / peso_misurato)
```

5. Inserisci il nuovo valore in `calibrationFactor`, ricarica lo sketch, ripeti se necessario.

⚠️ Ogni cella ha il suo fattore calibrato salvato:

* LEFT: 330000.00
* RIGHT: -346708.00
* THRUST: -213499.00

---

## ▶️ Avvio del Test

1. **Controlla**:

   * che tutti i cavi siano ben collegati
   * che l'elica sia **ben fissata con dado autobloccante**
   * che il LED verde sia acceso (sistema disarmato)
   * che sul monitor seriale di arduino appaia **"Baseline completata"**

2. Premi il **pulsante superiore (pin 9)** per **armare** il sistema:

   * Il LED rosso si accende indicando che il sistema è armato, quindi pericoloso

3. Premi il **pulsante inferiore (pin 12)** per **avviare il test**:

   * Vedrai sul monitor seriale di arduino "Taratura celle..." → "Avvio test..."
   * Il motore inizierà a girare per step
   * Verranno stampati sul monitor seriale:

     * PWM
     * Spinta
     * Coppia
     * Corrente, Tensione, Potenza

4. Il test si interrompe automaticamente se:

   * Raggiunge la fine degli step
   * La **corrente media** supera i **38 A** *(questo valore ****non deve essere modificato****)*.

     > ⚠️ I 38 A si riferiscono alla media dello step precedente: se il valore medio raggiunge questa soglia, significa che in realtà il sistema potrebbe già stare assorbendo **tra i 43 e i 45 A** nel momento successivo. Questo si avvicina pericolosamente al limite di sicurezza della scheda, che è di **50 A massimi** per l'ACS758 e per la PCB. Superarlo potrebbe causare **danni irreversibili** alla scheda e al sensore di corrente. La **corrente media** supera i **38 A (da non modificare)**

5. A fine test:

   * LED torna verde
   * Il sistema è disarmato
   * È sicuro avvicinarsi

6. **Salva i dati** copiandoli manualmente dal Monitor Seriale in un file `.txt`

7. **Smonta** tutto con attenzione e **svita l’elica**

---

## 📚 Librerie Richieste

Assicurati di installare le seguenti librerie Arduino:

* `Servo.h` (inclusa di default)
* `HX711.h` per le celle di carico
* `INA226_WE.h` per il sensore INA226

---

## 📤 Formato Output Seriale

Esempio riga stampata sul monitor seriale:

```
PWM 30% → THRUST: -1234g | TORQUE: 0.0123 Nm | VOLT: 11.72V | CURR: 12.45A | POWER: 145.97W
```

Ogni valore rappresenta:

* **Spinta**: media in grammi (5 secondi)
* **Coppia**: differenza (Left - Right) × distanza (Nm)
* **Corrente**: da ACS758 calibrato
* **Tensione**: da INA226

---

## ⚠️ Note di Sicurezza

* Eseguire i test **solo con stand fissato al tavolo**
* Non toccare il motore o l'elica finché il LED è rosso
* Tenere cavi lontani dall’elica
* Testare inizialmente **senza elica** per sicurezza
* Scollegare **batteria e USB** prima di riporre il sistema

---

## 🗂️ Struttura della Repository

```
lift-up-thrust-stand/
├── README.md
├── firmware/
│   ├── Guida_Firmware.md
│   └── thrust_stand_may2025.ino
├── calibration/
│   ├── Guida_Calibrazione.md
│   └── cell_calibration.ino
├── hardware/
│   ├── Guida_Hardware.md
│   ├── gerber_pcb_liftup.zip
│   ├── PDF_PCB_liftup.pdf
│   └── immagini/
│       ├── pcb_fronte.jpg
│       ├── banco_laterale.jpg
│       └── setup_completo.jpg
└── 
```

---

## 🙌 Credits

Sviluppato da **Claudio Di Bartolo** nell’ambito del **Progetto LiftUp** presso l’**Università di Padova**.

Contatti: [claudiodb42@gmail.com](mailto:claudiodb42@gmail.com) – @Klaus\_00
