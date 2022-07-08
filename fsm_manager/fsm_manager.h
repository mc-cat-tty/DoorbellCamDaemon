#pragma once

#include <time.h>

#include "states.h"

namespace fsm {
    typedef void (*fsm_callback) (State s, const void *args);

    class FsmManager {
        private:
            State current_state;  /**< current state of the finite state machine */
            fsm_callback callback_function;  /**< callback_function is called every time the state of the fms changes */
            time_t person_detected_sec;  /**< number of seconds in which there is a person in camera's fov */
            unsigned int min_sec;  /**< if person_detected_sec is over this threshold, fms moves to PERSON_STILL state */
            const void *callback_args;  /**< callback_function arguments */

        public:
            FsmManager()                = default;
            FsmManager(FsmManager &fm)  = default;
            
            /**
             * @brief Construct a new Fsm Manager object
             * 
             * @param callback Callback function called every time the state changes
             * @param frm_num Minimum number of frames to consider a the subject still in front of the door
             * @param start_state Starting state of the finite state machine
             */
            FsmManager(fsm_callback callback, const void* args, unsigned int sec, State start_state = State::WAITING_PERSON)
                : callback_function(callback), callback_args(args), min_sec(sec), current_state(start_state), person_detected_sec(time(NULL)) {
                    callback_function(this->current_state, this->callback_args);
                }
            
            /**
             * @brief Set callback_function to c
             * 
             * @param c New value of callback_function
             */
            void setCallback(fsm_callback c);

            /**
             * @brief Set callback_args to args
             * 
             * @param args New value of callback_args
             */
            void setCallbackArgs(const void* args);

            /**
             * @brief Set min_sec to set
             * 
             * @param num New value of min_sec attribute
             */
            void setMinSec(unsigned int sec);

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

#ifdef INLINE_ENABLED  // inline guard macro
#define INLINE inline
#include "fsm_manager.ipp"
#endif