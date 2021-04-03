# Among US with ZMQ

## General explanation

The server expects 10 players to connect to the server before the game starts. When the game has started, the server takes 2 random players and puts them in a separate "room". Here the server chooses 2 random players as the imposters. After the 2 imposters have been chosen, they return to the general area where all crewmates are present, including the imosters. The purpose of these imposters is to kill the crewmates. Every round it becomes "night" and the imposters can kill 1 person. When it becomes daytime again, the killed player is identified. The players have to guess who the imposter is by voting. The person with the most votes is kicked from the server. Ultimately, the goals is for the crewmates to win the game by voting out the imposters.
