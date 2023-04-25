import time
import board
import busio
import adafruit_ads1x15.ads1015 as ADS
from adafruit_ads1x15.analog_in import AnalogIn

GPIO.setup(23, GPIO.OUT)
GPIO.output(23, GPIO.LOW)

i2c = busio.I2C(board.SCL, board.SDA)
ads = ADS.ADS1015(i2c)
chan = AnalogIn(ads, ADS.P0)


print("{:>5}\t{:>5}".format("raw", "v"))

while True:

    if(chan.value > 11300):
        print("Agua a la verga")
        GPIO.output(23, GPIO.HIGH)
    else:
        print("Cierrale joto")
        GPIO.output(23, GPIO.LOW)

    print("{:>5}\t{:>5.3f}".format(chan.value, chan.voltage))
    time.sleep(0.5)