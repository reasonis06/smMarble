//
//  smm_object.c
//  SMMarble
//
//  Created by Seongeun Lee on 2025/12/10.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>
#include <stdlib.h>

#define MAX_NODENR											100
#define MAX_ACHIEVABLE_GRADE 						9


//구조체로 만들기 
//struct int smm_players[MAX_PLAYER];

// 각 플레이어가 수강한 강의 이력 노드를 나타내는 구조체
typedef struct smmGradeNode {
    char lecture_name[MAX_CHARNAME];
    int credit;
    smmGrade_e grade;
    struct smmGradeNode *next;
} smmGradeNode_t;
	
typedef struct {
	int player_pos[MAX_PLAYER];
	int player_credit[MAX_PLAYER];
	char player_name[MAX_PLAYER][MAX_CHARNAME];
	int player_energy[MAX_PLAYER];
	int flag_graduated[MAX_PLAYER];
	int is_experimenting[MAX_PLAYER]; 
  int exp_success_value[MAX_PLAYER];
	smmGradeNode_t *grade_history_head[MAX_PLAYER]; // pointer
} smm_player_t;

static 	smm_player_t smm_players;
static int smm_PlayerNr = 0;
static int smm_nodeNr = 0;

static char smmNodeName[MAX_NODETYPE][MAX_CHARNAME] = { // array size [7][200]
						"lecture",
						"restaurant",
						"laboratory",
						"home",
						"gotlab",
						"foodchange",
						"festival"
};

static char smmGradeName[MAX_GRADE][MAX_CHARNAME] = { // array size [7][200]
						"A+",
						"A0",
						"A-",
						"B+",
						"B0",
						"B-",
						"C+",
						"C0",
						"C-",
						"D+",
						"D0",
						"D-",
						"F"
};

static smmObj_object_t *smmObj_board[MAX_NODENR];

// 1. PlayerNr setting and return
void smmObj_updatePlayerNr(int n) {
	smm_PlayerNr = n;
	}
	
int smmObj_getPlayerNr(void) {
	return smm_PlayerNr;
	}

void smmObj_initPlayerFields(int player, int initEnergy)
{
    smm_players.player_pos[player] = 0;
    smm_players.player_credit[player] = 0;
    smm_players.player_energy[player] = initEnergy;
    smm_players.flag_graduated[player] = 0;
    smm_players.grade_history_head[player] = NULL; 
    smm_players.is_experimenting[player] = 0;
    smm_players.exp_success_value[player] = 0;
}

// 2. Initialization and update function
void smmObj_updatePlayerPos(int player, int pos) {
    smm_players.player_pos[player] = pos;
}

void smmObj_updatePlayerCredit(int player, int credit) {
    smm_players.player_credit[player] += credit;
}

void smmObj_updatePlayerEnergy(int player, int energy) {
    smm_players.player_energy[player] += energy;
}

void smmObj_updateGraduatedFlag(int player, int flag) {
    smm_players.flag_graduated[player] = flag;
}

// 3. Get Function
char* smmObj_getPlayerName(int player) {
    return smm_players.player_name[player]; 
}

void smmObj_setPlayerName(int player, char* name) {
    strcpy(smm_players.player_name[player], name);
}

int smmObj_getPlayerPos(int player) {
    return smm_players.player_pos[player];
}

int smmObj_getPlayerCredit(int player) {
    return smm_players.player_credit[player];
}

int smmObj_getPlayerEnergy(int player) {
    return smm_players.player_energy[player];
}

int smmObj_getGraduatedFlag(int player) {
    return smm_players.flag_graduated[player];
}

//object generation
int smmObj_genNode(char* name, int type, int credit, int energy)
{
	if (smm_nodeNr >= MAX_NODENR) {
		return -1;
	}
	
	smmObj_board[smm_nodeNr] = (smmObj_object_t*)malloc(sizeof(smmObj_object_t));
	if (smmObj_board[smm_nodeNr] ==NULL) {
		return -1;
	}
	
	strcpy(smmObj_board[smm_nodeNr]->smm_name, name);
  smmObj_board[smm_nodeNr]->smm_type = type;
  smmObj_board[smm_nodeNr]->smm_credit = credit;
	smmObj_board[smm_nodeNr]->smm_energy = energy;
	smmObj_board[smm_nodeNr]->smm_grade = 0; // initialization
  
  
  smm_nodeNr++;
    
  return (smm_nodeNr);

}



//member retrieving
char* smmObj_getNodeName(int node_nr)
{
	return(smmObj_board[node_nr]->smm_name);
}

int smmObj_getNodeType(int node_nr)
{
	return(smmObj_board[node_nr]->smm_type);
}

int smmObj_getNodeCredit(int node_nr)
{
	return(smmObj_board[node_nr]->smm_credit);
}

int smmObj_getNodeEnergy(int node_nr)
{
	return(smmObj_board[node_nr]->smm_energy);
}


//element to string
char* smmObj_getTypeName(smmNode_e type)
{
    return smmNodeName[type];
}

char* smmObj_getGradeName(smmGrade_e grade)
{
    return smmGradeName[grade];
}

smmGrade_e smmObj_getRandomGrade(void)
{
    // 0 ~ 8 number random generate and return
    int random_val = rand() % MAX_ACHIEVABLE_GRADE;
    return (smmGrade_e)random_val;
}

void smmObj_addGradeToHistory(int player, char *lectureName, int credit, smmGrade_e grade)
{
    smmGradeNode_t *newNode = (smmGradeNode_t *)malloc(sizeof(smmGradeNode_t));
    if (newNode == NULL) {
        return; 
    }
    
    strcpy(newNode->lecture_name, lectureName);
    newNode->credit = credit;
    newNode->grade = grade;
    newNode->next = NULL;
    
    int player_idx = player;
    newNode->next = smm_players.grade_history_head[player_idx];
    smm_players.grade_history_head[player_idx] = newNode;
}

// Search Lecture in History
smmGradeNode_t* smmObj_findLectureGrade(int player, char *lectureName)
{
    int player_idx = player;
    smmGradeNode_t *current = smm_players.grade_history_head[player_idx];
    
    while (current != NULL) {
        if (strcmp(current->lecture_name, lectureName) == 0) {
            return current; // Find. Pointer Return
        }
        current = current->next;
    }
    
    return NULL; // 찾지 못함
}


// EXP STATE Getter/Setter 
int smmObj_getExpFlag(int player) {
    return smm_players.is_experimenting[player];
}

void smmObj_updateExpFlag(int player, int flag) {
    smm_players.is_experimenting[player] = flag;
}

int smmObj_getExpValue(int player) {
    return smm_players.exp_success_value[player];
}

void smmObj_updateExpValue(int player, int value) {
    smm_players.exp_success_value[player] = value;
}
