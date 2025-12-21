//
//  main.c
//  SMMarble
//
//  Created by Seongeun Lee on 2025/12/21.
//

#include <time.h>
#include <string.h>
#include "smm_common.h"
#include "smm_database.h"
#include "smm_object.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"

#define SMMNODE_TYPE_LECTURE						0
#define SMMNODE_TYPE_RESTAURANT					1
#define SMMNODE_TYPE_LABORATORY					2
#define SMMNODE_TYPE_HOME								3
#define SMMNODE_TYPE_GOTOLAB						4
#define SMMNODE_TYPE_FOODCHANGE					5
#define SMMNODE_TYPE_FESTIVAL						6

//board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;
static int player_nr;
static int player_step;

//function prototypes
int isAnyoneGraduated(void); //check if any player is graduated
void printGrades(int player); //print grade history of the player
float calcAverageGrade(int player); //calculate average grade of the player
smmGrade_e takeLecture(int player, char *lectureName, int credit, int energy); //take the lecture (insert a grade of the player)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history

void generatePlayers(int n, int initEnergy) //generate a new player
{
	int i;
	smmObj_updatePlayerNr(n);

	for (i=0;i<n;i++)
	{
		smmObj_initPlayerFields(i, initEnergy);
		smmObj_updateExpFlag(i, 0);
		smmObj_updateExpValue(i, 0);
		
		printf("Input %i-th player name:", i);
		char name_buffer[MAX_CHARNAME];
		scanf("%s", name_buffer);
		smmObj_setPlayerName(i, name_buffer);
	}
}

void goForward(int player, int step)
{
	int i;
	int pos = smmObj_getPlayerPos(player);
  
	printf("start from %i(%s) (%i)\n", pos, smmObj_getNodeName(pos), player_step);
	
	for (i=0;i<step;i++)
	{
		pos = (pos+1) % board_nr;
		printf("	=> moved to %i(%s)\n", pos, smmObj_getNodeName(pos));
	}
	smmObj_updatePlayerPos(player, pos);
}

void printPlayerStatus(void)
{
	int i;
	int n = player_nr;
	for (i=0;i<n;i++)
	{
		int pos = smmObj_getPlayerPos(i);
		printf("%s - position: %i(%s), credit: %i, energy: %i\n",
						smmObj_getPlayerName(i),
						pos,
						smmObj_getNodeName(pos),
						smmObj_getPlayerCredit(i),
						smmObj_getPlayerEnergy(i));
	}
}

int rolldice(int player)
{
    char c;
    printf("\n[%s's Turn] Press any key to roll a dice (press 'g' to see grade history): ", 
            smmObj_getPlayerName(player));
    
    c = getchar();
    fflush(stdin);
    
    if (c == 'g' || c == 'G') {
        printGrades(player);
        printf("Press any key to continue with the dice roll...");
        getchar();
        fflush(stdin);
    }
    
    return (rand() % MAX_DICE + 1);
}

//action code when a player stays at a node
void actionNode(int player)
{
	int pos = smmObj_getPlayerPos(player);
	int type = smmObj_getNodeType(pos);
	int credit;
	int energy;
	int target = (rand() % MAX_DICE + 1);
		
	switch(type)
	{
			case SMMNODE_TYPE_LECTURE:
			{
				char* lectureName = smmObj_getNodeName(pos);
				credit = smmObj_getNodeCredit(pos);
				energy = smmObj_getNodeEnergy(pos);
				takeLecture(player, lectureName, credit, energy);
				break;
			}
			case SMMNODE_TYPE_RESTAURANT:
			{
				int current_energy = smmObj_getPlayerEnergy(player);
				energy = smmObj_getNodeEnergy(pos);
				smmObj_updatePlayerEnergy(player, energy);
				
				printf("\n>>> [RESTAURANT: %s] <<<\n\n", smmObj_getNodeName(pos));
				printf(" -> %s gained %i energy. Energy: %i -> %i\n", 
					smmObj_getPlayerName(player), energy, current_energy, smmObj_getPlayerEnergy(player));
				break;
			}
				
			case SMMNODE_TYPE_LABORATORY:
			{
				int is_exp = smmObj_getExpFlag(player);
				int energy_cost = smmObj_getNodeEnergy(pos);
    
				printf("\n>>> [LABORATORY] <<<\n\n");

				if (is_exp == 1)
				{
					int target_val = smmObj_getExpValue(player);
					int dice = rolldice(player);

					smmObj_updatePlayerEnergy(player, -energy_cost);
					printf(" -> %s spent %i energy for the experiment. Current Energy: %i.\n", 
						smmObj_getPlayerName(player), energy_cost, smmObj_getPlayerEnergy(player));

					if (dice >= target_val)
					{
						smmObj_updateExpFlag(player, 0); 
						smmObj_updateExpValue(player, 0);
						printf(" -> EXPERIMENT SUCCESS! Dice: %i (Target: %i). Experiment finished.\n", dice, target_val);
					}
					else
					{
						printf(" -> EXPERIMENT FAILED. Dice: %i (Target: %i). Experiment status maintained.\n", dice, target_val);
					}
				}
				else
				{
					printf(" -> %s just passing through the Laboratory. No action taken.\n", smmObj_getPlayerName(player));
				}
				break;
			}
				
			case SMMNODE_TYPE_HOME:
			{
				int home_energy = smmObj_getNodeEnergy(pos);
				int current_energy = smmObj_getPlayerEnergy(player);
				smmObj_updatePlayerEnergy(player, home_energy);

				printf("\n>>> [HOME] <<<\n\n");
				printf(" -> %s arrived at Home. Gained %i energy. Energy: %i -> %i\n", 
					smmObj_getPlayerName(player), home_energy, current_energy, smmObj_getPlayerEnergy(player));

				if(smmObj_getPlayerCredit(player) >= GRADUATE_CREDIT)
				{
					smmObj_updateGraduatedFlag(player, 1);
					printf("Congratulation! %s is graduated and the game will end after this turn!\n", smmObj_getPlayerName(player));
				}
				break;
			}
				
			case SMMNODE_TYPE_GOTOLAB:
			{
				printf("\nGo to Laboratory!\n");
				smmObj_updatePlayerPos(player, 8);
				smmObj_updateExpFlag(player, 1);
				smmObj_updateExpValue(player, target);
				printf(" -> Experiment mode ON! Target dice number: %d\n", target);
				break;
			}
				
			case SMMNODE_TYPE_FOODCHANGE:
			{
				printf("\n>>> [FOOD CHANGE!] <<<\n\n");
				int card_idx = rand() % smmdb_len(LISTNO_FOODCARD); 
				char* foodName = smmDb_getFoodCardName(card_idx);
				int foodEnergy = smmDb_getFoodCardEnergy(card_idx);
				int current_energy = smmObj_getPlayerEnergy(player);
				smmObj_updatePlayerEnergy(player, foodEnergy);
				printf(" -> %s drew the card [%s]! Gained %i energy. Energy: %i -> %i\n", 
					smmObj_getPlayerName(player), foodName, foodEnergy, current_energy, smmObj_getPlayerEnergy(player));
				break;
			}
				
			case SMMNODE_TYPE_FESTIVAL:
			{
				printf("\n>>> [ENJOY FESTIVAL!] <<<\n\n");
				int card_idx = rand() % smmdb_len(LISTNO_FESTCARD);
				char* mission = smmDb_getFestivalCardMission(card_idx);
				printf(" -> %s drew the card: \"%s\"\n", smmObj_getPlayerName(player), mission);
				break;
			}

			default:
				printf("\n>>> [Unknown Node Type %i] <<<\n\n", type);
				break;
	}
}

int main(int argc, const char * argv[]) 
{
    FILE* fp;
    char name[MAX_CHARNAME];
    int type, credit, energy;
    int turn=0;
    
    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;
    srand(time(NULL));
    
    //1. import parameters ---------------------------------------------------------------------------------
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL) 
    {
        printf("[ERROR] failed to open %s.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while ( fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4 ) 
    {
        void* newNode = smmObj_genObject(name, type, credit, energy, NULL);
        smmdb_addTail(LISTNO_NODE, newNode);
        board_nr++;
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);
    
    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s.\n", FOODFILEPATH);
        return -1;
    }
    printf("\n\nReading food card component......\n");
    char foodName[MAX_CHARNAME];
    int foodEnergy;
    while (fscanf(fp, "%s %i", foodName, &foodEnergy) == 2) 
    {
        food_nr = smmDb_genFoodCard(foodName, foodEnergy);
    }
    fclose(fp);
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s.\n", FESTFILEPATH);
        return -1;
    }
    printf("\n\nReading festival card component......\n");
    char festMission[MAX_CHARNAME*3];
    while (fgets(festMission, sizeof(festMission), fp) != NULL) 
    {
        size_t len = strlen(festMission);
        if (len > 0 && festMission[len-1] == '\n') festMission[len-1] = '\0';
        festival_nr = smmDb_genFestivalCard(festMission);
    }
    fclose(fp);

    //2. Player configuration ---------------------------------------------------------------------------------
    do {
        printf("Input player number: ");
        scanf("%i", &player_nr);
        fflush(stdin);
        if (player_nr <= 0 || player_nr > MAX_PLAYER) printf("Invalid player number!\n");
    } while(player_nr <= 0 || player_nr > MAX_PLAYER);
	  
    generatePlayers(player_nr, smmObj_getNodeEnergy(0));

    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (isAnyoneGraduated() == 0)
    {
        printPlayerStatus();
        if (smmObj_getExpFlag(turn) == 1) {
            int dice_result = rolldice(turn);
            int target = smmObj_getExpValue(turn);
            if (dice_result >= target) {
                smmObj_updateExpFlag(turn, 0);
                printf("Success! You can move next turn.\n");
            } else printf("Failed to escape laboratory.\n");
        } else {
            int dice_result = rolldice(turn);
            goForward(turn, dice_result);
            actionNode(turn);
        }
        turn = (turn + 1) % player_nr;
    }
    
    int i;
    for (i = 0; i < player_nr; i++) {
        if (smmObj_getGraduatedFlag(i)) {
            printf("\n========================================\n");
            printf("  GRADUATION CONGRATULATIONS! : %s\n", smmObj_getPlayerName(i));
            printf("  Total Credit: %d\n", smmObj_getPlayerCredit(i));
            printGrades(i);
            printf("========================================\n");
        }
    }
    system("PAUSE");
    return 0;
}

int isAnyoneGraduated(void) {
	int i;
	for (i = 0; i < player_nr; i++) {
		if (smmObj_getGraduatedFlag(i) == 1) return 1;
	}
	return 0;
}

void printGrades(int player) {
    smmObj_t* current = smmObj_getGradeHistoryHead(player);
    if (current == NULL) {
        printf("-----------------No History-----------------\n");
        return;
    }
    printf("----------- Player %d Grade History -----------\n", player);
    printf("%-20s %-10s %-10s\n", "lecture", "credit", "grade");
    while (current != NULL) {
        printf("%-20s %-10d %-10s\n", 
               smmObj_getHistoryLectureName(current),
               smmObj_getHistoryCredit(current), 
               smmObj_getGradeName(smmObj_getHistoryGrade(current)));
        current = smmObj_getNextHistoryNode(current);
    }
}

float calcAverageGrade(int player) {
    smmObj_t* current = smmObj_getGradeHistoryHead(player);
    int total_credits = 0;
    float total_grade_points = 0.0f;
    if (current == NULL) return 0.0f;
    while (current != NULL) {
        int credit = smmObj_getHistoryCredit(current);
        smmGrade_e grade = smmObj_getHistoryGrade(current);
        float grade_value = 0.0f;
        switch(grade) {
            case GRADE_A_PLUS: grade_value = 4.3f; break;
            case GRADE_A_ZERO: grade_value = 4.0f; break;
            case GRADE_A_MINUS: grade_value = 3.7f; break;
            case GRADE_B_PLUS: grade_value = 3.3f; break;
            case GRADE_B_ZERO: grade_value = 3.0f; break;
            case GRADE_B_MINUS: grade_value = 2.7f; break;
            case GRADE_C_PLUS: grade_value = 2.3f; break;
            case GRADE_C_ZERO: grade_value = 2.0f; break;
            case GRADE_C_MINUS: grade_value = 1.7f; break;
            default: grade_value = 0.0f; break;
        }
        total_grade_points += (grade_value * credit);
        total_credits += credit;
        current = smmObj_getNextHistoryNode(current);
    }
    return (total_credits == 0) ? 0.0f : (total_grade_points / total_credits);
}

smmGrade_e takeLecture(int player, char *lectureName, int credit, int energy) {
	char choice;
	printf("\n>>> [LECTURE: %s (%i Credit)] <<<\n\n", lectureName, credit);
	if (smmObj_findLectureGrade(player, lectureName) != NULL) {
        printf("%s: Already taken this lecture.\n\n", smmObj_getPlayerName(player));
        return GRADE_F;
    }
    if (smmObj_getPlayerEnergy(player) < energy) {
        printf("%s: Not enough energy.\n\n", smmObj_getPlayerName(player));
        return GRADE_F;
    }
  	printf("%s: Take lecture (%i energy)? (y/n): ", smmObj_getPlayerName(player), energy);
    choice = getchar(); fflush(stdin);
    if (choice == 'y' || choice == 'Y') {
        smmGrade_e final_grade = smmObj_getRandomGrade();
        smmObj_updatePlayerCredit(player, credit);
        smmObj_updatePlayerEnergy(player, -energy);
        smmObj_addGradeToHistory(player, lectureName, credit, final_grade);
        return final_grade;
    }
    return GRADE_F;
}

void* findGrade(int player, char *lectureName) {
    return (void*)smmObj_findLectureGrade(player, lectureName);
}
