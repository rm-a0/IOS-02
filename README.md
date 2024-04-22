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
TB - travel time between stops (bus) (in ms)

# Output
Output of the program is .out file with sequentially written processes. \
Example:
<pre>
  1: L 1: started
  2: L 3: started
  3: L 1: arrived to 2
  4: L 4: started
  5: L 3: arrived to 4
  6: L 2: started
  7: L 5: started
  8: L 2: arrived to 2
  9: L 6: started
  10: L 4: arrived to 2
  11: L 8: started
  12: L 5: arrived to 2
  13: L 6: arrived to 2
  14: L 7: started
  15: BUS: started
  16: L 8: arrived to 2
  17: L 7: arrived to 2
  18: BUS: arived to 1
  19: BUS: leaving 1
  20: BUS: arived to 2
  21: L 1: boarding
  22: L 2: boarding
  23: L 4: boarding
  24: L 5: boarding
  25: L 6: boarding
  26: L 8: boarding
  27: L 7: boarding
  28: BUS: leaving 2
  29: BUS: arived to 3
  30: BUS: leaving 3
  31: BUS: arived to 4
  32: L 3: boarding
  33: BUS: leaving 4
  34: BUS: arived to final
  35: L 2: going to ski
  36: L 7: going to ski
  37: L 6: going to ski
  38: L 3: going to ski
  39: L 1: going to ski
  40: L 5: going to ski
  41: L 4: going to ski
  42: L 8: going to ski
  43: BUS: leaving final
  44: BUS: finish
</pre>
