#include "fsm_manager.h"

#include <iostream>

using namespace fsm;

#ifndef INLINE_ENABLED
#define INLINE
#include "fsm_manager.ipp"
#endif

State FsmManager::nextState(bool person_in_frame) {
    State old_state = this->current_state;

    switch (current_state) {
        case State::WAITING_PERSON:
            this->person_detected_sec = time(NULL);
            if (person_in_frame)
                this->current_state = State::PERSON_DETECTED;
            // else stay in WAITING_PERSON state
            break;
        case State::PERSON_DETECTED:
            std::cout << time(NULL)-person_detected_sec << std::endl;
            if (person_in_frame && time(NULL)-person_detected_sec >= min_sec)
                this->current_state = State::PERSON_STILL;
            else if (!person_in_frame)
                this->current_state = State::WAITING_PERSON;
            // else stay in PERSON_DETECTED
            break;
        case State::PERSON_STILL:
            if (person_in_frame)
                this->current_state = State::PERSON_STILL;
            else
                this->current_state = State::WAITING_PERSON;
            break;
        default:
            this->current_state = State::WAITING_PERSON;
    }

    if (this->current_state != old_state)
        callback_function(this->current_state);

    return old_state;
}

