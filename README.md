Graphic weather station
This thing measures the temperature and airpressure and logs this to a graph every 15 minutes. It has 283 pixels to do so, wich is almost enough for 3 days. You can set the time, and calibrate the temp and airpr. by a menu. All parts are build in an old cicar box, exept for the BMP280, whitch is mounted outside. The BMP280 noticed a warmth coming from the screen. For protection most parts of the BMP280 are covered with hot glue.

Its powered by micro USB. A powerloss means a loss of all datapoints, but you can save the graph in EEprom  before you unplug. A timestamp is also saved and used to check, if the saved data is still valid. Old data is ignored and inactivity is given as blanks.

The thinking is done by a PRO TRINKET 3,3 V . The measeruments by a BMP 280 breakout. The timekeeping by A DS 3231. A 2.8" TFT LCD shows whats going on. All Adafruit products.

On the screen. The yellow line gives the airpressure. The green line gives the teperature. The vertical lines are hours. The red verticals are days. The orange verticals are 6 hour intervals.
For powersaving the screenleds can be powered off.
