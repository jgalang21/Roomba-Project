# Mars Rover Roomba Project

In this project, we were assigned to program a Roomba with an embedded system to navigate a course with various obstacles. 

Our Roomba is equipped with a servo scanner, which has a PING and IR sensor. The PING sensor is better for calculating 
distance, while the IR is better for object detection. It also has UART capabilities, meaning it can transmit or receive
data through wifi/bluetooth. The user can control the bot's movement, meaning it can go forward, back, left, right, etc. 
Our group had used an open source terminal called PuTTY to control the bot. We equipped the bot to take in keystrokes as
commands for the bot. For example, 'w' (or 'W') would move the bot forward. Throughout the semester, we had to implement 
small functions of the bot. One lab that we had to work on was getting the bot to move a certain distance and when it 
hits an object, the bot had to backup, turn, go forward, turn again, and continue moving forward.

Our design needed to meet the following requirements:
1. Stay within bounds marked by white tape.
2. Avoid hitting any tall objects, except small ones that can't be picked up by sensors. The small ones can be hit, 
but the bot must be able to navigate and act accordingly. 
3. Detect objects and specify how many objects there are, and their widths. 
4. Reach the goal area marked by 4 thin pillars and colored paper on the floor. When it's reached, it would play a song!
5. Provide reliable feedback to the user, who will not be able to see the course when navigating. 
5. Navigate the entire course blindly, within 20 minutes or less. 

Here are a few pictures: https://imgur.com/a/jGPMwRS

All the files in this repository contain the source code for this project. 
