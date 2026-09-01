# 📘 Spesifikasi Sistem & Kalkulasi Rekayasa (High Performance Edition - 120 km/jam)

Dokumen ini merinci spesifikasi teknis dan kalkulasi fisika traksi yang telah dimodifikasi secara khusus agar sepeda motor listrik **Laksanasoft-EV** mampu mencapai dan mempertahankan kecepatan puncak **120 km/jam** secara stabil, efisien, dan aman.

---

## 1. Kalkulasi Kebutuhan Daya Traksi untuk Kecepatan 120 km/jam

Untuk melaju pada kecepatan $v = 120\text{ km/jam} = 33.33\text{ m/s}$, hambatan aerodinamika meningkat secara eksponensial (kuadratik terhadap kecepatan):

$$F_{total} = F_{rr} + F_{aero} + F_{accel}$$

### A. Perhitungan Gaya Hambat pada 120 km/jam:
1. **Rolling Resistance ($F_{rr}$):**
   - Bobot Total ($m$): $210\text{ kg}$ (Kendaraan 115 kg + Pengendara 95 kg)
   - $F_{rr} = m \cdot g \cdot C_{rr} = 210 \times 9.81 \times 0.015 = \mathbf{30.9\text{ N}}$

2. **Aerodynamic Drag ($F_{aero}$):**
   - Densitas udara ($\rho$): $1.225\text{ kg/m}^3$
   - Koefisien drag ($C_d$): $0.62$ (posisi tuck-in / menunduk aerodinamis)
   - Luas penampang depan ($A$): $0.68\text{ m}^2$
   - Kecepatan ($v$): $33.33\text{ m/s}$ ($120\text{ km/jam}$)
   $$F_{aero} = \frac{1}{2} \cdot \rho \cdot C_d \cdot A \cdot v^2$$
   $$F_{aero} = 0.5 \times 1.225 \times 0.62 \times 0.68 \times (33.33)^2 = \mathbf{287.0\text{ N}}$$

3. **Total Gaya Traksi Konstan pada 120 km/jam:**
   $$F_{cruise} = F_{rr} + F_{aero} = 30.9 + 287.0 = \mathbf{317.9\text{ N}}$$

4. **Kebutuhan Daya Mekanikal Roda ($P_{wheel}$):**
   $$P_{wheel} = F_{cruise} \times v = 317.9\text{ N} \times 33.33\text{ m/s} = \mathbf{10.595\text{ Watt}} \approx \mathbf{10.6\text{ kW}}$$

5. **Kebutuhan Daya Elektrikal Baterai ($P_{battery}$):**
   - Efisiensi transmisi rantai/belt ($\eta_{trans} \approx 95\%$)
   - Efisiensi motor & inverter FOC ($\eta_{elec} \approx 88\%$)
   - Efisiensi total: $\eta_{total} \approx 0.95 \times 0.88 = 83.6\%$
   $$P_{battery} = \frac{10.6\text{ kW}}{0.836} = \mathbf{12.68\text{ kW}}\text{ (Daya Jelajah Konstan 120 km/jam)}$$
   - **Daya Puncak Akselerasi (Peak Power 0-100 km/jam dalam 5.2 detik):** $\mathbf{15.0\text{ kW} - 18.0\text{ kW}}$.

---

## 2. Spesifikasi Powertrain Edisi 120 km/jam

| Parameter | Spesifikasi Upgrade (120 km/jam) | Keterangan Rekayasa |
| :--- | :--- | :--- |
| **Motor Penggerak** | **QS Motor 138 90H V3 Mid-Drive** (atau QS 273 50H Hub Motor) | Daya Nominal: 4000W – 5000W<br>Daya Puncak: **15.000W – 18.000W (18 kW Peak)** |
| **Torsi Puncak Motor** | **72 Nm** pada shaft motor | Torsi pada roda belakang: **216 Nm** (setelah reduksi gir) |
| **Max RPM Motor** | 4500 RPM (Base) / **6200 RPM (Field Weakening)** | Kecepatan sudut tinggi untuk mencapai 120+ km/jam |
| **Rasio Reduksi Gir** | **1 : 3.0** (Sprocket Depan 15T, Belakang 45T Tipe 428H) | Dikalibrasi khusus untuk top speed 120–125 km/jam |
| **Inverter / Controller** | **FarDriver ND72680 / Votol EM-200-2 / EM-260** | Tegangan: 72V–88V DC<br>Arus Bus: **200A Continuous / 250A Peak**<br>Arus Fasa: **550A – 680A Phase Current** |
| **Teknologi Inverter** | FOC Sine Wave + **Active Flux Weakening (Field Weakening)** | Meningkatkan RPM motor di atas tegangan *Back-EMF* normal |
| **Kapasitas Baterai** | **72V 50Ah – 60Ah (3.6 – 4.3 kWh) High Discharge** | Sel Lithium NMC / LiFePO4 Grade A Discharge Rate $\ge 3\text{C}-5\text{C}$ |
| **Smart BMS** | **Smart BMS 20S 200A Continuous / 300A Peak** | Active Balancer 2A, Antarmuka CAN Bus / Bluetooth |
| **Proteksi HV** | **MCB DC 2P 150A – 200A + XT90-S Anti-Spark** | Mampu menahan lonjakan arus tarikan gas penuh 200A |
| **Ukuran Kabel Daya** | **Kabel Silikon AWG 4 / AWG 2 (25 mm² - 35 mm²)** | Tahan panas 200°C untuk arus fasa dan baterai tinggi |

---

## 3. Kalkulasi RPM & Rasio Gir pada 120 km/jam

- **Ukuran Ban Belakang:** 110/80 - 14 inci ($r_{wheel} = 0.267\text{ m}$, Keliling Ban $K = 2 \times \pi \times 0.267 = 1.677\text{ meter}$).
- **Putaran Roda Belakang pada 120 km/jam:**
  $$RPM_{roda} = \frac{120.000\text{ meter/jam}}{60 \times 1.677\text{ meter}} = \mathbf{1192.6\text{ RPM}}$$
- **Putaran Motor pada Rasio 1 : 3.0:**
  $$RPM_{motor} = 1192.6 \times 3.0 = \mathbf{3578\text{ RPM}}$$
- **Margin Kecepatan Maksimum:**
  Dengan kemampuan *Field Weakening* hingga **5000 - 6000 RPM**, sistem memiliki margin tenaga berlebih untuk menembus **125 - 130 km/jam**.

---

## 4. Mode Berkendara & Profil Kecepatan (Drive Modes)

1. **Mode ECO:** Dibatasi pada $50\text{ km/jam}$ (Torsi $35\%$, Arus maks $35\text{A}$, Jarak tempuh maksimal $110\text{ km}$).
2. **Mode URBAN:** Dibatasi pada $85\text{ km/jam}$ (Torsi $70\%$, Arus maks $90\text{A}$, Performa komuter harian lincah).
3. **Mode SPORT (Unrestricted):** **Top Speed 120 - 125 km/jam** (Torsi $100\%$, Arus maks $200\text{A}-250\text{A}$ Peak, Akselerasi 0-100 km/jam < 5.5 detik).
4. **Mode REVERSE:** Dibatasi pada $5\text{ km/jam}$ demi keselamatan.
