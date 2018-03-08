#ifndef __NODE__
#define __NODE__

#include "utils.h"

/**
 * Data structure containing the node information
 */
struct node_s{
    uint32_t priority;

    uint32_t score;     // used to store the maximum propagation score

    /* used to count the number of scores, used for average score propagation */
    int num_of_scores;

    double average;     // used to store the average propagation score

    int depth;
    int num_childs;
    move_t move;
    uint8_t board[SIZE][SIZE];
    struct node_s* parent;
};

typedef struct node_s node_t;


#endif
