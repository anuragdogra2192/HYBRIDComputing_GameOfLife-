# HYBRID Computing: Game of Life
HYBRID computing project, Game of Life (C++, MPI &amp; OPENMP)

Problem Descreiption here:
https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life

Assignment solution:
* Grid size or board size row X column: 240 X 240
* Divide the whole grid into sub-grids
* Uses MPI to communicate between boundary cells on different sub-grids
* Within each sub-grid we have used openmp for shared memory parallel to compute the rules within.

The rum-time analysis profile is also available. 
