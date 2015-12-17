
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define DECKSIZE 52
#define VALUE 9
#define FACE 4

typedef struct{
	int value;
	char suit [10];
	char name [10];
}Card;


void printDeck(Card cards[], int num_decks){
	int i;

	for(i=0; i < DECKSIZE * num_decks;i++){
		printf("%s of %s value=%d\n",cards[i].name,cards[i].suit,cards[i].value);
		if((i+1)%13==0 && i!=0)printf("------------------\n\n");
	}
}


void shuffle(Card cards[], int num_decks){
	Card temp;
	int i;
	int this;
	int that;

	for(i=0;i<DECKSIZE * num_decks * 2;i++){
		this=rand()%(DECKSIZE * num_decks);
		that=rand()%(DECKSIZE * num_decks);
		temp = cards[this];
		cards[this]=cards[that];
		cards[that]=temp;

	}
}

void initDeck(Card cards[], int num_decks){
	char faceName[13][10]={"two","three","four","five","six","seven","eight",
			"nine","ten","jack","queen","king","ace"};

	char suitName[4][10]={"spades","diamonds","clubs","hearts"};

	int suitCount=0;
	int faceCount=0;
	int i;
	int j;
	srand(time(NULL));
	//set card values and suits for deck(s). Each time through outer loop position
	//in cards struct will be adjusted by j * DECKSIZE to prevent overwriting previous values (if 1 deck, j * DECKSIZE is 0)
	for (j = 0; j < num_decks; j++){
		for(i=0;i<DECKSIZE;i++){
			if(faceCount<9 ){
				cards[(j * DECKSIZE) + i].value=faceCount+2;
			}
			else{
				cards[(j * DECKSIZE) + i].value=10;
			}
			strcpy(cards[(j * DECKSIZE) + i].suit, suitName[suitCount]);
			strcpy(cards[(j * DECKSIZE) + i].name, faceName[faceCount++]);
			if(faceCount==13){
				cards[(j * DECKSIZE) + i].value=11;
				suitCount++;
				faceCount=0;
			}
		}
		suitCount = 0;
		faceCount = 0;
	}
	shuffle(cards, num_decks);
}

int evaluateHand(Card hand[], int numCards)
{
	int points = 0;
	int i;

	for (i = 0; i < numCards; i++)
	{
		points = points + hand[i].value;
	}

	for (i = 0; i < numCards; i++)
	{
		if (points > 21 && hand[i].value == 11)
			points = points - 10;
	}

	if (points > 21)
		points = -1;

	return points;
}
//check for blackjack scenario for both dealer and player
void dealerBlackJack(Card player[], int playerCount,
		Card dealer[], int dealerCount, int player_num)
{
	int i;

	printf ("Dealer\'s cards:\n");
	for ( i = 0; i < dealerCount ; i++)
		printf ("  %s of %s\n", dealer[i].name, dealer[i].suit);
	printf ("  Points: %d\n", evaluateHand(dealer, dealerCount));

	if (evaluateHand(player, playerCount) == 21)
	{
		printf ("Both Dealer and Player%i have Blackjack.  Tie\n", player_num+1);
	}
	else
	{
		printf ("The Dealer has Blackjack.  Player%i loses\n", player_num+1);
	}
}
//each player plays hand, points set to -1 if they go over 21
int playHand (Card player[], int playerCount,
		Card dealer[], int dealerCount,
		Card deck[], int cardsLeft, int player_num)
{
	int i;
	char input[100];

	do
	{
		printf ("Do you want another card? (y/n):");
		gets(input);
		if (input[0] == 'y')
			player[playerCount++] = deck[--cardsLeft];

		printf ("Player%i's cards:\n", player_num+1);
		for ( i = 0; i < playerCount ; i++)
			printf ("  %s of %s\n", player[i].name, player[i].suit);
		printf ("  Points: %d\n", evaluateHand(player, playerCount));
		printf ("Dealer shows: %s of %s\n", dealer[0].name, dealer[0].suit);
	}
	while (input[0] == 'y' && evaluateHand(player, playerCount) != -1);

	if(evaluateHand(player, playerCount) == -1)
	{
		printf ("Player%i's point value went over 21.  Player%i loses\n", player_num+1, player_num+1);
		return -1;
	}
	return playerCount;

}
//dealer plays hand, exit if dealer goes bust
int dealer_bust(Card dealer[], int dealerCount, Card deck[], int cardsLeft){
	int i;
	printf ("\nDealer\'s cards:\n");
	for ( i = 0; i < dealerCount ; i++)
		printf ("  %s of %s\n", dealer[i].name, dealer[i].suit);
	printf ("  Points: %d\n", evaluateHand(dealer, dealerCount));
	while (evaluateHand(dealer, dealerCount) < 17)
	{
		dealer[dealerCount++] = deck[--cardsLeft];
		printf ("  Newcard for dealer: %s of %s\n",
				dealer[dealerCount - 1].name, dealer[dealerCount - 1].suit);
		printf ("  Points: %d\n", evaluateHand(dealer, dealerCount));
		if (evaluateHand(dealer, dealerCount) == -1)
		{
			printf ("The dealer went over 21 points.  Dealer loses\nThanks for playing!");
			exit(0);
		}
	}
	return dealerCount;
}

int determine_winner(Card player[], int playerCount, Card dealer[], int dealerCount){

	int playerPoints;
	int dealerPoints;

	playerPoints = evaluateHand(player, playerCount);
	dealerPoints = evaluateHand(dealer, dealerCount);
	if (playerPoints == dealerPoints)
	{  //tie
		return 0;
	}
	else if (playerPoints > dealerPoints)
	{  //player wins
		return 1;
	}
	else
	{  //dealer wins
		return -1;
	}
}

void error_print(){
	printf("You entered an incorrect command.\nYou cannot play with less than one player or less than one deck ");
	printf("and the number of players and/or decks must be given in integers(decimal values will be rounded to nearest whole number).\n");
	printf("----------\nThe commands are:\n1. -d <number of decks>\n");
	printf("2. -p <number of players>\n----------\nBoth may be used at the same time, but ensure that the corresponding number ");
	printf("is given immediately after its respective command as shown.\nProgram exiting.\n");
	exit(0);

}

int main(int argc, char *argv[]){
	int num_decks = 1;
	int num_player = 1;
	//if user doesn't enter valid arguments or a valid number of arguments, print error screen, exit program
	if (argc == 2 || argc == 4 || argc > 5)
		error_print();
	if (argc == 3){
		if (!(strcmp("-d", argv[1]))){
			num_decks = atoi(argv[2]);
			if (num_decks < 1)
				error_print();
		}
		else if (!(strcmp("-p", argv[1]))){
			num_player = atoi(argv[2]);
			if (num_player < 1)
				error_print();
		}
		else
			error_print();
	}
	if (argc == 5){
		if (strcmp("-d", argv[1]) && strcmp("-p", argv[1]) || strcmp("-d", argv[3]) && strcmp("-p", argv[3]))
			error_print();
		if (!(strcmp("-d", argv[1]))){
			num_decks = atoi(argv[2]);

			if (num_decks < 1)
				error_print();

			num_player = atoi(argv[4]);
			if (num_player < 1)
				error_print();
		}
		else {
			num_player = atoi(argv[2]);

			if (num_player < 1)
				error_print();
			num_decks = atoi(argv[4]);
			if (num_decks < 1)
				error_print();
		}
	}

	Card deck[DECKSIZE * num_decks];
	Card player[5][5];
	Card dealer[5];
	int cardsLeft;
	int playerCount[5];
	int dealerCount;
	int i, win, j;
	//initialize deck with user given number of decks (if user specified
	initDeck(deck, num_decks);
	cardsLeft = 52 * num_decks;
	dealer[0] = deck[--cardsLeft];
	dealer[1] = deck[--cardsLeft];
	dealerCount = 2;
	//run through player hands for number of players specified
	for (j=0; j<num_player; j++){
		player[j][0] = deck[--cardsLeft];
		player[j][1] = deck[--cardsLeft];

		playerCount[j] = 2;


		printf ("Player%i's cards:\n", j+1);
		for ( i = 0; i < playerCount[j] ; i++)
			printf ("  %s of %s\n", player[j][i].name, player[j][i].suit);
		printf ("  Points: %d\n", evaluateHand(player[j], playerCount[j]));
		printf ("Dealer shows: %s of %s\n", dealer[0].name, dealer[0].suit);
		//check for blackjack conditions before player plays hand
		if (evaluateHand(dealer, dealerCount) == 21)
		{
			dealerBlackJack(player[j], playerCount[j], dealer, dealerCount, j);
		}
		else if (evaluateHand(player[j], playerCount[j]) == 21)
		{
			printf ("Player%i has Blackjack.  Player%i wins.\n", j+1, j+1);
		}
		else
		{
			playerCount[j] = playHand(player[j], playerCount[j], dealer, dealerCount, deck, cardsLeft, j);

		}
	}
	//determine dealer's cards
	dealerCount = dealer_bust(dealer, dealerCount, deck, cardsLeft);
	//determine win/loss of each player vs the dealer
	for(j=0; j<num_player; j++){

		win = determine_winner(player[j], playerCount[j], dealer, dealerCount);
		if (win == 1)
			printf("Player%i has the higher point total.  Player%i wins.\n", j+1, j+1);
		else if (win == -1)
			printf("Dealer has a higher point total than Player%i.  Player%i loses.\n", j+1, j+1);
		else
			printf("Dealer and Player%i have the same point total.  Tie\n", j+1);
	}
	printf("Thanks for playing!\n");
	return 0;
}
