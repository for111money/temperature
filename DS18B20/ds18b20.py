import time
import RPi.GPIO as GPIO
from w1thermsensor import W1ThermSensor


def ds18b20_setting():
    global sensor
    sensor = W1ThermSensor()
    
def ds18b20_temp(sensor):   
    temperature = sensor.get_temperature()
    return temperature


if __name__ == '__main__':
    try:
        GPIO.setmode(GPIO.BOARD)
        GPIO.setup(7,GPIO.IN,pull_up_down=GPIO.PUD_UP)
        ds18b20_setting()
        while True:         
            temp = ds18b20_temp(sensor)
            print(f"Temperature:{temp:.2f}°C")
            time.sleep(0.5)
    except KeyboardInterrupt:
        print("OVER")
        
        



