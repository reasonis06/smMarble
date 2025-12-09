//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include <time.h>
#include <string.h>
//#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

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
#if 0
int isGraduated(void); //check if any player is graduated
void printGrades(int player); //print grade history of the player
float calcAverageGrade(int player); //calculate average grade of the player
smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history
void printGrades(int player); //print all the grade history of the player
#endif

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
		
		printf("Input %i-th player name:");
		scanf("%s", smm_players.player_name[i]);

	}
}


void goForward(int player, int step)
{
	int i;
	
	/*
	smm_player_t *smm_players;
	
	smm_players = malloc(player*sizeof(smm_player_t));
	
	if (smm_players == NULL) {
        perror("malloc failed");
        exit(1);
  }
  */
  
	//player_pos[player] = player_pos[player]+step;
	printf("start from %i(%s) (%i)\n", smm_players.player_pos[i], smmObj_getName(smm_players.player_pos[i]), player_step);
	for (i=0;i<step;i++)
	{
		smm_players.player_pos[player] = (smm_players.player_pos[player]+1) % board_nr;
		printf("	=> moved to %i(%s)\n", smm_players.player_pos[player], smmObj_getName(smm_players.player_pos[i]));


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
						smmObj_getName(smm_players.player_pos[i]),
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
	int type = smmObj_getNodeType(smm_players.player_pos[player]);
  int credit;
  int energy;
		
	switch(type)
  {
		case SMMNODE_TYPE_LECTURE:
		{
				credit = smmObj_getNodeCredit(player);
				energy = smmObj_getNodeEnergy(player);
				smm_players.player_credit[player] += credit;
				smm_players.player_energy[player] -= energy;
				break;
		}
			case SMMNODE_TYPE_RESTAURANT:
				break;
				
			case SMMNODE_TYPE_LABORATORY:
				break;
				
			case SMMNODE_TYPE_HOME:
				if(smm_players.player_credit[player] <= GRADUATE_CREDIT)
				{
					smm_players.flag_graduated[player] = 1;
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
    int cnt;
    int pos;
    int turn;
    
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
        printf("Input player number");
        scanf("%i", &player_nr);
        fflush(stdin); // buffer delete
        
        if (player_nr <= 0 || player_nr > MAX_PLAYER)
	    		printf("Invalid player number!\n");
    }
	  while(player_nr <= 0 || player_nr > MAX_PLAYER);
	  
    generatePlayers(player_nr, smmObj_getNodeEnergy(0));
    

    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (cnt<5) //is anybody graduated?
    {
        int dice_result;
        
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. dice rolling (if not in experiment)
        
        
        //4-3. go forward
        goForward(turn, dice_result);
        pos = pos + rand()%6+1;
        printf("node: %s, type: %i (%s)\n", smmObj_getNodeName(pos), smmObj_getType(pos),smmObj_getNodeNameType(pos));

		//4-4. take action at the destination node of the board
        //actionNode();
        
        //4-5. next turn
        cnt++;
        turn = (turn + 1) % player_nr;
        
    }
    
    //free(smm_player);
    
    system("PAUSE");
    return 0;
}
