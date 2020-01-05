# smart traffic light

Code convention is snake case and we use tabs for structure:
so we write methodes and variables like day_mode and not dayMode.

### Git basic commands:

git pull  --> Pullen vom remote branch <br />
git status --> Aktuellen stand sehen rot wenn nicht zu git hinzugefügt. <br />
git add *|filename --> Ein File oder mehrere zu git hizufügen. <br />
git status --> Aktuellen stand sehen grün wenn zu git hinzugefügt. <br />
git commit -m "Message" --> Einene Commit erstellen zum pushen danach. <br />
git push --> Aktuellen commit zu remote pushen. <br />

## States:

1) Init -- depending on the sensor data(Clock) the Maste micro Controller switches the states to day_mode or night_mode 
    1.1) The masters microcontroller lcd display(Menue 1), displays the current time.
    1.2) The masters microcontroller lcd display(Menue 2), displays which state(Red, Green) the (Cars, Walkers) have.
2) Normal mode -- The Master Micro Controller decided that it is day time and starts the normal mode which says
    2.1) The Slave Microcontroller sets an intervall for how long the light 

### Codes 
1) Tagesmodus(Wird nicht geschickt) -->  Init function set traffic Light green and Walker Red 
2) Nachtmodus --> Send to slaves code 2 
3) Switch to Red Cars Traffic Light and Green Walkers Traffic Light
4) Switch to Green Cars Traffic Light and Red Walkers Traffic Light
5) Switch to Yellow <b>Cars</b> Traffic Light
6) Someone is near the <b>Walkers</b> Trafic Light (Slave to Master communication)
7) Someone is near the <b>Cars</b> Trafic Light (Slave to Master communication)

## Sources 
https://www.avrfreaks.net/forum/atmega32-interfacing-spi

1) Tagesmodus 
2) Nachtmodus 
3) Switch to Red 
4) Switch to Green
5) Switch to Yellow
5) Someone is near the Trafic Light (Slave to Master communication)
