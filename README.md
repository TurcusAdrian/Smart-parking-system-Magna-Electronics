# Smart-parking-system-Magna-Electronics
Smart parking system made for Liga AC LABS 2024 at Magna.

The project is composed of a Raspberry Pi 4, Arduino Nano, 2 servo motors SG90, 4 sonic sensors HC-SR04 and a Raspberry Pi V2 camera.

This smart parking system is modeled as a private parking lot, accepting only romanian license plate numbers that are already in the database inserted by us (technology used : MariaDB - local on our Raspberry Pi).

  The system works as following: A car comes in front of the entry sonic sensor and it sends a signal using threads (found in last_threads.c file) to the image processing python script that it needs to start the V2 camera. The camera starts the preview and then takes a picture of the car with its license plate. If the conditions are met ( license plate is a rectangle, enough light in the room, camera is focused) the script returns the characters on the license plate and calls the license_plate_updated python script to check if the number is indeed romanian and it doesn't break a set of rules (for counties besides "B" it should have 2 numbers from 01-99, letters at the end must not be 'Q' and many more). That script validates license plate numbers that are regular, temporary plates numbers, Consulate numbers, MAI numbers, etc. Once the number is validated, it returns to the threads a message to check if the number detected is in the database. If it is in the database, the servo motor opens up the barrier and the car can enter (if it stands right below the barrier between the sonic sensors, the barrier remains opened until the car has passed both sensors. If the car is not from the database, it will not be granted access.

  In case of the picture not being good enough (or an invalid number), the arduino script waits a set amount of time until it sends the .py script a signal to take a picture once again redoing the whole process from above. In the case of the car entering the parking lot a message is displayed on the LCD with the number of spots and if it is not accepted, it will display a message.


