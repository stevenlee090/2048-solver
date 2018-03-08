#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "ai.h"
#include "utils.h"
#include "priority_queue.h"

#define INIT_NUM 10

void print_broad(uint8_t board[SIZE][SIZE]);
void open_square_bonus(uint8_t board[SIZE][SIZE], uint32_t *score);
void corner_bonus (uint8_t board[SIZE][SIZE], uint32_t *score);

struct heap h;

void initialize_ai(){
	heap_init(&h);
}

/**
 * Find best action by building all possible paths up to depth max_depth
 * and back propagate using either max or avg
 */

move_t get_next_move( uint8_t board[SIZE][SIZE], int max_depth, propagation_t propagation
 	, int *generated, int *expanded, int* max_tile) {

	if (max_depth == 0) {
		move_t best_action = rand() % 4;
		return best_action;
	}


	node_t **explored;			// the "explored" Array
	int n_explored = -1;		// number of elements in the explored Array, -1 + 1 = 0
	int array_size = INIT_NUM;	// used to keep track of the explored Array's current size
	int i, j, k;						// variables used for looping

	node_t *start = malloc(sizeof(node_t));

	/* check if memory allocation was successful */
	if (start == NULL) {
		printf("malloc error\n");
		exit(EXIT_FAILURE);
	}


	start->priority = 0; 		/* set the start node's priority to 0 */
	start->depth = 0; 			/* set the start node's depth to 0 */
	start->num_childs = 0; 	/* set num_childs to 0 */
	start->parent = NULL; 	/* set the start node's parent to null */

	start->score = 0; 			/* set the start node's score to 0 */
	start->num_of_scores = 0;
	start->average = 0;
	start->move = 0;

	/* copy the board over to the starting node */
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			start->board[i][j] = board[i][j];
		}
	}

	/* allocate memory to the explored array */
	explored = malloc(INIT_NUM * sizeof(node_t*));

	/* check if memory allocation was successful */
	if (explored == NULL) {
		printf("malloc error\n");
		exit(EXIT_FAILURE);
	}

	initialize_ai();				/* initialise the heap */
	heap_push(&h, start); 	/* add the start node into the heap */

	while (h.count > 0) {

		/* pop the heap and assign that node to the current node */
		node_t* current_node = heap_delete(&h);

		/* increment for the number of expanded nodes */
		*expanded = *expanded + 1;

		/* increment the counter for explored array */
		n_explored++;

		/* check if it is necessary to allocate more memory for
		the explored array */
		if (n_explored == array_size) {
			array_size = array_size * 2;
			explored = realloc( explored, array_size * sizeof(node_t*) );

			/* check if realloc operation was successful */
			if (explored == NULL) {
				printf("realloc error\n");
				exit(EXIT_FAILURE);
			}
		}

		/* add the new_node to the explored array */
		explored[n_explored] = current_node;


		/* check if the current node's depth is
		less than the maximum specified depth */
		if (current_node->depth < max_depth) {

			/* for each possible action, i.e. up, down, left, right */
			for (k = 0; k < 4; k++) {


				// node_t new_node = &current_node;
				/* ask what is going on in this case...
				it should work, but it just doesn't let me assign it for some reason */


				node_t *new_node = malloc(sizeof(node_t));

				/* check if memory allocation was successful */
				if (new_node == NULL) {
					printf("malloc error\n");
					exit(EXIT_FAILURE);
				}

				new_node->priority = 0; /* set priority to 0 */
				new_node->score = 0; /* set the score variable to 0 */
				new_node->num_of_scores = 0; /* set the score_count variable to 0 */
				new_node->depth = current_node->depth + 1; /* assign the depth */
				new_node->num_childs = 0; /* set num_childs to 0 */
				new_node->move = k; /* assign the current move */

				/* set the board */
				for (i = 0; i < SIZE; i++) {
					for (j = 0; j < SIZE; j++) {
						new_node->board[i][j] = current_node->board[i][j];
					}
				}

				new_node->parent = current_node; /* assign parent */

				/* applyAction here */
				bool change = execute_move_t(new_node->board, &new_node->priority,
				new_node->move);
				/* change equals true, 	if the move changes the board */
				/* change equals false, if the move does not change the board */

				/* Extra feature: grant bonus to open squares */
				open_square_bonus(new_node->board, &new_node->priority);

				/* Extra feature: grant bonus to keeping top tile at corner */
				// corner_bonus(new_node->board, &new_node->priority);

				/* increment for the number of generated nodes */
				*generated = *generated + 1;


				if (change == true) {

					/* check if the max_tile needs to be updated */
					for (i = 0; i < SIZE; i++) {
						for (j = 0; j < SIZE; j++) {
							//double temp = pow(2.0, (double)(new_node->board[i][j])/2.0);
							int temp = pow(2, new_node->board[i][j]);
							if (temp > *max_tile) {
								*max_tile = temp;
							}
						}
					}

					/* before adding new_node into the heap, call the addRandom
					function to improve the simulation */
					addRandom(new_node->board);

					/* add new_node to the priority queue */
					heap_push(&h, new_node);

					node_t* temp_node;
					temp_node = new_node;

					/* propagate the score back to the first action */
					if (propagation == max) {
						/* conduct maximise propagation */

						/* priority_sum is the maximum score of the current
						path */
						int priority_sum = 0;

						/* propagate all the way back to the node just before
						the parent node, while adding all the priorities
						along the way */
						while(temp_node->depth >= 2) {
							priority_sum = priority_sum + temp_node->priority;
							temp_node = temp_node->parent;
						}

						priority_sum = priority_sum + temp_node->priority;

						if (temp_node->score < priority_sum) {
							temp_node->score = priority_sum;
						}

					} else if(propagation == avg) {
						/* conduct average propagation */

						/* temporarily store the current node's priority in a
						temporary variable, i.e. score_to_add */
						int score_to_add = temp_node->priority;

						/* traverse up to one of the 4 applicable actions */
						while(temp_node->depth >= 2) {
							temp_node = temp_node->parent;
						}

						/* add score_to_add to that node's score variable */
						temp_node->score = temp_node->score + score_to_add;

						/* increment for the the number of scores */
						temp_node->num_of_scores++;

						/* update the average score */
						temp_node->average = (double)temp_node->score
						/ (double)temp_node->num_of_scores;
					}

					/* increment the number of children */
					current_node->num_childs++;

				} else {

					free(new_node);

				}

			}
			//printf("finished last loop\n");
		}
	}

	move_t best_action = 4;
	double best_score = 0;		// for propagation

	/* loop through the explored array to find best action,
	breaking ties randomly */
	if (propagation == max) {
		for (i = 0; i <= 5; i++) {


      /* check if we are at the first applicable action node */
      /* we can check this by looking at whether the depth is 1 */
			if (i > n_explored) {
				continue;
			}

			if (explored[i]->depth == 1) {  // this should be the line causing seg fault
				if (best_score <= explored[i]->score) {
					best_score = explored[i]->score;
					best_action = explored[i]->move;
				}
			}


		}
	} else {
		for (i = 1; i <= 4; i++) {
			if (i > n_explored) {
				continue;
			}

			if (explored[i]->depth == 1) {
				if (best_score <= explored[i]->average) {
					best_score = explored[i]->average;
					best_action = explored[i]->move;
				}
			}

		}
	}

	/* free memory for all the nodes inside the explored array */
	for (i = 0; i < n_explored; i++) {
		free(explored[i]);
	}

	free(explored);

	emptyPQ(&h);


	if (best_action > 3) {
		printf("best action is %d             \n", best_action);
		printf("error - best action out of bound\n");
		exit(EXIT_FAILURE);
	}

	printf("best action is %d\n", best_action);

	return best_action;
}


/* printing the board for debugging purposes */
void
print_board(uint8_t board[SIZE][SIZE]) {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			printf("%d ", board[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void
open_square_bonus(uint8_t board[SIZE][SIZE], uint32_t *score) {
	int i, j;
	uint32_t bonus;

	bonus = *score;

	for (i = 0; i < SIZE; i++) {
		for (j = 0; j< SIZE; j++) {
			if (board[i][j] == 0) {
				*score = *score + bonus;
			}
		}
	}
}


/* this function grants bonus to keeping the top tile at corner,
currently under development, therefore still incomplete */
void
corner_bonus (uint8_t board[SIZE][SIZE], uint32_t *score) {
	int i, j;
	uint32_t bonus;
	uint8_t top_score = 0;

	bonus = 100;

	for (i = 0; i < SIZE; i++) {
		for (j = 0; j< SIZE; j++) {
			if (top_score < board[i][j]) {
				top_score = board[i][j];
			}
		}
	}

	if (board[0][0] == top_score) {
		*score = *score + bonus;
	}
	/*else if (board[0][SIZE-1] == top_score) {
		*score = *score + bonus;
	} else if (board[SIZE-1][0] == top_score) {
		*score = *score + bonus;
	} else if (board[SIZE-1][SIZE-1] == top_score) {
		*score = *score + bonus;
	}
	*/
}
