#define MAX_STR       1500



//Global varaible to be able to call signal into my function (Gail allowed us to make this only for the signal handlers).
volatile int signalling;


// Make enum to know store the oracle and the guesser
enum {GUESSER, ORACLE} player;


// Make enum to hold the winner and the looser
enum {WINNER, LOSER} result;


// My personal function prototype
void  handleSigInt(int);
void  handleSigTSTP(int);
void  handleSigQuit(int);
char  playAgainOrQuit();
