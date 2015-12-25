//NOTE: Read my readme.txt to see instructuins on how to run my program

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#include "a5defs.h"

int main (int argc, char *argv[]) {
    // Declaring my variables
    int oracleSocket, guesserSocket = 0;
    struct sockaddr_in  myAddr, clientAddr;
    int i, addrSize, bytesRcv,portno,questionNumber,maximumGuess;
  
    // Initialize my question number to one to start it's count from 1.
    questionNumber  = 1;
 
    // make a char array to help store characters and send it from one socket to the other
    char buffer[MAX_STR];
    char inStr[MAX_STR];
        
    // make a char variable that I call using my signal handlers
    char message[]  = "Congratulations!!! You just won the game!!!";
  
    // if arg is less than two, then the user passed in an invalid arguement
    if(argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
 
    // Else if the arguement is equal to 2 eg ./a.out port number(6000 - etc) then the player is the oracle
    else if(argc == 2) {
        player = ORACLE;

        oracleSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (oracleSocket < 0) {
            printf("eek! couldn't open socket\n");
            exit(-1);
        }
             
        portno = atoi(argv[1]);

        /* setup my server address */
        memset(&myAddr, 0, sizeof(myAddr));
        myAddr.sin_family = AF_INET;
        myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        myAddr.sin_port = htons(portno);

       /* bind my listen socket */
       i = bind(oracleSocket,
           (struct sockaddr *) &myAddr,
                       sizeof(myAddr));

       if (i < 0) {
           printf("eek! couldn't bind socket\n");
           exit(-1);
       }

       /* listen */
       i = listen(oracleSocket, 5);
       if (i < 0) {
           printf("eek! couldn't listen\n");
           exit(-1);
       }

       printf("\nThe oracle is Waiting for a guesser on port %d:\n", portno);
       fflush(stdout);

       /* wait for connection request */
       addrSize = sizeof(clientAddr);

       guesserSocket = accept(oracleSocket,
       (struct sockaddr *) &clientAddr,
                            &addrSize);

       if (guesserSocket < 0) {   
           printf("eek! couldn't accept the connection\n");
           exit(-1);
       }
       else {
           printf("\n|---------------------------------------------------------|\n");
           printf("   I got a connection from a guesser on port no: (%d)\n",portno);
           printf("|---------------------------------------------------------|\n\n");
       }
    }

    // Else if its more than 2 e.g(./a.out ip address port number) then the player is the guesser
    else {
        // Set the player to be the guesser
        player = GUESSER;

        // To help recogniser my port
        portno = atoi(argv[2]);

        /* create socket */
        guesserSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (guesserSocket < 0) { 
            printf("eek! couldn't open socket\n");
            exit(-1);
        }

        /* setup address */
        memset(&myAddr, 0, sizeof(myAddr));
        myAddr.sin_family = AF_INET;
        myAddr.sin_addr.s_addr = inet_addr (argv[1]);
        myAddr.sin_port = htons((unsigned short)portno);

        /* connect to server */
        i = connect(guesserSocket,
            (struct sockaddr *) &myAddr,
                        sizeof(myAddr));
            
        if (i<0) {
            printf("guesser could not connect!\n");
            exit(-1);
        }
        else {
            printf("\n|----------------------------------------------------------|\n");
            printf("    I am now connected to an oracle on port no: (%d)\n",portno);
            printf("|----------------------------------------------------------|\n\n");
        } 
    }

    // A while loop which helps re-establish connection without inputting ip address and port number
    while(1) {
        if(player == ORACLE) {
            /* handle signals for oracle's answer*/
            signal(SIGINT, handleSigInt);
            signal(SIGTSTP, handleSigTSTP);
            signal(SIGQUIT, handleSigQuit);

	    /* read message from guesser and do something with it */
 	    while (1) {
                // initializing
                signalling = 0;
                     
                // Receives messages from the guesser
                bytesRcv = recv(guesserSocket, buffer, sizeof(buffer), 0);
	        buffer[bytesRcv] = '\0';
                
                // print statement added on my own to show when it receives a question from the guesser
	        printf("\n\nThis is the YES or NO question asked by the Guesser:  %s\n", buffer);

                // If the oracle wants to quit he/she just quit they just type quit
	        if(strcmp(buffer,"quit") == 0)
	            exit(0);
                    
                // Send signals to the guesser's socket to respond to the question
                printf("Enter the answer to the guesser's YES or NO question: ");
                gets(buffer);
                strcpy(buffer,inStr);
             
                /* Each signal handle it's special request depending on what the question the user asks 
                 * If the signalling is 1 send a "YES" signal to the guesser socket in reply to the guesser's question */ 
                if(signalling == 1) { 
                    strcpy(inStr, "YES");
                    send(guesserSocket, inStr, strlen(inStr),0);
                }

                // Else if signalling is 2 send a "NO" signal to guesser socket in reply to the question asked.
                else if(signalling == 2){
                    strcpy(inStr, "NO"); 
                    send(guesserSocket, inStr, strlen(inStr),0);
                }

                // Else if signalling is 3 send a "WIN" signal to the guess socket in reply to the question asked.
                else if(signalling == 3) {
                    strcpy(inStr,message);
                    send(guesserSocket, inStr, strlen(inStr),0);
                    result = LOSER;
                    printf("\n\nYou just lost the game!!!! Tough Luck next time\n");
                    printf("\nWait to see if winner wants to play again or quit/ Wait for another connection......\n");
                    fflush(stdout);
                    break;
                }

                // If question is equal to 20 and and there is no win message sent then the oracle wins and breaks out of the while loop to check what the winner does
                if(questionNumber == 3 && strcmp(inStr,message) != 0) {
                    result = WINNER;
                    printf("\n\nI Won the game yaaaaaay!!!!!");
                    questionNumber = 1;
                    break;
                }
                questionNumber++;
 	    }
 
            // If the result is the winer prompt a message to play again or quit and set the winner to the oracle
            if(result == WINNER) {
                char in = playAgainOrQuit();
                if(in == '1') {
                    send(guesserSocket,&in,sizeof(in),0);
                    questionNumber = 1;
                    player = ORACLE;
                    gets(inStr);
                 }
               
                 // Else if if the oracle is the winner and doesn't want to play again close the oracle socket and clean up it's resources after it closes
                 else if(in == '0') {
                     close(oracleSocket);
                     exit(0);
                 }
             }

             // else if player is looser and it receives a message from the winnner set the loser to the guesser
             else if(result == LOSER) {
		 char d = 0;
		 int result;
                 result = recv(guesserSocket,&d,sizeof(d),0); 

                 // if the user recieves the char '1' which means yes it means yes the winner wants to play again then reset question number and start again
                 if(d == '1') { 
		     player = GUESSER;
                     questionNumber = 1;
                 } 
                 
                 // Else if the winner which is the guesser does't want to play again close the guesser socket and wait for connection
                 else if(d == '0' || result == 0) {
                     gets(inStr);
                     close(guesserSocket);
                 }
             }
        }

        // else if it is guesser 
        else {
            while (1){ 
                // Send message to the oracle
                printf("\nQuestion %d. Enter a YES or No question to get an idea of what object the oracle has in mind \nQuestion: ",questionNumber);
                gets(inStr);
                strcpy(buffer,inStr);
                send(guesserSocket,buffer, strlen(buffer), 0);
 
                // I increment question number to keep track of when it gets to 20
                questionNumber++;

                // If the guesser doesn't want to play again and decides to quit he/she just types quit
                if(strcmp(buffer, "quit") == 0) {
                    exit(0);
                }
            
                // Receives the Oracle respose to the question 
                bytesRcv = recv(guesserSocket, buffer, sizeof(buffer), 0);
                buffer[bytesRcv] = 0;
		
                // Print the oracle signal response to see what the oracle answered the guesser's guess.	
                printf("\nThis oracle responded saying:  %s\n\n", buffer);
                fflush(stdout);

                // If buffer compared to the winner then the guesser wins the gamea dn reset the game by setting the counter to 1
                if(strcmp(buffer,message) == 0) {
                    result = WINNER;
                    printf("You just won the Game");
                    questionNumber = 1;
                    break;
                }

                // Else if the question number is 20 and the guesser doesn't get a win message then the guesser becomes the looser
                else {
                    if(questionNumber == 4){
                       if(strcmp(buffer,message) != 0) {
                          if(player == GUESSER) {
                              printf("\nYou Just Lost the Game!!!! You have used up your 20 guesses tough luck next time!! Wait for winners decisions\n");
                              result = LOSER;
                              break;
                          } 
                       }
                    } 
                } 
            }
         
            // If result is equal to the winner the prompt the yes / no message if the winner wants to play again or quit
            if(result == WINNER) {
                char i = playAgainOrQuit();

                // if it's yes return 1 and start the game afresh and send a signal to the oracle
                if(i == '1') {
                    player = ORACLE;
                    send(guesserSocket,&i,sizeof(i),0);
                    gets(inStr);
                }

                // else if it's no close the sockets and exit!
                else if(i == '0') {
                     close(guesserSocket);
                     exit(0);
                }
            }

            // Else if the result is the loser receive a signal if the winner wants to play again or quit and act with it
            else if(result == LOSER) {
                int result;
                char a;
                result = recv(guesserSocket,&a,sizeof(a),0);

                // If the result is a char '1' it means the winner which is the oracle wants to play again and I reset question number
                if(a == '1') {
                     player = GUESSER;
                     questionNumber = 1;
                 }

                 // Else if it's a char 0 then the winner closes it's socket and cleans up its resources while the user waits for connection again
                 else if(a == '0' || result == 0) {
                     gets(inStr);
                     close(guesserSocket); 
                 }
            } 
        }
    }
}



/* SIGINT/Ctrl-C == yes */
void handleSigInt(int i) {
    signal(SIGINT,handleSigInt);
    signalling = 1;
    printf("\n\nYour response is :YES\n");
    fflush(stdout);
}

/* SIGTSTP/Ctrl-Z == no */
void handleSigTSTP(int i) {
    signal(SIGTSTP,handleSigTSTP);
    signalling = 2;
    printf("\n\nYour response is :NO\n");
    fflush(stdout);
}

/*SIGQUIT/Ctrl-\ == win */
void handleSigQuit(int i) {
    signal(SIGQUIT,handleSigQuit);
    signalling = 3;
    printf("\n\nYour response is:  WIN\n");
    fflush(stdout);
}


//Functions that helps ask to play again or quit
char playAgainOrQuit() {
    printf("\n\nTo play again type Y / N to quit the game!!\nResponse: ");
    char track = getchar();

    if(track == 'Y' || track == 'y'){
        printf("\n\nOk! Setting you up for a new round......\n\n");
        return '1';
    }

    else if(track == 'N' || track == 'n') {
        printf("\nHave a safe day, Bye!!!!!\n");
        fflush(stdout);
        return '0';
    }
    else {
        printf("\nYou just entered an invalid input\n");
        return playAgainOrQuit();
    }
}

