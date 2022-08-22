# Map Generation

### Overworld Gen:
#### Create a simulation of real world continent forming
1. Generate a Pangea
2. Separate the Pangea into smaller continents
3. Create continental drift
4. Smaller continents generate from the drift and fractures + tectonic activity.
    * **Types: (https://www.youtube.com/watch?v=3ZpDjdFzQUM&ab_channel=MooMooMathandScience)**
    * Convergent:
        * Continental + continental = mountains
        * Continental + oceanic = volcanoes.
        * Oceanic + oceanic = Island arc + marine trench
    * Divergent:
        * oceanic + oceanic = new ocean floor + volcanoes
        * continental + continental = trough.
    * Transform:
        * earthquakes
        * The movement is not constant. its sporadic. 
    * https://www.youtube.com/watch?v=R_pDKyg5YKY&ab_channel=ClickView


* https://en.wikipedia.org/wiki/Plate_tectonics

**Implementation:**
1. Form a Pangaea:
    * DLA algorithm?
    * Cellular Automaton algorithm?
2. Seperate the pangea into smaller continents:
    * run voronoi diagrams algorithm to seperate tectonic plates. 
3. Create continental drift:
    * move each region in a random direction (only the cardinal directions?)
    * determine the type of fault at each edge depending on the directions, and form the specified features. 
4. Shift the centers of the voronoi centers slightly and regenerate the diagram (step 2) and repeat step 3.

**Questions to research:**
* How do Lakes form?
* How do rivers form?
* How are biomes formed? 
