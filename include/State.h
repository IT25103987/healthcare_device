#ifndef State_h
#define State_h

#include <Arduino.h>
class State{
      private:
    bool state;

    public:
    State() {}

    bool getState() {
        return state;
    }
    void setState(bool newState) {
        state = newState;
    }

};
#endif
