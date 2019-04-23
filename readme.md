# The Two-Sum Problem
Given an array of integers and a target number, return the indices of the two numbers that add up to the target. 

This is an implementation in C with a custom hash table data type to improve efficiency. There is also an implementation in Java. Comparing the two, we see that we trade performance for cleaner, more readable code. Java also has a built-in hash table (java.util.HashMap). On the other hand, C requires us to do defensive copying, array bounds checking, casting, and dynamic memory allocation. Clearly, Java solves many of the low-level troubleshooting that C requires, but sacrifices some performance (e.g. memory overhead for Java objects). 

You may assume:
- There is exactly one pair of numbers that add to the target
- The array is non-empty
- The array has 1024 or fewer elements (the hash table does not have the ability to be resized - this might be added in a later commit if time permits)
