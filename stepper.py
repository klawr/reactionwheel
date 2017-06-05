#!/usr/bin/python
from time import sleep
import wiringpi2 as wpi
wpi.wiringPiSetupGpio()

A=238
B=236
C=233
D=231
time=0.001
max=2000

wpi.pinMode(A, 1)
wpi.pinMode(B, 1)
wpi.pinMode(C, 1)
wpi.pinMode(D, 1)
wpi.digitalWrite(A, 0)
wpi.digitalWrite(B, 0)
wpi.digitalWrite(C, 0)
wpi.digitalWrite(D, 0)

def Step1():
    wpi.digitalWrite(D, 1)
    sleep(time)
    wpi.digitalWrite(D, 0)

def Step2():
    wpi.digitalWrite(D, 1)
    wpi.digitalWrite(C, 1)
    sleep(time)
    wpi.digitalWrite(D, 0)
    wpi.digitalWrite(C, 0)

def Step3():
    wpi.digitalWrite(C, 1)
    sleep(time)
    wpi.digitalWrite(C, 0)

def Step4():
    wpi.digitalWrite(B, 1)
    wpi.digitalWrite(C, 1)
    sleep(time)
    wpi.digitalWrite(B, 0)
    wpi.digitalWrite(C, 0)

def Step5():
    wpi.digitalWrite(B, 1)
    sleep(time)
    wpi.digitalWrite(B, 0)

def Step6():
    wpi.digitalWrite(B, 1)
    wpi.digitalWrite(A, 1)
    sleep(time)
    wpi.digitalWrite(B, 0)
    wpi.digitalWrite(A, 0)

def Step7():
    wpi.digitalWrite(A, 1)
    sleep(time)
    wpi.digitalWrite(A, 0)

def Step8():
    wpi.digitalWrite(A, 1)
    wpi.digitalWrite(D, 1)
    sleep(time)
    wpi.digitalWrite(D, 0)
    wpi.digitalWrite(A, 0)

a = [Step1,Step2,Step3,Step4,Step5,Step6,Step7,Step8]

i = 0
j = 0
while i <= max:
    b=a[i%8]
    b()
    if i < max and j==0:
        i+=1
    else:
        i-=1
    if i == max:
        j = 1
    if i == 0:
        j = 0
