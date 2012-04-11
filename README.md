# Siddhartha

PRAY is the file format used for installing new agents,
breeds, and rooms in the [Creatures Evolution Engine](http://creatures.wikia.com/wiki/Creatures_Evolution_Engine).
It also forms the basis of [NetBabel](http://creatures.wikia.com/wiki/NetBabel).

Erkleroo expected.

## how does it work?

A PRAY source file might look like this:

    "en-GB"

    group AGNT "Agent name"
    "Agent Type" 0
    "Agent Description" "Something cool"
    "Dependency Count" 1
    "Dependency 1" "agent.c16"
    "Dependency Category 1" 2

    inline FILE "agent.c16" "agent.c16"

A compiled PRAY file consists of various 'blocks'. Each
block starts with a common header, then has type-specific
data. A parser should read blocks in order, skipping
blocks it doesn't understand.

    struct block_header {
      char[4] block_type;
      char[128] block_name; // null-terminated
      long block_data_length;
      long block_data_length_uncompressed;
      long block_compressed;
    } __attribute__ ((little-endian));