#ifndef STATE_H
#define STATE_H

#define DISPLAY_STATE 0
#define TIMER_STATE 1
#define LOW_POWER_STATE 2
#include "FrameRefresh.h"
#include "WebControl.h"
// 前向声明
class StateMachine;

class State {
public:
    virtual ~State() {}
    virtual void handle(StateMachine& stateMachine) = 0;
    virtual void stateInit() = 0;
};

class DisplayTimeState : public State {
private:
    uint32_t time_out = 0;
    bool enable_display = true;
    FrameRefresh::TimePeriod displayPeriods[3] = {
            {10, 0, 12, 0},
            {13, 0, 17, 30},
            {18, 0, 23, 0},
    };
    void display(StateMachine &stateMachine);
public:
    void handle(StateMachine& stateMachine) override;
    void stateInit() override;
};

class TimerState : public State {
private:
    uint32_t time_out = 0;
    bool isRun = false;
    bool isFinish = false;
    int countdownHour=1;
    int countdownMinute=0;
    int countdownSecond=0;
    void display(StateMachine &stateMachine);
public:
    void stateInit() override;
    void handle(StateMachine& stateMachine) override;
    void initTime(int minute, int second, StateMachine *stateMachine, bool enable, bool start);
};

class LowPowerState : public State {
public:
    void handle(StateMachine& stateMachine) override;
    void stateInit() override;
};

class StateMachine {
private:
    FrameRefresh* frame;
    WebControl* webControl;
    State* stateList[3] = {nullptr};
public:
    State* curState = nullptr;
    int curStateId = -1;
    StateMachine();
    ~StateMachine();
    void init();
    void displayTime(int currentHour, int currentMinute, int currentSecond);
    void setState(int stateId);
    void running();
    void enableDisplay(bool enable);
    void setBrightness(int brightness);
    void setFont(bool bold);
    void resetFrame();
};

#endif // STATE_H
