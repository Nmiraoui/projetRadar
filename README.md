# 📡 Projet Radar à Balayage Ultrason (STM32 & Qt/C++)

> Réalisation d'un système de type radar interactif combinant une carte STM32F746G-DISCO, un capteur à ultrasons HC-SR04, un servomoteur et une application de bureau connectée en réseau (UDP).

---

## 📸 Présentation du Montage

![Montage du Projet Radar](image0.jpg)

---

## 📋 Sommaire
1. [Contexte et Objectif](#1-contexte-et-objectif)
2. [Matériel Utilisé](#2-matériel-utilisé)
3. [Architecture Générale](#3-architecture-générale)
4. [Configuration de l'Horloge](#4-configuration-de-lhorloge)
5. [Capteur Ultrason HC-SR04](#5-capteur-ultrason-hc-sr04)
6. [Servomoteur](#6-servomoteur)
7. [Partie Réseau (Serveur UDP sur la carte)](#7-partie-réseau-serveur-udp-sur-la-carte)
8. [Partie Interface (Application PC et écran de la carte)](#8-partie-interface-application-pc-et-écran-de-la-carte)
9. [Problèmes Rencontrés et Solutions](#9-problèmes-rencontrés-et-solutions)
10. [Points de Vigilance](#10-points-de-vigilance)
11. [Conclusion et Améliorations Possibles](#11-conclusion-et-améliorations-possibles)

---

## 1. Contexte et Objectif

* **Objectif :** Réaliser un radar à partir d'une carte STM32, d'un capteur ultrason et d'un servomoteur. Le servomoteur oriente le capteur sur un angle de balayage de 0 à 180 degrés ; à chaque position, la distance de l'obstacle est mesurée. L'ensemble des mesures est récupéré à distance par un PC via le réseau[cite: 11].

* **Démarche suivie :**
  1. Mise en place et test du capteur ultrason[cite: 11].
  2. Mise en place et test du servomoteur[cite: 11].
  3. Création du serveur UDP sur la carte[cite: 11].
  4. Création de l'interface PC[cite: 11].
  5. Intégration et correction des problèmes[cite: 11].

* **Dépôt GitHub :** [https://github.com/Nmiraoui/projetRadar.git](https://github.com/Nmiraoui/projetRadar.git)[cite: 11]
  * *Code Carte :* Projet Keil (`main.c`, `configuration.c`, `UDP_Socket.c`, écran AppWizard)[cite: 11]
  * *Code Interface :* Application Qt/C++ (`RadarPC`)[cite: 11]

---

## 2. Matériel Utilisé

* **Carte :** STM32F746G-DISCO (microcontrôleur STM32F746, Cortex-M7)[cite: 11]
* **Capteur :** HC-SR04 (ultrason)[cite: 11]
* **Actionneur :** Servomoteur commandé en PWM[cite: 11]
* **Environnement :** Keil MDK (RTX5, MDK-Net, emWin/AppWizard) pour la carte, Qt 6 pour l'interface PC[cite: 11]
* **Liaison :** Ethernet, protocole UDP[cite: 11]

### Brochage utilisé :
| Fonction | Broche | Périphérique | Remarque |
| :--- | :--- | :--- | :--- |
| **Commande servo** | `PF6` | TIM10_CH1 (AF3) | PWM 50 Hz[cite: 11] |
| **Trig du capteur** | `PF7` | TIM11_CH1 (AF3) | Impulsion périodique[cite: 11] |
| **Echo du capteur** | `PG6` | EXTI6 + TIM2 | Mesure de la durée[cite: 11] |
| **LED de contrôle** | `PI1` | GPIO sortie | Allumée pendant l'écho[cite: 11] |

---

## 3. Architecture Générale

Le système repose sur le RTOS Keil RTX5. Trois éléments tournent en parallèle[cite: 11] :
* Le serveur UDP (`Init_Ethernet`) et sa tâche de balayage `Thread_Scan`[cite: 11].
* La tâche graphique de l'écran de la carte (`Init_GUIThread`)[cite: 11].
* Les périphériques matériels (timers et interruption), qui fonctionnent sans mobiliser le processeur[cite: 11].

* **Chaîne de mesure :** 
  `TIM11 (PF7) --> Trig --> HC-SR04 --> Echo --> PG6 --> interruption EXTI6 --> TIM2 mesure la durée --> variable DIST`[cite: 11]

* **Chaîne de commande :** 
  `Commande UDP --> udp_cb_func --> servo_set_angle --> TIM10 (PF6) --> servo`[cite: 11]

---

## 4. Configuration de l'Horloge

* **Fichier :** `configuration.c`, fonction `configuration_overclock()`[cite: 11].
* La carte est configurée à **200 MHz** (HSE 25 MHz, PLL M=25, N=400, P=2, overdrive activé). Nécessaire pour l'écran LCD[cite: 11].
  * `SYSCLK / HCLK` : 200 MHz[cite: 11]
  * `APB1` (prédiviseur 4) : 50 MHz $\rightarrow$ horloge des timers APB1 : **100 MHz**[cite: 11]
  * `APB2` (prédiviseur 2) : 100 MHz $\rightarrow$ horloge des timers APB2 : **200 MHz**[cite: 11]
  * **TIM2** : 100 MHz | **TIM10 & TIM11** : 200 MHz[cite: 11]

---

## 5. Capteur Ultrason HC-SR04

### 5.1 Principe
Le HC-SR04 utilise un signal **Trig** (impulsion d'au moins 10 µs envoyée par le microcontrôleur) et un signal **Echo** (le capteur renvoie une impulsion dont la durée est proportionnelle à la distance)[cite: 11].
$$\text{distance (cm)} = \frac{\text{durée de l'écho (}\mu\text{s)} \times 17}{1000} \quad (\approx \text{durée} / 58,8)$$[cite: 11]

### 5.2 Génération du signal Trig (TIM11, PF7)
* `PSC = 399` (2 µs par tick), `ARR = 49999` (période de 100 ms), `CCR1 = 8` (impulsion de 16 µs)[cite: 11].

### 5.3 Mesure de l'écho (PG6, EXTI6, TIM2)
* Géré par interruption externe, avec `TIM2` configuré à 1 MHz (1 tick = 1 µs) comme chronomètre 32 bits[cite: 11].
* **Front montant :** LED `PI1` allumée, `TIM2` remis à zéro et démarré[cite: 11].
* **Front descendant :** LED `PI1` éteinte, `TIM2` arrêté, calcul de la distance stockée dans `DIST`[cite: 11].

---

## 6. Servomoteur

* **Configuration (TIM10, PF6) :** PWM 50 Hz (période 20 ms)[cite: 11].
  * `PSC = 79` (0,4 µs par tick), `ARR = 49999`[cite: 11].
* **Fonction `servo_set_angle(deg)` :** 
  $$\text{CCR1} = 2000 + \frac{\text{deg} \times 4000}{180}$$[cite: 11]
  * 0° $\rightarrow$ 0,8 ms | 90° $\rightarrow$ 1,6 ms | 180° $\rightarrow$ 2,4 ms[cite: 11]

---

## 7. Partie Réseau (Serveur UDP sur la Carte)

* **Port d'écoute :** `5000` (via la pile MDK-Net)[cite: 11].
* **Protocole textuel :**
  | Commande | Action | Réponse |
  | :--- | :--- | :--- |
  | `ANGLE n` | Place le servo à $n$ degrés ($0 \le n \le 180$) | `OK` ou `ERR angle`[cite: 11] |
  | `DIST` | Renvoie la dernière distance | `DIST x.x` ou `DIST TIMEOUT`[cite: 11] |
  | `SCAN` | Balayage complet de 0° à 180° (pas de 5°) | Lignes "angle distance" puis `END`[cite: 11] |

---

## 8. Partie Interface (Application PC & Écran)

* **Application PC (Qt 6 / C++) :** Gère la liaison UDP (socket `QUdpSocket` sur le port local `8001`), un curseur pour l'angle, et des boutons pour déclencher les actions (`ANGLE`, `DISTANCE`, `SCAN`)[cite: 11].
* **Écran de la carte (AppWizard / emWin) :** Interface tactile locale pour visualiser et contrôler certains aspects du système sans perturber les mesures[cite: 11].

---

## 9. Problèmes Rencontrés et Solutions

1. **Fréquence d'horloge (Overclocking) :** Les timings PWM et timers étaient décalés car calculés pour 16 MHz au lieu des 200 MHz réels de la carte $\rightarrow$ *Recalcul complet des `PSC` et `ARR`*[cite: 11].
2. **Broche Echo inopérante :** Conflit matériel initial sur `PE6` $\rightarrow$ *Migration propre sur la broche dédiée `PG6`*[cite: 11].
3. **Capteur défectueux :** Absence de signal due à un composant défectueux $\rightarrow$ *Vérification physique préalable et test croisé du HC-SR04*[cite: 11].
4. **Régression de code :** Utilisation des commits Git pour comparer et restaurer la version stable précédente ("test moteur")[cite: 11].
5. **Réseau / Routeur :** Validation des ports de destination et des règles de routage UDP pour s'assurer de la bonne transmission des trames[cite: 11].

---

## 10. Points de Vigilance

* Ne jamais modifier l'écran ou exécuter du code bloquant directement dans une interruption (utiliser des drapeaux et des tâches RTOS)[cite: 11].
* Toujours acquitter les drapeaux d'interruption (`EXTI->PR`) pour éviter les boucles infinies[cite: 11].
* Respecter scrupuleusement le brochage matériel (`PF6`, `PF7`, `PG6`, `PI1`)[cite: 11].
* Garder en tête qu'un balayage complet dure environ 10 secondes (réponse `ERR occupe` si sollicité en double)[cite: 11].

---

## 11. Conclusion et Améliorations

Le système opérationnel valide avec succès le contrôle à distance en UDP, l'acquisition ultrasonore et le positionnement angulaire précis[cite: 11]. 
* **Pistes d'amélioration :** Représentation graphique en coordonnées polaires (affichage radar visuel), ajout d'un timeout logiciel sur l'écho, et configuration dynamique de l'adresse IP depuis l'interface[cite: 11].
