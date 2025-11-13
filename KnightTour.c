#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#define GREEN   "\033[32m"    
#define RESET   "\033[0m"     
#define BOLD    "\033[1m"
#define CYAN    "\033[36m"
#define RED     "\033[31m"
#define BG_GREEN   "\033[42m"
#define BG_YELLOW  "\033[43m"
#define BG_MAGENTA "\033[45m"

void clearScreen(void);
void updatePos(int dest_row, int dest_col, int acc_matrix[][8], const int move[][8]);
int getAcc(int row, int col, const int acc_matrix[][8]);
void printBoard(int dest_indx, const int acc_matrix[][8], const int acc_pos_memo[][3]);

FILE *fp = NULL;

// these tow variables represent the x and y positions of the knight
int currentRow;
int currentCol;

int main() {

    // create .txt file
    fp = fopen("!!!your directory!!!", "w");

    if (fp == NULL) {

        perror("Failed to create moveRecord.txt");
        return EXIT_FAILURE;
    }
    
    // we use accMatrix as chess board too
    // bool chessBoard[8][8] { {0} };

    // accessibility matrix
    int accMatrix[8][8] = {
        {2, 3, 4, 4, 4, 4, 3, 2},
        {3, 4, 6, 6, 6, 6, 4, 3},
        {4, 6, 8, 8, 8, 8, 6, 4},
        {4, 6, 8, 8, 8, 8, 6, 4},
        {4, 6, 8, 8, 8, 8, 6, 4},
        {4, 6, 8, 8, 8, 8, 6, 4},
        {3, 4, 6, 6, 6, 6, 4, 3},
        {2, 3, 4, 4, 4, 4, 3, 2}
    };

    // row one for vertical - row two for horizontal
    int move[2][8] = {
        {-1, -2, -2, -1, 1, 2, 2, 1},
        {2, 1, -1, -2, -2, -1, 1, 2}
    };
    
    // this array records the acc. of possible squares the knight can move to
    // for example the value of index 2 represents the number of reachable squares
    // with acc. 2
    // the accessibilities are 0 1 2 3 4 8 9 10
    // acc 9 represents a room the knight's has already steped into
    // acc 10 represents invalid position which is outside the chessboard
    int nextPosAcc[11] = {0};

    // this 2D array records acc of a possible move alnong whith the cooardinates of the destination
    // every row of the array has following format -> (acc) (row position) (col position)
    int accPosMemo[8][3];

    // get starting 'currentRow' and 'currentCol' positions from the user
    puts("First input: currentRow, second input: currentCol");
    scanf("%d %d", &currentRow, &currentCol);;
    printf("Knight starts tour at square [%d][%d]\n", currentRow, currentCol);

    accMatrix[currentRow][currentCol] = 9;

    // clean buffer
    int z;
    while (z = getchar() != '\n');

    int counter = 1;
    while (counter <= 64) {

        printf("move number: (%d)\n", counter);

        int minAcc = 10;
        int minIndx;
        for (size_t i = 0; i < 8; ++i) {

            accPosMemo[i][1] = currentRow + move[0][i];
            accPosMemo[i][2] = currentCol + move[1][i];

            int squareAcc = getAcc(accPosMemo[i][1], accPosMemo[i][2], accMatrix);
        
            accPosMemo[i][0] = squareAcc;

            ++nextPosAcc[squareAcc];

            // find least acc
            if (squareAcc < minAcc) {
                
                minAcc = squareAcc;

                // this is only used when there is not a tie. this way
                // we don't have to perform a linear search
                minIndx = i;
            }
        }

        int minAccOcc = nextPosAcc[minAcc];

        // check for a tie
        if (minAccOcc > 1) {

            int tieMinAcc = 10;
            int priorityMinIndx;

            // linear search accPosMemo and look for minAcc. for each of them, grab the cooardinates, go to that cooardinate and see
            // what is the least acc. assocciated to reachable squares around.
            for (size_t i = 0; i < 8; ++i) {

                if (accPosMemo[i][0] == minAcc) {

                    for (size_t j = 0; j < 8; ++j) {

                        int squareTieAcc = getAcc(accPosMemo[i][1] + move[0][j], accPosMemo[i][2] + move[1][j], accMatrix);
                        
                        if (squareTieAcc < tieMinAcc) {
                
                            tieMinAcc = squareTieAcc;
                            priorityMinIndx = i;
                        }
                    }
                }
            }

            printBoard(priorityMinIndx, accMatrix, accPosMemo);
            
            // knight position is updated to the position associated to 'priorityMinIndx' 
            updatePos(accPosMemo[priorityMinIndx][1], accPosMemo[priorityMinIndx][2], accMatrix, move);
        } else {
            
            printBoard(minIndx, accMatrix, accPosMemo);

            updatePos(accPosMemo[minIndx][1], accPosMemo[minIndx][2], accMatrix, move);
        }

        // befor doing the next move it awaits for user input
        int input = getchar();

        // clear input buffer
        while(input != '\n' && getchar());

        if (counter < 64) {

            // this happens if all possible destinations for the knight to go
            // are either all 9 meaning that the knight has already visited them
            // or a combination of 9s and 10s
            if(9 == minAcc) {

                puts(RED "TASK FAILED" RESET);
                return EXIT_FAILURE;
            } else {

                clearScreen();
            }
        } 

        ++counter;
    }

    puts(GREEN "TASK SUCCESSFUL" RESET);

    fclose(fp);

    return EXIT_SUCCESS;
}

int getAcc(int row, int col, const int acc_matrix[][8]) {

    // if the position is outside the chess board
    if (row > 7 || row < 0 || col > 7 || col < 0) {

        return 10;
    } else {

        return acc_matrix[row][col];
    }
}

void updatePos(int dest_row, int dest_col, int acc_matrix[][8], const int move[][8]) {

    currentRow = dest_row;
    currentCol = dest_col;

    acc_matrix[currentRow][currentCol] = 9;

    for (size_t i = 0; i < 8; ++i) {
        int aroundDestrow = currentRow + move[0][i];
        int aroundDestCol = currentCol + move[1][i];

        int aroundDestAcc = getAcc(aroundDestrow, aroundDestCol, acc_matrix);
        
        if (aroundDestAcc != 9 && aroundDestAcc != 10) {

            --acc_matrix[aroundDestrow][aroundDestCol];
        }
    }
}

void printBoard(int dest_indx, const int acc_matrix[][8], const int acc_pos_memo[][3]) {

    int destRow = acc_pos_memo[dest_indx][1];
    int destCol = acc_pos_memo[dest_indx][2];

    printf("%s\t" BG_MAGENTA "%c " RESET " Knight   " BG_YELLOW "%c " RESET " Possible moves   " BG_GREEN "%c " RESET " Destination\n"
        , "   A  B  C  D  E  F  G  H", ' ', ' ', ' ');

    for (size_t i = 0; i < 8; ++i) {
        
        printf("%zu ", i + 1);
        for (size_t j = 0; j < 8; ++j) {

                int rowDist =  abs(i - currentRow);
                int colDist = abs(j - currentCol);

                if (3 == rowDist + colDist && rowDist * colDist != 0) {

                    if (i == destRow && j == destCol) {

                        printf(BOLD BG_GREEN "[%d]" RESET, acc_matrix[i][j]);
                    } else {

                        printf(BOLD BG_YELLOW "[%d]" RESET, acc_matrix[i][j]);
                    }
                } else if (0 == rowDist + colDist) {

                    printf(BOLD BG_MAGENTA "[%d]" RESET, acc_matrix[i][j]);
                } else {

                    (9 == acc_matrix[i][j]) ? printf(RED "[%d]" RESET, acc_matrix[i][j]) : printf("[%d]", acc_matrix[i][j]);
                }
        }
        printf(" %zu\n", i + 1);
    }

    printf("%s\t" CYAN "%c%d ---> %c%d\n" RESET
        , "   A  B  C  D  E  F  G  H"
        , currentCol + 65, currentRow + 1
        , destCol + 65, destRow + 1);
    
    // for printing to the text file
    fprintf(fp, "%c%d ---> %c%d\n"
        , currentCol + 65, currentRow + 1
        , destCol + 65, destRow + 1);
}

// function to clear terminal
void clearScreen() {
    #ifdef _WIN32
        system("cls");   // For Windows
    #else
        system("clear"); // For Unix/Linux/Mac
    #endif

}
