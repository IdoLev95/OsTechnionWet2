/*
 * globals.cpp
 *
 *  Created on: Dec 28, 2024
 *      Author: os
 */
#include "globals.hpp"
#include "Consumer_Producer.hpp"

// Static initialization of mutexes using PTHREAD_MUTEX_INITIALIZER
pthread_mutex_t Lock_bank_list_reader = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Lock_bank_list_writer = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lock_write = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lock_read = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lock_atms_active_write = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lock_atms_active_read = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lock_restore_req_list_write = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_lock_restore_req_list_read = PTHREAD_MUTEX_INITIALIZER;

// Initialize the Bank object with proper mutexes
Bank bank_params(&Lock_bank_list_reader, &Lock_bank_list_writer, "log.txt",
                 &mutex_lock_write, &mutex_lock_read,
                 &mutex_lock_atms_active_write, &mutex_lock_atms_active_read,
                 &mutex_lock_restore_req_list_write, &mutex_lock_restore_req_list_read,
                 0);

std::list<std::string> VIP_jobs[100]; // List of VIP jobs

consumer_producer cp; // Consumer producer object
bool isNeededToFinishVipThreads = false;
