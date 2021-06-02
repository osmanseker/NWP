# Among US in C with ZMQ

## General explanation

Everyone knows the game "Among us". This is a version written in C code but very basic. The server expects 10 clients/players to sent their name in order to connect to the server before the game starts. When the game has started, the server takes 2 random players and chooses them as imposter, the rest of the players are crewmembers. The purpose of these imposters is to kill the crewmates. Every round it becomes "night" and the imposters can kill 1 person by voting them out. When it becomes daytime again, the killed player is identified and kicked. The crewmembers have to guess who the imposter is by guessing which one it is and vote them out. The person with the most votes is kicked from the server. Ultimately, the goals is for the crewmates to win the game by voting out the imposters.

The win conditions are the following:

1. The crewmembers can only win if the 2 imposters are voted out.
2. if 1 imposter and 1 crewmember is left, the imposters win.
3. if there are 2 imposters and 2 crewmembers, the imposters win as well.


### compile

To compile this project you need to copy paste the ProFile in both the ProFiles of the service and clients. You will also need to download the inlcudes directory and follow the explained steps in that directory of how to save/use the includes.

More info on my wiki page
