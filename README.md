

# Rancilio Silvia Upgrade – Version 2


## Überblick

Die **Version 1** hat meine Rancilio Silvia mit der Shot-Timer-Funktion bereits deutlich alltagstauglicher gemacht die grundsätzlichen Einschränkungen einer Single-Boiler-Maschine bleiben aber bestehen.

Um zwischen **Espressobezug** und **Milchaufschäumen** zu wechseln, muss der komplette Boiler jeweils aufgeheizt bzw. wieder abgekühlt werden.  
Das ist unpraktisch und ineffizient, vor allem weil der Boiler dadurch nicht einfach thermisch isoliert werden kann. Sonst würde er nach dem Aufschäumen zu langsam abkühlen.

Außerdem habe ich die Heißwasserfunktion nie wirklich genutzt, da der kleine Boiler sehr schnell leer ist und dadurch der Nutzen begrenzt bleibt.

Ein weiterer Punkt ist die **Zweipunktregelung** der Temperatur. Wie bei der standardmäßigen Rancilio Silvia wird die Heizung über einfache Thermostate geschaltet.  
Wie in **Bild 1** zu sehen, haben wir einen Heizstab, mehrere Schalter und Thermostate. Diese Thermostate öffnen oder schließen den Stromfluss durch den Heizstab, je nachdem, ob ihre jeweilige Schalttemperatur unter- oder überschritten wird.  

Das führt zu einer relativ großen **Temperaturschwankung**.

In diesem Projekt (Version 2) sollen daher folgende Punkte angegangen werden:

- konstantere Brühtemperatur
- mehr Flexibilität zwischen Brühen und Steamen
- zusätzliche Funktionalitäten wie z.B. automatischer Stopp des Bezugs
- eventuell mehr nutzbares Volumen für Heißwasser
  
### Version 2.1 – Funktionsumfang

Als erstes soll in Version 2.1 die **konstante Brühtemperatur** angegangen werden.  
Da in den nächsten Schritten auch der **Steam-Part** grundlegend verändert werden soll, wird in dieser Phase die Dampflanze außer Betrieb genommen und der Abgang des Boilers, der zur Steam-Lanze führt, wird wie in den Bildern zu sehen verschlossen.
<p align="center">
  <img src="https://github.com/user-attachments/assets/815a7586-945b-400d-97c9-028f2bbbf6d5" height="500">
  <img src="https://github.com/user-attachments/assets/35cd0fc0-03ce-4914-986b-1967dddffdd9" height="500">
</p>


Damit die Temperatur dieses einzelnen Boilers später geregelt werden kann, wird in Version 2.1 eine eigene **Steuerplatine** entwickelt und aufgebaut.
Die Platine in Version 2.1 soll die Basis für alle weiteren Ausbaustufen bilden und bringt daher folgende Funktionen mit:

- **Temperaturauslesung:** Bis zu 3 Eingänge für Thermoelement-Sensoren (Thermocouples), um verschiedene Punkte (z.B. Boiler, Brühgruppe, Leitung) messen zu können und damit ist die Hardware bereits auf zukünftige Regelungs-Features vorbereitet.

- **Relais für selten geschaltete Verbraucher:** Mehrere Relais für Komponenten, die nicht ständig ein- und ausgeschaltet werden (z.B. Pumpe, Magnetventile, ggf. Heißwasserfunktionen). Zur Erweiterbarkeit werden dafür 8 Relaisplätze vorgesehen.

- **Steuerungselektronik für SSR (Solid State Relays):** Für Verbraucher, die später geregelt werden sollen (z.B. Heizung), sind SSR-Ansteuerungen sinnvoll, da häufiges Schalten die Lebensdauer mechanischer Relais reduziert und die Geräuschkulisse deutlich erhöht.  

- **Unterstützung der Features aus Version 1:** Ein Mikrocontroller zur Ansteuerung der Peripherie (Display, Taster, Sensoren, Relais). Ein Netzteil, das die benötigten **5 V bzw. 3,3 V** für Mikrocontroller und Logik bereitstellt.

#### Hardware-Details Version 2.1
<!-- Kurze Beschreibung der Blöcke: µC, Sensor-Eingänge, Relais/SSR, Versorgung, ggf. Schaltplan-Link -->

#### Software-Stand in Version 2.1
<!-- Was macht die Firmware in 2.1? (Messen, Anzeigen, Loggen …) -->

#### Erste Messergebnisse
<!-- Platz für Temperaturkurven / Beobachtungen / Bilder (Bild 2 etc.) -->

#### Einschränkungen von Version 2.1
<!-- Was kann 2.1 bewusst noch nicht? (z.B. keine echte Regelung, Steam deaktiviert …) -->

#### Ausblick auf Version 2.2
<!-- Was wird in 2.2 auf Basis von 2.1 gemacht? -->



---

# Rancilio Silvia Upgrade – Version 1

## Überblick

In diesem Projekt habe ich meine **Rancilio Silvia** Espressomaschine um einige Funktionen erweitert.  
Ziel war es, die Bezugszeit automatisch anzuzeigen und die Bedienung etwas moderner zu gestalten.


## Version 1 – Funktionsumfang

### Shot-Timer

Ich habe ein **vierstelliges 7-Segment-Display** eingebaut, das:

- automatisch startet, sobald der Bezug beginnt  
- mitläuft, solange der Espresso durchläuft  
- automatisch stoppt, wenn der Bezug beendet ist  

Damit habe ich immer die **Bezugszeit im Blick**, ohne zusätzliche Timer.

### Induktive Tasten

An der Front befinden sich jetzt **induktive Tasten**.  
In Version 1 sind sie vor allem als moderne Bedienelemente vorbereitet – sie sollen später in Version 2 für zusätzliche Funktionen genutzt werden (z.B. Profile, Menüs, Einstellungen).

### Vorbereitete, aber noch nicht aktive Features

Folgende Komponenten sind **hardwareseitig bereits verbaut**, aber in **Version 1 noch nicht im Code aktiv**:

- **Wasserstandssensor** im Tank  
- **Uhrzeitmodul (RTC)** für spätere Features wie Logging, Statistiken oder zeitgesteuerte Funktionen  

## Elektronik und Aufbau

Die Elektronik sitzt auf der Rückseite der Maschine.  
Hier sind einige Bilder des Aufbaus:

<p align="center">
  <img src="https://github.com/user-attachments/assets/dd5c7d4d-201a-4ffe-9bd7-999d21807594" width="400">
</p>

<p align="center">
  <img src="https://github.com/user-attachments/assets/814a3628-3d86-42c5-b331-13a92b44effe" width="400">
  <img src="https://github.com/user-attachments/assets/9913349e-9b8f-4fae-85b5-c59e36bbe7ad" width="400">
</p>

<p align="center">
  <img src="https://github.com/user-attachments/assets/9aad19d4-9091-49dd-bf42-d4141211aecd" width="400">
  <img src="https://github.com/user-attachments/assets/74dee1ea-438a-4764-8024-2ced9c2f64f7" width="400">
</p>

## Video

Ein kurzes Video, in dem das System in Betrieb gezeigt wird:  
https://youtube.com/shorts/bBcD29Wb2CY
