Popis:

    Vedoucí: Václav Šimek (simekv@fit.vutbr.cz)

    S využitím libovolného vývojového kitu na bázi SoC ESP32 od společnosti Espressif a rozšiřujícího modulu Analog RGB LED Shield realizujte jednoduché vestavěné zařízení pro zobrazení běžícího textu a grafických efektů.

    Upřesnění požadavků na implementaci:

    S využitím sady zapůjčeného vybavení a aplikačních rámců ESP-IDF realizujte vestavěný systém, jehož účelem bude zobrazovat s využitím rozšiřujícího modulu Analog RGB LED Shield krátké zprávy formou tzv. běžícího textu, který může být vhodně doplněn grafickými efekty.

    Rozšiřující modul Analog RGB LED Shield má formát Arduino UNO, což umožňuje jeho přímé nasazení na vývojovou desku Wemos D1 R32 s obvodem SoC ESP32. Rozšiřující modul je vybaven sadou analogových RGB LED diod , které jsou uspořádány do podoby maticového displeje v konfiguraci 16 sloupců a 8 řádků.

    Zobrazování na maticovém displeji bude řešeno pomocí tzv. časového multiplexingu, kdy je běžící text zobrazován postupně po jednotlivých sloupcích zprava doleva. V daném časovém okamžiku bude tedy v matici aktivně pracováno pouze s jedním sloupcem, jehož řádky (pixely) se rozsvítí jen na krátký okamžik. Díky režimu časového multiplexu pak vzniká výsledný dojem plynulého zobrazování informace.

    Nahlédněte do dokumentace [3], jakým způsobem se displej na obvodové ovládá. Pomocí dekodéru 4-na-16 s typovým označením 74HCT154 a jeho adresových vodičů řízených GPIO piny SoC obvodu ESP32 nejprve dojde k výběru konkrétního sloupce, do něhož bude přes P-kanálový MOSFET tranzistor sepnuto napájecí napětí.

    Následně obsah jednotlivých řádků v aktivovaném sloupci zvoleném výše popisovaným mechanismem, tj. podobu zobrazované informace, následně řídí PWM řadič TLC5947 prostřednictvím svých 24 kanálů, které jsou připojeny k barevným složkám jednotlivých LED diod. Tedy ve sloupci máme vždy 8 RGB LED diod a každá má 3 barevné složky, což nám dá  právě výsledných 24 kanálů. 

    Intenzita svitu či míra zastoupení každé barevné složky od jednotlivých RGB LED diod ve výsledné kombinaci je řízena s 12-bitovou přesností (hodnoty 0 až 4095). Tím pádem získáme bitový řetězec o výsledné délce 288 bitů, což odpovídá právě posuvnému registru, kterým je vybaven obvod TLC5947. Do tohoto registru jsou data ze strany SoC ESP32 zapisována přes rozhraní SPI, přičemž maximální rychlost komunikace by neměla přesáhnout 30 MHz. 

    Výsledné řešení by tedy mělo zobrazovat formou textu běžícího zprava doleva krátké textové zprávy (minimálně 5), které bude možno přepínat libovolným ze čtyř tlačítek, která jsou součástí zobrazovacího modulu. Další tlačítka mohou být využita např. k nastavení intenzity svitu displeje, rychlosti posunu textu či jiné vhodné akce dle vlastního uvážení. Pro ovládání výsledného zařízení kromě tlačítek taktéž možno použít i WiFi či BLE komunikaci, kterou nám SoC ESP32 nabízí.

    Zápůjčka potřebného vybavení k řešení projektu:

    Ohledně termínu zahájení zápůjček vybavení a jejich organizaci budete s dostatečným časovým předstihem informování prostřednictvím hromadné zprávy od vedoucího projektu.

    Zájemci/kyně o řešení projektu na jiném technickém vybavení (např. FITkit 1.2/3.0, Arduino, Zynq; toto vybavení je nutno opatřit si bez pomoci vedoucího, např. v knihovně FIT) zkonzultují toto vybavení s vedoucím a řešit začnou až s jeho souhlasem. Nemáte-li odsouhlaseno jiné technické vybavení, předpokládá se řešení na platformě ESP32, kterou si za tímto účelem včas zapůjčíte u vedoucího projektu.

    Dokumentace, odevzdání a hodnocení projektu:

        Vytvořte přehlednou dokumentaci k přípravě, způsobu realizace, k funkčnosti a vlastnostem řešení projektu.
        Řešení (projekt, bez binárních souborů sestavitelných na základě zdrojových souborů v projektu, a dokumentaci ve zdrojové i binární, tj. PDF, podobě) odevzdávejte prostřednictvím IS v jediném ZIP archívu pojmenovaném dle vašeho loginu (např. xlogin00.zip).
        Předvedení řešení se implicitně předpokládá distanční formou, kdy vytvoříte krátké demonstrační video prokazující vlastnosti výsledného řešení. Video nahrajete na vhodné uložiště (např. fakultní Next Cloud, Google Disk a podobně) a to ideálně s přístupem nevyžadujícím zadání hesla či požádání o udělení přístupu.
        V individuálních případech hodných zvláštního zřetele je možno se domluvit na osobní prezentaci výsledků řešení projektu. V tomto případě se předpokládá jeho kompletní odevzdání (tj. dokumentace, zdrojové kódy a další relevantní materiály) před samotnou obhajobou.
        Souhrnné hodnocení projektu bude provedeno na základě zohlednění několika dílčích složek: dokumentace (až 4 body), funkčnost (až 5 bodů), prezentace (až 1 bod) a celková kvalita (až 4 body). V případě mimořádně kvalitního řešení může být přistoupeno k udělení bonusových bodů (nenároková složka).

    Podpůrné materiály k řešení projektu:

        Schéma zapojení desky Wemos D1 R32
        Podrobný manuál k desce Wemos D1 R32
        Schéma zapojení modulu Analog RGB LED Shield
        Technická dokumentace k obvodu 74HCT154
        Technická dokumentace k obvodu TLC5947
        Vzorový projekt pro otestování modulu Analog RGB LED Shield
