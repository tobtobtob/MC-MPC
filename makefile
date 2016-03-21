all:
	g++ -o MC-MPC MPC.cpp solve_minflow.cpp main.cpp -lemon
test:
	g++ -o run_tests MPC.cpp decomposition.cpp tests/test_MPC.cpp tests/test_decomposition.cpp tests/test_utils.cpp tests/test_main.cpp -lemon

test_debug:
	g++ -o run_tests -g MPC.cpp decomposition.cpp tests/test_MPC.cpp tests/test_decomposition.cpp tests/test_overlap_graph.cpp  tests/test_utils.cpp tests/test_main.cpp -lemon

speedtest:
	g++ -o run_tests MPC.cpp decomposition.cpp tests/speedtest.cpp IBFS/ibfs.cpp MPC_IBFS.cpp -lemon

overlap_test:
	g++ -o run_tests MPC.cpp decomposition.cpp tests/test_overlap_graph.cpp -lemon