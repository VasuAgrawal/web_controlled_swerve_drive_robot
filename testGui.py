# No need for eventBasedAnimationClass here! :D

from Tkinter import *
import time
import socket
import struct

class Connection(object):
    def __init__(self):
        self.IP = '192.168.240.1'
        self.PORT = 6666
        
        self.DRIVE_MOTOR_SPEED_KEY = 0
        self.DRIVE_MOTOR_DIR_KEY = 1
        self.STEER_MOTOR_POSITION_KEY = 2
        self.STEER_MOTOR_DIR_KEY = 3

        self.KEY_SIZE = 5
        self.SPEC_SIZE = 3
        self.VALUE_SIZE = 8

        self.arduinoYun = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        
        while True:
            try:
                self.arduinoYun.connect((self.IP, self.PORT))
                break
            except:
                continue


        def clean(self):
            self.arduinoYun.close()

        def sendValue(self, key, value, spec = 0):
            if key > 31 or value > 255 or spec > 3:
                return False

            toSend = 0x0000
            toSend += key
            toSend <<= SPEC_SIZE
            toSend += spec
            toSend <<= VALUE_SIZE
            toSend += value

            buff = buffer(struct.pack(">H", toSend), 0, 2)
            arduinoYun.send(buff) #sendall?
            return True

class Robot(object):
    def __init__(self):
        self.canvasXDim = 240 #inches
        self.canvasYDim = 240 #inches

        self.xVelocity = 0 #inches / s
        self.yVelocity = 0 #inches / s
        self.rotational = 0 #rpm

        self.xPos = 100
        self.yPos = 100

        self.handles = []

        # self.robotConnection = Connection()

    def handleForward(self):
        self.yVelocity += 0.5
        self.checkMaxSpeed()
    def handleBack(self): pass
    def handleLeft(self): pass
    def handleRight(self): pass 
    def handleCW(self): pass
    def handleCCW(self): pass
    def pause(self):
        pass

    #power(1=forward, -1=back)
    #strafe(1 = right, -1 = left)
    #rotation(1 = CW, -1=CCW)
    #outputs [(w1speed,w1angle), (w2speed,w2angle), etc...]
    def doSwerveCalc(self, power, strafe, rotation):
        l = float(12) #inches
        w = float(12) # inches
        r = float(math.sqrt(l**2 + w**2))
        A = strafe - rotation * (l/r)
        B = strafe + rotation * (l/r)
        C = power - rotation * (w/r)
        D = power + rotation * (w/r)
        (ws1, wa1) = (math.sqrt(B**2 + C**2), math.atan2(B,C) * 180/math.pi) #front right
        (ws2, wa2) = (math.sqrt(B**2 + D**2), math.atan2(B,D) * 180/math.pi) #front left
        (ws3, wa3) = (math.sqrt(A**2 + D**2), math.atan2(A,D) * 180/math.pi) #rear left
        (ws4, wa4) = (math.sqrt(A**2 + C**2), math.atan2(A,C) * 180/math.pi) #rear right
        maximum = max(float(max(ws1, ws2, ws3, ws4)), 1)
        ws1 /= maximum
        ws2 /= maximum
        ws3 /= maximum
        ws4 /= maximum
        return [ws1, ws2, ws3, ws4], [wa1, wa2, wa3, wa4]
        # return ((ws1, wa1),(ws2, wa2),(ws3, wa3),(ws4, wa4))

    def tick(self, pressedKeys, timerDelay):
        if 'w' in pressedKeys: power = 1
        else if 's' in pressedKeys: power = -1
        else: power = 0

        if 'd' in pressedKeys: strafe = 1
        else if 'a' in pressedKeys: strafe = -1
        else: strafe = 0

        if 'Right' in pressedKeys: rotation = 1
        else if 'Left' in pressedKeys: rotation = -1
        else: rotation = 0

        drive, steer = self.doSwerveCalc(power, strafe, rotation)

        if 'Space' in pressedKeys:
            self.sendValues(drive, steer)

    @staticmethod
    def rangeMap(num, oldMin, oldMax, newMin, newMax):
        return (float(num - oldMin)/ (oldMax - oldMin)) * (newMax - newMin) + newMin

    def sendValues(self, drive, steer):
        #we'll worry about steer once potentiometers are applied
        maxDrive = 255
        # maxSteer = 125
        mDrive = map(lambda n: int(Robot.rangeMap(n, 0, 1, 0, maxDrive)), drive)
        # mSteer = map(lambda n: int(Robot.rangeMap(n, 0, 1, 0, maxSteer)), steer)
        for i in xrange(len(mDrive)):
            self.robotConnection.sendValues(self.robotConnection.DRIVE_MOTOR_SPEED_KEY, mDrive[i], i)
            self.robotConnection.sendValues(self.robotConnection.DRIVE_MOTOR_DIR_KEY, 1, i)
        # timerSeconds = timerDelay / 1000.0
        # self.xPos += (self.xVelocity * timerSeconds)
        # self.yPos += self.yVelocity * timerSeconds

    def draw(self, canvas):
        #scale for canvas size
        canvasFactorX = float(canvas.winfo_width()) / self.canvasXDim
        canvasFactorY = float(canvas.winfo_height()) / self.canvasYDim
        self.handles.append(canvas.create_oval(self.xPos *  canvasFactorX,
            self.yPos * canvasFactorY,
            self.xPos * canvasFactorX + 30,
            self.yPos * canvasFactorY + 30))

    def redraw(self, canvas):
        for handle in self.handles:
            canvas.delete(handle)
        self.draw(canvas)

    def clean(self):
        self.robotConnection.clean()
        
# class KeyData(object):
#     def __init__(self, time):
#         self.time = time
#         self.released = False


class Gui(object):
    def __init__(self):
        self.root = Tk()
        self.canvas = Canvas(self.root, width = 400, height = 400)
        self.canvas.pack()
        self.sparky = Robot()
        self.timerDelay = 30 # ms
        self.pressedKeys = {}
        self.lastPressTime = time.time()

    def timerEventHandler(self):
        self.sparky.tick(self.pressedKeys, self.timerDelay)
        self.sparky.redraw(self.canvas)
        self.canvas.after(self.timerDelay, lambda: self.timerEventHandler())

    @staticmethod
    def almostEqual(num1, num2, tolerance):
        if num1 <= num2 + tolerance and num1 >= num2 - tolerance:
            return True
        else:
            return False

    def keyDownHandler(self, event):
        # check exclusivity
        if (event.keysym == 'w' and 's' not in self.pressedKeys or
            event.keysym == 's' and 'w' not in self.pressedKeys or
            event.keysym == 'a' and 'd' not in self.pressedKeys or
            event.keysym == 'd' and 'a' not in self.pressedKeys or
            event.keysym == 'Right' and 'Left' not in self.pressedKeys or
            event.keysym == 'Left' and 'Right' not in self.pressedKeys or
            event.keysym == 'Space'):
            self.pressedKeys[event.keysym] = time.time()
        # self.pressedKeys[event.keysym] = KeyData(event.time)

    def keyReleaseHandler(self, event):
        self.pressedKeys.pop(event.keysym, None)
        # if (Gui.almostEqual(event.time
        #     - self.pressedKeys[event.keysym].time, 30, 1) or 
        #     Gui.almostEqual(event.time
        #     - self.pressedKeys[event.keysym].time, 500, 1)):
        #     self.pressedKeys[event.keysym].released = True
        # else:
        #     self.pressedKeys.pop(event.keysym)

    def mousePressHandler(self, event):
        self.sparky.pause()

    def bind(self):
        self.root.bind("<Button-1>", lambda e: self.mousePressHandler(e))
        self.root.bind("<KeyPress>", lambda e: self.keyDownHandler(e))
        self.root.bind("<KeyRelease>", lambda e: self.keyReleaseHandler(e))

    def run(self):
        self.bind()
        self.timerEventHandler()
        self.root.mainloop()

Gui().run()
