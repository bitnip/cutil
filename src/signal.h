#ifndef __SIGNAL_H
#define __SIGNAL_H

#include "map/map.h"

struct SignalCallback {
    void (*callback)(void *context, void *data);
    void *context;
};

struct Signal {
    struct Map callbacks; // key: void *context, value: struct SignalCallback *
};

int signalCompose(struct Signal *signal);
void signalRelease(struct Signal *signal);
int signalReg(struct Signal *signal, void *context, void (*callback)(void *, void *) );
void signalUnr(struct Signal *signal, void *context);
void signalPublish(struct Signal *signal, void *data);

#endif