#include <mpi.h>

#include <iostream>
#include <chrono>

#include <litetensor/config.h>
#include <litetensor/tensor.h>
#include <litetensor/tensor_mpi_coarse.h>
#include <litetensor/als_omp.h>
#include <litetensor/als_mpi_coarse.h>

int main(int argc, char** argv) {
  using namespace std;
  using namespace std::chrono;
  using namespace litetensor;
  typedef std::chrono::high_resolution_clock Clock;
  typedef std::chrono::duration<double> dsec;

  Config config(argc, argv);

  if (config.use_mpi) {     // MPI code
    int proc_id;            // Process ID
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);
    MPI_Comm_size(MPI_COMM_WORLD, &config.num_procs);

    Partitioner partitioner;
    CoarseTensor tensor;

    // Master node will read tensor, partition tensor to chunk of rows,
    // and send rows to each node
    if (proc_id == 0)
      partitioner.partition(config);

    tensor.construct_tensor(partitioner, config);
    CoarseMPIALSSolver solver;

    double start_time = MPI_Wtime();
    solver.solve(tensor, config);
    double end_time = MPI_Wtime();
    double compute_time = end_time - start_time;


    if (proc_id == 0) {
      cout << "\n";
      cout << "======================= Time Statistics =======================";
      cout << "\n";
    }
    cout << "Process " << proc_id << " ";
    cout << "Computation time: " << compute_time << " seconds" << "\n";

    MPI_Finalize();

  } else {              // Single node, shared address model
    high_resolution_clock::time_point init_start = Clock::now();
    RawTensor tensor(config.tensor_file);
    double init_time = duration_cast<dsec>(Clock::now() - init_start).count();
    cout << "Initialization time: " << init_time << " seconds" << "\n\n";

    OMPALSSolver solver;

    high_resolution_clock::time_point compute_start = Clock::now();
    solver.decompose(tensor, config);
    double compute_time =
            duration_cast<dsec>(Clock::now() - compute_start).count();

    cout << "\n";
    cout << "======================= Time Statistics =======================\n";
    cout << "Computation time: " << compute_time << " seconds" << "\n";
    cout << "Total time: " << compute_time + init_time << " seconds" << "\n";
  }

  return 0;
}
