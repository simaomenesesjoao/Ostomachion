# Ostomachion solver

Finds all the possible combinations of polygons inside another polygon. To run, do

cmake -S . -B build && cmake --build build -j 10
build/main polygon_type puzzle_type num_threads

ex:
- build/main LLPoly Ostotiny 1
- build/main ContigPoly Ostomachion 8

With 8 threads, this program finds the correct solution in 3 minutes 20 seconds on my laptop.
