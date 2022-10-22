# Wombat
Wombat crosswords compiler

This is research code, provided with no guarantees of any kind.
The latest version of the code can be found at url `https://github.com/adibotea/wombat-cpp`.

# Sample run

`./wombat -v 0 -ntw3456 20 20 20 20 -mnw 100 -s 1 -i input-file.pzl -mts 150 -t 1800 -start_slot -1 -search 5 -heur 13 -weight .5`
- `-i input-file.pzl` specifies the input file with the grid (black cells) and paths to directories. See a sample pzl file in folder `sample-data`.
- `-mts 150` specifies a target score
- `-t 1800` gives a timeout in seconds
For other options, run `./wombat help`.

