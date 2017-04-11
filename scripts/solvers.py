import glob
import os

BIN_FOLDER = os.getenv("BIN_FOLDER", "../bin/")
OUTPUT_FOLDER = os.getenv("OUTPUT_FOLDER", "output/")

def generate_k_path_graph(k, n, m, output_file):
  os.system(BIN_FOLDER + "generator {0} {1} {2} {3}".format(output_file, k, n, m))

def decompose(input_file, output_folder):
  os.system(BIN_FOLDER + "decompose {0} {1}".format(input_file, output_folder))

def create_clean_folders():
  # create necessary directories, if they don't exist
  os.system("mkdir -p {0}decomposition".format(OUTPUT_FOLDER))
  os.system("mkdir -p {0}results".format(OUTPUT_FOLDER))

  # cleanup possible previous decompositions & results
  os.system("rm {0}decomposition/*".format(OUTPUT_FOLDER))
  os.system("rm {0}results/*".format(OUTPUT_FOLDER))

def solve_with_decomposition(input_file):

  create_clean_folders()

  os.system("{0}decompose {1} {2}".format(BIN_FOLDER, input_file, OUTPUT_FOLDER+"decomposition/"))

  solution_index = 0
  for(decomp in glob.glob(OUTPUT_FOLDER+"decomposition/*")):
    os.system("{0}mc-mpc {1} {2}".format(BIN_FOLDER, decomp, OUTPUT_FOLDER+"results/"+str(solution_index)))
    solution_index += 1
  
  sum = 0
  for(result_file in glob.glob(OUTPUT_FOLDER+"results/*")):
    f = open(result_file, 'r')
    for line in f:
      line = line.split()
      amount = int(line[1])
      weight = int(line[2])
      sum += amount*weight
    f.close()
  
  return sum

def solve_without_decomposition(input_file):

  create_clean_folders()
  os.system("{0}mc-mpc {1} {2}".format(BIN_FOLDER, decomp, OUTPUT_FOLDER+"results/"+"result")

  sum = 0
  f = open(OUTPUT_FOLDER+"/results/result", 'r')
  for line in f:
    line = line.split()
    amount = int(line[1])
    weight = int(line[2])
    sum += amount*weight
  f.close()

  return sum
