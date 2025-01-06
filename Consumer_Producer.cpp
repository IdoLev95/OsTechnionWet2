

#include "Consumer_Producer.hpp"
#include <pthread.h>
#include <string> 
#include <unistd.h> // Include this for the sleep function


consumer_producer::consumer_producer(){
    pthread_mutex_init(&lock, NULL);
    production = 0;
}

consumer_producer::~consumer_producer(){
    pthread_mutex_destroy(&lock);
}

std::string consumer_producer::consumer(){
     std::string job = "";
    pthread_mutex_lock(&lock);
    while(production == 0){
        pthread_mutex_unlock(&lock);
        sleep(1);
        pthread_mutex_lock(&lock);
    }
    for (int i = 99; i >=0; i--){
        if(VIP_jobs[i].size() > 0){
            job = VIP_jobs[i].front();
            VIP_jobs[i].pop_front();
            production--;
            std::cout << "Consumed: " << job << std::endl;//print the consumed job לבדיקה
            pthread_mutex_unlock(&lock);
            return job;
        }
    }
    return job;
}
    
void consumer_producer::producer(const std::string& str , int priority){
    pthread_mutex_lock(&lock);
    cout << "Produced: " << str <<" "<<priority << endl; //print the produced job לבדיקה
    VIP_jobs[priority-1].push_back(str);
    production++;
    pthread_mutex_unlock(&lock);
}

