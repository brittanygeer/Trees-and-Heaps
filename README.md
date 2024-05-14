# Trees-and-Heaps
A project led by the instructional team of CS 251 at Purdue University. Implements a generalized tree data structure which is then used to implement a pseudo filesystem that mimics the operations of a real filesystem.

I worked on the implmentation in three files: tree.hpp, file_size_max_heap.cpp, and filesystem.cpp. The skeleton code was created by the CS251 instructional team.

Part 1: Tree:

Implements a generalized tree data structure in tree.hpp with array-based representation, pooling for performance, and default root node.

Part 2: Pseudo Filesystem:

In filesystem.cpp, uses the generalized tree to simulate the operations of a real filesystem.

Part 3: Max Heap for File Size Statistics:

In file_size_max_heap.cpp, uses a max heap and the specialized tree filesystem to get the largest file's handle.
