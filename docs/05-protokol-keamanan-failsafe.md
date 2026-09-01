# 🛡️ Protokol Keamanan & Interlock Failsafe (Laksanasoft-EV)

Keselamatan pengendara dan integritas perangkat keras adalah prioritas utama rancang bangun **Laksanasoft-EV**. Sistem ini menerapkan prinsip-prinsip keselamatan fungsional otomotif (terinspirasi dari standar ISO 26262 ASIL-B).

---

## 1. Matriks Penanganan Kondisi Bahaya (Fault Matrix)

| Kode Fault | Kondisi Pemicu | Tingkat Bahaya | Respon Sistem Seketika | Metode Pemulihan |
| :--- | :--- | :--- | :--- | :--- |
| **ERR_01** | Deviasi APPS1 & APPS2 $> 10\%$ selama $> 100\text{ms}$ | 🔴 Kritis | Potong sinyal gas ke Inverter ($0\%$), nyalakan indikator fault | Lepas gas penuh ($0\%$) selama 2 detik |
| **ERR_02** | *Brake Over Accelerator* (Gas & Rem aktif bersamaan) | 🟠 Sedang | Prioritas Rem: Potong output gas ke motor secara instan | Lepas tuas rem |
| **ERR_03** | Standar samping turun saat mode DRIVE | 🟡 Rendah | Putus transmisi torsi motor, beralih ke STANDBY | Lipat standar samping + rem |
| **ERR_04** | Tegangan Baterai $< 60.0\text{V}$ (Low Voltage Cutoff) | 🟠 Sedang | Pembatasan arus baterai hingga $10\text{A}$ (Limp Mode) | Lakukan pengisian daya baterai |
| **ERR_05** | Tegangan Sel Baterai Kritis $< 2.8\text{V}$ (BMS Alert) | 🔴 Kritis | Buka Main Contactor, matikan inverter | Pengisian daya & balancing sel |
| **ERR_06** | Suhu Motor / Inverter $> 95^\circ\text{C}$ | 🔴 Kritis | Pemutusan daya motor (*Thermal Lockout*) | Tunggu suhu mendingin $< 70^\circ\text{C}$ |
| **ERR_07** | Hilang Komunikasi CAN Bus ($> 500\text{ms}$ no heartbeat) | 🔴 Kritis | Inverter otomatis beralih ke *Safe Torque Off (STO)* | Periksa kabel CAN Bus |
| **ERR_08** | *Emergency Kill Switch* Ditekan | 🔴 Kritis | Buka Main Contactor, buang muatan kapasitor | Putar & lepas tombol Emergency |

---

## 2. Algoritma Plausibilitas Akselerator (APPS Plausibility Check)

Untuk mencegah akselerasi liar akibat kabel putus, konektor basah, atau sensor rusak:

```c
// Implementasi Pseudo-Code Plausibilitas Throttle Laksanasoft-EV
bool check_throttle_plausibility(float apps1_volt, float apps2_volt) {
    // Range fisik normal APPS1: 0.8V - 4.2V
    // Range fisik normal APPS2: 4.2V - 0.8V (Inverted)
    
    // 1. Cek Out-of-Bounds (Korsleting ke 5V atau Putus ke GND)
    if (apps1_volt < 0.5f || apps1_volt > 4.5f) return false;
    if (apps2_volt < 0.5f || apps2_volt > 4.5f) return false;
    
    // 2. Hitung persentase bukaan masing-masing sensor
    float p1 = (apps1_volt - 0.8f) / (4.2f - 0.8f) * 100.0f;
    float p2 = (4.2f - apps2_volt) / (4.2f - 0.8f) * 100.0f;
    
    // 3. Batasi pada rentang 0 - 100%
    p1 = constrain(p1, 0.0f, 100.0f);
    p2 = constrain(p2, 0.0f, 100.0f);
    
    // 4. Validasi deviasi antar sensor (Maksimal selisih 10%)
    if (fabs(p1 - p2) > 10.0f) {
        return false; // Terjadi diskrepansi sensor!
    }
    
    return true; // Valid & Aman
}
```

---

## 3. Sirkuit Interlock Perangkat Keras (Hardware Safety Loop)

Selain proteksi berbasis perangkat lunak pada VCU, sistem dilengkapi dengan **Hardware Safety Interlock Loop** yang dirangkai secara seri:

```
[+12V Supply] 
     |
     v
 [EMERGENCY KILL SWITCH] (Tombol Merah di Stang Kanan)
     |
     v
 [BMS ALARM DISCHARGE RELAY] (Dry Contact BMS Pintar)
     |
     v
 [INCLINOMETER / CRASH SENSOR] (Deteksi Motor Terjatuh > 65°)
     |
     v
 [VCU HARDWARE RELAY ENABLE PIN]
     |
     v
 [MAIN CONTACTOR COIL] ---> [GND]
```

> **Keunggulan Sirkuit Seri:**
> Jika salah satu titik keamanan terputus (apakah tombol darurat ditekan, BMS mendeteksi over-discharge sel, atau motor terjatuh), arus ke *coil* kontaktor utama seketika terputus secara fisik tanpa menunggu instruksi software, mengisolasi baterai 72V dari inverter dalam waktu $< 25\text{ milidetik}$.

---

## 4. Prosedur Darurat Pengendara (Emergency Operational Procedures)

1. **Jika Terjadi Akselerasi Tidak Terkendali:**
   - Tarik kedua tuas rem sekuat tenaga (Brake priority akan memotong daya motor).
   - Tekan tombol merah besar **Emergency Kill Switch** di stang kanan.
   - Matikan kunci kontak.
2. **Jika Indikator Baterai Berkedip Merah & Bunyi Beep Panjang:**
   - Kendaraan mengalami *Over-Temperature* atau tegangan sel kritis.
   - Segera tepikan kendaraan ke tempat aman dan matikan sistem selama 15 menit.
