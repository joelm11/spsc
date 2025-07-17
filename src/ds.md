# General Design Principles

- These data structures are SPSC.
- These data structures have templated types.
- These data structures have a constant size.

## Testing

In addition to basic API functionality, tests should cover a range of different possible SPSC behaviour.

- Rate of production > rate of consumption.
- Rate of consumption > rate of production.
- Rate of production == rate of consumption.

## Benchmarking

Developing these datastructures is a way for me to learn about designing good SPSC datastructures. The information I'm most interested in is the speed of execution of these datastructures under the different test conditions. Speed of execution here refers to the time for a thread to get through a set amount of work. As such, it's probably fine to use `google/benchmark` for this purpose.

## Datastructures

### Locked Queue - LQueue

- This queue uses a lock to synchronize reads and writes.
- Attempts to push to a full queue should fail.
- Attemps to pop from an empty queue should fail.
