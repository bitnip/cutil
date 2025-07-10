#include <stdlib.h>
#include "signal.h"
#include "error.h"

int signalCompose(struct Signal *signal) {
    int result = mapCompose(&signal->callbacks);
    signal->callbacks.hashKey = ptrHash;
    signal->callbacks.freeData = free;
    return result;
}

void signalRelease(struct Signal *signal) {
    mapRelease(&signal->callbacks);
}

int signalReg(struct Signal *signal, void *context, void (*callback)(void *, void *) ) {
    struct SignalCallback *sc = malloc(sizeof(struct SignalCallback));
    if(sc) {
        sc->callback = callback;
        sc->context = context;
        return mapAdd(&signal->callbacks, context, sc);
    }
    return STATUS_ALLOC_ERR;
}

void signalUnr(struct Signal *signal, void *context) {
    free(mapRemove(&signal->callbacks, context));
}

void signalPublish(struct Signal *signal, void *data) {
    struct Iterator it = mapIterator(&signal->callbacks);
    struct SignalCallback *sc;
    while((sc = mapNext(&it))) {
        sc->callback(sc->context, data);
    }
}