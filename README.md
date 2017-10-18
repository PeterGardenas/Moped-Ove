# Moped - DAT255


## Lånad kod
Mycket av koden i projektet är importerad och lånad. 
- Mappen 'moped' är tagen från kursen. 
- canJava används till att ansluta direkt till MOPEDens can-buss, för enklare kommunikation. Har tagits från en annan grupp i kursen (Grupp Absolut), som gjort vissa förändringar från kod som Arndt Jonasson (skaparen av MOPED-projektet) skrivit.


## Egen kod
### ImageDetection
- Letar med hjälp av MOPEDens kamera efter cirklar av en viss färg. Används för att styra i vilken riktning MOPEDen åker. 
- PiCamera, där kamerafunktionaliteten finns, består av modifierade metoder tagna från Optipos-mappen i MOPED-projektet.
- Har en egen server för bildöverföring. 
- Se ImageDetections README för mer detaljer. 

### ACC i canJava
För att styra avståndet till föremålet framför har ACC.java i canJava använts. Det är alltså inte samma ACC som den Absolut-gruppen använt, utan en egen. Även andra filer i canJava har ändrats på.

## Dokumentation
D-uppgifter från kursen och möten från sprintarna finns här. I kort: Det som inte är kod finns här. 

## Gruppens deltagare

| Namn               |  Github            |
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
