# smart traffic light

Code convention is snake case and we use tabs for structure:
so we write methodes and variables like day_mode and not dayMode.

### Git basic commands:

git pull  --> Pullen vom remote branch
git status --> Aktuellen stand sehen rot wenn nicht zu git hinzugefügt.
git add *|filename --> Ein File oder mehrere zu git hizufügen
git status --> Aktuellen stand sehen grün wenn zu git hinzugefügt.
git commit -m "Message" --> Einene Commit erstellen zum pushen danach.
git push --> Aktuellen commit zu remote pushen.

## States:

1) Init -- depending on the sensor data(Clock) the Maste micro Controller switches the states to day_mode or night_mode
