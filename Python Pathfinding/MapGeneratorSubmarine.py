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

# This converts the data collected from MapDataSubmarine into a HUGE list

import threading 
from itertools import *
from SubmarineMM import GetXPos, GetYPos

GlobalCoordinates = []

# Plan is to create a list which contains a tuple, containing a truple and a bool or just char. The truple will obviously be a 3D coordinate and the bool (or char) will 
# signify whether that coordinate is "accessible" by walking to it in game. That way, it will be possible to apply the A* algorithm as well as section off areas for peeking 
# spots etc...
# the second part of the tuple could also signify what type of scaleable cover is there - whether it is possible to jump up, crounch jump etc...
# 
# Do not take the real z - have only two: 1, 0.
# 0 will be almost everything, 1 will be above CT, short, and some of catwalk  

# Automate the coordinates filling process by reading the notepad file.
# For the angled rectangles, write another formula which finds which coordinates would be in the 4 right angles, and if they are, then return false.
# When actually implementing the A* algorithm, its there that you increment in 0.5s.


class coordinates:
    corner1 = [] # These contain trupls of EVERY shape's corner1
    corner2 = []
    corner3 = []
    corner4 = []
    
    def __init__(self):
        # This will read the text file just parse it then fill the corner lists 
        with open('C:/Users/Martin/Documents/Visual Studio 2017/Projects/SubmarineAIsolution/MapDataSubmarine/submarineD2.txt') as f:
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

    def ParseTruple(self, line):
        arrowPosition = line.find("->")
        line = line[arrowPosition+2: len(line)]
        output = tuple(map(float, line.split(',')))
        return output

coObject = coordinates()

def Walkable(x, y, z):
    Valid = None # bool

    length = len(coObject.corner1)

    for count in range(0, length):
        if x > coObject.corner1[count][0] and x < coObject.corner2[count][0]:
            if y < coObject.corner1[count][1] and y > coObject.corner4[count][1]:
                Valid = True
                return Valid
    Valid = False
    return Valid



def GetCoordinateStatus(x, y, z):
    if x > -2500 and x < 2000:
        if y > -1270 and y < 3200:
            if z == 0 or z == 1:
                print(Walkable(x, y, z))
            else:
                print("Not valid")
        else: 
            print("Not valid")
    else:
        print("Not valid")

while True == True:
    x = GetXPos(0)
    y = GetYPos(0)
    GetCoordinateStatus(x, y, 0)


'''
corner1 = [(-876.00, -660.03, 0)] # These contain trupls of EVERY shape's corner1
    corner2 = [(440.00, -177.68, 0)]
    corner3 = [(439.97, -999.97, 0)]
    corner4 = [(-876.50, -998.69, 0)]

corner1 = (-876.00, -660.03, 194.61)
corner2 = (440.00, -177.68, 65.25)
corner3 = (439.97, -999.97, 70.10)
corner4 = (-876.50, -998.69, 193.32)

Length12 = corner2[0] - corner1[0]

###############
GlobalLists = [[], [], []]

def ListFiller1():
    for x in range(0, 10):
        for y in range(0, 1000):
            for z in range(0, 1000):
                GlobalLists[0].append((x, y, z))

def ListFiller2():
     for x in range(10, 20):
        for y in range(0, 1000):
            for z in range(0, 1000):
                GlobalLists[1].append((x, y, z))

def ListFiller3():
     for x in range(20, 30):
        for y in range(0, 1000):
            for z in range(0, 1000):
                GlobalLists[2].append((x, y, z))

Filler1P = threading.Thread(target=ListFiller1,)
Filler2P = threading.Thread(target=ListFiller2,)
Filler3P = threading.Thread(target=ListFiller3,)

Filler1P.start()
Filler2P.start()
Filler3P.start()

Filler1P.join()
Filler2P.join()
Filler3P.join()

print(list(chain(GlobalLists[0], GlobalLists[1], GlobalLists[2])))
'''






