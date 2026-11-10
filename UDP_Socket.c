/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network:Service
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    UDP_Socket.c
 * Purpose: UDP Socket Code Template
 * Rev.:    V7.0.0
 *----------------------------------------------------------------------------*/
//! [code_UDP_Socket]
#include "rl_net.h"
#include "stm32f7xx.h"               // Device header
#include "AppWizard.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int32_t udp_sock;                      // UDP socket handle

// Fonction de rappel appelée automatiquement à chaque datagramme UDP reçu
uint32_t udp_cb_func (int32_t socket, const NET_ADDR *addr, const uint8_t *buf, uint32_t len) {
    char command[64];
    char response[64];
    uint32_t resp_len = 0;

    // 1. Copie sécurisée des données reçues
    if (len >= sizeof(command)) {
        len = sizeof(command) - 1;
    }
    memcpy(command, buf, len);
    command[len] = '\0';

    // 2. Traitement des commandes
    int angle = -1;

    if (sscanf(command, "ANGLE %d", &angle) == 1) {
        // Commande ANGLE reçue
        if (angle >= 0 && angle <= 180) {
            GPIOI->ODR ^= (1 << 1); // Inverse l'état de la LED
            sprintf(response, "OK\n");
        } else {
            sprintf(response, "ERR INVALID_ANGLE\n");
        }
    } 
   /* else if (strcmp(command, "DIST") == 0) {
        // Commande DIST reçue
        float dist = distance_cm();
        
        if (dist < 0) {
            sprintf(response, "DIST TIMEOUT\n");
        } else {
            sprintf(response, "DIST %.1f\n", dist);
        }
    } 
    else {
        // Si la commande ne correspond à rien de connu
        sprintf(response, "ERR UNKNOWN_COMMAND\n");
    }*/

    // 3. Envoi de la réponse au client UDP
    resp_len = strlen(response);
    uint8_t *sendbuf = netUDP_GetBuffer(resp_len);
    if (sendbuf != NULL) {
        memcpy(sendbuf, response, resp_len);
        netUDP_Send(socket, addr, sendbuf, resp_len);
    }

    return (0);
}

// Send UDP data to destination client.
void send_udp_data (void) {
    // Non utilisé pour la réception directe de commandes
}

// Allocate and initialize the socket.
void Init_Ethernet (void) {
    netInitialize ();

    // Initialisation du socket UDP et ouverture du port 5000[cite: 4, 8, 9]
    udp_sock = netUDP_GetSocket (udp_cb_func);
    if (udp_sock > 0) {
        netUDP_Open (udp_sock, 5000);
    }
}
//! [code_UDP_Socket]