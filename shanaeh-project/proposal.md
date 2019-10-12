Project Proposal: The Plant Protection Program
Solo Team: Shana Hadi

Goal: To create a raspberry-pi-powered system for measuring temperature, humidity, soil moisture, and other factors that can affect a growing plant. These sensors will be situated nearby or in the soil of the plant’s pot, and the raspberry pi will periodically check the sensors and output information onto a monitor. 

Milestone for Week 10: Get my raspberry pi to interact with the current sensors I have, the water level sensor and the temperature/humidity sensor. I want to make it functional so that I can get the data and put it on a monitor via HDMI. Once my soil sensors arrive, I will add this functionality as well. Then I will gather this information and output it onto a basic HDMI monitor by the end of the week. I will start work on using the analog monitor.

Resources/Questions: 
I am currently waiting for Amazon to send a SunFounder LCD2004 Module (a backlit char display screen) that I will later solder so I can connect it to my pi via pins — hopefully it will arrive by Wednesday so I can solder during that lab. $8, https://www.amazon.com/gp/product/B071W8SW9R?pf_rd_p=c2945051-950f-485c-b4df-15aac5223b10&pf_rd_r=C3GCKXD8JD4202675BJY
I also purchased a set of Kuman soil sensors — it comes in a pack of 5, and apparently the copper on the sensors oxidizes quickly, so it’ll be a race to keep it functional. $9, https://www.amazon.com/Kuman-Moisture-Compatible-Raspberry-Automatic/dp/B071F4RDHY/ref=sr_1_3?ie=UTF8&qid=1543702241&sr=8-3&keywords=moisture+sensor+raspberry+pi
How does wifi interaction with the pi work?
I’m not very familiar with how multiplexers work, but perhaps I don’t need it, so woohoo!
Is it possible to keep my pi going on this specific program, even if not attached to my computer (e.g. connect to the wall directly and have it do its thing)?


General outline of tasks:

Basic plan: 
Gather information from the sensors to output onto a monitor via HDMI. 
Afterward, be able to periodically gather information (every 30 minutes or so) and output it onto an lcd display.

Reach plan: 
Figure out the photoresistor so I can use it to track sunlight that my plant is receiving, or use another sensor.
Add the digital temperature sensor for another input (apparently the temp/humidity sensor wears out somewhat quickly too).

Potential extension (that’s frankly really cool, so I want to do it):
Figure out wifi capability(?) and write a script so that the pi can send an email to myself when a particular sensor reports low water levels or dangerous temperature/humidity.
