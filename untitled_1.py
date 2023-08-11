# Untitled - By: Administrator - 周四 8月 10 2023
THRESHOLD = (5, 70, -23, 15, -57, 0) # Grayscale threshold for dark things...
import sensor, image, time
from pyb import LED
import time
from pyb import UART

LED(1).on()
LED(2).on()
LED(3).on()

uart = UART(3, 115200)

sensor.reset()
sensor.set_vflip(True)
sensor.set_hmirror(True)
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQQVGA) # 80x60 (4,800 pixels) - O(N^2) max = 2,3040,000.
#sensor.set_windowing([0,20,80,40])
sensor.skip_frames(time = 2000)     # WARNING: If you use QQVGA it may take seconds
clock = time.clock()                # to process a frame sometimes.

while(True):
    clock.tick()
    img = sensor.snapshot().binary([THRESHOLD])
    line = img.get_regression([(100,100)], robust = True)
    if (line):
        rho_err = abs(line.rho())-img.width()/2
        if line.theta()>90:
            theta_err = line.theta()-180
        else:
            theta_err = line.theta()
        img.draw_line(line.line(), color = 127)
        print(rho_err,line.magnitude(),rho_err)
        if line.magnitude()>8:
            uart.write(rho_err)
        else:
            car.run(0,0)
    else:
        car.run(50,-50)
        pass
    #print(clock.fps())

