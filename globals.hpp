/*
 * globals.hpp
 *
 *  Created on: Dec 22, 2024
 *      Author: os
 */

#ifndef GLOBALS_HPP_
#define GLOBALS_HPP_


#include "Bank.hpp"
#include <list>
#include <string>



using namespace std;

struct AtmInfo {
    int atm_id;
    std::string str;

    // Constructor to initialize both fields
    AtmInfo( const std::string& s,int id) : atm_id(id), str(s) {}
};

class consumer_producer; // Forward declaration

extern Bank bank_params;
extern std::list<AtmInfo> VIP_jobs[100];
extern consumer_producer cp;
extern bool isNeededToFinishVipThreads;

#endif /* GLOBALS_HPP_ */
