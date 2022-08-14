# General
A collection of generally useful snippets

To run tests do the following:

cmake -S . -B build

cmake --build build

cd build

ctest


## What is in each File and how do I use it?

### Timer.h

This is, well, a timer ^^

Can be used as follows:

```cpp
{
  Timer timer("doSomething");
  doSomething();
}
```

Exemplary output: 
```
doSomething started
doSomething stopped after 63milliseconds 154microseconds 300nanoseconds
```

The timer also has a pause and a resume function as well as manual start and stop functions (by default this is done at con- and destruction) and the option to directly get the nanoseconds.

### HPRTree.h

A spatial index, based on JTS's Java version, based on the space-filling hilbert curve. The code that handles the curves themselves was not written by me and can be found on [GitHub](https://github.com/rawrunprotected/hilbert_curves).

Can be used as follows:

```cpp
struct city {
	std::string name;
	size_t population;
};

HPRTree<city> tree;

tree.add({ "cityName", 12345 }, { 139.6922, 35.6897 }); // one would propably load this data from some sort of database or file
for(auto& elem : dataSource){
  tree.add({elem.name, elem.population}, {elem.lon, elem.lat}); // lon=x, lat=y
  // first parameter is the element to be indexed, the second one is a J::Point (alternatively a J::Envelope) adjustable via the indexGeom typedef
}

tree.build();

std::vector<city> candidates(5000);
tree.query({ -80, 80, -80, 80 }, candidates); 

// 'candidates' now holds all previously inserted elements within the query envelope

```

Don't attempt to query a tree that has not been built, do not attempt to build a tree twice, don't add items to an already built tree
