//
//  smm_database.c
//  Sookmyung Marble
//  Database platform code for storing marble board elements
//  Created by Seongeun Lee on 2025/12/21.
//

#include "smm_common.h"
#include "smm_database.h"
#include "smm_object.h"
#include <string.h>
#include <stdlib.h>

#define MAX_LIST (LISTNO_OFFSET_GRADE + MAX_PLAYER)

typedef struct node {
    int index;
    void* obj;
    struct node* next;
    struct node* prev;
} node_t;

static node_t* list_database[MAX_LIST];
static node_t* listPtr[MAX_LIST];
static int list_cnt[MAX_LIST];

static node_t* genNode(void) {
    node_t* ndPtr = (node_t*)malloc(sizeof(node_t));
    if (ndPtr) { ndPtr->next = NULL; ndPtr->obj = NULL; ndPtr->prev = NULL; }
    return ndPtr;
}

static node_t* smmList(int list_nr, int index) {
    node_t* ndPtr = list_database[list_nr];
    if (index < 0 || index >= list_cnt[list_nr]) return NULL;
    while (ndPtr != NULL) {
        if (ndPtr->index == index) break;
        ndPtr = ndPtr->next;
    }
    return ndPtr;
}

int smmdb_addTail(int list_nr, void* obj) {
    if (obj == NULL) return -1;
    node_t* newNdPtr = genNode();
    if (newNdPtr == NULL) return -1;
    newNdPtr->obj = obj;
    if (list_database[list_nr] == NULL) {
        list_database[list_nr] = newNdPtr;
        newNdPtr->index = 0;
    } else {
        node_t* ndPtr = smmList(list_nr, list_cnt[list_nr] - 1);
        ndPtr->next = newNdPtr;
        newNdPtr->prev = ndPtr;
        newNdPtr->index = ndPtr->index + 1;
    }
    list_cnt[list_nr]++;
    return 0;
}

void* smmdb_getData(int list_nr, int index) {
    node_t* ndPtr = smmList(list_nr, index);
    return ndPtr ? ndPtr->obj : NULL;
}

int smmdb_len(int list_nr) { return list_cnt[list_nr]; }

// Card Data
char* smmDb_getFoodCardName(int index) {
    smmObj_t* card = (smmObj_t*)smmdb_getData(LISTNO_FOODCARD, index);
    return card ? card->smm_name : "Unknown";
}

int smmDb_getFoodCardEnergy(int index) {
    smmObj_t* card = (smmObj_t*)smmdb_getData(LISTNO_FOODCARD, index);
    return card ? card->smm_energy : 0;
}

char* smmDb_getFestivalCardMission(int index) {
    smmObj_t* card = (smmObj_t*)smmdb_getData(LISTNO_FESTCARD, index);
    return card ? card->smm_mission : "Unknown Mission";
}

int smmDb_genFoodCard(char *foodName, int foodEnergy) {
    void* newCard = smmObj_genObject(foodName, 0, 0, foodEnergy, NULL);
    return smmdb_addTail(LISTNO_FOODCARD, newCard);
}

int smmDb_genFestivalCard(char *mission) {
    void* newCard = smmObj_genObject(NULL, 0, 0, 0, mission);
    return smmdb_addTail(LISTNO_FESTCARD, newCard);
}
