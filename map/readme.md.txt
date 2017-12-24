System Requirement:
------------------------

To be Run on linux based OS , preferably Ubuntu .


1.Install below components:
-----------------------------------
•	sudo apt-get install graphviz (for graphical display components)
•	sudo apt-get install evince (PDF reader/PS Reader)

2.Compile the program
-----------------------
g++ final_color_map.cpp -o final_colour -std=c++11

3.To test an example with default values please run batch file
./batch_color_map.sh


4.To test with a different example run
     ./final_color
	 
	 
5. The first prompt asks number of nodes, please enter number of nodes eg. 6

6. Second prompt asks file with node links, please enter filename eg. data.txt

7.Third prompt asks number of colors to be used, please enter eg. 3

IMPORTANT NOTE: I have provided four examples files with different number of nodes , please type in respective “number of nodes” value
 and “file name” in the prompt .
 
Files provided:
1.	data.txt – number of nodes =6
2.	petersen_data.txt – number of nodes=10
3.	us.txt – number of nodes =20
4.	state.txt – number of nodes=50
please choose “number of color” as 3 or 4.
For nodes greater than 50 and above please use 4 colors.

5.The program will create two .ps files namely "MRV.ps" and "Min-conflict.ps" , which contain the colored graph. 
The .ps files can be view with evince .

evince <filename>

