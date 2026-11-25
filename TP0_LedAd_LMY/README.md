# TP0 – Chenillard et lecture AD (PIC32MX795F512L)

**Auteur :** Loïc Marmy
**Cours :** MINF – ETML-ES

## Description

- Le but du TP est de réaliser un chenillard en utilisant le Timer 0 du PIC32
- L'ADC du PIC sera aussi utilisé pour afficher les valeur brutes de 2 potentiomètre (0 à 1023) sur l'écran LCD
- La configuration du Timer 1 ainsi que les calculs sont expliqués dans ce document

## 1. Configuration du timer 1

| Paramètre | Valeur |
|---|---|
| Driver | Instance statique, `TMR_ID_1` |
| Mode | 16 bits, interruption activée |
| Priorité d'interruption | 3 (`ipl3AUTO`) |
| Horloge source | PBCLK = **80 MHz** *(à vérifier)* |
| Prescaler | **256** |
| Période (PR1) | **31249** |

**Calcul :**

```
f_timer = PBCLK / prescaler = 80 000 000 / 256 = 312 500 Hz
PR1     = f_timer × T - 1   = 312 500 × 0,1 - 1 = 31 249
```

PR1 est un registre 16 bits (max. 65535). Avec un prescaler de 64, on obtiendrait 124 999, une valeur trop grande : le prescaler 256 est donc nécessaire.

## 2. Timing du chenillard (LED0 à LED3)

![Chenillard LED0-LED3](images/chenillard.png)

**Réglages de l'oscilloscope :** CH1 = LED0, CH2 = LED1, CH3 = LED2, CH4 = LED3, base de temps 200 ms/div, déclenchement sur le flanc descendant de CH1.

**Commentaire :**
Les LEDs du kit sont actives à l'état bas : un niveau **bas (0 V)** correspond à une LED **allumée**.
On observe qu'une seule LED est allumée à la fois et que chaque LED reste allumée **≈ 200 ms** (mesuré : ___ ms). Les LEDs s'allument successivement : LED0, puis LED1, puis LED2, puis LED3.
Une même LED se rallume toutes les **8 × 200 ms = 1,6 s** (mesuré : ___ s).
La durée de 200 ms correspond à 2 interruptions du timer 1 (2 × 100 ms).

## 3. Durée de 3 s avant `APP_STATE_SERVICE_TASKS`

![Délai de 3 s](images/delai_3s.png)

**Réglages de l'oscilloscope :** CH1 = LED1, base de temps 500 ms/div, mode *Single*, déclenchement sur le flanc descendant, puis appui sur le bouton reset du kit.

**Commentaire :**
Dans l'état `APP_STATE_INIT`, toutes les LEDs sont allumées (sortie à 0 V) et le timer 1 est démarré.
Après 30 interruptions (30 × 100 ms = 3 s), l'interruption appelle `APP_UpdateState(APP_STATE_SERVICE_TASKS)`. Au premier passage dans cet état, toutes les LEDs sont éteintes (sortie remontée à 3,3 V).
La durée mesurée entre l'allumage et l'extinction est de **___ s** (valeur attendue : 3 s).
