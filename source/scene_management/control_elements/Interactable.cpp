// Base interface for interactive control elements.
// Defines update priority to ensure interactables process before reactors.
// This allows reactors to read current interactable values in the same frame.

#include "Interactable.h"


// Higher priority means earlier update; reactors use default (0) priority
const int Interactable::interactable_update_priority = 10;