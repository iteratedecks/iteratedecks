iteratedecks
============

Simulator used for War Metal Tyrant decks, missions, quests and more

## Installation

### The Simulator

We do not currently have binaries available for download. You can download the source and build your own version here on GitHub.

### Tyrant files

You will need a copy of the following data files used by Tyrant.

* cards.xml
* missions.xml
* quests.xml
* raids.xml

These can be downloaded directly from the Tyrant servers at http://kg.tyrantonline.com/assets/[file] where [file] is from the list above. These need to be in the same directory as the simulator.

## Usage

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

## Resources

* [Kongregate discussion thread](http://www.kongregate.com/forums/65-tyrant/topics/328599-sim-iteratedecks-1-0-3)

## Release Notes

###1.0.4
* Now supports Stun
* Chaosed Jam now targets units that have already attacked
* Blitzed units can now be Rallied
* Mimicked Payback can now hit the mimicker
* Chaosed abilities can now target already played units

###1.0.3
* Crush damage no longer triggers On Attack abilities
* -a now uses achievement id instead of achievement index
* Now supports more than 256 units at a time

###1.0.2
* Enabled battleground flag (-b)
* Regenerate now happens after On Death abilities
* Infuse only changes faction for helpful abilities
* Crush damage now ignores Impenetrable battleground effect
* Berzerk no longer procs on walls during Impenetrable battleground effect
* Added support for Toxic battleground effect (Decay was already supported)
* Friendly Fire battleground effect no longer overrides Commander's chaos
* Mimic is no longer evadable during Copycat battleground effect
* Split now targets Jammed units

###1.0.1
* Windows XP and Windows 7 support
* On Death and On Attacked now Rally and Augment Wait 1 units

###1.0.0
* Initial public release
* Supports Legion

## Acknowledgements

Our codebase is a direct descendant of NETRAT's EvaluateDecks simulator which is still being hosted:

* [Codebase](http://code.google.com/p/evaluatedecks/)
