#ifndef _CONSUMER_PRODUCER_HPP_
#define _CONSUMER_PRODUCER_HPP_

#include <pthread.h>
#include <string>
#include "globals.hpp"



class consumer_producer{
    private:
    pthread_mutex_t lock;
    int production;
    pthread_cond_t full;
    public:
    consumer_producer();
    virtual ~consumer_producer();
    AtmInfo consumer();
    void send_bc_to_cv();
    void producer(const std::string& str , int priority,int atm_id);
};
#endif
