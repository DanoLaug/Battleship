#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <pthread.h>

#define BOARD_SIZE 10

// Tipos de casillas
#define UNEXPLORED '.'
#define HIT 'x'
#define MISS '-'
#define SHIP 'w'

char player1_board[BOARD_SIZE][BOARD_SIZE];
char player2_board[BOARD_SIZE][BOARD_SIZE];
pthread_mutex_t mutex;

void initialize_boards(int ship_count);
void print_board(char board[BOARD_SIZE][BOARD_SIZE]);
void attack(char attacking_player, char defending_player[BOARD_SIZE][BOARD_SIZE]);
int gameOver(char board[BOARD_SIZE][BOARD_SIZE]);
void* player_thread(void* player_board);

void boards(int ship_count){
    int i, j;

    for (i = 0; i < BOARD_SIZE; i++) {
        for (j = 0; j < BOARD_SIZE; j++) {
            player1_board[i][j] = UNEXPLORED;
            player2_board[i][j] = UNEXPLORED;
        }
    }

    for (i = 0; i < ship_count; i++) {
        printf("Player 1 enter the starting row and column for ship %d : ", i + 1);
        int row, col;
        scanf("%d %d", &row, &col);

        int ship_length = 1;
        for (j = 0; j < ship_length; j++) {
            player1_board[row][col + j] = SHIP;
        }
    }

    for (i = 0; i < ship_count; i++) {
        printf("Player 2 enter the starting row and column for ship %d : ", i + 1);
        int row, col;
        scanf("%d %d", &row, &col);

        int ship_length = 2;
        for (j = 0; j < ship_length; j++) {
            player2_board[row][col + j] = SHIP;
        }
    }
}
  
void print_board(char board[BOARD_SIZE][BOARD_SIZE]){
    int i, j;
    printf("  ");
    for (i = 0; i < BOARD_SIZE; i++) {
        printf("%d ", i);
    }
    printf("\n");

    for ( i = 0; i < BOARD_SIZE; i++) {
        printf("%d ", i);
        for (j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == UNEXPLORED || board[i][j] == SHIP) {
                printf("%c ", UNEXPLORED);
            } else {
                printf("%c ", board[i][j]);
            }
        }
        printf("\n");
    }
    
    printf("  ");
    for (i = 0; i < BOARD_SIZE; i++) {
        printf("%d ", i);
    }
    printf("\n");


    for (i = 0; i < BOARD_SIZE; i++) {
        printf("%d ", i);
        for (j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == UNEXPLORED || board[i][j] == SHIP) {
                printf("%c ", UNEXPLORED);
            } else {
                printf("%c ", board[i][j]);
            }
        }
        printf("\n");
    }
}


void attack(char attacking_player, char defending_player[BOARD_SIZE][BOARD_SIZE]){
    int row, col;
    printf("Player %c's turn. Enter the row and column to attack: ", attacking_player);
    scanf("%d %d", &row, &col);
    pthread_mutex_lock(&mutex);

    if (defending_player[row][col] == SHIP) {
        printf("Player %c hit a ship at (%d,%d)!\n", attacking_player, row, col);
        defending_player[row][col] = HIT;
    } 
    
    else if (defending_player[row][col] == HIT || defending_player[row][col] == MISS) {
        printf("Player %c already attacked (%d,%d).\n", attacking_player, row, col);
    } 
    
    else {
        printf("Player %c missed at (%d,%d).\n", attacking_player, row, col);
        defending_player[row][col] = MISS;
    }
    
    print_board(defending_player);  
    pthread_mutex_unlock(&mutex);
}


int gameOver(char board[BOARD_SIZE][BOARD_SIZE]){
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++) {
		for (j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j] == SHIP) {
				return 0; 
			}
		}
	}
	return 1; // Game over Finish
}

//Hilo para llamar a 'attack'
void* player_thread(void* player_board){
    char (*board)[BOARD_SIZE] = (char (*)[BOARD_SIZE])player_board;
    int player_number;
    if (board == player1_board) {
        player_number = 1;
    } 
	
	else if (board == player2_board) {
        player_number = 2;
    } 
	
	else {
        return NULL;
    }

    while (1) {
        attack(player_number + '0', board);
        if (gameOver(board)) {
            printf("Player %d win!\n", player_number);
            break;
        }
    }

    return NULL;
}

int main(){

    int ship_count;
    printf("Enter the number of ships for both players (minimum 2): ");
    scanf("%d", &ship_count);
    
    if(ship_count == 1){
    	ship_count+=1;
	}

    boards(ship_count);

    printf("Player 1:\n");
    print_board(player1_board);

    printf("\nPlayer 2:\n");
    print_board(player2_board);

    // Initialize mutex
    pthread_mutex_init(&mutex, NULL);
    pthread_t player1_thread, player2_thread;
    pthread_create(&player1_thread, NULL, player_thread, (void*)player1_board);
    pthread_create(&player2_thread, NULL, player_thread, (void*)player2_board);

    // Wait 
    pthread_join(player1_thread, NULL);
    pthread_join(player2_thread, NULL);

    // Destroy mutex
    pthread_mutex_destroy(&mutex);

    return 0;
}
