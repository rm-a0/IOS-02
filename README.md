# Overview
This project is inspired by Allen B. Downey's book: The Little Book of Semaphores (The Senate Bus Problem).

In the system, there are three types of processes: (0) main process, (1) ski bus, and (2) skier. Each skier goes to a ski bus stop after breakfast, where they wait for the bus to arrive. When the bus arrives at the stop, skiers board it. If the bus reaches its capacity, the remaining skiers wait for the next bus. The bus serves all stops sequentially and takes the skiers to the exit stop near the cable car. If there are more skiers interested in a ride, the bus continues for another round.

# How to use
For compiling you can use command make from Makefile or
<pre>
  gcc -std=gnu99 -Wall -Wextra -Werror -pedantic -pthread proj2.c -o proj2
</pre>
You can run the program using
<pre>
  ./proj2 L Z K TL TB
</pre>
where \
L - number of skiers \
Z - number of stops \
K - bus capacity \
TL - travel time to stop (skier) (in ms) \
TB - travel time between stops (bus) (in ms) \
