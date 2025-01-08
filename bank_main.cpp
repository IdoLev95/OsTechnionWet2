//smash.c

/*=============================================================================
* includes, defines, usings
=============================================================================*/
#include <stdlib.h>
#include <cstdio>
#include <bits/stdc++.h>
#include <pthread.h>
#include <unistd.h>
#include "ATM.hpp"
#include "globals.hpp"
#include "vipATM.hpp"
#include "Consumer_Producer.hpp"
#include <algorithm> // For std::any_of
using namespace std;
/*=============================================================================
* classes/structs declarations
=============================================================================*/

/*=============================================================================
* global variables & data structures
=============================================================================*/
bool IsExistingWorkingAtm();
/*=============================================================================
* main function
=============================================================================*/
int main(int argc, char* argv[])
{

	bank_params.EraseLoggerContent();

	const int N = 20;
	bank_params.set_num_atms(N);
	pthread_t atm_threads[N];
    void** pointers = new void*[N]; // Allocate array of void*
	for (int i = 0; i < N; ++i) {
		//string* value = new string("ATM_file"+to_string(i)+".txt"); // Create an int with value i
		int* value = new int(i);
		pointers[i] = static_cast<void*>(value); // Store pointer as void*
	}
	for(int ind =0; ind < N;ind++)
	{
		pthread_create(&atm_threads[ind], NULL, &single_atm_applier, pointers[ind]);
	}
	const int VIP_N = 10;
	pthread_t vip_threads[VIP_N];

	for (int i = 0; i < VIP_N; ++i) {
		string* value = new string(to_string(i)); // Create an int with value i
		cp.producer(*value, i+1);
	}
	//void** vip_pointers = new void*[VIP_N]; // Allocate array of void*
	for (int i = 0; i<VIP_N; ++i) {
		pthread_create(&vip_threads[i], NULL, &vip_atm_applier, NULL);

	}

	//bank_params.restore_status_from_remember(2);
	//bank_params.print_bank_status();
	int counter_for_tax_collection = 0;
	while(IsExistingWorkingAtm())
	{
		counter_for_tax_collection++;
		usleep(500000);
		bank_params.print_bank_status();
		bank_params.check_and_apply_restore();
		if (counter_for_tax_collection == 6)//once every 3 seconds
		{
			counter_for_tax_collection = 0;
			bank_params.collect_texas_from_all();
		}
	}
	for(int ind =0;ind < N;ind++)
	{
		// Waiting for the created thread to terminate
		pthread_join(atm_threads[ind], NULL);
	}
	isNeededToFinishVipThreads = true;
	for(int ind =0;ind < VIP_N;ind++){
		// Waiting for the created thread to terminate
		pthread_join(vip_threads[ind], NULL);
	}
	return 0;
}
bool IsExistingWorkingAtm()
{
	bank_params.reader_writer_atm_active_list.reader_locker();
	bool isAnyTrue = false;

	for (int i = 0; i < bank_params.numInitAtms; ++i) {
	    if (bank_params.isAtmActive[i]) {
	        isAnyTrue = true;
	        break; // No need to check further once we find a true value
	    }
	}
	bank_params.reader_writer_atm_active_list.reader_unlocker();
	return isAnyTrue;
}
