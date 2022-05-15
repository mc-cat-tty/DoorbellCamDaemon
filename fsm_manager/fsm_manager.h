#pragma once

#include "states.h"

namespace fsm {
    typedef void (*fsm_callback) (State s);

    class FsmManager {
        private:
            State current_state;  /**< current state of the finite state machine */
            fsm_callback callback_function;  /**< callback_function is called every time the state of the fms changes */
            unsigned int person_detected_count = 0;  /**< counts the number of frames in which there is a person */
            unsigned int min_frm_count;  /**< if person_detected_count is over this threshold, fms moves to PERSON_STILL state */
        public:
            FsmManager(FsmManager &fm) = default;
            
            /**
             * @brief Construct a new Fsm Manager object
             * 
             * @param callback Callback function called every time the state changes
             * @param frm_num Minimum number of frames to consider a the subject still in front of the door
             * @param start_state Starting state of the finite state machine
             */
            FsmManager(fsm_callback callback, unsigned int frm_num, State start_state = State::WAITING_PERSON)
                : callback_function(callback), min_frm_count(frm_num), current_state(start_state) {};
            
            /**
             * @brief Set callback_function to c
             * 
             * @param c New value of callback_function
             */
            void setCallback(fsm_callback c);

            /**
             * @brief Force fsm's state update
             * 
             * @param s New value for current_state
             */
            void updateState(State s);
            
            /**
             * @brief Get current state
             * 
             * @return State current state
             */
            State getState() const;

            /**
             * @brief Toggle the fsm with person_in_frame as input
             * 
             * @param person_in_frame true if someone has been detected in the frame, false otherwise
             * @return State old (before the update) fsm's state
             */
            State nextState(bool person_in_frame);
    };
}