# Moped - DAT255
Här står en översikt över vilken funktionalitet som finns var, och kort vad den används till. Fokus ligger på sådan kod som vi har skrivit, i ett försök att underlätta för examinator att snabbt hitta vad vi faktiskt gjort. 

## Lånad kod
Mycket av koden i projektet är importerad och lånad, för att snabbare kunna ta sig framåt i projektet och kunna nå våra mål. 
- Mappen **moped** är tagen från kursen. Används inte så mycket. 
- **canJava** används till att ansluta direkt till MOPEDens can-buss, för enklare kommunikation. Har tagits från en annan grupp i kursen (Grupp Absolut), som gjort vissa förändringar från kod som Arndt Jonasson (skaparen av MOPED-projektet) skrivit.
- I **OveControl** finns appen för att styra vår MOPED. Vi använder mycket kod från WirelessIno, men har ändrat något i de flesta filer.


## Egen kod
### ImageDetection
- Letar med hjälp av MOPEDens kamera efter cirklar av en viss färg. Används för att styra i vilken riktning MOPEDen åker. 
- **PiCamera**, där kamerafunktionaliteten finns, består av modifierade metoder tagna från Optipos-mappen i MOPED-projektet.
- Har en egen server för bildöverföring. 
- Se ImageDetections README för mer detaljer. 

### ACC i canJava
För att styra avståndet till föremålet framför har [ACC.java i canJava](https://github.com/PeterGardenas/Moped-Ove/tree/master/canJava/JavaInterfaceTest/ACC/absolut/acc) använts. Det är alltså inte samma ACC som den Absolut-gruppen använt, utan en egen. Även mainfilen i canJava är i stor del vår.

### OveControl 
App som baseras på WirelessIno, som används för att styra och via nätverk kommunisera med en MOPED. Förändringar från WirelessIno är
- Seekbars från android, istället för hårdkodade. Underlättar utbyggnad av UI. 
- Möjlighet att stänga av och sätta på Platooning och ACC från appen.

## Dokumentation
D-uppgifter från kursen och möten från sprintarna finns här. I kort: Det som inte är kod finns här. 

## Gitinspector
Att använda gitinspector för vår grupp kommer bli ganska missvisande på grund av flera anledningar. Vi har t.ex. parprogrammerat mycket och pushat till git från den enas dator, vi har också skrivit mycket kod som sedan inte använts, t.ex. i python. Vi har också lagt ner mycket tid på att fixa hårdvaran och många har haft andra uppgifter i projektet som inte är kodrelaterade. Därför kommer dokumentet om team evaluation ge en mer rättvis bild.

| Author           | Commits | Insertions | Deletions | % of changes |
|------------------|---------|------------|-----------|--------------|
| Elina Olsson     | 2       | 168        | 0         | 0.02         |
| Johan Wennerbeck | 46      | 1129       | 1562      | 0.30         |
| Julia Ortheden   | 6       | 1753       | 21        | 0.20         |
| Kitzing          | 1       | 181        | 181       | 0.04         |
| Luddevig         | 1       | 22         | 0         | 0.00         |
| MadeleineLexen   | 3       | 62         | 149       | 0.02         |
| Peter Gärdenäs   | 39      | 882024     | 1939      | 98.85        |
| Yeetii           | 43      | 1242       | 374       | 0.18         |
| hannacarlssoon   | 29      | 1797       | 1617      | 0.38         |


## Gruppens deltagare

| Namn               |  Github-nick       |
|--------------------|--------------------|
| Miranda Brannsgård |  mirban            |
| Hanna Carlsson     |  hannacarlssoon    |
| Ludvig Ekman       |  luddevig          |
| Jonathan Gildevall |  Schurickan        |
| Peter Gärdenäs     |  PeterGardenas     |
| Sara Kitzing       |  Kitzing           |
| Madeleine Lexén    |  MadeleineLexen    |
| Tobias Lindgren    |  Tuubaas           |
| Erik Magnusson     |  Yeetii            |
| Elina Olsson       |  olssonelina       |
| Julia Ortheden     |  Hjortheden        |
| Johan Wennerbeck   |  JohanWennerbeck   |
