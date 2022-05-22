using namespace fsm;

INLINE void FsmManager::setCallback(fsm_callback c) {
    this->callback_function = c;
}

INLINE void FsmManager::setCallbackArgs(const void* args) {
    this->callback_args = args;
}

INLINE void FsmManager::setMinSec(unsigned int sec) {
    this->min_sec = sec;
}

INLINE void FsmManager::updateState(State s) {
    this->current_state = s;
}

[[nodiscard]] INLINE State FsmManager::getState() const {
    return this->current_state;
}
