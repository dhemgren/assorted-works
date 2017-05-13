Small outtake of CUDA code (C/C++) from ongoing Bachelors Degree project at KTH.
The code is a single-program multiple-data implementation of Dijkstras algorithm as described in [this paper](http://link.springer.com/chapter/10.1007%2F978-3-642-01970-8_91), which solves the single-source shortest path problem; finding the shortest path from a source vertex to every other vertex in the graph. The implemented algorithm solves inherently sequential problems such as finding the minimum value in a dataset in parallel via reduction. 

The program takes DIMACS-format graphs and solves SSSP using the first specified vertex as source. Note that the code needs CUDA Toolkit 8.0 to compile, and a NVIDIA GPU with Compute 1.1 or greater to run.

Also included are sequential / parallel CPU implementations that apply the same methodology. These are inherently slower as the algorithm is designed for massively parallel processors (GPUs).