# Range

## Target

## Score Board  
![Scoreboard](MFUKLC-scoreboard-01_420.jpg)   
The scoreboard is made using seven segment displays by [Matthew Venn](https://github.com/mattvenn/big-led-driver)

Files: MFUKLC-scoreboard-working.cdr (all other formats are generated (will switch to SVG once base design work done))

## Todo
* Sound -- Do we want a speaker or something more ecletic (fog horn)
* Target -- How many, what size/s how to house/hang

## Modules

### Control Module (CM)
![Control Module](MFUKLC-scoreboard-CM-01_420.jpg)   
![Control Module](MFUKLC-scoreboard-CM-02_140.jpg)   
The main control module.  

#### Microcontroller Pins (Arduino Mega)
* DIGITAL 13 -- Start Button -- (pin 13 so can connect other pin to ground with standard 2 pin header)

#### Connectors
* (U1) USB Type B -- For Arduino Connection
* (DC1) 2.1mm DC Jack -- For Power (12 v) 
* (BM) -- Button Module -- 2.1mm DC Jack -- N/O Switch
* (TM) -- Time Module -- 8 pin 2.54mm Dupont style connector 
* (SM) -- Score Module -- 8 pin 2.54mm Dupont style connector
* (RM) A-D -- Target modules -- To be determined.  

### Parts
* BOLT-M6-X-12-01 (x32) -- M6 x 12 mm Bolt
* NUTT-M6-M-STAN-01 (x32) -- M6 Nut
* CM-01 (x1) -- Base Plate
* CM-02 (x2) -- Side Plate
* CM-02-C (x1) -- Side Plate (Connections) Score Board
* CM-02-B (x1) -- Side Plate (Connections) Arduino (NOT YET DESIGNED)
* CM-03 (x1) -- Top (to be cut from acrylic)
* OOBB-MP-BREB-05-03 (x2) -- Breadboard Mounting Plate (5x3)
* OOBB-MP-ARDM-07-03 (x1)Arduino Mega Mounting Plate (7x3)
* OOBB-MP-I01-08-03-02 (A-D) (x2) -- Mounting Plate 8 pin 2.54 mm Connector

### Time Module (TM)
![Time Module](MFUKLC-scoreboard-TM-01_420.jpg)   
![Time Module](MFUKLC-scoreboard-TM-02_140.jpg)
![Time Module](MFUKLC-scoreboard-TM-03_140.jpg)  

A 3 digit display to show the 30 second countdown
#### Connectors
* Only one (OE,LE,CLK,SDI,12V,5V,gnd) (gnd black)

#### Parts
* BOLT-M6-X-12-01 (x20) -- M6 x 12 mm Bolt
* NUTT-M6-M-STAN-01 (x20) -- M6 Nut
* NHFF-M3-X-30-01 (x12) -- M3 x 30 mm Nylon Standoff (F-F)
* NSCR-M3-X-12-70 (x24) -- M3 x 12 mm Nylon Screw Cross Head
* Matrix Module (x3) -- [Matrix Module](https://github.com/mattvenn/big-led-driver) -- By Matt Venn
* TM-01 (x1) -- Back -- Back, holds the matrices, and connector block
* TM-02 (x2) -- Side (Long) -- Long side piece.
* TM-03 (x1) -- Side (Short) -- Short side.
* TM-03-C (x1) -- Side (Short, Connections) -- Short Side with connections.
* TM-04 (x1) -- Top -- Top (to be cut from smoked acrylic)
* OOBB-MP-I01-08-03-02 (A-D) (x1) -- Mounting Plate 8 pin 2.54 mm Connector

### Score Module (SM)
A  6 digit display for showing the runs score.

#### Connectors
*    

#### Parts
* BOLT-M6-X-12-01 (x28) -- M6 x 12 mm Bolt
* NUTT-M6-M-STAN-01 (x28) -- M6 Nut
* NHFF-M3-X-30-01 (x12) -- M3 x 30 mm Nylon Standoff (F-F)
* NSCR-M3-X-12-70 (x24) -- M3 x 12 mm Nylon Screw Cross Head
* Matrix Module (x3) -- [Matrix Module](https://github.com/mattvenn/big-led-driver) -- By Matt Venn
* SM-01 (x1) -- Back -- Back, holds the matrices, and connector block
* SM-02 (x2) -- Side (Long) -- Long side piece.
* SM-03 (x1) -- Side (Short) -- Short side.
* SM-03-C (x1) -- Side (Short, Connections) -- Short Side with connections.
* SM-04 (x1) -- Top -- Top (to be cut from smoked acrylic)
* OOBB-MP-I01-08-03-02 (A-D) (x1) -- Mounting Plate 8 pin 2.54 mm Connector

### Target Module (RM) (x6) 
![Target Module](MFUKLC-scoreboard-RM-01_420.jpg)   
![Target Module](MFUKLC-scoreboard-RM-02_140.jpg)   
The targets

#### Connectors
* Only one 2.1mm barrel jack, center signal, outer gnd.

#### Parts
* BOLT-M6-X-12-01 (x8) -- M6 x 12 mm Bolt
* NUTT-M6-M-STAN-01 (x8) -- M6 Nut
* RM-01 (x1) -- Base -- The base that holds the PCB glued in place.
* RM-02 (x3) -- Side -- The side
* RM-02-C (x1) -- Side (Connections) -- The side with a hole for a 2.1mm barrel jack
* RM-03-2K (x1) -- Top (2000) -- The top (this one for 2000 points)


### Button Module (BM)
![Button Module](MFUKLC-scoreboard-BM-01_420.jpg)   
![Button Module](MFUKLC-scoreboard-BM-02_140.jpg)
A 100 mm button to start the timing

#### Connectors
* Only one two pin button close (NO), 2.1mm DC jack.

#### Parts
* BOLT-M6-X-12-01 (x24) -- M6 x 12 mm Bolt
* NUTT-M6-M-STAN-01 (x24) -- M6 Nut
* ABUT-100-R-STAN-01 (x1) -- 100 mm Red Arcade Button
* DCJP-21D-X-08PM-01 (x1) -- 2.1 mm Panel Mount (8mm) DC Jack
* CRFM-48D-R-STAN-01 (x2) -- 4.8 mm Red Female Crimp Connector
* BM-01 (x1) -- Top -- Has a hole for a 100 mm button, is  173 mm x 173 mm (9x9 OOBB)
* BM-02 (x3) -- Side -- Side Panel 
* BM-02-C (x1) -- Side (Connections) -- Side Panel with conenctions
* BM-03 (x1) -- Bottom -- Bottom Panel173 mm x 173 mm (9x9 OOBB)
* BM-04 (x2) -- Base -- A base for the box to sit on, has holes to screw down or glue stars to glue.


## Bin Lid

Transporting amunition is an issue. We use a rubbish bin with a custom lid.
	
![Bin Lid](MFUKLC-binLid-01_420.jpg)  
![Bin Lid](MFUKLC-binLid-01_140.jpg)  

## Stencil

Branding is always important. To aid in this we have an A4 #MFUKLC stencil
![Stencil](MFUKLC-stencil-01_420.jpg)  	
