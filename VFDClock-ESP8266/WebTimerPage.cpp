#include "WebControl.h"
#include "States.h"

void WebControl::initTimerPage() {
  timerPage = String(R"rawliteral(
<!DOCTYPE html><html lang="en"><head><meta charset="UTF-8"><meta name="viewport"content="width=device-width, initial-scale=1.0"><title>Time Progress Bar</title><style>body,.progress-circle-container{display:flex;justify-content:center;align-items:center;height:100vh;margin:0;flex-direction:column;font-family:Arial,sans-serif}.progress-circle{width:200px;height:200px;position:relative}.progress-circle svg{width:100%;height:100%;transform:rotate(-90deg)}.progress-circle circle{fill:none;stroke-width:10}.bg{stroke:#f3f3f3}.progress{stroke:#76c7c0;stroke-linecap:round}.time-display{position:absolute;top:50%;left:50%;transform:translate(-50%,-50%);font-size:24px}.start-button{margin-top:20px;padding:10px 20px;cursor:pointer;background-color:#76c7c0;border:none;color:white;border-radius:5px}</style></head><body ontouchmove="event.preventDefault()"><div class="progress-circle-container"><div class="progress-circle"id="progress-circle"><svg><circle cx="100"cy="100"r="90"class="bg"></circle><circle cx="100"cy="100"r="90"class="progress"></circle></svg><div class="time-display"id="time-display">00:00</div></div><button class="start-button"id="start-button">Start Countdown</button><button class="start-button"id="stop-button">Stop Countdown</button></div><script>const maxTime=120;const progressCircle=document.querySelector('.progress');const timeDisplay=document.getElementById('time-display');const startButton=document.getElementById('start-button');const stopButton=document.getElementById('stop-button');const radius=progressCircle.r.baseVal.value;const circumference=2*Math.PI*radius;let currentTime=0.1;let countdownInterval=null;let countdownStarted=false;let isTouch=false;progressCircle.style.strokeDasharray=`${circumference} ${circumference}`;progressCircle.style.strokeDashoffset=circumference;function setProgress(minutes){const offset=circumference-(minutes/maxTime)*circumference;progressCircle.style.strokeDashoffset=offset;const displayMinutes=String(Math.floor(minutes)).padStart(2,'0');const displaySeconds=String(Math.floor((minutes*60)%60)).padStart(2,'0');timeDisplay.textContent=`${displayMinutes}:${displaySeconds}`}function sendRequest(path){var xhttp=new XMLHttpRequest();xhttp.open('GET',path,true);xhttp.send()}function calculateTimeFromPosition(x,y){const bbox=progressCircle.getBoundingClientRect();const cx=bbox.left+bbox.width/2;const cy=bbox.top+bbox.height/2;const dx=x-cx;const dy=y-cy;const angle=Math.atan2(dy,dx)*(180/Math.PI)+90;const adjustedAngle=(angle<0)?angle+360:angle;return Math.round((adjustedAngle/360)*maxTime/5)*5}function handleDrag(e){const x=e.clientX||(e.touches&&e.touches[0].clientX);const y=e.clientY||(e.touches&&e.touches[0].clientY);const time=calculateTimeFromPosition(x,y);setProgress(time);currentTime=time}document.getElementById('progress-circle').addEventListener('mousedown',()=>{if(countdownStarted)return;document.addEventListener('mousemove',handleDrag);isTouch=true});document.addEventListener('mouseup',()=>{if(countdownStarted)return;document.removeEventListener('mousemove',handleDrag);if(isTouch){sendRequest('/timerSet?m='+String(Math.floor(currentTime))+'&r=2&t='+String(Date.now()).slice(-7));isTouch=false}});document.getElementById('progress-circle').addEventListener('touchstart',()=>{if(countdownStarted)return;document.addEventListener('touchmove',handleDrag);isTouch=true});document.addEventListener('touchend',()=>{if(countdownStarted)return;document.removeEventListener('touchmove',handleDrag);if(isTouch){sendRequest('/timerSet?m='+String(Math.floor(currentTime))+'&r=2&t='+String(Date.now()).slice(-7));isTouch=false}});function startCountdown(){if(countdownStarted)return;countdownStarted=true;sendRequest('/timerSet?m='+String(Math.floor(currentTime))+'&r=1&t='+String(Date.now()).slice(-7));if(countdownInterval)clearInterval(countdownInterval);countdownInterval=setInterval(()=>{currentTime-=1/60;if(currentTime<=0){clearInterval(countdownInterval);currentTime=0;countdownStarted=false;setProgress(currentTime);return}setProgress(currentTime)},1000)}startButton.addEventListener('click',startCountdown);stopButton.addEventListener('click',()=>{if(!countdownStarted)return;countdownStarted=false;currentTime=0;sendRequest('/timerSet?m=0&r=0&t='+String(Date.now()).slice(-7));setProgress(0)});setProgress(currentTime);</script></body></html>
)rawliteral");
}

void WebControl::handleTimerPage() {
    this->initTimerPage();
    server.send_P(200, "text/html", timerPage.c_str());
}


void WebControl::handleTimeSet() {
    String timeStampStr = server.arg("t");
    int curTimeStamp = timeStampStr.toInt();
    if (curTimeStamp <= preTimeStamp) return;

    preTimeStamp = curTimeStamp;

    String minuteStr = server.arg("m");
    String runStr = server.arg("r");

    int minute = minuteStr.toInt();
    int runId = runStr.toInt();

    if (runId == 2) {
        if (minute == TimerMinute) return;
        TimerMinute = minute;
        isTimerRunning = false;
        stateMachine->setState(TIMER_STATE);
        if (TimerState* timerState = static_cast<TimerState*>(stateMachine->curState)) {
            timerState->initTime(minute, 0, stateMachine, true, false);
        }
        return;
    }
    bool isRun = (runId == 1);
    if (isTimerRunning == isRun) return;
    isTimerRunning = isRun;

    if (stateMachine) {
        if (stateMachine->curStateId != TIMER_STATE) return ;
        if (TimerState* timerState = static_cast<TimerState*>(stateMachine->curState)) {
            timerState->initTime(minute, 0, stateMachine, isRun, true);
        }
    }
}