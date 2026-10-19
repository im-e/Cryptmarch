==============================================================
WM908 GAUNTLET PROJECT - BUILD LOG
==============================================================


==============================================================
ENTRY TEMPLATE (copy to the TOP of the entries for new days)
==============================================================

--------------------------------------------------------------
YYYY-MM-DD : short title
--------------------------------------------------------------
WHAT I BUILT / CHANGED:
  -

WHAT WENT WRONG AND HOW I FIXED IT:
  -

ALTERNATIVES I CONSIDERED (AND WHY I CHOSE WHAT I DID):
  -

NUMBERS (FPS, ms per frame, entity / tile counts, conditions):
  -

REPORT SECTION THIS FEEDS:
  (e.g. 2.3.1 Tile map)

SCREENSHOT TAKEN?  yes / no   (file name: )

STILL TO DO NEXT:
  -


==============================================================
ENTRIES (NEWEST FIRST)
==============================================================

--------------------------------------------------------------
2026-10-19 : Optimisations, Bugfixes, Map building by file
--------------------------------------------------------------
WHAT I BUILT / CHANGED:
  - Vector: operator[] out of bounds bug, made it return a safe value after asserting out of index. Applied the rule of three by looking at Image and seeing how it handles its copy + copy assignment and deleted them.
  - Graphics: Removed repeated blend writing, dramatically improved rendering performance by fixing bugs and improving drawing.
  - Map: created loadMap which loads a map from a file

WHAT WENT WRONG AND HOW I FIXED IT:
  - Vector: Two vectors could have pointed to the same memory if it were copied, which would crash when tried to be deleted. so I used the rule of three to fix that by deleting the copy and copy assignment constructors
  - Graphics: I had an issue in the semi transparent draw branch of my DrawImage and DrawTilemapImage function. I was taking a pointer of the window pixel and modifying the pixel to do my alpha changes, but then I was also drawing the pixel to the screen after already doing so via the pointer. This then gave me an idea on how to further optimise my drawing by using pointers. Working out the positions of both the window and image pixel by row, I would go across the row by bytes instead of recalculating every pixel. I had issues with the math of the window being RGB and the the image being RGBA but once I understood that was the case it became simple to implement.
  - Map: I had issues figuring out the width/height of the map just from the data alone, so included 2 numbers at the start on the first line to signify the width and height of the map. 

ALTERNATIVES I CONSIDERED (AND WHY I CHOSE WHAT I DID):
  - Vector: I could wrote a correct copy assignment constructor, which builds a new vector from an existing one. I chose not to simply because I do not see the need to do so, deleting them to forbid copying is the best and simplest option.
  -Graphics: The implementation I used forces all images to be RGBA as having fewer than 4 channels will break the alpha check and cause the image to be unrendered. I consider this a fine comprimise however as I can re-export my assets using paint.net to force them into RGBA.
  - Map: I could have read the raw data from fstream to see when a new line was created to determine the width and height of a map from its data, however i went for a manual input inside the data instead just to make file i/o easier. it allowed me to just use the >> operator to take the first two values from the file, and then read the rest of the data using the same operator in a for loop. this does give me a lot more oppourtunity of error if i input the wrong height and width, which i might change in the future. 

NUMBERS (FPS, ms per frame, entity / tile counts, conditions):
  - BASELINE: full-screen 32x24 tile map.
	BEFORE OPTIMISATION
    FPS = 900-950 (Release), 130 (Debug).
	AFTER OPTIMISATION
    FPS = 1800 (Release), 700 (Debug).

REPORT SECTION THIS FEEDS:
	3.Tile Map, Data driven loading

SCREENSHOT TAKEN?  yes

STILL TO DO NEXT:


--------------------------------------------------------------
2026-10-18 : Maps with tilemaps
--------------------------------------------------------------
WHAT I BUILT / CHANGED:

  - A Map with a TileDefinition table (Floor, Wall) and a hard-coded flat 32x24 tile array, drawn with DrawTilemapImage.

WHAT WENT WRONG AND HOW I FIXED IT:
  - Tile rendering was very slow in debug with a full-screen map. I optimised it further by using alphaAtUnchecked and atUnchecked, which i wasnt using previously. Due to my clipping code I can safely use these functions, which gained me an extra 50-60 fps in debug.

ALTERNATIVES I CONSIDERED (AND WHY I CHOSE WHAT I DID):
- Tile Rendering: I tried some basic cpu optimisation by swapping y to the outside and x on the inside while drawing my tiles, this matters because when the cpu reads a byte it fetches a cache line (64 bytes) and keeps it, so reading close by data is faster, but it didnt make much of a difference.
  - Tile storage: flat 1D array vs 2D array vs one object per tile. I chose a 1d array so i could search through it at O(1), rather than having a 2D array to store the values.

NUMBERS (FPS, ms per frame, entity / tile counts, conditions):
  - BASELINE: full-screen 32x24 tile map.
	BEFORE OPTIMISATION
    FPS = 600 (Release), 60-70 (Debug).
	AFTER OPTIMISATION
    FPS = 900-950 (Release), 130 (Debug).

REPORT SECTION THIS FEEDS:
  2.2.1 Containers, 2.3.1 Tile map, 3 Evaluation

SCREENSHOT TAKEN?  no

STILL TO DO NEXT:
  - Level file loader and runtime-sized tile array.


--------------------------------------------------------------
2026-10-17 : Assets, Vector, FPS counter and tilemap renderer, Logging
--------------------------------------------------------------
WHAT I BUILT / CHANGED:
 - Added the Kenney Desert Shooter Pack (tile sheets,interface/font tiles, sounds, licence) under resources/ShooterPack.
 - Own dynamic array template, Vector<T>: starting capacity 8, doubles when full, remove (keeps order) and removeSwap (does not).
 - Created the FPS calculation with the value shown on screen using a tilemap
 - Created a tilemap imagerenderer via tilemap offsets
 - My own logger, using cout << for type safe printing

WHAT WENT WRONG AND HOW I FIXED IT:
- Vector: my operator[] originally didnt have a bounds check on the index,
- the fps maths was first difficult because i had to split the digits one by one to draw it, i had issues with converting a double to int
  and had to use static_cast to explicitly convert it.
- the draw loop in main was wrong and i had to draw after .clear()
- I had to remove the cached files from my gitignore to keep my git clean
- I originally built my logger using printf, but realised that printing arguments was quite difficult.

ALTERNATIVES I CONSIDERED (AND WHY I CHOSE WHAT I DID):
- Vector Capacity: I chose to start with 8 since most small lists of things will probably be around 8 or smaller, and not be too
big which would waste memory for smaller objects.
- Vector growth: constant growth vs doubling, i had the decision between 1.5x or 2x, and i ended up going with 2x since it has the fewest grows in the long run, im expecting that as the game gets harder id probably spawn a lot more enemies and so growth would happen pretty often, i dont want that affecting performance as its quite costly, even if it uses more memory.
- FPS counter: I used a bitmap font for a tilesheet because I thought it would be the best way to render text considering what we had. it was a good intro for me to start getting used to tilesheets which i will need later. i also chose to update the fps counter every 0.1 seconds because without it, the value would change too fast.
- Tilemap renderer: The easiest way i could comprehend tackling a tilesheet draw was to use the same logic for image, but compress the area to the section of a tilesheet. i used the tilesheet offset method as i have a tool which can easily give me tileset coordinates rather than the raw pixel x,y which makes setting up tiles much easier
- Logger: i originally chose printf but went with a cout << approach so i could pass arguments much easier for more verbose logging. i can use std::tostring to print out values just by adding them to the message string.

NUMBERS (FPS, ms per frame, entity / tile counts, conditions):
  - 6000fps in debug mode with just a player and the fps counter

REPORT SECTION THIS FEEDS:
  2.1.1 Rendering, 2.1.3 FPS counter, 3 Evaluation

SCREENSHOT TAKEN? Not so far

STILL TO DO NEXT:
  - Own dynamic array and a tile map


--------------------------------------------------------------
2026-10-16 : Player, image renderer and first Vector
--------------------------------------------------------------
WHAT I BUILT / CHANGED:
  - implemented a player with basic normalised movement, and an image renderer with clamping and transparent alpha support. Started a Vector implementation.

WHAT WENT WRONG AND HOW I FIXED IT:
  - My first real commit included the Visual Studio .vs folder, including a 64.8 MB Browse.VC.db database file.
  - Blit/clipping: signed vs unsigned mismatch, and using windowsize vs image size in the clipping maths.
  - Movement originally wasnt normalised, so i checked if we were moving diagonally and normalised the direction vector

ALTERNATIVES I CONSIDERED (AND WHY I CHOSE WHAT I DID):
  - N/A

NUMBERS (FPS, ms per frame, entity / tile counts, conditions):
  - None recorded.

REPORT SECTION THIS FEEDS:
  2.1.1 Rendering and clipping, 2.1.2 Player and input, 2.2.1 Containers, 4 Limitations

SCREENSHOT TAKEN?  none

STILL TO DO NEXT:
  - Finish Vector, add FPS counter and a tile renderer.


--------------------------------------------------------------
2026-10-15 : Project setup
--------------------------------------------------------------
WHAT I BUILT / CHANGED:
  - Visual Studio solution and project
    files, and the GamesEngineeringBase.h framework header

WHAT WENT WRONG AND HOW I FIXED IT:
  - The initial commit also included the .vs folder (Browse.VC.db etc.). See 2026-10-16.

ALTERNATIVES I CONSIDERED (AND WHY I CHOSE WHAT I DID):
  - Start with the github setup first, do a gitignore before creating the project

REPORT SECTION THIS FEEDS:
  1 Introduction (technologies used), 5 What I would do
  differently

STILL TO DO NEXT:
  - Player and renderer.


==============================================================
EVALUATION CHECKLIST (tick as you go: replace [ ] with [x])
==============================================================
[ ] Baseline FPS recorded (Release build, same machine, same
    window size)
[ ] FPS after each optimisation, one change at a time
[ ] FPS with N enemies (try e.g. 10, 50, 200, 500)
[ ] FPS with many projectiles on screen
[ ] Debug vs Release noted
[ ] Screenshots saved with clear file names


==============================================================
KNOWN BUGS / LIMITATIONS (feeds report section 4)
==============================================================
- Vector::operator[]: out-of-bounds path uses "return;" in a
  function returning T&. Needs a valid return.
- Vector: no copy constructor / copy assignment (copying would
  double-delete the buffer).
- Git history contains a 64.8 MB .vs database file from commit
  276bba5 (removed from tracking afterwards).
- Full-screen tile rendering is slow (see 2026-10-18).
- (add more as you find them)
