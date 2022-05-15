#include "fsm_manager.h"

using namespace fsm;

void FsmManager::setCallback(fsm_callback c) {
    this->callback_function = c;
}

void FsmManager::updateState(State s) {
    this->current_state = s;
}

State FsmManager::getState() {
    return this->current_state;
}

State FsmManager::nextState(bool person_in_frame) {
    State old_state = this->current_state;

    switch (current_state) {
        case State::WAITING_PERSON:
            this->person_detected_count = 0;
            if (person_in_frame)
                this->current_state = State::PERSON_DETECTED;
            // else stay in WAITING_PERSON state
            break;
        case State::PERSON_DETECTED:
            this->person_detected_count++;
            if (person_in_frame && person_detected_count >= min_frm_count)
                this->current_state = State::PERSON_STILL;
            else if (!person_in_frame)
                this->current_state = State::WAITING_PERSON;
            break;
        case State::PERSON_STILL:
            if (person_in_frame)
                this->current_state = State::PERSON_DETECTED;
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

