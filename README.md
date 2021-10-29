# Cab403_CarPark

**Car Park**
1-5 exits 
1-5 levels
20 spaces per level

**Entry**
Cars queue up at one of 5 entrances. 
When at front of queue license plate is read by LPR. 

**Manager**
**Monitors the status of LPR sensors and keeps tack of where each car is**
- Assigns cars where to park
- Counts cars coming in and out through LPR
- Direct cars to a parking spot which is displayed on screen
**Bill**
- Keeps track of how long car as  been there
- Produces Bill on exit (Triggered by exit LPR)
**Controls what is displayed on information screens**
- Current status on frequently-updating screen
- Shows how full level is
- Current status of the boom ates
- signs
- temperatures
- Revenue

**Simulator**
- A simulator receives a random license plate (randomly generate or not)
- Then queues up
**Simulator needs to generate a random length of time and put into struct**
- Car parks for 100-10000ms

**Shared memory**
-Processes communicate via a shared memory segment named PARKING (ALL CAPS)
-Segment is created when first created
-Segment contains sapce for 5 parking lot entrances, 5 exits and 5 levles
