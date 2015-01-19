#swerve code

import math

def getData():
    return
#power(1=forward, -1=back)
#strafe(1 = right, -1 = left)
#rotation(1 = CW, -1=CCW)
#outputs [(w1speed,w1angle), (w2speed,w2angle), etc...]
def doSwerveCalc((power, strafe, rotation)):
    l = float(12) #inches
    w = float(12) # inches
    r = float(math.sqrt(l**2 + w**2))
    A = strafe - rotation * (l/r)
    B = strafe + rotation * (l/r)
    C = power - rotation * (w/r)
    D = power + rotation * (w/r)
    (ws1, wa1) = (math.sqrt(B**2 + C**2), math.atan2(B,C) * 180/math.pi) #front right
    (ws2, wa2) = (math.sqrt(B**2dd + D**2), math.atan2(B,D) * 180/math.pi) #front left
    (ws3, wa3) = (math.sqrt(A**2 + D**2), math.atan2(A,D) * 180/math.pi) #rear left
    (ws4, wa4) = (math.sqrt(A**2 + C**2), math.atan2(A,C) * 180/math.pi) #rear right
    maximum = max(float(max(ws1, ws2, ws3, ws4)), 1)
    ws1 /= maximum
    ws2 /= maximum
    ws3 /= maximum
    ws4 /= maximum
    return ((ws1, wa1),(ws2, wa2),(ws3, wa3),(ws4, wa4))

# takes in readings from potentiometers
# outputs current wheel angle (between -180 and 180, 0 is forward of bot)    
def getCurrentAngles(potReadings):
    return

def getCurrentSpeeds(speedReadings):
    return
        
def mainControlLoop(controls, data):
    ((ws1, wa1),(ws2, wa2),(ws3, wa3),(ws4, wa4)) = doSwerveCalc(controls)
    (a1, a2, a3, a4) = getCurrentAngles(data)
    (s1,s2, s3, s4) = getCurrentSpeeds(data)
    w1command = getWheelCommand(ws1, s1, wa1, a1)
    w2command = getWheelCommand(ws2, s2, wa2, a2)
    w3command = getWheelCommand(ws3, s3, wa3, a3)
    w4command = getWheelCommand(ws4, s4, wa4, a4)
    return (w1command, w2command, w3command, w4command)
    
# takes in a desired speed/angle and the current speed/angle
# outputs signed, differences (speed, angle)
def getWheelCommand(desiredS, currentS, desiredA, currentA):
    delSpeed = desiredS - currentS
    delAngle = desiredA - currentA
    if currentA > 180:
        return (delSpeed, -30) # turn ccw
    if currentA < 180:
        return (delSpeed, 30) #turn cw
    if abs(delSpeed) < 5: #CHANGE, some tolerance
        if abs(delAngle) < 5:
            return (0, 0)
        else:
            return (0, delAngle)
    elif abs(delAngle) < 5:
        return (delSpeed, 0)
    else:
        return (delSpeed, delAngle)
        
        
from eventBasedAnimationClass import EventBasedAnimationClass

class SwerveControllerApp(EventBasedAnimationClass):
    def __init__(self):
        super(SwerveControllerApp, self).__init__(500, 500)
        self.w = self.a = self.s = self.d = self.left = self.right = False
        self.timerDelay = 1
    
    def onKeyPressed(self, event):
        key = event.keysym
        if key == "w": self.w = True
        if key == "a": self.a = True
        if key == "s": self.s = True
        if key == "d": self.d = True
        if key == "left": self.left = True
        if key == "right": self.right = True
            
    def onKeyReleased(self, event):
        key = event.keysym
        if key == "w": self.w = False
        if key == "a": self.a = False
        if key == "s": self.s = False
        if key == "d": self.d = False
        if key == "left": self.left = False
        if key == "right": self.right = False
           
    def onTimerFired(self):
        self.mainLoop()
           
    def mainLoop(self):
        if self.w: power = 1
        elif self.s: power = -1
        else: power = 0
            
        if self.d: strafe = 1
        elif self.a: strafe = -1
        else: strafe = 0
            
        if self.right: rotation = 1
        elif self.left: rotation = -1
        else: rotation = 0
            
        controls = power, strafe, rotation
        data = getData() #CHANGE
        wheelCommands = mainControlLoop(controls, data)
        
            
        
SwerveControllerApp().run()
        
    
    