import matplotlib.pyplot as plt
import time
import solvers

TEST_GRAPH_NAME = "test_graph"
PLOT_FILE = "plot.png"

def plot():
  k = 10
  values = [10, 500, 1000, 1500, 2000, 2500, 3000, 3500, 4000, 4500, 5000]
  m = 1000
  results_decomp = [0]*len(values)
  results_normal = [0]*len(values)
  index = 0
  for value in values:
    solvers.generate_k_path_graph(k, value, m, TEST_GRAPH_NAME)
    print "graph generated"
    start = time.time()
    decomposed_sum = solvers.solve_with_decomposition(TEST_GRAPH_NAME)
    print decomposed_sum
    decomp_end = time.time()
    normal_sum = solvers.solve_without_decomposition(TEST_GRAPH_NAME)
    print normal_sum
    end = time.time()
    results_decomp[index] = decomp_end - start
    print "decomp: " + str(results_decomp[index])
    results_normal[index] = end - decomp_end
    print "normal: " + str(results_normal[index])
    index += 1
  plt.plot(values, results_decomp, 'r', label="with decomposition")
  plt.plot(values, results_normal, 'b', label="without decomposition")
  plt.savefig(PLOT_FILE)

plot()