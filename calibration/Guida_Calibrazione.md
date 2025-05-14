# 📏 Guida alla Calibrazione delle Celle di Carico

Questa guida ti spiega passo-passo come calibrare correttamente ciascuna delle tre celle di carico (Thrust, Torque Left, Torque Right) del banco test, utilizzando il file `cell_calibration.ino` fornito nella cartella `calibration/` della repository.

---

## 🧰 Cosa serve

* Banco test con le tre celle correttamente collegate
* Arduino Uno collegato al computer via USB
* Un peso noto (es. 1000 g)
* Arduino IDE con il file `cell_calibration.ino`

---

## 🧪 Come funziona la calibrazione

Il sensore HX711 misura un segnale analogico e lo trasforma in un valore numerico. Per ottenere la massa reale in grammi, dobbiamo **moltiplicare questo valore grezzo per un "Calibration Factor"**.

Più il Calibration Factor è alto, **più piccolo sarà il peso misurato**. Più è basso (o negativo), **più grande sarà il peso misurato**.

Quindi, per calibrare, andiamo a regolare il fattore fino a ottenere il valore corretto.

---

## ⚙️ Istruzioni passo passo

1. Apri il file `cell_calibration.ino` in Arduino IDE.
2. Scegli quale cella vuoi calibrare modificando il parametro `cellSelect`:

```cpp
const int cellSelect = 0;  // 0 = THRUST, 1 = LEFT, 2 = RIGHT
```

3. Imposta il `calibrationFactor` iniziale. Puoi partire con il valore noto:

```cpp
const float calibrationFactor = -213499.00;
```

(Sostituisci con il valore iniziale della cella scelta se lo conosci)

4. Carica il programma su Arduino

5. Apri il **Monitor Seriale** (baudrate 9600)

6. Attendi che venga completata la tara. Ti verrà mostrato:

```
Offset medio impostato: XXXXX
Taratura completata.
Posiziona il peso noto sulla cella per verificare la calibrazione.
```

7. Ora **posiziona il peso noto** sulla cella

8. Leggi i valori che appaiono nel monitor seriale, ad esempio:

```
Peso misurato: 933.2 g
```

9. Se il valore è diverso da quello reale, calcola il nuovo calibration factor:

```
NuovoFactor = AttualeFactor × (peso_noto / peso_misurato)
```

🔧 Esempio:

* Peso noto = 1160 g
* Peso misurato = 933.2 g
* Fattore attuale = -213499.00

```
NuovoFactor = -213499 × (1160 / 933.2) = -265638.42
```

10. Inserisci il nuovo valore in `calibrationFactor` e ripeti il processo finché il peso misurato non è corretto.

---

## 💾 Valori finali salvati (maggio 2025)

* THRUST: `-213499.00`
* TORQUE LEFT: `330000.00`
* TORQUE RIGHT: `-346708.00`

---

## 📂 Dove si trova il file

`/calibration/cell_calibration.ino`

---

## ❗ Attenzione

* Assicurati che **nessun peso** sia presente durante la tara
* Non muovere la struttura durante la misurazione
* Posiziona il peso **sempre nello stesso punto** al centro della cella
* Ricordati di rimuovere il peso dopo la calibrazione

---

Hai completato la calibrazione! Ora puoi usare i valori ottenuti nel file `thrust_stand_may2025.ino` nel blocco dei `CALIB_FACTOR_*`.

Buoni test!
