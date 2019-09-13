// Christian Jarani
// Professor Misurda
// CS 449, Project 4: Yahtzee using /dev/dice
// 4/10/16

/** Commenting done in Sublime Text. Formatting might look messier in other text editors, so, for readabilities sake, review in Sublime. **/

//TO DO: Add upper section bonus, figure out \dev\dice reading and implement in QEMU

#include <stdlib.h>
#include <stdio.h>
#include <time.h>    // Remove once /dev/dice is implemented

// Function Prototypes
int comp();
void upper_score();
void lower_score();

typedef struct scores
{
	int player_score;
	int upper_bonus;
	int upper_bonus_received;

	int upper_totals[6];	//0 = ones, 1 = twos, 2 = threes, 3 = fours, 4 = fives, 5 = sixes
	int upper_used[6];

	int lower_totals[7];	//0 == three of a kind, 1 == fours of a kind, 2 == small straight, 3 == large straight, 4 == full house, 5 == yahtzee, 6 == chance
	int lower_used[7];
};

struct scores Score = {0,0,0,{0,0,0,0,0,0},{0,0,0,0,0,0},{0,0,0,0,0,0,0},{0,0,0,0,0,0,0}};

int main()
{
   // Game Logic Variables

	int num_turns = 0;        // Number of turns played
	int roll_count = 0;      // Number of rerolls during a player's turn (maximum of 3)
	int dice[5];		    // Player's current dice
	int count;			   // Counter used in multiple loops
	char input[10];	      // Holds user input
	int selection;	     // Holds choice of section to place dice values in
	char temp_rand[5];  // Stores the random bytes from /dev/dice

	FILE *dev_dice = fopen("/dev/dice","rb"); // Open /dev/dice

   // Game Logic Loop
	while(num_turns < 13)
	{
		printf("Your roll: \n\n"); 							 // Read random numbers in from /dev/dice
		fread(temp_rand,1,5,dev_dice);

		for(count = 0; count < 5; count++)
			dice[count] = atoi(&temp_rand[count]);				// Convert 1 byte values from dev_dice into integers
															   //
		qsort(dice,5,4,comp);    							  // Sort dice from least to greatest
												   			 //
		for(count = 0; count < 5; count++)					//
			printf("%d ", dice[count]);					   // Print out each dice values
														  //
	   													 //
		while(1){										//============================// START: Reroll Loop
			fflush(stdin);															 // Clean up the buffer
			printf("\n\nWhich dice to reroll? ");					  			 	//
		    fgets(input, 10, stdin);											   // Read reroll choice(s) from player
		    if(atoi(&input[0]) == 0)							  				  //
		    {																	 //
		    	roll_count = 0;													//
		    	break;														   //
		    }			  											  		  // If 0 entered, no dice requested to reroll   
		    count = 0;														 // Reinitialize count
				while(input[count] && input[count] != '\n')		  			// While input to read and not the end of input
				{												 		   //
					int roll_again = atoi(&input[count]);				  // Convert char input to int for easier processing
					if(input[count] == ' ' || roll_again == 0){	 		 // Skip parsing input if whitespace or 0
						count++;						   				// Increment array address
						continue;						  			   //
					}									 			  //
					fread(temp_rand,1,1,dev_dice);	 		 		 // Read in random value from /dev/dice
					dice[roll_again-1] = atoi(&temp_rand[0]);		// Convert value to an integer
					count++;						   			   //
				}									  			  //
													 			 //==========//
			roll_count++;												    // Increment reroll count
			qsort(dice,5,4,comp);           							   // Sort dice from least to greatest
																	      //
			if(roll_count == 1) printf("\nYour second roll: \n\n");    	 // If rolled once
			if(roll_count == 2) printf("\nYour third roll: \n\n");    	// If rolled twice
			for(count = 0; count < 5; count++)						   // Print out each dice values	
			{								   //=====================//
				printf("%d ", dice[count]);   //
			}   						     //
			if(roll_count == 2)		   		// If player has rerolled twice, end rerolling
			{							   //
				roll_count = 0;	      	  // Reinitialize roll count
				break;				     //
			}					        //
		}							   // END: Reroll Loop
									  //=========================================================//
	   // Prompt for choice of section 															//
		printf("\n\nPlace dice into:\n1) Upper Section\n2) Lower Section\n\nSelection? ");	   // START: Section Processing
		fgets(input,10,stdin);																  //
		sscanf(input,"%d",&selection);														 // Read choice of section
																							// 
	   // Upper Section selected														   //
		if(selection == 1)														  	  	  //==========================//
		{																											 // 
			printf("\nPlace die into:\n1) Ones\n2) Twos\n3) Threes\n4) Fours\n5) Fives\n6) Sixes\n\nSelection? ");  // Prompt for choice of category
			fgets(input,10,stdin);																				   //
			sscanf(input,"%d",&selection);																		  // Read choice of category
			upper_score(dice,selection);													 				 	 // Process dice in chosen category and add to total score
		}																										// Calculate upper section score
	   // Lower Section selected																			   //
		else if(selection == 2)																		      	  //
		{																									 //
			printf("\nPlace die into:\n1) Three-of-a-Kind\n2) Four-of-a-Kind\n3) Full House");				// Prompt for choice of category
			printf("\n4) Small Straight\n5) Large Straight\n6) Yahtzee\n7) Chance\n\nSelection? ");		   //
			fgets(input,10,stdin);																		  //
			sscanf(input,"%d",&selection);																 // Read choice of category
			lower_score(dice,selection);																// Calculate lower section score
		}
		printf("\nYour score so far: %d\n\n", Score.player_score);

	   // Ones and Fours
		if(Score.upper_used[0] == 0 && Score.upper_used[3] == 0) printf("%-25s %-7s\n","Ones: ","Fours: ");
		else if(Score.upper_used[0] == 1 && Score.upper_used[3] == 0) printf("%-6s %-18d %-7s\n","Ones: ",Score.upper_totals[0],"Fours: ");
		else if(Score.upper_used[0] == 0 && Score.upper_used[3] == 1) printf("%-25s %-7s %-d\n","Ones: ","Fours: ",Score.upper_totals[3]);
		else printf("%-6s %-18d %-7s %-d\n","Ones: ",Score.upper_totals[0],"Fours: ",Score.upper_totals[3]);
	   // Twos and Fives
		if(Score.upper_used[1] == 0 && Score.upper_used[4] == 0) printf("%-25s %-7s\n","Twos: ","Fives: ");
		else if(Score.upper_used[1] == 1 && Score.upper_used[4] == 0) printf("%-6s %-18d %-7s\n","Twos: ",Score.upper_totals[1],"Fives: ");
		else if(Score.upper_used[1] == 0 && Score.upper_used[4] == 1) printf("%-25s %-7s %-d\n","Twos: ","Fives: ",Score.upper_totals[4]);
		else printf("%-6s %-18d %-7s %-d\n","Twos: ",Score.upper_totals[1],"Fives: ",Score.upper_totals[4]);
	   // Threes and Sixes
		if(Score.upper_used[2] == 0 && Score.upper_used[5] == 0) printf("%-25s %-7s\n","Threes: ","Sixes: ");
		else if(Score.upper_used[2] == 1 && Score.upper_used[5] == 0) printf("%-8s %-16d %-7s\n","Threes: ",Score.upper_totals[2],"Sixes: ");
		else if(Score.upper_used[2] == 0 && Score.upper_used[5] == 1) printf("%-25s %-7s %-d\n","Threes: ","Sixes: ",Score.upper_totals[5]);
		else printf("%-8s %-16d %-7s %-d\n","Threes: ",Score.upper_totals[2],"Sixes: ",Score.upper_totals[5]);
	   // Three-of-a-Kind and Four-of-a-Kind
		if(Score.lower_used[0] == 0 && Score.lower_used[1] == 0) printf("%-25s %-25s\n","Three of a Kind: ","Four of a Kind: ");
		else if(Score.lower_used[0] == 1 && Score.lower_used[1] == 0) printf("%-17s %-7d %-8s\n","Three of a Kind: ",Score.lower_totals[0],"Four of a Kind: ");
		else if(Score.lower_used[0] == 0 && Score.lower_used[1] == 1) printf("%-25s %-16s %-d\n","Three of a Kind: ","Four of a Kind: ",Score.lower_totals[1]);
		else printf("%-17s %-7d %-16s %3d\n","Three of a Kind: ",Score.lower_totals[0],"Four of a Kind: ",Score.lower_totals[1]);
		printf("%-22s %-d\n","Upper Section Bonus: ",Score.upper_bonus);
	   // Small Straight and Large Straight
		if(Score.lower_used[3] == 0 && Score.lower_used[4] == 0) printf("%-25s %-25s\n","Small Straight: ","Large Straight: ");
		else if(Score.lower_used[3] == 1 && Score.lower_used[4] == 0) printf("%-16s %-8d %-8s\n","Small Straight: ",Score.lower_totals[3],"Large Straight: ");
		else if(Score.lower_used[3] == 0 && Score.lower_used[4] == 1) printf("%-25s %-16s %-d\n","Small Straight: ","Large Straight: ",Score.lower_totals[4]);
		else printf("%-16s %-6d %-16s %-d\n","Small Straight: ",Score.lower_totals[3],"Large Straight: ",Score.lower_totals[4]);
       // Full House and Yahtzee
		if(Score.lower_used[2] == 0 && Score.lower_used[5] == 0) printf("%-25s %-25s\n","Full House: ","Yahtzee: ");
		else if(Score.lower_used[2] == 1 && Score.lower_used[5] == 0) printf("%-12s %-12d %-9s\n","Full House: ",Score.lower_totals[2],"Yahtzee: ");
		else if(Score.lower_used[2] == 0 && Score.lower_used[5] == 0) printf("%-25s %-9s %-d\n","Full House: ","Yahtzee: ",Score.lower_totals[5]);
		else printf("%-12s %-12d %-9s %-d\n","Full House: ",Score.lower_totals[2],"Yahtzee: ",Score.lower_totals[5]);
       // Chance
		if(Score.lower_used[6] == 0) printf("%-25s\n\n","Chance: ");
		else printf("%-8s %-d\n\n","Chance: ",Score.lower_totals[6]);

		num_turns++;		// Increment number of turns
	}

	return 0;				// Exit program
}
																	     
void upper_score(int dice[5], int category)								    			// Calculate score total for in chosen category
{																	   				   //
	int count;														  				  //
	int total_num = 0;												 				 // Total number of die matching chosen category
	for(count = 0; count < 5; count++)										    	// For each die rolled
	{															   				   //
		if(dice[count] == category) total_num++;				  			      // If die value matches category, add to total number found
	}															 			     //
	Score.player_score += total_num*category;									// Multiply total number found by value searched for to find score of category
															   				   //
	if(category == 1 && Score.upper_used[0] == 0){							  // If one's chosen, add score to one's category (same goes for other categories)
		Score.upper_totals[0] += total_num*category;
		Score.upper_used[0] = 1;
	}
	if(category == 2 && Score.upper_used[1] == 0){
		Score.upper_totals[1] += total_num*category;
		Score.upper_used[1] = 1;
	}
	if(category == 3 && Score.upper_used[2] == 0){
		Score.upper_totals[2] += total_num*category;
		Score.upper_used[2] = 1;
	}			
	if(category == 4 && Score.upper_used[3] == 0){
		Score.upper_totals[3] += total_num*category;
		Score.upper_used[3] = 1;
	}
	if(category == 5 && Score.upper_used[4] == 0){
		Score.upper_totals[4] += total_num*category;
		Score.upper_used[4] = 1;
	}
	if(category == 6 && Score.upper_used[5] == 0){
		Score.upper_totals[5] += total_num*category;
		Score.upper_used[5] = 1;
	}
	if( Score.upper_bonus_received == 0 && (Score.upper_used[0] + Score.upper_used[1] + Score.upper_used[2] + Score.upper_used[3] + Score.upper_used[4] + Score.upper_used[5] >= 63) )  // If all upper section categories are equal to or greater than 63
	{
		Score.upper_bonus = 35;
		Score.upper_bonus_received = 1;
		Score.player_score += Score.upper_bonus;		// Add 35 points to player score
	}
	return;
}

void lower_score(int dice[5], int category)
{
	int count;
   // Three of a Kind
	if(category == 1 && Score.lower_used[0] == 0)
	{
		if( (dice[0] == dice[1] && dice[1] == dice[2]) || (dice[1] == dice[2] && dice[2] == dice[3]) || (dice[2] == dice[3] && dice[3] == dice[4]) ){
			for(count = 0; count < 5; count++){
				Score.player_score += dice[count];
				Score.lower_totals[0] += dice[count];
			}
		}
		Score.lower_used[0] = 1;
	}
   // Four of a Kind
	if(category == 2 && Score.lower_used[1] == 0)
	{
		if( (dice[0] == dice[1] && dice[1] == dice[2] && dice[2] == dice[3]) || (dice[1] == dice[2] && dice[2] == dice[3]) && dice[3] == dice[4]){
			for(count = 0; count < 5; count++){
				Score.player_score += dice[count];
				Score.lower_totals[1] += dice[count];
			}
		}
		Score.lower_used[1] = 1;
	}
   // Full House
	if(category == 3 && Score.lower_used[2] == 0)
	{
		if( (dice[0] == dice[1] && dice[2] == dice[3] && dice[3] == dice[4])  || (dice[0] == dice[1] && dice[1] == dice[2] && dice[3] == dice[4]) ) {   
			Score.player_score += 25;
			Score.lower_totals[2] += 25;
		}
		Score.lower_used[2] = 1;
	}
   // Small Straight
	if(category == 4 && Score.lower_used[3] == 0)
	{
		if( (dice[0] == (dice[1] - 1) && (dice[1] - 1) == (dice[2] - 2) && (dice[2] - 2) == (dice[3] - 3)) || (dice[1] == (dice[2] - 1) && (dice[2] - 1) == (dice[3] - 2) && (dice[3] - 2) == (dice[4] - 3)) ){
			Score.player_score += 30;
			Score.lower_totals[3] += 30;
		}
		Score.lower_used[3] = 1;
	}
   // Large Straight
	if(category == 5 && Score.lower_used[4] == 0)
	{
		if(dice[0] == (dice[1] - 1) && (dice[1] - 1) == (dice[2] - 2) && (dice[2] - 2) == (dice[3] - 3) && (dice[3] - 3) == (dice[4] - 4)){
			Score.player_score += 40;
			Score.lower_totals[4] += 40;
		}
		Score.lower_used[4] = 1;
	}
   // Yahtzee boiiiiiiii
	if(category == 6 && Score.lower_used[5] == 0)
	{
		if(dice[0] == dice[1] && dice[1] == dice[2] && dice[2] == dice[3] && dice[3] == dice[4]){
			Score.player_score += 50;
			Score.lower_totals[5] += 50;
		}
		Score.lower_used[5] = 1;
	}
   // Chance
	if(category == 7 && Score.lower_used[6] == 0)
	{
		for(count = 0; count < 5; count++){
			Score.player_score += dice[count];
			Score.lower_totals[6] += dice[count];
		}
		Score.lower_used[6] = 1;
	}
	return;
}

int comp(const void *a, const void *b)
{
   const int *ia = (const int *)a; // casting pointer types 
   const int *ib = (const int *)b;
   return *ia  - *ib; 
}