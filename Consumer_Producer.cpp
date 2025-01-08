

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
std::string consumer_producer::consumer() {
    std::string job = "";
    pthread_mutex_lock(&lock);

    // Wait for condition
    while (!isNeededToFinishVipThreads && production == 0) {
        pthread_cond_wait(&full, &lock);
    }

    // Exit if needed
    if (isNeededToFinishVipThreads) {
        pthread_mutex_unlock(&lock);  // Unlock before exiting
        pthread_exit(0);
    }

    // Process jobs
    for (int i = 99; i >= 0; i--) {
        if (!VIP_jobs[i].empty()) {
            job = VIP_jobs[i].front();
            VIP_jobs[i].pop_front();
            production--;

            std::cout << "Consumed: " << job << std::endl;  // Print the consumed job

            pthread_mutex_unlock(&lock);  // Unlock before returning the job
            return job;  // Returning by value
        }
    }

    pthread_mutex_unlock(&lock);  // Unlock mutex
    return job;  // Returning by value
}
void consumer_producer::producer(const std::string& str , int priority){
        pthread_mutex_lock(&lock);

        cout << "Produced: " << str <<" "<<priority << endl; //print the produced job לבדיקה
		VIP_jobs[priority-1].push_back(str);
        production++;

        pthread_cond_signal(&full);
        pthread_mutex_unlock(&lock);

}
void consumer_producer::send_bc_to_cv(){
	pthread_cond_broadcast(&full);
}
/*
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
*/
/*void consumer_producer::producer(const std::string& str , int priority){
    pthread_mutex_lock(&lock);
    cout << "Produced: " << str <<" "<<priority << endl; //print the produced job לבדיקה
    VIP_jobs[priority-1].push_back(str);
    production++;
    pthread_mutex_unlock(&lock);
}
*/
