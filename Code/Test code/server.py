from socket import *
import time
from dataclasses import dataclass

def wrap(time, info):
    pacchetto = "Time:"+str(time)+";Data:"+info+";"
    return pacchetto


def current_time():
    result = time.localtime()
    result = str(result.tm_hour)+":"+str(result.tm_min)
    return result

serverPort = 42069
serverSocket = socket(AF_INET, SOCK_DGRAM)
serverSocket.bind(('', serverPort))
clientAddress = ('192.168.178.83', 1234)

while 1:
    message = wrap(current_time(), "ciao")
    #message = input("MSG: ")
    print("Server: ")
    print(message)
    serverSocket.sendto(message.encode('utf-8'), clientAddress)

    serverSocket.settimeout(2)

    try:
        ack, clientAddress = serverSocket.recvfrom(1234)
        ack = ack.decode('utf-8')
        print("Client: "+ack)
    except timeout:
        print("Timer scaduto")

    time.sleep(1)