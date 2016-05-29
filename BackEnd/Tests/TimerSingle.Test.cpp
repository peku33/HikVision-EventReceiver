#include "EPoll.hpp"
#include "TimerSingle.hpp"

#include <iostream>
#include <assert.h>

int main()
{
	EPoll EP;

	std::chrono::steady_clock::time_point Previous = std::chrono::steady_clock::now();

	//This strange syntax is lambda expression (unnamed function) as timer event callback.
	TimerSingle TS(EP, std::chrono::milliseconds(1500), [&]
	{
		std::chrono::steady_clock::time_point Now = std::chrono::steady_clock::now();

		std::chrono::steady_clock::duration Elapsed = Now - Previous;
		std::cout << "Elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(Elapsed).count() << "ms" << std::endl;
		Previous = Now;

		//This is a single-shot timer, so we restart it upon finish
		TS.Restart();
	});

	//Initial startup
	TS.Restart();

	EP.Main();

	return 0;
}