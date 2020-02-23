# MPack PoC

Testing function to validate serialized msgpack items.

## Build

```
$ make
```

## Run

```
$ ./reader apache_10k.mp
```

## Data Sample

the file ```apache_10k.mp``` contains 10k of serialized msgpack items in the following format:

```
 array (2 items)
   [0] =>  extension fixext8
   [1] =>  map
```
