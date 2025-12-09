//
//  smm_node.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODENR											100
#define MAX_NODETYPE   									7
#define MAX_GRADE      									13

#define SMMNODE_TYPE_LECTURE						0
#define SMMNODE_TYPE_RESTAURANT					1
#define SMMNODE_TYPE_LABORATORY					2
#define SMMNODE_TYPE_HOME								3
#define SMMNODE_TYPE_GOTOLAB						4
#define SMMNODE_TYPE_FOODCHANGE					5
#define SMMNODE_TYPE_FESTIVAL						6

#define GRADE_A+												0
#define GRADE_A0												1
#define GRADE_A-												2
#define GRADE_B+												3
#define GRADE_B0												4
#define GRADE_B-												5
#define GRADE_C+												6
#define GRADE_C0												7
#define GRADE_C-												8
#define GRADE_D+												9
#define GRADE_D0												10
#define GRADE_D-												11
#define GRADE_F													12





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

static char smmNodegradeName[MAX_GRADE][MAX_CHARNAME] = { // array size [7][200]
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

//object generation
int smmObj_genNode(char* name, int type, int credit, int energy)
{
	if (smm_nodeNr >= MAX_NODENR) {
		printf("[ERROR] Board node limit reached. \n");
		exit(1);
	}
	
	smmObj_board[smm_nodeNr] = (smmObj_object_t*)malloc(sizeof(smmObj_object_t));
	if (smmObj_board[smm_nodeNr] ==NULL) {
		printf("[ERROR] malloc failed");
		exit(1);
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

char* smmObj_getTypeName(int node_type)
{
	return (smmNodeName[node_type]);
}

#if 0
//element to string
char* smmObj_getNodeName(smmNode_e type);
{
    return smmNodeName[type];
}

char* smmObj_getGradeName(smmGrade_e grade)
{
    return smmGradeName[grade];
}
#endif

