# No need for eventBasedAnimationClass here! :D

from Tkinter import *

class Robot(object):
    def __init__(self):
        self.canvasXDim = 240 #inches
        self.canvasYDim = 240 #inches

        self.xVelocity = 10 #inches / s
        self.yVelocity = 0 #inches / s
        self.rotational = 0 #rpm

        self.xPos = 0
        self.yPos = 0

        self.handles = []

    def tick(self, timerDelay):
        timerSeconds = timerDelay / 1000.0
        self.xPos += (self.xVelocity * timerSeconds)
        self.yPos += self.yVelocity * timerSeconds

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
        
class Gui(object):
    def __init__(self):
        self.root = Tk()
        self.canvas = Canvas(self.root, width = 400, height = 400)
        self.canvas.pack()
        self.sparky = Robot()
        self.timerDelay = 30 # ms

    def timerEventHandler(self):
        self.sparky.tick(self.timerDelay)
        self.sparky.redraw(self.canvas)
        self.canvas.after(self.timerDelay, lambda: self.timerEventHandler())

    def keyPressHandler(self, event):
        if event.keysym == "Up":
            self.sparky.redraw(self.canvas)
        elif event.keysym == 'q':
            exit()

    def bind(self):
        self.root.bind("<Key>", lambda e: self.keyPressHandler(e))

    def run(self):
        self.bind()
        self.timerEventHandler()
        self.root.mainloop()

Gui().run()
