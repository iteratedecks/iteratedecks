iteratedecks
============

Simulator used for War Metal Tyrant decks, missions, quests and more

# Installation

## The Simulator

We do not currently have binaries available for download. You can download the source and build your own version here on GitHub.

## Tyrant files

You will need a copy of the following data files used by Tyrant.

* cards.xml
* missions.xml
* quests.xml
* raids.xml

These can be downloaded directly from the Tyrant servers at http://kg.tyrantonline.com/assets/[file] where [file] is from the list above. These need to be in the same directory as the simulator.

# Usage

All usage is through the command line. You will need to convert your deck into an appropriate deck hash. We recommend the Fansite's [deck builder](http://tyrant.40in.net/kg/deck.php) for this.

Open a command prompt and navigate to the simulator's directory. Enter the following command to make sure your simulator is working properly (replace [attackhash] and [defensehash] with appropriate hashes.)

    iteratedecks-cli.exe [attackhash] [defensehash]

For example:

    iteratedecks-cli.exe QVH0+ofc+i P+DA+kBW+iBnBID2Bo

There are various flags available to alter how the simulation works. Here are the full usage notes:

    -n --number-of-iterations  sets the number of simulations to do
    -o --first-deck-is-ordered  marks the first deck (player deck) as ordered
    -a --achievement-index     index (not id) of achievement. not sure where the difference is
       --verify                verify a result, provide an accepted range in the form <lower bound>:<upper bound>, like "--verify 1:1" if the deck should win all the time
    -v --verbose               verbose output
       --seed                  set the seed, takes an optional argument. if none given use seed based on time.
       --color                 color the output, currently only ANSI colors supported
    -h --help                  print help
    -s --surge                 first deck will surge
    -r --raid-id               raid id
    -Q --quest-id              quest id
    -m --mission-id            mission id

# Acknowledgements

Our codebase is a direct descendant of NETRAT's EvaluateDecks simulator which is still being hosted:

* [Kongregate thread](http://www.kongregate.com/forums/65-tyrant/topics/195043-evaluate-decks-game-simulator)
* [Codebase](http://code.google.com/p/evaluatedecks/)
