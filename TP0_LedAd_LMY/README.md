# TP0 – Chenillard et lecture AD (PIC32MX795F512L)

**Auteur :** Loïc Marmy

**Cours :** MINF – ETML-ES

## Description

- Le but du TP est de réaliser un chenillard en utilisant le Timer 1 du PIC32
- L'ADC du PIC sera aussi utilisé pour afficher les valeurs brutes de 2 potentiomètre (0 à 1023) sur l'écran LCD
- La configuration du Timer 1 ainsi que les calculs sont expliqués dans ce document

## 1. Configuration du timer 1

| Paramètre | Valeur |
|---|---|
| Driver Implementation | STATIC |
| Timer Module ID | TMR_ID_1 |
| Mode | 16 bits, interruption activée |
| Interrupt priority | INT_PRIORITY_LEVEL3 |
| Interrupt Sub-priority | INT_SUBPRIORITY_LEVEL0 |
| Clock Source | DRV_TMR_CLKSOURCE_INTERNAL (80MHz) |
| Prescaler | 256 |
| Timer Period | 31249 |

**Image configuration :**

<img width="555" height="281" alt="ConfigTimer1" src="https://github.com/user-attachments/assets/055774b8-84cc-4f98-9e66-1622f9792a6b" />

**Calcul :**

```
f_timer = PBCLK / prescaler = 80 000 000 / 256 = 312 500 Hz

Timer Period = (f_timer × T) - 1   = (312 500 × 0,1) - 1 = 31 249

-> Légende : PBCLK (fréquence interne du PIC -> 80MHz), T période d'interruption (toutes les 100ms -> selon consigne TP)
```

C'est un timer 16 bits (max. 65535). Avec un prescaler de 64, on obtiendrait 124 999, une valeur trop grande !! Le prescaler 256 est donc celui qu'il faut utiliser.

## 2. Timing du chenillard (LED0 à LED3)

<img width="1462" height="914" alt="Capture d&#39;écran 2026-11-23 151725" src="https://github.com/user-attachments/assets/bc90a0e4-3798-43b6-b547-157332d861eb" />

**Réglages de l'oscilloscope :** CH1 = LED0, CH2 = LED1, CH3 = LED2, CH4 = LED3, base de temps 200 ms/div, déclenchement sur le flanc descendant de CH1.

**Commentaire :**
Les LEDs du kit sont actives à l'état bas -> 0 V correspond à une LED allumée.
On observe qu'une seule LED est allumée à la fois et que chaque LED reste allumée 200 ms (mesuré : 200 ms). Les LEDs s'allument successivement : LED0, puis LED1, puis LED2, puis LED3.
La durée de 200 ms correspond à 2 interruptions du timer 1 (2 × 100 ms).

## 3. Durée de 3 s avant `APP_STATE_SERVICE_TASKS`

<img width="1461" height="913" alt="Capture d&#39;écran 2026-11-23 152314" src="https://github.com/user-attachments/assets/ff0beb56-4f81-455e-aa93-523d6b866e56" />

**Réglages de l'oscilloscope :** CH1 = LED0, CH2 = LED1, base de temps 500 ms/div, déclenchement sur le flanc descendant, puis appui sur le bouton reset du kit.

**Commentaire :**
Dans l'état "APP_STATE_INIT", toutes les LEDs sont allumées pendant 3 secondes (sortie à 0 V) et le timer 1 est démarré.
Après 30 interruptions (30 × 100 ms = 3 s), l'interruption appelle "APP_UpdateState(APP_STATE_SERVICE_TASKS)". Au premier passage dans cet état, toutes les LEDs sont éteintes (selon consigne).
La durée mesurée entre l'allumage et l'extinction est de 3.05s (valeur attendue : 3 s). La différence entre la valeur théorique (3s) et la valeur mesurée (3.05s) peut être expliquée par la précision des curseurs sur l'oscillogramme.

**Remarques :**
Sur l'oscillogramme, après les 3s d'attente, c'est la LED1 (trace verte) qui s'allume en premier et non la LED0 (trace jaune). Ca vient de la façon dont la fonction "chenillard()" est écrite.

<img width="448" height="529" alt="image" src="https://github.com/user-attachments/assets/f83a9480-3aab-4f45-b6b5-a1e541bd1c04" />

La variable "led" démarre à 0, mais elle est incrémentée avant l'allumage. Au premier changement de LED, le programme éteint donc la LED0, passe à la LED1 et l'allume. C'est la raison pour laquelle la LED1 est allumée en premier. Si on veut allumer la LED0 en premier il faudrait initialiser la variable "led" à 7.

## Conclusion

Le programme répond au cahier des charges du TP :
- le timer 1 génère une interruption toutes les 100ms
- après un délai de 3 secondes, la machine d'état passe dans "APP_STATE_SERVICE_TASKS"
- les 2 potentiomètres sont lus par l'ADC et leur valeurs brutes s'affichent sur le LCD
- le chenillard allume une seule LED à la fois, chacune pendant 200ms

Les mesures à l'oscilloscope confirment les durées calculées.

**Difficultés rencontrées**
- Configuration du BSP, trouver les bons menus, etc.
- L'apparition et disparition du kit ETML-ES dans le BSP

