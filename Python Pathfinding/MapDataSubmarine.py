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

from SubmarineMM import *
import time

# Map being used for reference: https://steamcommunity.com/sharedfiles/filedetails/?id=625150083

def TakeXsnap():
    xPos = round(GetXPos(0), 2)
    return xPos

def TakeYsnap():
    yPos = round(GetYPos(0), 2)
    return yPos

def TakeZsnap():
    zPos = round(GetZPos(0), 2)
    return zPos 

def WriteToFile():
    xString = str(TakeXsnap())
    yString = str(TakeYsnap())
    zString = str(TakeZsnap())
    combinedString = xString + ", " + yString + ", " + zString + "\n"
    
    text_file = open("submarineD2.txt", "a")
    text_file.write(combinedString)
    text_file.close()
        

while True == True:
    userInput = input()
    if userInput == '1':
        WriteToFile()
        time.sleep(0.001)

