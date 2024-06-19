#ifndef STATE_H
#define STATE_H
#include <memory>

// 前向声明
class StateMachine;

class State {
public:
    virtual ~State() {}
    virtual void handle(StateMachine& stateMachine) = 0;
};

class NormalState : public State {
public:
    void handle(StateMachine& stateMachine) override;
};

class TimerState : public State {
public:
    void handle(StateMachine& stateMachine) override;
};

class LowPowerState : public State {
public:
    void handle(StateMachine& stateMachine) override;
};

class StateMachine {
private:
    std::shared_ptr<State> currentState;
public:
    StateMachine(std::shared_ptr<State> state);
    void setState(std::shared_ptr<State> state);
    void request();
};

#endif // STATE_H
