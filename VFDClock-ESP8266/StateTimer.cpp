#include "State.h"

void TimerState::display(StateMachine &stateMachine) {
    uint32_t now_mill = millis();
    if (time_out == 0) {
        time_out = now_mill + 1000;
    }
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

void TimerState::displayMillis(StateMachine &stateMachine) {
    uint32_t now_mill = millis();
    if (time_out == 0) {
        time_out = now_mill;
    }
    uint32_t interval = now_mill - time_out;
    if (interval >= 30) {
        time_out = now_mill;
        if(countdownMillis > 0) {
            countdownMillis -= interval;
        } else {
            countdownMillis = 1000 + countdownMillis - interval;
            if (countdownSecond > 0) {
                countdownSecond --;
            } else {
                if (countdownMinute > 0) {
                    countdownMinute --;
                    countdownSecond = 59;
                } else {
                    isFinish = true;
                    stateMachine.displayTimeMs(0, 0, 0);
                    return;
                }
            }
        }
        stateMachine.displayTimeMs(countdownMinute, countdownSecond, countdownMillis / 10);
    }

}

void TimerState::handle(StateMachine &stateMachine) {
    // isRun: 计时器是否工作
    // isFinish：是否完成计时
    if (isRun && !isFinish) {
        if (isMs) {
            displayMillis(stateMachine);
        } else {
            display(stateMachine);
        }
    } else if (isRun && isFinish) {
        stateMachine.setState(LOW_POWER_STATE);
        delay(200);
    }
}

void TimerState::stateInit() {
    isRun = false;
    isFinish= false;
    time_out=0;
}

void TimerState::initTime(int minute, int second, StateMachine* stateMachine, bool enable, bool start) {
    // start == false 表示不开始计时，isRun = false
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
    stateMachine->displayTime(countdownHour, countdownMinute, countdownSecond);
    if (this->isMs) {
        countdownMillis = 0;
        countdownSecond = countdownMinute;
        countdownMinute = countdownHour;
    }
}
