## Goals for Now
1) Heightmap Only
2) make the easiest approach work
3) --
## Goals for Later
1) Biomes
2) --
## Generation
#### APPROACH 1 - Node Generation
Several **Nodes** around the map 
Nodes will be of these types:
- Shield Node - Geographical shield anchor points act as **relatively flat, low-moderate altitude, seismologically passive** areas.
	- Coast Nodes - Decide the coastline of the continent. Random noise present in some places to produce peninsulas and ex/intrusions from the main landmass
- Mountain Node - Connects to other nearby mountain nodes *if present*, forming a **range**. If alone only a small increase in height.

All above nodes are considered when a continent is formed, influencing it's shape.
- Island Node - Forms a island independent of continents. 
#### APPROACH 2 - Plate Tectonics

Simulates real plate tectonics, significantly better results *(?)* but hardest, probably best to avoid for now. Also requires geographical study.
Nodes will likely still be needed for features like Peninsulas, Islands, Random Intrusions like *Hudson Bay, Islands, Large Archipelagos like Indonesia* etc

**Plate Tectonic Boundary Types**
1. Convergent - Plates move towards each other `(normals collide at <=40 degrees)`
	1. Subduction (Oceanic `-->` Continental)
		Oceanic plate goes under continental, mountain chain on continent *like Andes* 
	2. Subduction (Oceanic `-->` Oceanic)
		Oceanic plate goes under another oceanic plate. Deep oceanic trenches
	3. Continental Collision
		Continental plates collide, forming massive mountains *like Himalayas*
2. Divergent - Plates move away from each other
	1. Oceanic Rift - New ocean floor formed from magma
	2. Continental Rift - Continent splits apart, new ocean basin formed *like Africa-Somali Plate*
3. Transform - Plates slide past each other `(assume their normals collide at >=60 degrees` *like San Andreas Fault*
4. Convergent-Transform `(normals collide at between 40 & 60 degrees)`
	Mix of convergent and transform behaviours, maybe just convergent behaviour but less intense *(?)*

- Mantle Plumes in some places to allow for random terrain features *like Iceland, Hawaii, etc* but mostly near ridges.
- At a point where 2 or more places collide, features are amplified/cancelled out/independently do their own thing based on their convergence/divergence/transformation 
**Pre-Generation** 
- Have **25-40%** Land area, rest water/ocean
**Generation**
1) Create a massive mostly connected landmass *like Pangaea* in the beginning, split it up via plate tectonics. **(Supercontinent Generation will likely require APPROACH 1 for nodes or noisemap at first)**
2) Use math functions to shift plates around on the grid matrix (experiment trial and error for how much the world age should be before realistic continents form), form higher altitudes where plates collide.
3) Remember to assign plate boundaries realistically

> [!note] CONTINENTS CONCEPT WITHOUT PLATE BOUNDARY TYPES - Check Image

### APPROACH 3 - Noise Generation
Easiest to do and most practical. Will give Minecraft-like terrain instead of continents BUT terrain will be just fine, probably should try this first. A few modifications could make realistic macroterrain *(?)* 

Use Perlin Noise.

> [!note] TERRAIN CONCEPT - Check Image
> Terrain would look like this, no realistic continental masses but good enough for small scale terrain demonstration

> [!note] Biomes
> Multiple layers for other features like biomes.
