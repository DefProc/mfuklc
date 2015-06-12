# MFUKLC Registration Station

## Hardware

A Raspberry Pi, hooked up to a monitor and a USB keyboard, with networking (either Ethernet or a WiFi dongle) and an SL030 RFID reader.

## Software

 * Node RED
 * The [SL030 Node RED node](http://www.mcqn.com/weblog/adding_sl030_rfid_readers_node_red_raspberry_pi)
 * [Optional] wicd-curses (if you're using WiFi)
 * sqlite3
 * screen
 * kweb

## Installation

 1. Install the software
 1. Make Node RED run at boot time
 1. Copy the mfuk_myfid folder to /home/pi/
 1. sudo npm install -g node-red-node-sqlite
 1. sudo vi /usr/local/lib/node_modules/node-red/settings.js (configure a username/password for Node RED to prevent tampering)
 1. sudo vi /etc/rc.local (add the following lines before the "exit 0")
   # Give NodeRed time to get up
   sleep 60
   su -l pi -c "xinit /home/pi/mfuk_myfid/rfid_browser"
 1. sqlite3 /home/pi/mfuk_myfid/mfuk.db (create the registration database)
 1. sudo cp mfuk_myfid/flows_nodered.json ~root/.node-red/ (Copy the registration station flow to the right place to run)

