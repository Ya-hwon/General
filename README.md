# General
A collection of generally useful snippets

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

### RemoveList.h

This is intended to be used as a candidate list which is then reduced based on some criterium. 

Can be used as follows:

```cpp
RemoveList<size_t, false> removeList(30'000);
for (size_t i = 0; i < 30'000; i++) {
	removeList.add(i);
}
removeList.removeIf([](const size_t& elem) { return elem % 2; });
removeList.forEach([](const size_t& elem) { std::cout << elem; });
```

Exemplary output: 
```
imagine a sizable block of numbers here
```

#### Ok, but why?

Usually one would probably use a linked list for this but those require one (de)allocation per element, which I don't like (because it's slow) so I made this single (de)allocation (forward) linked list (well in the best case anyway) - works best if the starting size of the collection can be estimated at construction (because then I don't have to get more memory, copy all of the existing data and free the old memory), can be even faster if the use case guarantees that there won't be more elements than anticipated.

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

RemoveList<city, true> candidates(5000);
tree.query({ -80, 80, -80, 80 }, candidates); 

// 'candidates' now holds all previously inserted elements within the query envelope

```

Don't attempt to query a tree that has not been built. After the tree has been built further calls to add start filling a secondary tree that can be made the primary tree by calling build again.