//-------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BOARD_SIZE 6
#define TARGET_SCORE 2024
#define MAX_PATHS 1000000
#define MAX_PATH_LENGTH 15

// Define the specific structure of the board with A, B, and C
char game_board_template[BOARD_SIZE][BOARD_SIZE] = {
    {'a', 'a', 'a', 'b', 'b', 'c'},
    {'a', 'a', 'a', 'b', 'b', 'c'},
    {'a', 'a', 'b', 'b', 'c', 'c'},
    {'a', 'a', 'b', 'b', 'c', 'c'},
    {'a', 'b', 'b', 'c', 'c', 'c'},
    {'a', 'b', 'b', 'c', 'c', 'c'}
};

// Define all possible moves for the knight to make (x, y)
int knight_moves[8][2] = {
    {1, 2}, {1, -2}, {-1, 2}, {-1, -2},
    {2, 1}, {2, -1}, {-2, 1}, {-2, -1}
};

// Check if a position is within the range of our 6x6 board
bool is_within_board(int x, int y) {
    return (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE);
}

// Define a structure to store a successful path
typedef struct {
    int path[MAX_PATH_LENGTH][2];
    int path_length;
    int score;
    int a, b, c; // Values of a, b, c for this path
} SuccessfulPath;

SuccessfulPath successful_paths[MAX_PATHS];
int successful_path_count = 0;

// Define a struct for queue elements
typedef struct {
    int row, col;
    bool visited[BOARD_SIZE][BOARD_SIZE];
    int score;
    int path[MAX_PATH_LENGTH][2];
    int path_length;
} QueueElement;

// Queue struct for breadth-first search
typedef struct {
    QueueElement *elements;
    int front, rear, size, capacity;
} Queue;

//-------------------------------------------------------------------------------

Queue *createQueue(int capacity) {
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->elements = (QueueElement *)malloc(capacity * sizeof(QueueElement));
    return queue;
}

//-------------------------------------------------------------------------------

bool isQueueEmpty(Queue *queue) {
    return (queue->size == 0);
}

//-------------------------------------------------------------------------------

void enqueue(Queue *queue, QueueElement element) {
    if (queue->size == queue->capacity) {
        return;
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->elements[queue->rear] = element;
    queue->size++;
}

//-------------------------------------------------------------------------------

QueueElement dequeue(Queue *queue) {
    QueueElement item = queue->elements[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return item;
}

//-------------------------------------------------------------------------------
// Breadth-first search algo to find all paths from (0, 0) to (5, 5) with a target score of 2024
void bfs_knight_paths(int start_x, int start_y, int end_x, int end_y, int game_board[BOARD_SIZE][BOARD_SIZE], int a, int b, int c) {
    Queue *queue = createQueue(1000000);
    QueueElement start;
    start.row = start_x;
    start.col = start_y;
    memset(start.visited, false, sizeof(start.visited));
    start.visited[start_x][start_y] = true;
    start.score = game_board[start_x][start_y];
    start.path[0][0] = start_x;
    start.path[0][1] = start_y;
    start.path_length = 1;
    enqueue(queue, start);

    while (!isQueueEmpty(queue)) {
        QueueElement current = dequeue(queue);
        int row = current.row;
        int col = current.col;
        int score = current.score;

        // If we reached the end position, check if the score is exactly the target
        if (row == end_x && col == end_y && score == TARGET_SCORE) {
            if (successful_path_count < MAX_PATHS) {
                SuccessfulPath *sp = &successful_paths[successful_path_count++];
                memcpy(sp->path, current.path, sizeof(current.path));
                sp->path_length = current.path_length;
                sp->score = score;
                sp->a = a;
                sp->b = b;
                sp->c = c;
            }
            continue;
        }

        // Explore all possible knight moves
        for (int i = 0; i < 8; i++) {
            int new_row = row + knight_moves[i][0];
            int new_col = col + knight_moves[i][1];
            if (is_within_board(new_row, new_col) && !current.visited[new_row][new_col]) {
                QueueElement next = current;
                next.row = new_row;
                next.col = new_col;
                next.visited[new_row][new_col] = true;
                next.path[next.path_length][0] = new_row;
                next.path[next.path_length][1] = new_col;
                next.path_length++;
                int new_value = game_board[new_row][new_col];

                // Update the score based on the move
                if (game_board[row][col] == new_value) {
                    next.score += new_value;  // Same value, increment by the value
                } else {
                    next.score *= new_value;  // Different value, multiply by the new value
                }

                enqueue(queue, next);
            }
        }
    }

    free(queue->elements);
    free(queue);
}

//-------------------------------------------------------------------------------
// Implement the integer_combinations function
void integer_combinations(int limit, void (*callback)(int, int, int)) {
    for (int a = 1; a < limit; a++) {
        for (int b = 1; b < limit; b++) {
            if (b == a) continue;
            for (int c = 1; c < limit; c++) {
                if (c == a || c == b) continue;
                if (a + b + c == 6) {
                    callback(a, b, c);
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------
// Callback function for each distinct integer combination of a, b, c
void process_combination(int a, int b, int c) {
    int game_board[BOARD_SIZE][BOARD_SIZE];

    // Create a new game board with the current values of a, b, and c
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (game_board_template[i][j] == 'a') {
                game_board[i][j] = a;
            } else if (game_board_template[i][j] == 'b') {
                game_board[i][j] = b;
            } else if (game_board_template[i][j] == 'c') {
                game_board[i][j] = c;
            }
        }
    }

    // Find all successful paths from (0, 0) to (5, 5) with a score of exactly 2024
    bfs_knight_paths(0, 0, 5, 5, game_board, a, b, c);
}

//-------------------------------------------------------------------------------
// Check paths from (5, 0) to (0, 5) with the saved values of a, b, c
void check_additional_paths() {
    for (int i = 0; i < successful_path_count; i++) {
        SuccessfulPath sp = successful_paths[i];
        int game_board[BOARD_SIZE][BOARD_SIZE];

        // Create a new game board with the values of a, b, and c from the successful path
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (game_board_template[i][j] == 'a') {
                    game_board[i][j] = sp.a;
                } else if (game_board_template[i][j] == 'b') {
                    game_board[i][j] = sp.b;
                } else if (game_board_template[i][j] == 'c') {
                    game_board[i][j] = sp.c;
                }
            }
        }

        Queue *queue = createQueue(1000000);
        QueueElement start;
        start.row = 5;
        start.col = 0;
        memset(start.visited, false, sizeof(start.visited));
        start.visited[5][0] = true;
        start.score = game_board[5][0];
        start.path[0][0] = 5;
        start.path[0][1] = 0;
        start.path_length = 1;
        enqueue(queue, start);

        while (!isQueueEmpty(queue)) {
            QueueElement current = dequeue(queue);
            int row = current.row;
            int col = current.col;
            int score = current.score;

            // If we reached the end position, check if the score is exactly the target
            if (row == 0 && col == 5 && score == TARGET_SCORE) {
                printf("Successful paths for a=%d, b=%d, c=%d:\n", sp.a, sp.b, sp.c);
                printf("  Path from (0, 0) to (5, 5):\n");
                for (int j = 0; j < sp.path_length; j++) {
                    printf("    (%d, %d)\n", sp.path[j][1], sp.path[j][0]);
                }
                printf("  Path from (5, 0) to (0, 5):\n");
                for (int j = 0; j < current.path_length; j++) {
                    printf("    (%d, %d)\n", current.path[j][1], current.path[j][0]);
                }
            }

            // Explore all possible knight moves
            for (int i = 0; i < 8; i++) {
                int new_row = row + knight_moves[i][0];
                int new_col = col + knight_moves[i][1];
                if (is_within_board(new_row, new_col) && !current.visited[new_row][new_col]) {
                    QueueElement next = current;
                    next.row = new_row;
                    next.col = new_col;
                    next.visited[new_row][new_col] = true;
                    next.path[next.path_length][0] = new_row;
                    next.path[next.path_length][1] = new_col;
                    next.path_length++;
                    int new_value = game_board[new_row][new_col];

                    // Update the score based on the move
                    if (game_board[row][col] == new_value) {
                        next.score += new_value;  // Same value, increment by the value
                    } else {
                        next.score *= new_value;  // Different value, multiply by the new value
                    }

                    enqueue(queue, next);
                }
            }
        }

        free(queue->elements);
        free(queue);
    }
}

//-------------------------------------------------------------------------------
//Main function
int main() {
    // For each combination of integers, check the paths
    integer_combinations(7, process_combination);

    // Check additional paths from (5, 0) to (0, 5) for saved successful paths
    check_additional_paths();
    return 0;
}
