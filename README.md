# 2048 Solver

This is a project which was assigned and completed for COMP20003 Algorithms and Data Structures class (2016 Semester 2).

In this project, an AI solver for the game [2048](https://gabrielecirulli.github.io/2048/) was implemented, using an online graph variant of [Dijkstra's Algorithm](https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm).

Depending on the commands given to the program, it will search for the best move for the current board state, based on the maximum/average score of the propagation and the specified depth.

For those who are not very familiar with the algorithm being discussed here, the implementation basically tries every combination of moves which we can perform and returns the best possible move to make.

## Compiling & Running The Program
Use the makefile to compile the executable file:
(please make sure that you are in the directory with the makefile)
```
make
```

Running the program with maximum flag and slow animation:
```
./2048 ai max 6 slow
```

Running the program with average flag, but without slow animation:
```
./2048 ai avg 6
```

## Example Input
An example could be:
```
./2048 ai avg 2 slow
```

`2048` is the program executable which we are running, and `ai` means that we are running the game with the implemented algorithm.

`avg` means that we are finding the action which will provide the highest possible average score, taking into account all the possibilities up to the specified depth D, which is `2` in this case. 

However, if you change the `avg` to `max`, then we shift focus to finding the action which has the maximum score based on any of its children.

`slow` is an optional flag which slows down the animation of the board and allows the user to visually see each individual move. If this flag is not used, then the program will speed through the solving process as fast as possible, until the game is finished.

Since the game allows the board to be manipulated in four ways (up, down, left, right), the algorithm will go through the depth of 2 by computing each of the following possibilities:

* Up Up
* Up Down
* Up Left
* Up Right
* Down Up
* Down Down
* Down Left
* Down Right
* (and so on...)

Then the program will sum up and take the average of all the scores which started with Up, Down, Left and Right. The best move is the one with the highest average score.

*Once the best move is applied, the whole process starts over again, and this process repeats until the board is completely filled and that would be game over.*