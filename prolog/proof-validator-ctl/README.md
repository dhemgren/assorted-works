---
title: 'DD1350 Logik för dataloger'
subtitle: 'Laboration 2: Modellprovning för CTL'
author:
- 'Dan Hemgren (dhemgren@kth.se)'
- 'Ernst Widerberg (ernstwi@kth.se)'
---

# Modellering

Vi har valt att modellera en bankomat, där man kan logga in med användarnamn och lösenord, för att sedan antingen ta ut eller sätta in pengar. Det finns bara en atom, *l*, som står för "logged in".

![Bankomatmodell](modell.png)

## Specificering (systemegenskaper)

Starttillståndet för båda egenskaper nedan är *Enter username*.

### Sann: $EF(EG(l))$

Det existerar en path där ifrån något framtida tillstånd (till exempel "menu") det existerar en path där alltid *l*.

Denna formel formaliserar systemegenskapen att när man loggat in på bankomaten kan man utföra hur många ärenden (sätta in/ta ut pengar) som helst, utan att bli utloggad. Det finns ingen tidsgräns.

### Falsk: $EG(l))$

Det finns ingen path där alltid *l*.

Denna formel formaliserar systemegenskapen att man inte från starttillståndet kan gå direkt till att vara inloggad utan att skriva in ett giltigt användarnamn och lösenord.

# Predikattabell

### `check(T, L, S, U, F)`

Detta predikat är sant om *formeln* `F` stämmer för modellen som definieras av *transitions* `T` (övergångar) och *labels* `L` (atomer), från *starttillståndet* `S`. `U` är en lista av tidigare besökta tillstånd, vilken vi behöver för att kunna avbryta exekveringen då vi når en "loopande" path i vår modell.

### `elements(L, S, SElements)`

Detta predikat är sant om `SElements` är listan av övergångar/atomer som "tillhör" tillståndet `S` i listan av listor med övergångar/atomer `L`.

### `adjacent(T, A, B)`

Detta predikat utnyttjar `elements` för att plocka ut ett element i taget ur en lista av övergångar `T`. Formellt sett är predikatet sant om `B` är ett element i listan av övergångar/atomer som tillhör tillståndet `S` i listan av listor med övergångar/atomer `T`, men vi använder bara `adjacent` för övergångar.

