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
                } else {
                    isFinish = true;
                }
            }
        }
        stateMachine.displayTime(countdownHour, countdownMinute, countdownSecond);
        delay(200);
    }
}

void TimerState::handle(StateMachine &stateMachine) {
    if (isRun && !isFinish) {
        display(stateMachine);
    } else if (isRun && isFinish) {
        stateMachine.setState(LOW_POWER_STATE);
        delay(200);
    }
}

void TimerState::stateInit() {
    isRun = false;
    isFinish= false;
    time_out = millis()+1000;
}

void TimerState::initTime(int minute, int second, StateMachine* stateMachine, bool enable, bool start) {
    this->isRun = (enable && start);
    if (!enable) {
        stateMachine->displayTime(0, 0, 0);
        stateMachine->setState(LOW_POWER_STATE);
        delay(200);
        return;
    }
    countdownHour = minute / 60;
    countdownMinute = minute % 60;
    countdownSecond = second;
    time_out = millis()+1000;
    stateMachine->displayTime(countdownHour, countdownMinute, countdownSecond);
    delay(200);
}
