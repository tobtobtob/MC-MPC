all:
	g++ -o MC-MPC MPC.cpp solve_minflow.cpp main.cpp -lemon
test:
	g++ -o run_tests MPC.cpp tests/test_MPC.cpp tests/test_utils.cpp tests/test_main.cpp -lemon
