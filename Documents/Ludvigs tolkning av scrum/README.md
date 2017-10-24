# Scrum


## Ordlista
- PO/Product owner, har ansvaret för hur produkten ska se ut och fungera. Prioritera user stories. 
- User stories, något som bidrar till produktens värde. (Se User stories). 
- Sprint, en cykel där scrum-teamet ska genomföra en hel arbetsfas, inklusive planering, arbete, presentation och utvärdering. 

## Intro - Syftet med scrum
Om scrum från Wikipedia: 
> Scrum is a framework for managing software development. It is designed for teams who break their work into actions that can be completed within fixed duration cycles (called "sprints"), track progress and re-plan in daily stand-up meetings, and collaborate to deliver workable software every sprint.

Mjukvaruutveckling möjliggör nya arbetsmetoder, det är enkelt att prova och testa. Istället för att planera allt från början kan man skapa en produkt direkt, om än med extremt begränsad funktionalitet, och fråga sin kund (PO) om det var detta hen hade tänkt sig. Feedbacken kommer direkt, och produkten blir mer som kunden vill. 

I scrum ligger focus på att man alltid ska ha något att göra, och att detta kommer att bidra till produkten på ett konkret sätt som alla i gruppen förstår. Att alltid förstå syftet och arbeta mot ett nära mål underlättar motivationen hos gruppmedlemmarna, och arbetet blir förhoppningsvis mer effektivt. 





## Intro II - Kursens begränsningar
Ha i åtanke att kursen går utt på att lära sig arbetsmetoden scrum. Att faktiskt skapa en lyckad produkt är inte prio. Och tur är det, för förutsättningarna är inte de bästa.

Några av de viktigaste förutsättningarna för ett lyckat scrumprojekt (enligt boken) är: 
- En sprint borde vara 2-3 veckor för en grupp som jobbar heltid. I kursen är det 1 vecka för en grupp som jobbar halvtid. Detta skapar mycket overhead-arbete (möten, planering) för lite verkstad. 
- Gruppen borde arbeta tillsammans i samma rum, och samtliga gruppens medlemmar borde jobba heltid på projektet. Detta för att underlätta kommunikation i gruppen, något som inte kan underskattas. 
- Ett scrumteam borde vara 3-9 personer. I kursen 2017 var vi 12 personer, vilket gjorde allt mer rörigt och omständigt. 

Kursen är inte optimal om det viktigaste hade varit att göra ett bra projekt. Det viktigaste är inte

De viktigaste praktiska tipsen till scrum är: 
- Kom överens om visionen/målet/syftet med projektet, så att ni arbetar mot samma sak.
- Skapa något av värde som ni presenterar i slutet av varje sprint.
- Arbeta så nära varandra som möjligt. Optimalt är att hela gruppen sitter i samma rum, varje dag. 
- Små steg. Testa, diskutera, arbeta vidare. 
- Begränsa er. Jobba inte mer än ni ska. 

## Intro III - Stulet från kursboken
Innan jag börjar kompendiet vill jag tillägga att mina erfarenheter av scrum i stort sett enbart är från denna kursen, och från en av kursböckerna Scrum from the trenches - II. Texten baseras på saker jag tyckte var viktiga att nämna för resten av min grupp som jag skrev av mig om i vår slack. 

Resultatet är förhoppningsvis en ännu kortare text än kursboken som innehåller de mest för kursen relevanta delarna av scrum. 

## Text

### En grupp som har samma mål
Rugbyjämförelsen finns där för att kärnan i scrum är att ha ett tydligt mål som alla rör sig mot. Alla måste inte ha full koll på allting. Det viktigaste är att stuff blir gjort.

### Fokusera på de viktigaste funktionerna
Prioritet ska sättas efter hur mycket man tror en funktion bidrar med till projektet. Ofta är det 20% av funktionerna som står för 80% av användningen, så det är ofta bättre att göra 20% riktigt bra än 100% halvdant. 

### Gruppflow 
Det är sjukt viktigt att gruppen kan arbeta ostört och utan hinder. Se till att ha en bös-person (scrum master?) som har ansvar för yttre problem som uppkommer. Vi andra borde inte bry oss om problemet i fråga, utan fokusera på vår arbetsuppgift. 

### Velocity 
När man inför en sprint ska räkna på hur många tidspoäng man hinner med, är det enkelt och bra att ta ett genomsnitt från föregående sprintar. Tänk på att göra uppgifterna/user storiesen så små att de helt säkert hinns med under samma sprint. Ju längre tid som går och ju bättre gruppen arbetar tillsammans, märker man förhoppningsvis att gruppen levererar mer, och på så sätt ökar sin velocity. 

I boken pushar författaren mycket på att göra alla uppgifter ungefär lika stora, och på att köra på känsla hur mycket man hinner med. 

Vattenfall är den gamla tekniken där man först bryter ned vad som ska göras i mindre delar - planering, GUI, funktionalitet - och sedan bygger upp en plan för när varje del ska vara klar. Men det är omöjligt att veta inan man vet vilken hastighet/velocity man arbetar i, och innan man lärt sig uppskatta arbetsuppgifterna och hur lång tid de tar. Scrum gör att man alltid måste omvärdera, omstrukturera och omplanera allt löpande. En annan dum sak med vattenfall är att man ger större uppgifter till enskilda aktörer, och även ifall aktören levererar en bra produkt, är det kanske inte så som PO tänkt sig att den skulle vara.

### Backloggen
Varje produkt i backloggen ska ha ett unikt värde i importance. Detta kan man också lösa genom att helt enkelt sortera dem efter viktighet, uppifrån och ned. PO bestämmer i slutändan prioriteringen av backloggen. 

### Sprintmöten
Det tar massa tid, och man behöver därför ha begränsad mötestid inför mötet, 1-8h beroende på grupp och projekt, och hur stora sprintarna är. De kan vara betydligt kortare, om man har ett möte om product backlog innan i ett separat möte, där man typ kommer överens om vad alla relevanta user stories:en innebär (inkl tidsuppskattning) och prioriterar dem.

Under varje möte bör man fråga sig "Vad är syftet med denna sprint?". Svaret ska en utomstående och oteknisk person förstå, och det ska vara nytt för denna sprint. Typ: att kunna starta och köra mopeden. Skälet till att ha ett gemensamt mål för varje sprint är att vi ska ha något att sikta på, något som motiverar.

### Exempel på sprint planning meeting från boken
Sprint planning meeting: 13:00-17:00 (10-minute break each hour)
• 13:00-13:30 – Product owner goes through sprint goal and summarizes
product backlog. Demo place, date, and time is set.
• 13:30-15:00 – Team time-estimates, and breaks down items as
necessary. Product owner updates importance ratings as necessary.
Items are clarified. “How to demo” is filled in for all high-importance
items.
• 15:00-16:00 – Team selects stories to be included in sprint. Do velocity
calculations as a reality check.
• 16:00-17:00 – Select time and place for daily scrum (if different from
last sprint). Further breakdown of stories into tasks.

### Att prioritera user stories som inte är user stories
Tech stories kallas de uppgifter som inte har direkt värde för produktägaren, men som är essentiella för att kunna genomföra projektet. Typ att bygga server eller göra nått meta, typ göra en plan för hur designen bör se ut rent generellt, så att andra kan förhålla sig till det. Tech stories kan finnas i en egen backlog - eftersom de inte ska nedprioriteras av produktägaren - och så säger man att man lägger ner 15% av gruppens tid/resurser på den backloggen. I vårt fall, där produktägaren inte finns, och inte ensam sätter prioriteringen, spelar allt detta dock mindre roll. Vi kan behandla tech stories som en vanlig user story, och prioritera inom gruppen. 

### Daily scrum 
Viktigt att visa progress. Man kan använda burndown charts, eller bara ta ett steg bakåt, och sammanfatta dagen i form av resultat. 

Burndown chart: Ett diagram som börjar på 100 (eller hur många poäng vi nu kör på), och om man efter en dag har löst en uppgift - på säg 5 poäng - tar man bort fem poäng, så att det är 95 kvar. Så kan man se att man antingen jobbar för långsamt, eller att man behöver ta bort uppgifter och prioritera om för att hinna med sprinten. Burndown chart har man där gruppen arbetar, och är alltså vanligtvis fysisk. 

Jag har inte pushat på att vi ska ha en burndown chart (såhär i efterhand hade någon lösning med trello fungerat bra som substitut till en fysisk chart), men det finns en mening i att stämma av lite bättre hur vi ligger till, för scrum fokuserar på resultat.

Under arbetets gång har vi lärt oss att daily scrum ger mycket i det att man stämmer av och ser vad alla håller på med, och får någon slags gruppkänsla på köpet. Fysisk närhet är positivt när man jobbar i grupp. 

### Sprint review - demonstration
Fokus ska vara på att man ska visa något som är helt klart varje gång. Så att produktägaren kan säga "ja, precis så" eller "nej, jag hade tänkt något mer såhär". Annars får man inte den feedbacken. Om något är helt klart behöver man inte heller lägga fokus på det i nästa sprint. Det är vitalt för att man ska kunna ta dessa små steg framåt, så att det inte blir stora steg åt fel håll.

I kort: Bättre att göra två saker fullt ut, än fem saker 99%. Bättre en fågel i handen än tio i skogen. Typ.

Under reviewen/demon: Fokusera på produkten och vad vi gjort. Inte vad vi gjort, och inte småsaker som bugfix. Bugfix kan man berätta om men inte demoa. Bra att demoa, och låta folk testa produkten/det man gjort, om möjligt.

Om man har problem med hur man ska visa upp något: Hur vet du själv att du är klar med det? Det beviset kan du visa upp.

### Sprint retrospectives - utvärdering
Att inte göra en sådan är som att säga: “I’m in such a hurry to chop down trees, I don’t have time to stop
and sharpen my saw!”. Viktigaste delen i scrum. "Without retrospectives you will find that the team keeps making thesame mistakes over and over again."

Alla ska vara med, PO och manager också, om möjligt. Boken tipsar om att man ska ha en avslappnande, tillåtande och mysig miljö. Man kollar på backloggen+velocity och summerar sprinten, med viktiga beslut och sånt. Runda bordet är bra, med vad som gått bra, dåligt och förslag på förbättring. Man diskuterar lösningar, och prioriterar vad man ska fokusera på att förbättra tills nästa gång. För man kan inte göra allt. Mot slutet konkretiserar scrum mastern vad som ska tas med från mötet, vad vi ska förändra för att nästa sprint ska bli bättre.

Innan man kommer till beslut att förändra något; tänk på följderna. Ibland är det bättre att låta saker vara, att göra gruppen uppmärksam på problemet kan vara nog för att medlemmarna ska tänka på det. Det är enklare att inte göra något drastiskt än att göra det. Och om man är restriktiv med förändringar behöver inte folk vara oroliga över att deras lilla klagomål kommer att påverka hela teamet på jobbigare sätt.

Att dela med sig av smarta slutsatser till andra grupper: I arbetslivet hade det varit bra att ha en ansvarig för att gå runt på de andras sprint retrospective-möten, dels för att ta in deras slutsatser och förbättringsförslag och dels för att dela med av vårt.

### Tid för avkoppling
Efter sprint retrospective bör man ta det lugnt. Man borde inte direkt köra på sprint planning, utan vänta till minst nästa dag. Med så korta sprintar som vi har är detta inte helt lätt. Vi hade kunnat ha ett 1-timmesmöte med sprint retrospect på torsdag, och sen vänta till fredagen eller till och med säga att vi gör annat på fredag och ha planeringsmötet först på måndagen. Men då försvinner ju typ halva veckan, men å andra sidan heter det sprint av en anledning.

### Scrum team management - irrelevant för denna kurs men intressant
Hur länge borde man vara scrumteam och hur ofta kan man byta medlemmar? Minst ett kvarts år, så att man hinner få upp kemin och produktiviteten i gruppen. Gruppmedlemmar i vilket team ska team manager inte peta så mycket i. Förändring uppifrån är mer onaturligt än förändringar som team-medlemmarna gör.

Deltidsmedlemmar: Dåligt. Undvik. Det går fortfarande om man har en gruppmedlem som är deltid, om man absolut måste (typ att det är en specialist som flera andra teams behöver). Men då är det viktigt att den personen har ett team som huvudteam.

### Kort om kommunikation med andra grupper
Om det är något team man delar projekt med kan det vara bra att en person från varje av dessa grupper träffas varje dag (i 15 min) och diskuterar gemensamma problem och sånt.

### Scrum of scrums
Det finns oerhört många sätt att göra detta på. Det viktigaste är att förstå syftet; att alla ska få en bättre överblick över företagets status med utvecklingen. Vissa företag passar det att representanter från varje scrumteam träffas varje dag och diskuterar. Andra passar det bättre att låta alla (ja, alla!) att träffas i ett möte som varar en kvart, där representanter från varje team får göra en kort sammanfattning av standard-"vad vi gjort, vad vi ska göra och eventuella problem"l och om det börjar bli diskussioner skriver man upp vilka som är intresserade av problemet i fråga så kan de diskutera detta senare. Poängen med att alla ska vara med är i det fallet att folk helt enkelt vill veta vad som händer, och att det är ett väldigt direkt sätt att lösa det på. Huvudpoängen är att man har en tanke med hur mötena ska vara.

### Firefighting
Om man fått ett trasigt system/prudukt, med stor technical debt, är prio att buggfixa. Om man samtidigt vill utveckla systemet måste man ha ett designerat team till att bara fixa buggar (kanban kan man använda här), så att de andra kan arbeta relativt ostört och som vanligt.

### Git
With modern version control systems (like GIT), there’s no excuse not to commit often, keep the trunk clean, release often, and keep branches short-lived.

### Geografiska problem (dåligt!)
Vikten av nära kommunikation i separerade team: Magin i scrum är den ständiga kontakten och samarbetet. Så om man ska göra scrum på distans gäller det att använda tekniska hjälpmedel så mycket som möjligt, kostsamma hjälpmedel är värt det, det är så viktigt!
