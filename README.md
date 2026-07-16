# Antivirus (C)

Sıfırdan C ile yazılan, katmanlı tespit mimarisine sahip bir antivirüs tarama motoru. Bellek güvenliği (valgrind ile doğrulanmış), gerçek dünya hata senaryolarına (izin hataları, symlink döngüleri) karşı dayanıklılık ve performans göz önünde bulundurularak geliştiriliyor.

## Özellikler

- **SHA256 tabanlı imza tespiti** — OpenSSL EVP arayüzü ile chunk-based (stream) hashleme, büyük dosyalarda düşük bellek kullanımı
- **Hash table imza veritabanı** — O(1)'e yakın arama süresi, chaining ile collision yönetimi
- **Aho-Corasick byte-pattern taraması** — binlerce pattern'i tek bir dosya geçişinde aynı anda arayabilen, failure-link tabanlı arama motoru
- **Çift katmanlı tarama** — önce hızlı hash kontrolü, eşleşme yoksa pattern taraması (performans optimizasyonu)
- **Recursive dizin tarayıcı** — izin hatalarını ve symlink döngülerini güvenli şekilde atlayan, çökmeyen tarama mantığı
- **Esnek CLI** — `getopt_long` ile kısa/uzun flag desteği (`-v`/`--verbose`, `-q`/`--quiet` vb.)
- **Bellek güvenliği** — tüm modüller valgrind `--leak-check=full` ile sıfır leak, sıfır hata olarak doğrulandı

## Proje Yapısı

```
antivirus/
├── Makefile
├── README.md
├── signatures.csv              # SHA256 imza veritabanı (hash,malware_adi)
├── patterns.csv                # Byte-pattern veritabanı (hex_pattern,malware_adi)
├── include/
│   ├── hasher.h
│   ├── hash_table.h
│   ├── trie.h
│   ├── aho_corasick_search.h   # Aho-Corasick arama fonksiyonu ve match_list_t
│   ├── scanner.h
│   ├── stats.h                 # scan_stats_t tanımı
│   ├── cli.h
│   └── utils.h
├── src/
│   ├── main.c
│   ├── hasher.c                 # SHA256 hesaplama (OpenSSL EVP)
│   ├── hash_table.c             # İmza veritabanı (hash table + chaining)
│   ├── trie.c                   # Aho-Corasick trie (insert, failure link, free)
│   ├── aho_corasick_search.c    # Trie üzerinde dosya tarama / eşleşme bulma
│   ├── scanner.c                # Recursive dizin tarayıcı
│   ├── cli.c                    # Komut satırı argüman parsing
│   └── utils.c                  # Ortak hex/byte yardımcı fonksiyonları
└── obj/                         # Derleme çıktıları (otomatik oluşur)
```

## Gereksinimler

- GCC (veya uyumlu bir C derleyicisi)
- OpenSSL geliştirme kütüphaneleri (`libssl-dev`)

```bash
sudo apt install build-essential libssl-dev
```

## Kurulum

```bash
git clone <repo-url>
cd antivirus
make
```

## Kullanım

```bash
./antivirus -d <taranacak_dizin> -s <imza_dosyasi.csv> [secenekler]
```

### Zorunlu argümanlar

| Flag | Uzun form | Açıklama |
|------|-----------|----------|
| `-d` | `--dir` | Taranacak dizin |
| `-s` | `--sigdb` | SHA256 imza veritabanı (CSV) |

### Opsiyonel argümanlar

| Flag | Uzun form | Açıklama |
|------|-----------|----------|
| `-p` | `--patterns` | Byte-pattern veritabanı (CSV) — verilmezse sadece hash taraması yapılır |
| `-v` | `--verbose` | Temiz dosyalar için de çıktı göster |
| `-q` | `--quiet` | Sadece tehdit ve özet çıktısı göster |
| `-h` | `--help` | Yardım mesajını göster |

### Örnekler

```bash
# Sadece hash tabanlı tarama
./antivirus -d /home/user/indirilenler -s signatures.csv

# Hash + pattern taraması, detaylı çıktı ile
./antivirus -d /home/user/indirilenler -s signatures.csv -p patterns.csv -v

# Sessiz mod, sadece tehdit ve özet
./antivirus -d /home/user/indirilenler -s signatures.csv -q
```

## Veritabanı Formatları

**signatures.csv** (SHA256 hash tabanlı):
```
a3f5b2c19d8e7f0123456789abcdef0123456789abcdef0123456789abcdef0,Trojan.Generic
```

**patterns.csv** (hex-encoded byte pattern tabanlı):
```
736865,Trojan.A
```
> Pattern'ler hex string olarak yazılır (`"she"` → `736865`), çünkü ham byte dizileri yazdırılamayan karakterler içerebilir.

## Mimari Notlar

### Neden hash table + trie birlikte?

Tam dosya hash'i (SHA256) bilinen dosyaları **kesin** ve **hızlı** tespit eder, ama tek bir byte değişikliğinde tamamen farklı bir hash üretir — polymorphic malware'e karşı zayıftır. Aho-Corasick byte-pattern taraması ise dosya içindeki **belirli imza parçalarını** arar, dosyanın geri kalanı değişse bile tespit sağlayabilir. İki katman birlikte, hız ve dayanıklılık arasında bir denge kurar.

### Neden Aho-Corasick, naive string arama değil?

Yüzlerce/binlerce pattern'i tek tek `strstr` ile aramak, dosya boyutu × pattern sayısı ile orantılı bir maliyet çıkarır. Aho-Corasick, tüm pattern'leri bir trie + failure-link yapısında birleştirerek, dosyayı **tek bir geçişte** okuyup tüm pattern'leri aynı anda arar — karmaşıklık pattern sayısından bağımsızlaşır.

### Güvenlik ve sağlamlık pratikleri

- Tüm boyut/sayaç değişkenleri `size_t` (işaretsiz, overflow-güvenli)
- `strncpy` + manuel null-terminator (buffer overflow koruması)
- `snprintf` ile yol birleştirme (yol taşması koruması)
- Kütüphane fonksiyonları `exit()` çağırmaz, hata kodu döner — çağıran karar verir (tek bir hatalı dosya tüm taramayı durdurmaz)
- `lstat` kullanımı ile symlink'ler takip edilmez, sonsuz döngü riski ortadan kaldırılır
- `errno` sıfırlama disiplini ile `readdir` gibi fonksiyonlarda "hata mı, normal bitiş mi" ayrımı netleştirilir

## Test

Bellek güvenliği valgrind ile doğrulanmalıdır:

```bash
make test
valgrind --leak-check=full ./test_runner
```

Beklenen çıktı: `All heap blocks were freed -- no leaks are possible`, `0 errors`.

## Roadmap

- [x] **Aşama 1** — SHA256 tabanlı signature scanner
- [x] **Aşama 2** — Aho-Corasick byte-pattern taraması
- [ ] **Aşama 3** — PE/ELF dosya formatı analizi (header/section parsing, entropy hesaplama)
- [ ] **Aşama 4** — Heuristik/statik davranış analizi (kural tabanlı puanlama motoru)
- [ ] **Aşama 5** — Gerçek zamanlı izleme (`inotify` ile dosya sistemi olayları)
- [ ] **Aşama 6** — Karantina ve loglama sistemi
- [ ] **Aşama 7** — Sandbox/emülasyon 
## Lisans

Bu proje eğitim amaçlı geliştirilmektedir.
