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

class consumer_producer;

extern Bank bank_params;
extern std::list<std::string> VIP_jobs[100];
extern consumer_producer cp;

#endif /* GLOBALS_HPP_ */
