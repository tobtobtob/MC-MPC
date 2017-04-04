all:
	g++ -o decompose -I/usr/local/include MPC.cpp decomposition.cpp main.cpp

test:
	g++ -o decomposition_tests -I/usr/local/include MPC.cpp decomposition.cpp ../tests/test_utils.cpp tests/main.cpp tests/test_decomposition.cpp tests/test_MPC.cpp