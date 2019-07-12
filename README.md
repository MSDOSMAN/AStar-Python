# AStar-Python

I completed this project a long long time ago and, really, it was more of a prelude to the more expansive and comprehensive work I did on CS:GO analysis later. This project comes in two parts, a CS:GO memory reading/ "hacking" .cpp module and a python A* implementation.

The .cpp module is fairly self-explanatory but note it was written for an older version of Counter-Strike, so some/ all of the offsets may be wrong (especially anything remotely tied to the HUD as this was before panorama). 

The A* implementation is a little harder to explain as it too comes in two parts. There is a core A* pathfind algo implemented, but at the time I tailored this to CS:GO map data I'd collect. Essentially, Dust2 (the name of the CS:GO map in question) is segmented into many, many, many squares each denoted with 4 corner coordinates as you might imagine. This data is read at the start of the algo and it defines the "arena" or "board" for the algorithm to take place. In the example uploaded, there is a simple rectangular box (a car, to be specific) blocking the path from A to B (this is the car in T-spawn D2 if you have experience with the game). Porting the algo implementation back to a regular, general application should be fairly straightforward. 

Now a quick disclaimer, this is technically the A* algorithm but it's more of a "real-time" implementation (as that is what I needed for my project). It does not "see" into the future more than one step which is of course suboptimal for certain cases. 

Take a look at [example output](https://github.com/MSDOSMAN/AStar-Python/blob/master/Example%20Output.png) as well as the data that was [fed into the algorithm](https://github.com/MSDOSMAN/AStar-Python/blob/master/Python%20Pathfinding/submarineD2.txt). It's essentially the path to be taken by the man standing in [this](https://github.com/MSDOSMAN/AStar-Python/blob/master/Example%20Map%20Location.png) picture from his current location over to the fence on the left side with the challenge being avoiding the car (no jumping!). 

*P.S.*
*Submarine is used as a naming convention throughout the project. This was inspired by the first computer to beat a super-grandmaster at chess, IBM's Deep Blue, because what's deep and blue?... A submarine of course - I know it's genius. :)*
