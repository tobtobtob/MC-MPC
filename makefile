all:
	g++ -o MC-MPC *.cpp
test:
	g++ -o run_tests tests/test_MPC.cpp tests/test_main.cpp
