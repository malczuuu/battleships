#include "board.h"

#define TARGET_REGEX "(^([A-Ja-j])([1-9]|10)$)|(^([1-9]|10)([A-Ja-j])$)"
#define TARGET_GROUPS 6

int match(coords_t *coords, char *string, char *pattern) {
    int status;
    regex_t re;
    regmatch_t groups[TARGET_GROUPS];
    if (regcomp(&re, pattern, REG_EXTENDED | REG_NOSUB) != 0) {
        return 0;
    }
    status = regexec(&re, string, TARGET_GROUPS, groups, 0);
    if (status != 0) {
        return -1;
    }

    char buff[8] = "";
    if (isalpha(string[0])) {
        memcpy(buff, &string[1], strlen(string) - 1);
        coords->x = atoi(buff);
        coords->y = toupper(string[0]) - 'A' + 1;
    } else if (isalpha(string[strlen(string) - 1])) {
        memcpy(buff, string, strlen(string) - 1);
        coords->x = atoi(buff);
        coords->y = toupper(string[strlen(string) - 1]) - 'A' + 1;
    }

    return 0;
}

void clearconsole() {
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    system("clear");
#endif
#if defined(_WIN32) || defined(_WIN64)
    system("cls");
#endif
}

void append(char *line, int coords_status) {
    switch (coords_status) {
        case COORD_ALIVE:
            strcat(line, "[ ]");
            break;
        case COORD_DAMAGED:
            strcat(line, "[o]");
            break;
        case COORD_MISSED:
            strcat(line, "(.)");
            break;
        default:
            strcat(line, "   ");
    }
}

void print_boards(board_t player, board_t computer) {
    int i;
    int j;
    printf(" Player:                               Computer:\n");
    printf("  +------------------------------+     +------------------------------+\n");
    char letter = 'A';
    for (i = 1; i <= BOARD_SIZE; ++i) {
        char buff[16] = "";
        char line[128] = "";
        sprintf(buff, "%2d|", i);
        strcat(line, buff);
        for (j = 1; j <= BOARD_SIZE; ++j) {
            int coords_status = board_coords_status(&player, coords_create(i, j));
            append(line, coords_status);
        }

        sprintf(buff, "|   %2d|", i);
        strcat(line, buff);
        for (j = 1; j <= BOARD_SIZE; ++j) {
            int coords_status = board_coords_status_hidden(&computer, coords_create(i, j), true);
            append(line, coords_status);
        }
        strcat(line, "|");

        printf("%s\n", line);
        ++letter;
    }
    printf("  +------------------------------+     +------------------------------+\n");
    printf("    A  B  C  D  E  F  G  H  I  J         A  B  C  D  E  F  G  H  I  J  \n");
}

int main(int argc, char *argv[]) {
    srand((unsigned int) time(NULL));

    board_t player;
    board_t computer;

    board_init(&player);
    board_init(&computer);

    board_randomize(&player);
    board_randomize(&computer);

    bool play = true;

    int player_score = 0;
    int computer_score = 0;

    int player_status = -1;
    int computer_status = -1;

    printf("Score:\n    Player: %d\n    Computer: %d\n", player_score, computer_score);

    print_boards(player, computer);

    while (play) {
        coords_t coords = coords_create(1, 1);
        char target[256];


        printf("Your target (do not use spaces): ");
        scanf("%s", target);
        if (match(&coords, target, TARGET_REGEX)) {
            printf("Invalid target!\n");
            player_status = -1;
            computer_status = -1;
            continue;
        }

        player_status = board_fire(&computer, coords);
        if (player_status == STRIKE_REPEATED) {
            printf("Cannot strike two times in the same spot\n");
            continue;
        }
        if (!board_alive(&computer)) {
            play = false;
            printf("You win!\n");
        }

        coords = board_random_hit(&player);
        computer_status = board_fire(&player, coords);
        if (!board_alive(&player)) {
            play = false;
            printf("Computer wins!\n");
        }
        clearconsole();

        if (player_status == STRIKE_HIT) {
            printf("Player: hit!\n");
            ++player_score;
        } else if (player_status == STRIKE_SINK) {
            printf("Player: hit and sink!");
            player_score = computer_score + 5;
        } else if (player_status == STRIKE_MISS) {
            printf("Player: miss!\n");
        }

        if (computer_status == STRIKE_HIT) {
            printf("Computer: hit!\n");
            ++computer_score;
        } else if (computer_status == STRIKE_SINK) {
            printf("Computer: hit and sink!");
            computer_score = computer_score + 5;
        } else if (computer_status == STRIKE_MISS) {
            printf("Computer: miss!\n");
        }

        printf("Score:\n    Player: %d\n    Computer: %d\n", player_score, computer_score);

        print_boards(player, computer);
    }

    board_destroy(&player);
    board_destroy(&computer);
}
