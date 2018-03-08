/*
 ============================================================================
 Name        : 2048.c
 Author      : Maurits van der Schee
 Description : Console version of the game "2048" for GNU/Linux
 ============================================================================
 */

#include "node.h"
#include "ai.h"
#include "utils.h"

void output_to_file(int max_depth, int generated, int expanded, double time,
	int max_tile, int score);

/* function for output to output.txt */
void
output_to_file(int max_depth, int generated, int expanded, double time,
	int max_tile, int score) {
	FILE *fptr;

	fptr = fopen("output.txt", "w");
	/* "w" - Creates an empty file for writing.
	If a file with the same name already exists,
	its content is erased and the file is considered
	as a new empty file. */

	// fptr = fopen("output.csv", "w");
	/* "a" - append the file, used for experimentation" */

	if (fptr == NULL) {
		perror("Error: ");
		printf("fopen error\n");
		exit(EXIT_FAILURE);
	}


	fprintf(fptr, "MaxDepth 		= %d\n", max_depth);
	fprintf(fptr, "Generated 		= %d\n", generated);
	fprintf(fptr, "Expanded 		= %d\n", expanded);
	fprintf(fptr, "Time 			= %f\n", time);
	fprintf(fptr, "Expanded/Second 	= %f\n", (double)expanded / time);
	fprintf(fptr, "max_tile 		= %d\n", max_tile);
	fprintf(fptr, "Score 			= %d\n\n", score);


	/* below output option is used for experimentation */
	// fprintf(fptr, "%d,%f,%d,%d\n", max_depth, time, max_tile, score);

	fclose(fptr);

}


/**
 * Setting up terminal to draw the game board
 */
void setBufferedInput(bool enable) {
	static bool enabled = true;
	static struct termios old;
	struct termios new;

	if (enable && !enabled) {
		// restore the former settings
		tcsetattr(STDIN_FILENO,TCSANOW,&old);
		// set the new state
		enabled = true;
	} else if (!enable && enabled) {
		// get the terminal settings for standard input
		tcgetattr(STDIN_FILENO,&new);
		// we want to keep the old setting to restore them at the end
		old = new;
		// disable canonical mode (buffered i/o) and local echo
		new.c_lflag &=(~ICANON & ~ECHO);
		// set the new settings immediately
		tcsetattr(STDIN_FILENO,TCSANOW,&new);
		// set the new state
		enabled = false;
	}
}


void signal_callback_handler(int signum) {
	printf("         TERMINATED         \n");
	setBufferedInput(true);
	printf("\033[?25h\033[m");
	exit(signum);
}

/**
 * Valid keys for playing
 */

bool execute_keyboard( uint8_t board[SIZE][SIZE], uint32_t* score, char c){
	bool success = false;

	switch(c)
		{
		case 97:	// 'a' key
		case 104:	// 'h' key
		case 68:	// left arrow
		    success = moveLeft(board, score);  break;
		case 100:	// 'd' key
		case 108:	// 'l' key
		case 67:	// right arrow
			success = moveRight(board, score); break;
		case 119:	// 'w' key
		case 107:	// 'k' key
		case 65:	// up arrow
			success = moveUp(board, score);    break;
		case 115:	// 's' key
		case 106:	// 'j' key
		case 66:	// down arrow
			success = moveDown(board, score);  break;
		default: success = false;
		}
	return success;

}

void print_usage(){
    printf("To run the AI solver: \n");
    printf("USAGE: ./2048 ai <max/avg> <max_depth> slow\n");
    printf("or, to play with the keyboard: \n");
    printf("USAGE: ./2048\n");
}


int main(int argc, char *argv[]) {

    uint32_t score=0;
	uint8_t board[SIZE][SIZE];

	int max_depth=0;

	char c;
	bool success;
	bool ai_run = false;
	propagation_t propagation=max;
	bool slow = false;

	/* define variables required for solver output */
	int generated = 0;
	int expanded = 0;
	double time = 0;
	int max_tile = 0;
	clock_t begin;
	clock_t end;

	/**
	 * Parsing command line options
	 */
	if( argc > 1 && argc < 4 ){
	    print_usage();
	    return 0;
	}

	if (argc > 1 ) {
		ai_run = true;
		initialize_ai();
		if( strcmp(argv[2],"avg")==0 ){
		    propagation = avg;
		}
		else if( strcmp(argv[2],"max")==0 ){
		    propagation = max;
		}
		else{
		    print_usage();
		    return 0;
		}

		sscanf (argv[3],"%d",&max_depth);

		if(argc == 5 && strcmp(argv[4],"slow")==0 )
		    slow = true;
	}


	printf("\033[?25l\033[2J");

	/**
	 *  register signal handler for when ctrl-c is pressed
	 */

	signal(SIGINT, signal_callback_handler);
	setBufferedInput(false);

	/**
	 * Create initial state
	 */
	initBoard(board, &score);

	if(ai_run) {
		/* begin the clock operation */
		begin = clock();
	}

	while (true) {
	    /**
	     * AI execution mode
	     */
	    if(ai_run) {
			/**
			 * ****** HERE IS WHERE YOUR SOLVER IS CALLED
			 */
			move_t selected_move = get_next_move( board, max_depth,
				propagation, &generated, &expanded, &max_tile );
			/**
			 * Execute the selected action
			 */
			success = execute_move_t( board, &score, selected_move);
		} else {
			/**
			 * Keyboard execution mode
			 */
			c=getchar();
			success = execute_keyboard(board, &score, c);

			if (c=='q') {
			    printf("        QUIT? (y/n)         \n");
			    c=getchar();
			    if (c=='y') {
				break;
			    }
			    drawBoard(board,score);
			}
			if (c=='r') {
			    printf("       RESTART? (y/n)       \n");
			    c=getchar();
			    if (c=='y') {
				initBoard(board, &score);
			    }
			    drawBoard(board,score);
			}
	    }

	    /**
	     * If selected action merges tiles,
	     * then, add a random tile and redraw the board
	     */
	    if (success) {

			drawBoard(board,score);

			if(slow) usleep(100000); //0.15 seconds

			addRandom(board);
			drawBoard(board,score);

			if (gameEnded(board)) {
			    printf("         GAME OVER          \n");
			    break;
			}
	    }

	}

	if(ai_run) {
		/* end the clock operation */
		end = clock();

		/* evaluate the time taken */
		time = (double)(end - begin) / CLOCKS_PER_SEC;

		/* write to output.txt */
		output_to_file(max_depth, generated, expanded, time, max_tile, score);
	}

	setBufferedInput(true);

	printf("\033[?25h\033[m");

	return EXIT_SUCCESS;
}
