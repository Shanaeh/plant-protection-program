#**Project: Plant Protection Program**

#**Solo Team: Shana Hadi**

#**Brief Overview**

This project takes in data from three sensors: the digital DHT11 (temperature and humidity) sensor, analog HOYA water sensor, and analog Kuman soil moisture sensor. After interpreting the data, it displays it onto the SunFounder LCD2004 Module of 20x4 characters.
It also checks the soil moisture sensor and water sensor and will activate the LED if the soil moisture is too low or if the water sensor detects a leak.


#**Necessary hardware components**

1. dht11 sensor (temperature and humidity, blue)
2. analog HOYA water sensor
3. analog Kuman soil moisutre sensor (though any type will work)
4. SunFounder LCD2004 Module, 20X4 characters, intended for Arduino
5. mcp3008 (for interpreting analog to digital)
6. led light to activate if plant needs water or if there's a significant leak
7. various resistors, male to female jumpers, female to female jumpers


#**Default values**

Timer delay in-between sensor reading/display refresh is 6 seconds (for the purposes of the demo).
Moisture comparison value is 20.00%; if dips below this, the LED will activate.
Water level is normal when between 20 and 90; above 90 indicates a significant leak.


#**Usage**

Navigate to the shanaeh-project folder and type 'make install' and the project will run, and the sensor values will display on the module. 
Go to the plant_protection_program enum to change default values to customize for your particular plant needs.


#**Goals and Design**

I wanted to build this project (relatively) completely from my existing rpi library so that it would not be reliant on outside code (or an OS). This meant that particularly for the digital DHT11 sensor and the lcd module, I had to wade through the data sheets and figure out the nitty-gritty details of microsecond timing, how to interpret series of bits and to what instructions the bytes correspond, and how to make all the hardware components behave (for example, the lcd module has 16 pins, and uses 12 gpio pins). 

My intention was to write a program that would have practical (and personal) use after this class, for example to monitor the well-being of a fragile mango plant currently living in a sheltered corner of my house. 
While I considered outputting the sensor data onto an HDMI monitor, I thought it would be best to have all the hardware components small, portable, and as independent as possible from needing additional cords or components (e.g. all the lcd module needs are gpio pins, and the other sensors can remain attached to my pi).


#**Files I wrote** (the remaining are from lecture or libpi)

1. dht11_sensor.c and .h -- digital, reads the temperature and humidity, could display temperature in celsius and fahrenheit (I am using fahrenheit for my lcd display).
2. soil_sensor.c and .h -- analog, reads the soil moisture (coupled with some arithmetic).
3. water_sensor.c and .h -- analog, reads the water level for water present/nonexistent.
4. lcd_2004.c and .h -- takes in bytes of data and interprets as instructions (e.g. set display on) or as characters to output.
5. plant_protection_program.c -- calls the sensor functions and checks to see if water or soil sensors detect the plant needs watering or if there's a leak in the pot. 


#**Significant Challenges**

1. DHT11 (temperature and humidity) sensor had specific instructions in the data sheet for timing (like turn on the gpio pin for output, then turn it low for 18 ms, turn it high for 40 us, and then set to input). Interpreting the set of 80ish bits was also challenging. Only every other bit is useful, which was a surprising discovery; afterward, the set of useful 40 bits has to be divided among 5 bytes and then checked against the 5th byte (a checksum). At first, I thought this would be the most significant hurdle, until I wrote my lcd library.
2. I had a short-lived spi and adc adventure of several hours in trying to interpreting the analog data, only to realize that Chris had already written an excellent mcp3008 and spi library.
3. LCD module (described in the next section).


#**Favorite Part :)**

I'm really proud of my process of writing (and completing!) the lcd display library. Though the DHT11 sensor was also a significant challenge, I found that the LCD module had more complexity. Armed with only the hefty 60-page datasheet (included in the repo), I drew multiple diagrams for timing and ordering of commands (e.g. set display on, turn cursor on, make cursor blink, set address for char output) and also used trial and error to find the right instructions. Initializing the display depended on either 0/1 for a particular data pin in a given byte of information (and would decide if display was on/off and other crucial details).

At the beginning of the process, I was writing in quasi-binary by turning on/off sets of 8 GPIO pins at a time (0s and 1s to form one byte of data) and then toggling the Enable pin (on/off) with timer delays (500 microseconds) to signal to the display that data had been sent. Gradually, I abstracted the binary instructions into a lcd_write_byte and a lcd_write_line function, useful for a series of initialization instructions or for displaying pre-set characters. I found the line addresses to display at the start of each line through the char diagram and trial and error (the manual is intended for 2-line display, and mine is 4-line).

Chris was helpful in helping me understand where to start in the data sheet, and he pointed out that once I am able to translate a data sheet into specific commands for my pi, I'll be able to do it in the future with new hardware. This process was quite a wave of emotions (I was so glad just to see the cursor blink on after 32 quasi-binary instructions), but ultimately very rewarding. 

I feel like I've truly built this lcd library (and this general project of hardware/software) from the ground up! This was really helpful so I could recognize how much I've learned over the course of the quarter and how I can use it in the future, in classes or for my own side projects. Yay, and thank you!
