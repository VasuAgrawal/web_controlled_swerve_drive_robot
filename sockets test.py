import socket
import struct

# globals / macros
IP = '192.168.240.1'
PORT = 6666
DRIVE_MOTOR_SPEED_KEY = 0
DRIVE_MOTOR_DIR_KEY = 1
STEER_MOTOR_POSITION_KEY = 2
STEER_MOTOR_DIR_KEY = 3

KEY_SIZE = 5
SPEC_SIZE = 3
VALUE_SIZE = 8

arduinoYun = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

def main():
    arduinoYun.connect((IP, PORT))

def sendValue(key, value, spec = 0):
    toSend = 0x0000
    toSend += key
    toSend <<= SPEC_SIZE
    toSend += spec
    toSend <<= VALUE_SIZE
    toSend += value

    print bin(toSend)
    buff = buffer(struct.pack(">H", toSend), 0, 2)
    arduinoYun.send(buff)
