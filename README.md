# HashTable

Self-written efficient *HashTable*, based on the idea of [Robin Hood hashing](https://en.wikipedia.org/wiki/Hash_table#:~:text=31%5D%3A%E2%80%8A353-,Robin%20Hood%20hashing,-%5Bedit%5D) 

Achieved asymptotics: 
- __O(n)__ memory, achieved by fixing load factor
- __O(1)__ time per query


# Techniques implemented 
- Self-written iterator 
- User-friendly interface achieved by using templates
- Selection of different load factors to increase effectiveness

# Usage
In order to use HashMap 
- clone this repository
- include ```hashmap.h``` header

Unit tests in ```main.cpp``` file!

# Usage example
```
#include <iostream>
#include "hash_map.h"

HashMap <int, int> hash_map;
hash_map[1] = 2;
hash_map[100000] = 200;
hash_map.erase(1);
auto iterator = hash_map.find(100000);
std::cout << iterator->second << '\n';
```
