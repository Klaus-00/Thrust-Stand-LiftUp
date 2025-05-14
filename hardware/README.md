# 🔩 Guida Hardware - Thrust Test Stand (Maggio 2025)

Questa guida descrive tutti i componenti necessari per costruire il banco prova **Thrust Stand**, le connessioni corrette sulla PCB, e include gli schemi elettrici in PDF e immagini di riferimento.

---

## 🧰 Componenti Necessari

### 1. **Microcontrollore**

* **Arduino Uno** o compatibile
* Alimentazione via USB

### 2. **Motore + ESC**

* Motore brushless
* ESC compatibile con segnali PWM (es. 1100–2000 µs)
* Collegamento JST per alimentazione

### 3. **Celle di Carico**

* **3 celle** da 10 kg:

  * Torque Left
  * Torque Right
  * Thrust
* Cablate su A+, A-, E+, E- (B+ e B- vengono ignorati)
* Ognuna collegata a un modulo **HX711**

### 4. **Sensori**

* **ACS758-050B** (per corrente, analogico, max 50A)
* **INA226** (tensione via I2C)

### 5. **LED e Pulsanti**

* 1 LED Verde (SAFE - pin 10)
* 1 LED Rosso (ARMED - pin 8)
* Pulsante ARM (pin 9)
* Pulsante START TEST (pin 12)

### 6. **Altro**

* Batteria LiPo (es. 3S 11.1V)
* Morsetti/clamp per fissaggio meccanico
* Cavi JST per alimentazione ESC

---

## 📌 Collegamenti sulla PCB

### 📥 Celle di Carico:

* **LEFT** → DT 3, SCK 2
* **RIGHT** → DT 7, SCK 6
* **THRUST** → DT 5, SCK 4

📌 Ogni connettore ha una **freccia**: deve corrispondere con il **pallino nero** sulla serigrafia della PCB.

### ⚡ ESC:

* Connettore a 3 pin (GND, 5V, PWM)
* Il **cavo nero dell’ESC** deve corrispondere al **pallino nero** sulla scheda (GND)

### 🔋 Alimentazione:

* L’alimentazione motore avviene tramite **connettore JST** rosso

### 🟢 LED:

* **LED Verde** → Pin 10 (SAFE)
* **LED Rosso** → Pin 8 (ARMED)

### 🔘 Pulsanti:

* **Pulsante ARM** → Pin 9
* **Pulsante START TEST** → Pin 12

### 📶 Sensori:

* **ACS758** → A0
* **INA226** → I2C SDA/SCL (Wire.begin)

---

## 🧾 Schema Elettrico PCB

📎 Allegato nella cartella `hardware/` trovi:

* `PCB_PCB_liftup_2.pdf` → Schema della scheda realizzata
* `gerber/` → Cartella contenente i file Gerber per produzione

---

## 🖼️ Immagini di Riferimento

Nella cartella `hardware/immagini/` sono incluse:

* `pcb_fronte.jpg` → Vista ravvicinata della PCB
* `banco_laterale.jpg` → Vista laterale del banco montato
* `setup_completo.jpg` → Vista completa con motore, celle e batteria

Queste immagini aiutano il montaggio e il riconoscimento dei componenti direttamente sulla struttura.

---

📌 Per ulteriori dettagli sul montaggio e l’utilizzo, consulta la **Guida Utente**.
