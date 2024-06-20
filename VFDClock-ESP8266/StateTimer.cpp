#include "States.h"

void TimerState::display(StateMachine &stateMachine) {
    uint32_t now_mill = millis();

    if(now_mill >= time_out)
    {
        time_out = now_mill + 1000;

        if (countdownSecond > 0) {
            countdownSecond--;
        } else {
            if (countdownMinute > 0) {
                countdownMinute--;
                countdownSecond = 59;
            } else {
                if (countdownHour > 0) {
                    countdownHour--;
                    countdownMinute = 59;
                    countdownSecond = 59;
                }
            }
        }

        stateMachine.displayTime(countdownHour, countdownMinute, countdownSecond);
    }
}

void TimerState::handle(StateMachine &stateMachine) {
    display(stateMachine);
}

void TimerState::stateInit() {
    time_out = 0;
}
