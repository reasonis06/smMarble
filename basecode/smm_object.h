//
//  smm_object.h
//  SMMarble object
//
//  Created by Seongeun Lee on 2025/12/10.
//

#ifndef smm_object_h
#define smm_object_h

#define MAX_CHARNAME               			 200


typedef struct {
	char smm_name[MAX_CHARNAME];// array size [100][200]
	int smm_type;
	int smm_credit;
	int smm_energy;
	int smm_grade;
} smmObj_object_t;

struct smmGradeNode;

typedef enum {
    SMMNODE_TYPE_LECTURE = 0,
    SMMNODE_TYPE_RESTAURANT,
    SMMNODE_TYPE_LABORATORY,
    SMMNODE_TYPE_HOME,
    SMMNODE_TYPE_GOTOLAB,
    SMMNODE_TYPE_FOODCHANGE,
    SMMNODE_TYPE_FESTIVAL,
} smmNode_e;


typedef enum {
	GRADE_A_PLUS = 0,
	GRADE_A_ZERO ,
	GRADE_A_MINUS,
	GRADE_B_PLUS,
	GRADE_B_ZERO,
	GRADE_B_MINUS,
	GRADE_C_PLUS,
	GRADE_C_ZERO,
	GRADE_C_MINUS,
	GRADE_D_PLUS,
	GRADE_D_ZERO,
	GRADE_D_MINUS,
	GRADE_F
}smmGrade_e;



//object generation
int smmObj_genNode(char* name, int type, int credit, int energy);

// PlayerNr setting and return
void smmObj_updatePlayerNr(int n);
int smmObj_getPlayerNr(void);

// Player Initialization
void smmObj_initPlayerFields(int player, int initEnergy);

//member retrieving
char* smmObj_getNodeName(int node_nr); // return node name
int smmObj_getNodeType(int node_nr); // return node type
int smmObj_getNodeCredit(int node_nr); // return credit
int smmObj_getNodeEnergy(int node_nr); // return energy

// Player Update functions 
void smmObj_updatePlayerPos(int player, int pos);
void smmObj_updatePlayerCredit(int player, int credit);
void smmObj_updatePlayerEnergy(int player, int energy);
void smmObj_updateGraduatedFlag(int player, int flag);
char* smmObj_getPlayerName(int player);
void smmObj_updateExpFlag(int player, int flag);
void smmObj_updateExpValue(int player, int value);

// Player Set functions 
void smmObj_setPlayerName(int player, char* name);
int smmObj_getPlayerPos(int player);
int smmObj_getPlayerCredit(int player);
int smmObj_getPlayerEnergy(int player);
int smmObj_getGraduatedFlag(int player);
int smmObj_getExpFlag(int player);
int smmObj_getExpValue(int player);


//element to string
char* smmObj_getTypeName(smmNode_e type); // return node type name(char*)
char* smmObj_getGradeName(smmGrade_e grade);

smmGrade_e smmObj_getRandomGrade(void);
void smmObj_addGradeToHistory(int player, char *lectureName, int credit, smmGrade_e grade);
struct smmGradeNode* smmObj_findLectureGrade(int player, char *lectureName);
struct smmGradeNode* smmObj_getGradeHistoryHead(int player);

// History prototype
char* smmObj_getHistoryLectureName(struct smmGradeNode* node);
int smmObj_getHistoryCredit(struct smmGradeNode* node);
smmGrade_e smmObj_getHistoryGrade(struct smmGradeNode* node);
struct smmGradeNode* smmObj_getNextHistoryNode(struct smmGradeNode* node);

struct smmGradeNode* smmObj_findLectureGrade(int player, char *lectureName);

#endif/* smm_object_h */
