cpu:
	g++ -Wall -Werror -std=c++11 map_reduce_host.cpp -o map_reduce_host

gpu:
	pgc++ -std=c++11 map_reduce_gpu.cpp -acc -Minfo=accel -Minfo=opt
