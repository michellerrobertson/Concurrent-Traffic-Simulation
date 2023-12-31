#include <iostream>
#include <random>
#include <queue>
#include <thread>
#include <future>
#include "TrafficLight.h"

/* Implementation of class "MessageQueue" */

 
template <typename T>
T MessageQueue<T>::receive()
{
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 

    std::unique_lock<std::mutex> lck(mtx);
    cond.wait(lck, [this] {return !_queue.empty(); });
    T msg = std::move(_queue.back());
    _queue.pop_back();
    return msg;
}

template <typename T>
void MessageQueue<T>::send(T &&msg)
{
    // FP.4a : The method send should use the mechanisms std::lock_guard<std::mutex> 
    // as well as _condition.notify_one() to add a new message to the queue and afterwards send a notification.
    std::lock_guard<std::mutex> lck(mtx);
    _queue.push_back(std::move(msg));
    cond.notify_one();
}


/* Implementation of class "TrafficLight" */


TrafficLight::TrafficLight()
{
    _currentPhase = TrafficLightPhase::red;
}

void TrafficLight::waitForGreen()
{
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
    while (true) {
        if(_trafficMessages.receive() == TrafficLightPhase::green) {
            return;
        }
    }
}

TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

void TrafficLight::simulate()
{
    // FP.2b : Finally, the private method „cycleThroughPhases“ should be started in a thread when the public method „simulate“ is called. To do this, use the thread queue in the base class. 
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // FP.2a : Implement the function with an infinite loop that measures the time between two loop cycles 
    // and toggles the current phase of the traffic light between red and green and sends an update method 
    // to the message queue using move semantics. The cycle duration should be a random value between 4 and 6 seconds. 
    // Also, the while-loop should use std::this_thread::sleep_for to wait 1ms between two cycles. 
   
    auto startTime = std::chrono::system_clock::now();
    double r = (((double) rand() / (RAND_MAX))*2) + 4;

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto currentTime = std::chrono::system_clock::now();
        std::chrono::duration<double> diff = currentTime - startTime;
        //std::cout << "r = " << r << std::endl;
        //std::cout << "diff = " << diff.count() << std::endl;
        if(diff.count() > r) {
            //std::cout << "GOT HERE" << std::endl;
            if(_currentPhase == red) {
                _currentPhase = green;
            }
            else {
                _currentPhase = red;
                
            }
            startTime = std::chrono::system_clock::now();
            r = (((double) rand() / (RAND_MAX))*2) + 4;
            _trafficMessages.send(std::move(_currentPhase));
        }

        
        /*
        auto tempPhase = _currentPhase;
        auto future = std::async(std::launch::async, &MessageQueue<TrafficLightPhase>::send, _trafficMessages, std::move(tempPhase));
        future.wait();
        */

        
        
        
    }
}

