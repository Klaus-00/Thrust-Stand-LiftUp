# 📖 Guida Utente - Thrust Test Stand May 2025

## Introduzione

Questa guida spiega passo dopo passo come utilizzare correttamente il **Thrust Test Stand** sviluppato per il progetto LiftUp all’Università di Padova. Segui attentamente ogni sezione per eseguire test in modo sicuro, efficiente e preciso.

---

## 1. Preparazione dello Stand

### Posizionamento fisico

1. Posiziona il banco prova al **bordo di un tavolo robusto**.
2. Blocca saldamente entrambe le estremità della base dello stand con **clamp a pressione (tipo pappagallo)**.
3. Assicurati che lo stand sia perfettamente stabile prima di continuare.

### Collegamento delle celle

1. Collega le celle di carico ai corrispondenti connettori sulla PCB:

   * `Left` → connettore **"Load Cell Left"**
   * `Right` → connettore **"Load Cell Right"**
   * `Thrust` → connettore **"Load Cell Thrust"**
2. ⚠️ Ogni connettore ha una **freccia bianca**: questa deve coincidere con il **pallino nero** serigrafato sulla scheda.

### Collegamento ESC e alimentazione

1. Collega il connettore a 3 pin dell’ESC nel connettore segnato "ESC" sulla PCB.

   * ⚠️ Il **filo nero dell'ESC** deve essere orientato verso il **pallino nero** del connettore sulla scheda.
2. Collega l'ESC al motore.
3. Collega la batteria LiPo (es. 3S) al cavo JST di alimentazione.
4. Collega Arduino al PC tramite cavo USB.

---

## 2. Verifica e Setup iniziale

1. Dopo aver alimentato il sistema, apri l'Arduino IDE e apri il **Monitor Seriale**.
2. Attendi il messaggio:

   ```
   Baseline completata.
   ```

   Questo indica che la calibrazione della corrente a riposo è andata a buon fine.
3. Assicurati che il **LED verde** sia acceso (sistema disarmato).

---

## 3. Avvio del Test

### 1. Armare il sistema

* Premi il **pulsante superiore (pin 9)**.
* Il **LED rosso** si accende: il sistema è pronto e pericoloso.

### 2. Avviare il test

* Premi il **pulsante inferiore (pin 12)**.
* Compariranno sul monitor seriale:

  * `Taratura celle...`
  * `Avvio test...`
* Il motore inizierà a girare gradualmente.

### 3. Durante il test

* Il test procede a step: es. 10%, 20%, ..., fino a 100%
* A ogni step:

  * Il motore gira per **5 secondi**
  * I LED lampeggiano alternativamente per indicare pericolo
  * I dati vengono mostrati in formato:

    ```
    PWM 30% → THRUST: -1234g | TORQUE: 0.0123 Nm | VOLT: 11.72V | CURR: 12.45A | POWER: 145.97W
    ```

### 4. Interruzione automatica

* Se la **corrente media** supera i **38 A**, il test viene **fermato automaticamente**.

  * Questo valore non deve essere modificato, è stato impostato per sicurezza.

### 5. Fine del test

* Alla fine degli step o in caso di interruzione:

  * Il motore si ferma
  * Il LED verde si riaccende
  * Il sistema torna disarmato

---

## 4. Salvataggio dei dati

* Copia tutto il contenuto del **monitor seriale** durante o dopo il test
* Salvalo in un file `.txt` con data e nome del test

---

## 5. Smontaggio

1. Scollega la **batteria**.
2. Scollega il **cavo USB**.
3. Scollega tutte le celle e l'ESC.
4. Svita l’elica dal motore per sicurezza.
5. Riponi il tutto in modo ordinato nella scatola.

---

## 6. Sicurezza

* Non lasciare mai il sistema acceso incustodito.
* Non avvicinare mani o cavi all’elica durante il test.
* Controlla sempre che la spinta sia in direzione libera da ostacoli.
* Tieni lontano ogni oggetto leggero (può essere risucchiato o spinto).

---

## 7. Supporto

In caso di dubbi, contatta lo sviluppatore: **Claudio Di Bartolo**

* Email: [claudiodb42@gmail.com](mailto:claudiodb42@gmail.com)
* Instagram: @Klaus\_00
