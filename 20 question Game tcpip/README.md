## Author                                  
- Christian Michael

## Source Code                          
- tcpip.c


## compilation command                         
- make tcpip



## Run command                      
- For the oracle run: ./tcpip - port number   ,   For the guesser run: ./tcpip IP - address - port number



## Result:                                      
- Connects to the server using and IP address and a specified port using command line arguement. When connected,
it's connected, the guesser ask bunch of question not more than 20 questions though to get idea of what the 
oracle has in mind. If the guesser guesses it right then the guesser wins else if it doesn't get any of the
guesses right it looses the game and the oracle becomes the winner, else if it gets it right, then the oracle
looses the game and the winner gets a choice to quit or loose while the looser doesn't get any choice but wait
to see if the winner wants to play another round. If the winner wants to play another round, the winner becomes
the oracle, else if the winner doesn't want to play another round it just quits and the looser waits for connection
on thesame port from an oracle. 
