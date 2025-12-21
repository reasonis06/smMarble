//
//  main.c
//  SMMarble
//
//  Created by Seongeun Lee on 2025/12/21.
//

#include "smm_common.h"
#include "smm_object.h"
#include "smm_database.h"
#include <string.h>
#include <stdlib.h>

#define MAX_NODETYPE 7
#define MAX_GRADE 13
#define MAX_ACHIEVABLE_GRADE 9

// Struct
typedef struct {
    int player_pos[MAX_PLAYER];
    int player_credit[MAX_PLAYER];
    char player_name[MAX_PLAYER][MAX_CHARNAME];
    int player_energy[MAX_PLAYER];
    int flag_graduated[MAX_PLAYER];
    int is_experimenting[MAX_PLAYER]; 
    int exp_success_value[MAX_PLAYER];
    smmObj_t *grade_history_head[MAX_PLAYER];
} smm_player_t;

static smm_player_t smm_players;
static int smm_PlayerNr = 0;

static char smmNodeName[MAX_NODETYPE][MAX_CHARNAME] = {
    "lecture", "restaurant", "laboratory", "home", "gotlab", "foodchange", "festival"
};

static char smmGradeName[MAX_GRADE][MAX_CHARNAME] = {
    "A+", "A0", "A-", "B+", "B0", "B-", "C+", "C0", "C-", "D+", "D0", "D-", "F"
};

// object function
void* smmObj_genObject(char* name, int type, int credit, int energy, char* mission) {
    smmObj_t* obj = (smmObj_t*)malloc(sizeof(smmObj_t));
    if (obj != NULL) {
        if (name != NULL) strcpy(obj->smm_name, name);
        else obj->smm_name[0] = '\0';
        
        obj->smm_type = type;
        obj->smm_credit = credit;
        obj->smm_energy = energy;
        
        if (mission != NULL) strcpy(obj->smm_mission, mission);
        else obj->smm_mission[0] = '\0';
        
        obj->smm_grade = 0;
        obj->next = NULL;
    }
    return (void*)obj;
}

// player function
void smmObj_updatePlayerNr(int n) { smm_PlayerNr = n; }
int smmObj_getPlayerNr(void) { return smm_PlayerNr; }

void smmObj_initPlayerFields(int player, int initEnergy) {
    smm_players.player_pos[player] = 0;
    smm_players.player_credit[player] = 0;
    smm_players.player_energy[player] = initEnergy;
    smm_players.flag_graduated[player] = 0;
    smm_players.grade_history_head[player] = NULL; 
    smm_players.is_experimenting[player] = 0;
    smm_players.exp_success_value[player] = 0;
}

void smmObj_updatePlayerPos(int player, int pos) { smm_players.player_pos[player] = pos; }
void smmObj_updatePlayerCredit(int player, int credit) { smm_players.player_credit[player] += credit; }
void smmObj_updatePlayerEnergy(int player, int energy) { smm_players.player_energy[player] += energy; }
void smmObj_updateGraduatedFlag(int player, int flag) { smm_players.flag_graduated[player] = flag; }
char* smmObj_getPlayerName(int player) { return smm_players.player_name[player]; }
void smmObj_setPlayerName(int player, char* name) { strcpy(smm_players.player_name[player], name); }
int smmObj_getPlayerPos(int player) { return smm_players.player_pos[player]; }
int smmObj_getPlayerCredit(int player) { return smm_players.player_credit[player]; }
int smmObj_getPlayerEnergy(int player) { return smm_players.player_energy[player]; }
int smmObj_getGraduatedFlag(int player) { return smm_players.flag_graduated[player]; }

// node info
char* smmObj_getNodeName(int node_nr) {
    smmObj_t* node = (smmObj_t*)smmdb_getData(LISTNO_NODE, node_nr);
    return (node != NULL) ? node->smm_name : NULL;
}
int smmObj_getNodeType(int node_nr) {
    smmObj_t* node = (smmObj_t*)smmdb_getData(LISTNO_NODE, node_nr);
    return (node != NULL) ? node->smm_type : -1;
}
int smmObj_getNodeCredit(int node_nr) {
    smmObj_t* node = (smmObj_t*)smmdb_getData(LISTNO_NODE, node_nr);
    return (node != NULL) ? node->smm_credit : 0;
}
int smmObj_getNodeEnergy(int node_nr) {
    smmObj_t* node = (smmObj_t*)smmdb_getData(LISTNO_NODE, node_nr);
    return (node != NULL) ? node->smm_energy : 0;
}

// common
char* smmObj_getTypeName(smmNode_e type) { return smmNodeName[type]; }
char* smmObj_getGradeName(smmGrade_e grade) { return smmGradeName[grade]; }
smmGrade_e smmObj_getRandomGrade(void) { return (smmGrade_e)(rand() % MAX_ACHIEVABLE_GRADE); }

// history
void smmObj_addGradeToHistory(int player, char *lectureName, int credit, smmGrade_e grade) {
    smmObj_t *newNode = (smmObj_t*)smmObj_genObject(lectureName, 0, credit, 0, NULL);
    if (newNode == NULL) return;
    newNode->smm_grade = (int)grade;
    newNode->next = smm_players.grade_history_head[player];
    smm_players.grade_history_head[player] = newNode;
}
smmObj_t* smmObj_findLectureGrade(int player, char *lectureName) {
    smmObj_t *current = smm_players.grade_history_head[player];
    while (current != NULL) {
        if (strcmp(current->smm_name, lectureName) == 0) return current;
        current = current->next;
    }
    return NULL;
}
smmObj_t* smmObj_getGradeHistoryHead(int player) { return smm_players.grade_history_head[player]; }
char* smmObj_getHistoryLectureName(smmObj_t* node) { return node->smm_name; }
int smmObj_getHistoryCredit(smmObj_t* node) { return node->smm_credit; }
smmGrade_e smmObj_getHistoryGrade(smmObj_t* node) { return (smmGrade_e)node->smm_grade; }
smmObj_t* smmObj_getNextHistoryNode(smmObj_t* node) { return node->next; }

// exp
int smmObj_getExpFlag(int player) { return smm_players.is_experimenting[player]; }
void smmObj_updateExpFlag(int player, int flag) { smm_players.is_experimenting[player] = flag; }
int smmObj_getExpValue(int player) { return smm_players.exp_success_value[player]; }
void smmObj_updateExpValue(int player, int value) { smm_players.exp_success_value[player] = value; }
