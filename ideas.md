# Key:
* [ ] - A todo tickbox
    * [ ] - Did not start working on it
    * [?] - Possible thing to do, need to think about it more
    * [V] - Done.
    * [O] - Working on it now or need to start working on it soon. 

# TODO NOW:
* [ ]Take out all struct and data types into a seperate "skytypes.h" that will be included in every file.
* [ ]Change so that game bitmap buffer gets passed as a param instead of being global.

* [O]Make dt more persise than just ms. Need more something like microseconds.

* [O]Fix the BlitBitmap() method to work properly at different scales. Right now, higher scales cause jittery movement, 
    likely because we are bitting at large pixel-steps (the whole scale is one step).

* [V]Change the rendering code (and drawing code) so that if user tries to draw outside the bounds, we handle it instead of having segmentation fault.
* [ ]Rotation of bitmaps. Do the rotation code inside the BlitBitmap function.
    * The bitmap gets rotated when it is blitted, not beforehand.
        * Maybe find a way to save a rotated bitmap's data as well, in case we need to cache the rotated bitmap (if a rotation shows up alot, best to just save it).

* [?]Also try and make a second coordinate system for objects that appear outside the screen (such as asteroids spawning offscreen).
* [?]Add a virtual camera module, so that you can specify where in world coordinates your virtual camera is. 

# TODO (for later):


# DONE:


### Features to implement:
* 2D collisions.
* 2D kinematics.
* Add an alpha channel when storing bitmaps, so they can have transparency when rendering.
* Log mouse input in the input buffer
    * Position (x, y)
    * Mouse buttons (left and right click)
* More drawing methods (for primitives):
    * Drawing individual pixel
    * Drawing cirles and elipses
    * Drawing lines of variable thickness
    * Drawing Triangles
* Add a method to blit a whole bitmap to the screen.
* Physics engine. (simple kinematics only for now).
* Sound

### Refactoring
* Replace SDL with OpenGL. 
    * OpenGL is good for 2D.
    * Look into which version to use. Probably looks like 3.3 is good?
    * Pair it with SDL for window management. 

* For asset loading, implement a FIFO allocator. So that old assets can be flushed out in favour of new ones. In case asset size and level size are big, and not enough memory is
available for the asset arena. 

* Add mersenne twister and permutated conguential generator(PCG64) to the PRNG module.

### Bugs

### Assets
* Create a converter that will convert bitmaps from assets (bmps and ttfs)
into a custom asset file. 
    a. We can load the custom asset file on our shipped builds instead of having
    seperate bmp and ttf parsers that do it on runtime. Since loading the 
    custom asset file will be faster than a raw bmp or ttf, we can save on frame
    time this way.








# Game ideas:

## Graphical styles:
* Instead of having bitmaps such that pixels are adjacent, leave a gap between the pixels, so that the seperation between pixels is visible. It looks like a cool way to render pixels.
* Example of a 3x3 bitmap(P represents a filled pixel, and O represents an empty space):
    P O P O P
    O O O O O
    P O P O P
    O O O O O
    P O P O P
* as opposed to:
    P P P
    P P P
    P P P



## Map Generation

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
