# General Design Principles

- These data structures are SPSC.
- These data structures have templated types.
- These data structures have a constant size.

## Locked Queue - LQueue

- This queue uses a lock to synchronize reads and writes.
- Attempts to push to a full queue should fail.
- Attemps to pop from an empty queue should fail.

# Testing

In addition to basic API functionality, tests should cover a range of different possible SPSC behaviour.

- Rate of production > rate of consumption.
- Rate of consumption > rate of production.
- Rate of production == rate of consumption.
