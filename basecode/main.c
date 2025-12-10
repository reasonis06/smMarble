//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
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

//구조체로 만들기 
//struct int smm_players[MAX_PLAYER];
	
typedef struct {
	int player_pos[MAX_PLAYER];
	int player_credit[MAX_PLAYER];
	char player_name[MAX_PLAYER][MAX_CHARNAME];
	int player_energy[MAX_PLAYER];
	int flag_graduated[MAX_PLAYER];

} smm_player_t;



//function prototypes
int isAnyoneGraduated(void); //check if any player is graduated

void printGrades(int player); //print grade history of the player

float calcAverageGrade(int player); //calculate average grade of the player

smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)

void* findGrade(int player, char *lectureName); //find the grade from the player's grade history


static 	smm_player_t smm_players;

void generatePlayers(int n, int initEnergy) //generate a new player
{
	int i;
	
	for (i=0;i<n;i++)
	{
		smm_players.player_pos[i] = 0;
		smm_players.player_credit[i] = 0;
		smm_players.player_energy[i] = initEnergy;
		smm_players.flag_graduated[i] =0;
		
		printf("Input %i-th player name:", i);
		scanf("%s", smm_players.player_name[i]);

	}
}


void goForward(int player, int step)
{
	int i;
  
	//player_pos[player] = player_pos[player]+step;
	printf("start from %i(%s) (%i)\n", smm_players.player_pos[player], smmObj_getNodeName(smm_players.player_pos[player]), player_step);
	for (i=0;i<step;i++)
	{
		smm_players.player_pos[player] = (smm_players.player_pos[player]+1) % board_nr;
		printf("	=> moved to %i(%s)\n", smm_players.player_pos[player], smmObj_getNodeName(smm_players.player_pos[i]));


	}
}

void printPlayerStatus(void)
{
	int i;
	int n = player_nr;
	for (i=0;i<n;i++)
	{
		printf("%s - position:%i(%s), credit:%i, energy:%i\n",
						smm_players.player_name[i],
						smm_players.player_pos[i],
						smmObj_getNodeName(smm_players.player_pos[i]),
						smm_players.player_credit[i],
						smm_players.player_energy[i]);

	}
}



int rolldice(int player)
{
    char c;
    printf(" Press any key to roll a dice (press g to see grade): ");
    c = getchar();
    fflush(stdin);
    
    if (c == 'g')
        printGrades(player);

    
    return (rand()%MAX_DICE + 1);
}


//action code when a player stays at a node
void actionNode(int player)
{
	int current_pos = smm_players.player_pos[player];
	int type = smmObj_getNodeType(current_pos);
  int credit;
  int energy;
		
	switch(type)
  {
		case SMMNODE_TYPE_LECTURE:
		{
				credit = smmObj_getNodeCredit(current_pos);
				energy = smmObj_getNodeEnergy(current_pos);
				smm_players.player_credit[player] += credit;
				smm_players.player_energy[player] -= energy;
				break;
		}
			case SMMNODE_TYPE_RESTAURANT:
				energy = smmObj_getNodeEnergy(player);
				smm_players.player_energy[player] += energy;
				break;
				
			case SMMNODE_TYPE_LABORATORY:
				break;
				
			case SMMNODE_TYPE_HOME:
				if(smm_players.player_credit[player] >= GRADUATE_CREDIT)
				{
					smm_players.flag_graduated[player] = 1;
					printf("Congratulation! %s is graduated!\n", smm_players.player_name[player]);
				}
				break;
				
			case SMMNODE_TYPE_GOTOLAB:
				break;
				
			case SMMNODE_TYPE_FOODCHANGE:
				break;
				
			case SMMNODE_TYPE_FESTIVAL:
				break;

        //case lecture:
        default:
            break;
    }
}



int main(int argc, const char * argv[]) 
{
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int pos=0;
    int turn=0;
    
    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;
    
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while ( fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4 ) //read a node parameter set
    {
        //store the parameter set
        //printf("%s %i %i %i", name, type, credit, energy);
        board_nr = smmObj_genNode(name, type, credit, energy);

    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);
    
    
    
    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    //while () //read a food parameter set
    {
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of food cards : %i\n", food_nr);
    
    
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    //while () //read a festival card string
    {
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", festival_nr);

    
    
    //2. Player configuration ---------------------------------------------------------------------------------
    
    do
    {
        //input player number to player_nr
        printf("Input player number: ");
        scanf("%i", &player_nr);
        fflush(stdin); // buffer delete
        
        if (player_nr <= 0 || player_nr > MAX_PLAYER)
	    		printf("Invalid player number!\n");
    }
	  while(player_nr <= 0 || player_nr > MAX_PLAYER);
	  
    generatePlayers(player_nr, smmObj_getNodeEnergy(0));
    

    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (isAnyoneGraduated() == 0) //is anybody graduated?
    {
        int dice_result;
        
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. dice rolling (if not in experiment)
        dice_result = rolldice(turn);
        
        //4-3. go forward
        goForward(turn, dice_result);
        printf("node: %s, type: %i (%s)\n", smmObj_getNodeName(pos), smmObj_getNodeType(pos),smmObj_getTypeName(pos));

		//4-4. take action at the destination node of the board
        actionNode(turn);
        
        //4-5. next turn
        turn = (turn + 1) % player_nr;
        
    }
        
    system("PAUSE");
    return 0;
}

// 1. isAnyoneGraduated function define
int isAnyoneGraduated(void)
{
	int i;
	for (i = 0; i < player_nr; i++) {
		if (smm_players.flag_graduated[i] == 1)
		{
			return 1; // anyone graduated
		}
	}
	return 0; // nobody graduated
}

// 2. printGrades function define
void printGrades(int player)
{
    // 해당 플레이어의 학점 이력을 출력하는 로직 구현
    printf("--- Player %d Grade History ---\n", player);
    // ... 학점 출력 로직 ...
}

// 3. calcAverageGrade 함수 정의
float calcAverageGrade(int player)
{
    // 평균 학점을 계산하는 로직 구현
    // 현재는 미구현이므로 임시로 0.0f 반환
    return 0.0f; 
}

// 4. takeLecture 함수 정의
smmGrade_e takeLecture(int player, char *lectureName, int credit)
{
    // 학점을 처리하고 등급을 부여하는 로직 구현
    // smm_players 구조체에 학점 이력 추가, smm_players.player_credit 업데이트 등
    // ...
    // 임시로 A+ 반환
    return GRADE_A_PLUS; 
}

// 5. findGrade 함수 정의
void* findGrade(int player, char *lectureName)
{
    // 특정 강의의 학점을 찾아 반환하는 로직 구현
    // 현재는 미구현이므로 임시로 NULL 반환
    return NULL;
}
