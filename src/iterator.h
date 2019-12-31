#ifndef __ITERATOR_H
#define __ITERATOR_H

enum ITERATION_MODE {
    ITERATION_DONE,
    ITERATION_FORWARD,
    ITERATION_REVERSE
};

struct Iterator {
    /*TODO: ideally const, so const lists can be walked. However, poping current on a list is nice...*/
    enum ITERATION_MODE mode;
    void *collection;
    void *current;
    unsigned int index;
};

#endif