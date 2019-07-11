# MIT License

# Copyright (c) 2019 Martin Slomczykowski

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# http://homepages.abdn.ac.uk/f.guerin/pages/teaching/CS1013/practicals/aStarTutorial.htm 
# ^ ty 

import matplotlib.pyplot as plt 
import math


class coordinates:
    corner1 = [] # These contain trupls of EVERY shape's corner1
    corner2 = []
    corner3 = []
    corner4 = []
    
    length = None

    def __init__(self):
        # This will read the text file just parse it then fill the corner lists 
        with open('./submarineD2.txt') as f:
            lines = f.readlines()      

        for line in lines:
            if line.find("corner1") != -1:
                self.corner1.append(self.ParseTruple(line))
            elif line.find("corner2") != -1:
                self.corner2.append(self.ParseTruple(line))
            elif line.find("corner3") != -1:
                self.corner3.append(self.ParseTruple(line))
            elif line.find("corner4") != -1:
                self.corner4.append(self.ParseTruple(line))

        self.length = len(self.corner1)

    def ParseTruple(self, line):
        arrowPosition = line.find("->")
        line = line[arrowPosition+2: len(line)]
        output = tuple(map(float, line.split(',')))
        return output

coObject = coordinates()


def GetCoordinateStatus(x, y, z):
    if x > -2500 and x < 2000:
        if y > -1270 and y < 3200:
            if z == 0 or z == 1:
                for count in range(0, coObject.length):
                    if x > coObject.corner1[count][0] and x < coObject.corner2[count][0]:
                        if y < coObject.corner1[count][1] and y > coObject.corner3[count][1]:
                            return True
                return False
            else:
                return False
        else: 
            return False
    else:
        return False

class Astar:

    # add the starting coordinates to the children list 
    # repeat:
    # which ever of the children has that lowest F, add to closedList
    # this child has now become the parent 
    # add each of the 8 adjacent nodes to the children list 
    # filter out the non-walkable options as well as the options on the closed list
    # cycle through the children list:
        # find G H and F <- G IS ADDED ONTO THE FULL PATH OF Gs 
        # if it isnt on the open list, add it to the open list
        # if it is already on the open list, check to see if that node is better using G as the means to determine. 
            # if that G is better (lower) set it as the parent :repeat:
    # :repeat:
    # stop when you:
        # Add the target square to the closed list, in which case the path has been found or
        # Fail to find the target square, and the open list is empty, there is no path

    # Save the path. Working backwards from the target square, go from each square to its parent square until you reach the starting square. That is your path.

    # I think that this has been a success but we need to adjust the mapping. Instead of mapping the entire map into huge rectangles, I think we need to split the 
    # entire map into an army of 10x10 (or whatever) nodes. This way pathfinding will be better and it will just be better in general. If we need a precise location 
    # whether it be for nades or etc, you can still operate on that basis, but for most cases, 10x10 units should be more than accurate enough
    # Also, we can map every 10x10 node as corner, cover, obstacle, vaultable, jumpable, etc

    graphX = []
    graphY = []

    def __init__(self, x, y, z, target):
        self.children = [[x, y, z, False, 0, 0, 0]]    # 0, 0, 0 = F G H
        self.openList = []
        self.closedList = []
        self.parent = None

        self.xOrigin = x
        self.yOrigin = y
        self.zOrigin = z

        self.target = target
        self.AtTarget = False

        self.assingParent = True

        while self.AtTarget == False:
            self.Sequence()

        """for xGraphWorker in self.closedList:
            self.graphX.append(xGraphWorker[0])
        for yGraphWorker in self.closedList:
            self.graphY.append(yGraphWorker[1])"""


    def Sequence(self):
        while self.AtTarget == False:
            position = self.LowestFValue()

            if self.assingParent == True:
                self.parent = self.children[position]
                self.graphX.append(self.parent[0])# delete this
                self.graphY.append(self.parent[1])
                self.closedList.append(self.children[position])
            else:
                self.closedList.append(self.parent)
                self.assingParent = True
            self.children = []

            self.AdjacentNodeFiller()
            self.NodeKiller()

            # not implemented correctly V
            for count, dank in enumerate(self.children):
                self.WriteKeyValues(count) 

            if self.WriteOpenList() == True:
                self.assingParent = False
                self.closedList.pop()
                return 1

            print(self.parent)

            self.AtTarget = self.TargetCheck()

    def LowestFValue(self):
        CurrentLowest = self.children[0][4]
        CurrentLowestCount = 0
        for count, potential in enumerate(self.children):
            if CurrentLowest == 0 and potential[4] == 0:
                return count 
            elif potential[4] < CurrentLowest:
                CurrentLowest = potential[4]
                CurrentLowestCount = count
        return CurrentLowestCount
        
                 

    def AdjacentNodeFiller(self):
        x = self.parent[0]
        y = self.parent[1]
        z = self.parent[2]

        distance = 1

        self.children.append([x, y + distance, z, False, 0, 0, 0])     # zeros here because we have not defined their F G H values yet - could be replaced with none 
        self.children.append([x + distance, y + distance, z, True, 0, 0, 0])
        self.children.append([x + distance, y, z, False, 0, 0, 0])
        self.children.append([x + distance, y - distance, z, True, 0, 0, 0])
        self.children.append([x, y - distance, z, False, 0, 0, 0])
        self.children.append([x - distance, y - distance, z, True, 0, 0, 0])
        self.children.append([x - distance, y, z, False, 0, 0, 0])
        self.children.append([x - distance, y + distance, z, True, 0, 0, 0])
        
    def NodeKiller(self):
        # here we eliminate non-walkable nodes, as well as nodes already on the closed list
        length = len(self.children)
        count = 0 

        while count < length:
            x = self.children[count][0]
            y = self.children[count][1]
            z = self.children[count][2]
            walkable = GetCoordinateStatus(x, y, z)

            if walkable == False:
                del self.children[count]

                length = len(self.children)
            else:
                count = count + 1

        length = len(self.children)
        count = 0

        while count < length:
            testNode = self.children[count]
            removeNode = False

            for item in self.closedList:
                if item[0] == testNode[0] and item[1] == testNode[1]:
                    removeNode = True
                    break

            if removeNode == True:
                del self.children[count]
                length = len(self.children)
            else:
                count = count + 1
    
    def WriteKeyValues(self, nodeNumber):
        self.WriteHValue(nodeNumber)
        self.WriteGValue(nodeNumber)

        self.children[nodeNumber][4] = self.children[nodeNumber][5] + self.children[nodeNumber][6]

    def WriteHValue(self, nodeNumber):
        currentTuple = self.children[nodeNumber]
        currentX = currentTuple[0]
        currentY = currentTuple[1]
        currentZ = currentTuple[2]

        # I think that there is something inaccurate here
        diffX = 10*(abs(currentX - self.target[0])) # 50 here because demoing 50 per node
        diffY = 10*(abs(currentY - self.target[1]))

        self.children[nodeNumber][6] = math.sqrt(diffX ** 2 + diffY ** 2)
       
    def WriteGValue(self, nodeNumber):
        if len(self.closedList) != 0:
            if self.children[nodeNumber][3] == True:
                self.children[nodeNumber][5] = 14 + self.closedList[len(self.closedList) - 1][5] # 5 * 10 per node
            else:
                self.children[nodeNumber][5] = 10 + self.closedList[len(self.closedList) - 1][5]
        else:
            if self.children[nodeNumber][3] == True:
                self.children[nodeNumber][5] = 14 # 5 * 10 per node
            else:
                self.children[nodeNumber][5] = 10 

    def WriteOpenList(self):
        
        for count, node in enumerate(self.children):
            isInOpen = node in self.openList
            
            if isInOpen == False:
                self.openList.append(node)
            else:
                gCheck = self.BetterGCheck(count)

                if gCheck == True:
                    self.parent = self.children[count]
                    return True
                else:
                    #continue 
                    pass

    def BetterGCheck(self, nodeNumber):
        if self.parent[5] == self.children[nodeNumber][5]:
            return False

        if self.parent[5] < self.children[nodeNumber][5]:
            return False
        else:
            self.parent = self.children[nodeNumber]
            return True

    def TargetCheck(self):
        targetFound = False

        tx = self.target[0]
        ty = self.target[1]
        tz = self.target[2]

        for items in self.closedList:
           x = items[0]
           y = items[1]
           z = items[2]

           if tx == x and ty == y:
               targetFound = True

        return targetFound


astarinst = Astar(-1710, -880, 0, (-2150, -920, 0))

xTest = []
yTest = []
for count in range(0, len(astarinst.closedList)):
    xTest.append(astarinst.closedList[count][0])
for count in range(0, len(astarinst.closedList)):
    yTest.append(astarinst.closedList[count][1])

plt.plot(xTest, yTest)
plt.show()

