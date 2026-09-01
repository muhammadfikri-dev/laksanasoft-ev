# 🛒 Panduan Belanja Alat & Bahan Edisi 120 km/jam (Pasar Indonesia)

Dokumen ini merinci daftar belanja komponen berperforma tinggi yang terbukti mampu mencapai kecepatan **120 km/jam**, namun tetap **mudah dibeli di e-commerce lokal Indonesia (Tokopedia, Shopee, Bukalapak, Komunitas EV)**.

---

## 1. Daftar Komponen Kunci untuk Target Kecepatan 120 km/jam

| No | Nama Komponen | Rekomendasi Part 120 km/jam | Tempat Beli & Keyword Pencarian | Estimasi Harga (IDR) |
| :-: | :--- | :--- | :--- | :--- |
| **1** | **Motor Penggerak** | **QS Motor 138 90H V3 Mid-Drive (Gearbox/Belt Type)**<br>*Daya Peak 15kW - 18kW, torsi 72Nm, putaran tinggi hingga 6000 RPM (Paling direkomendasikan untuk 120 km/jam).*<br>*Alternatif:* **QS Motor 273 50H Hub Motor 4000W-8000W.** | Tokopedia / Shopee (Cari: "QS Motor 138 90H" atau "QS Motor 273") | Rp 6.800.000 – Rp 9.500.000 |
| **2** | **Inverter / Controller** | **FarDriver ND72680 / ND72850** atau **Votol EM-200-2 / EM-260**<br>*Support Field Weakening (Flux Weakening) 200A-250A Bus Current, 550A-680A Phase Current.* | Tokopedia / Shopee (Cari: "FarDriver ND72680" atau "Votol EM200") | Rp 2.650.000 – Rp 4.200.000 |
| **3** | **Baterai Pack 72V** | **Baterai 72V 50Ah – 60Ah High Discharge (Cell CATL / CALB / Samsung 50S / Lishen 21700)**<br>*Pastikan continuous discharge rate $\ge 150\text{A}$ dan peak $\ge 250\text{A}$.* | Spesialis Baterai EV Lokal / Tokopedia (Cari: "Baterai Motor Listrik 72V 50Ah") | Rp 7.800.000 – Rp 11.500.000 |
| **4** | **Smart BMS** | **Smart BMS 20S 200A – 250A Peak (Daly / JK BMS / ANT BMS) dengan Active Balancer** | Tokopedia / Shopee (Cari: "Daly Smart BMS 200A 20S") | Rp 850.000 – Rp 1.650.000 |
| **5** | **Pengaman Utama (HV)** | **MCB DC 2-Pole 150A – 200A (Merk: Nader / Chint / FEEO / Schneider DC)** | Toko Listrik / Tokopedia (Cari: "MCB DC 2P 150A" atau "MCB DC 200A") | Rp 110.000 – Rp 220.000 |
| **6** | **Konektor Daya Berat** | **Konektor Anderson SB120 / SB175 atau Dual XT90-S Anti-Spark** | Tokopedia / Shopee (Cari: "Anderson SB120") | Rp 65.000 – Rp 120.000 |
| **7** | **Kabel Tegangan Tinggi** | **Kabel Silikon Tahan Panas 200°C AWG 4 / AWG 2 (25 mm² - 35 mm²)** | Toko Audio Mobil / E-Commerce (Cari: "Kabel Silikon AWG 4") | Rp 65.000 – Rp 95.000 / meter |
| **8** | **Transmisi Rantai** | **Rantai Baja Tipe 428H / 520 O-Ring Heavy Duty + Sprocket Depan 15T & Belakang 45T** | Bengkel Motor / Toko Sparepart Rantai Racing (SSS/DID) | Rp 280.000 – Rp 480.000 |
| **9** | **Pengereman Kecepatan Tinggi** | **Kaliper Rem Depan Double Disc 4-Piston (Nissin / Brembo Clone) + Selang Rem Braided Steel** | Toko Variasi Motor / Tokopedia | Rp 450.000 – Rp 950.000 |
| **10**| **Ban Kecepatan Tinggi** | **Ban Tubeless Speed Rating P / Q (Max 150-160 km/jam) Ukuran 90/80-14 & 110/80-14** *(Pirelli Diablo Rosso Scooter / Michelin City Grip 2 / Maxxis)* | Toko Ban Motor Lokal | Rp 550.000 – Rp 900.000 |
| **11**| **DC-DC Stepdown** | **Modul DC-DC Converter 72V ke 12V 20A Alumunium Casing Tahan Air (240W)** | Tokopedia / Shopee | Rp 85.000 – Rp 150.000 |
| **12**| **Charger Cepat (Fast)** | **Smart Charger 72V 20S Output 84V 10A – 15A Otomatis Cut-off** | Tokopedia / Shopee | Rp 650.000 – Rp 1.200.000 |

---

## 2. Tips Tuning Software Controller untuk Top Speed 120 km/jam

1. **Aktifkan Fitur Flux Weakening (Field Weakening):**
   - Pada aplikasi software FarDriver atau Votol (via PC/Bluetooth), naikkan nilai `Flux Weakening Current` secara bertahap (mulai dari 30A hingga 80A). Fitur ini memperlemah medan magnet rotor sehingga motor mampu berputar melampaui batas *Back-EMF* pada tegangan 72V.
2. **Set Arus Fasa (Phase Current):** Set pada $450\text{A} - 550\text{A}$ untuk akselerasi awal yang responsif.
3. **Set Arus Baterai (Bus Current):** Set pada $150\text{A} - 200\text{A}$ sesuai batas kemampuan debit (*C-rate*) baterai pack Anda.
