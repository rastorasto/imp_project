
Projekt - ESP32 (M)
Požadavky na absolvování
Odevzdávání

Prezentace projektů budou v posledních dvou (možná třech) týdnech semestru. Dříve ne. Pokud chcete již mít hotovo a nechcete riskovat, že  by se mohlo zařízení změnit (rozpojit drátky), aktualizovat knihovny atd, můžete už teď nahrát video cca 1 minuty, kde ukazujete funkčnost vašeho řešení. Toto video nahrejte na nějaký server (youtube, google drive, fit nextcloud, ...) a do dokumentace vložte odkaz. To bude potom stačit pro hodnocení (+ samozřejmě stručná dokumentace a podobně). Bohužel není možné odevzdávat dříve z technických důvodů.


Dostali jste do ruky sáček s potřebnými periferiemi pro řešení projektu. Vaším úkolem vytvoření kódu pro ESP32 bude i udělat jednoduché zapojení. Kity si můžete vyzvedávat po ukončení přihlašování, tj od 5. 10.:

    pondělí v 14:00 - 14:10
    v úterý v 18:15 - 18:30
    ve středu ihned po cvičení končící v 11 a v 15

ESP32

Jedná se o desku programovatelnou např. přes prostředí PlatformIO (doplněk do VS Code). Je možné použít i přímo rozhraní IDF (každým rokem je lepší, není pak nutné pracovat s Platformio). Pokud zvolíte Arduino prostředí, dá se použít i Arduino studio. Práce s procesorem ESP32 bude představena na přednáškách, jednoduchý tutoriál, který jsem natočil pro cvičení TOI a NAV v době lockdownu naleznete zde: https://www.youtube.com/watch?v=v1lCXLQuA9s 

ss

Tutoriál pro rozblikání LED diody naleznete na zde. Další návody hledejte v dokumentaci.
Tipy a FAQ pro spuštění

    V poslední verzi Ubuntu je problém s tím, že se nevytvoří zařízení /dev/ttyUSB0. Problém zjistíte v logování

$ sudo dmesg
[  150.814089] usb 3-4.3: new full-speed USB device number 7 using xhci_hcd
[  150.939368] usb 3-4.3: New USB device found, idVendor=1a86, idProduct=7523, bcdDevice= 2.64
[  150.939372] usb 3-4.3: New USB device strings: Mfr=0, Product=2, SerialNumber=0
[  150.939374] usb 3-4.3: Product: USB Serial
[  150.995402] ch341 3-4.3:1.0: ch341-uart converter detected
[  151.009435] usb 3-4.3: ch341-uart converter now attached to ttyUSB0
[  151.589633] input: BRLTTY 6.4 Linux Screen Driver Keyboard as /devices/virtual/input/input19
[  151.595376] usb 3-4.3: usbfs: interface 0 claimed by ch341 while 'brltty' sets config #1
[  151.598482] ch341-uart ttyUSB0: ch341-uart converter now disconnected from ttyUSB0
[  151.598497] ch341 3-4.3:1.0: device disconnected
[  172.595864] usb 3-4.3: USB disconnect, device number 7
$ sudo apt remove brltty
# po odpojeni a pripojeni
$ sudo dmesg
[  989.189243] usb 1-7: new full-speed USB device number 4 using xhci_hcd
[  989.438445] usb 1-7: New USB device found, idVendor=1a86, idProduct=7523, bcdDevice= 2.64
[  989.438451] usb 1-7: New USB device strings: Mfr=0, Product=2, SerialNumber=0
[  989.438453] usb 1-7: Product: USB Serial
[  989.451486] ch341 1-7:1.0: ch341-uart converter detected
[  989.465559] usb 1-7: ch341-uart converter now attached to ttyUSB0

    Pokud je problém
    Serial port /dev/ttyUSB0
    Connecting........_____....._____....._____....._____....._____.
        podržte při připojování tlačítko (cca na 4 sekundy)
        pokud to nepomůže, spojte pin IO0 (RX0) s GND při připojování

Použité periferie
Breadboard

Abyste nemuseli pájet vaše spoje, v balíčku máte (někteří) vloženou prototypovací destičku. Měla by stačit na všechny úkoly. Propojení pak budete dělat pomocí drátků z UTP kabelu, které musíte na koncích odizolovat. K tomu vám bude stačit obyčejný nůž nebo nůžky. Pozor, ať projekt IMP nezaplatíte krví :-)

Jednotlivé linie nám spojují vodiče. Jak jsou vedené spoje vidíte na žlutých čarách na následujícím obrázku vlevo. Vedle vidíte příklad správného zapojení LED, rezistoru a klávesnice. Klávesnice nikdy nemůže být vložena naopak (otočená o 90° - všechny piny by se zkratovaly)

bread

Vyvarujte se toho, aby součástka měla obě nožičky v jedné plošce - dojde ke zkratu. Také pozor na to, že plošky + a - neobsahují žádné napájen, pokud je tam nepřipojíte. Navíc levé a pravé nejsou propojené. Toto označení je jen pro přehlednost (typicky do + zapojíte 3.3V a do - zapojíte GND)

Klávesnice

Jedná se o maticovou klávesnici. Postupuje se tak, že např sloupce jsou z pohledu MCU výstup, řádky jsou vstup. Poté vždy aktivujete jeden sloupec a podíváte se, zda je některý z řádků aktivován. Díky tomu ušetříte velké množství pinů (7 pinů místo 12, u klávesnice 4x4 je to 8 pinů vs. 16). Je nutné používat pull-up/down rezistory! Datasheet: https://www.farnell.com/datasheets/1662617.pdf 

Teplotní čidlo

Používáte analogové čidlo, tzn. teplota se mění na napětí. Jedná se o čidlo LMT85LPG (s napájením 3.3 V!). Datasheet: https://www.ti.com/lit/ds/symlink/lmt85.pdf

LED diody

LED diody potřebují mít přeřazený 330 ohm rezistor (potřebujeme z napětí 3.3 V udělat pouze 2 V). U RGB LED platí to samé (každý kanál potřebuje vlastní rezistor, úroveň svitu jednotlivých barev regulujte pomocí PWM). https://www.farnell.com/datasheets/2046599.pdf 

Bzučák

Bzučák je kontinuální, takže obsahuje vnitřní oscilátor. Vám tedy stačí pouze regulovat napětí pro rozumnou hlasitost.

Display

Displeje mají dvě varianty. SPI (7 pinů) a I2C (4 piny). Demo použití displeje naleznete zde: https://github.com/nopnop2002/esp-idf-ssd1306

Postup pro rozchození

    Stáhněte složku components do vašeho projektu
    Proveďte build
    V menuconfig proveďte nastavení (viz níže, podle varianty) - ukázka přístupu
       

     
    V main souboru se inspirujte  např. v text demu ( https://github.com/nopnop2002/esp-idf-ssd1306/tree/master/TextDemo )

Display SPI

Požadované nastavení v menuconfigu: 


Vlastní zapojení (doporučené):

VCC - 3.3V; GND - GND; D0 (SCLK) - IO18, D1 (MOSI) - IO23, CS - IO5, DC (DATA/control) - IO27, RESET - IO17


Display I2C

Pro I2C připojení využijeme piny IO16 a IO17. Ze zadní strany uvidíte, že I2C adresa je 0x78 (ale v nastavení ssd1306.h se musí použít 0x3c, protože dále v kódu se shiftuje o 1 pozici).



Sensor teploty a vlhkosti (meteostanice)

Jedná se o I2C čip, jehož dokumentaci naleznete zde https://www.laskakit.cz/senzor-teploty-a-vlhkosti-vzduchu-sht31/ 

Nezapomeňte, že I2C potřebuje pull-up rezistory, pro které můžete využít vestavěné rozhraní (inspirace https://github.com/nopnop2002/esp-idf-ssd1306/blob/master/components/ssd1306/ssd1306_i2c.c )

Sensor gest

Jedná se o I2C čip, jehož dokumentaci naleznete zde https://www.laskakit.cz/arduino-senzor-priblizeni-a-gest-gy-9960llc-apds-9960--i2c/ 

Nezapomeňte, že I2C potřebuje pull-up rezistory, pro které můžete využít vestavěné rozhraní (inspirace https://github.com/nopnop2002/esp-idf-ssd1306/blob/master/components/ssd1306/ssd1306_i2c.c )

Sensor intenzity osvětlení

Jedná se o I2C čip, jehož dokumentaci naleznete zde https://www.laskakit.cz/snimac-intenzity-osvetleni-bh1750/

Nezapomeňte, že I2C potřebuje pull-up rezistory, pro které můžete využít vestavěné rozhraní (inspirace https://github.com/nopnop2002/esp-idf-ssd1306/blob/master/components/ssd1306/ssd1306_i2c.c )

Cílem tohoto projektu postupně (lineárně) rozsvěcovat LED diody podle intenzity osvětlení.
MQTT komunikace
V některých zadání se očekává komunikace přes MQTT protokol. Jedná se o jednoduchý internetový komunikační protokol, kde komunikujete se serverem, tzv. brokerem. Existuje několik volně přístupných, kde jste omezeni jen počtem zpráv. Princip je takový, že jedno zařízení publikuje zprávy (topic, např xlogin00/zarizeniABC/temp1 a jeho hodnotu, která může být číslo, řetězec a podobně) a k serveru se připojí jeden či více klientů, kteří mají přihlášené nějaké téma(ta) (topic).  Takový protokol je pak ideální pro řešení chytré domácnosti - v Home Assistant či NodeRed si můžete napsat rutiny pro zpracování či ukládání hodnot z různých jednoduchých čidel. A stejným způsobem pak můžete vytvořit nějaký akutátor. Tip: máte doma něco "chytrého" a máte projekt s MQTT? Nebojte se to spojit dohromady, uvidíte, že je to jednoduché!



Naposledy změněno: úterý, 14. října 2025, 11.15