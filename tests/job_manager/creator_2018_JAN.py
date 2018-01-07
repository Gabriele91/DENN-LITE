

def main():
    EXECUTABLE = "DENN-float"
    NUM_THREADS = 12
    NUM_RUNS = 5

    CMD = "{exe} -t {tot_gen} -s {batch_step} -b {batch_size} -bo {batch_offset} -np {population_size} -f 0.5 -cr 0.9 -cmax {cmax} -cmin {cmin} -rmax {rmax} -rmin {rmin} -rc -1 -rd 0.001 -tp {num_threads} -i {dataset} -em {method} -m {mutation} -co {crossover} -ro bench -o results/{outname}"

    OUTNAME = "{dataset_name}_{tot_gen}_{batch_step}_{batch_offset}_{population_size}_{method}_{mutation}_{crossover}_run{run}.json"

    # tot_gen, batch_size, batch_step, population_size, clamp, dataset_name, dataset
    DATASETS = [
        (2000, 10, 20, 123, (-150, 150), "QSAR", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/qsar.gz"),
        (2000, 10, 20, 123, (-1, 1), "QSAR_norm", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/qsar_normalized.gz"),
        (2000, 20, 40, 33, (-600, 600), "MAGIC", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/magic.gz"),
        (2000, 20, 40, 33, (-1, 1), "MAGIC_norm", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/magic_normalized.gz"),
        (2000, 20, 40, 384, (-70000, 70000), "GASS", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/gass.gz"),
        (2000, 20, 40, 384, (-1, 1), "GASS_norm", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/gass_normalized.gz"),
        (4000, 100, 100, 400, (-255, 255), "MNIST", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/mnist.gz"),
        (4000, 100, 100, 400, (-1, 1), "MNIST_norm", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/mnist_normalized.gz"),
        (4000, 100, 100, 400, (-255, 255), "FASHION_MNIST", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/fashion-mnist.gz"),
        (4000, 100, 100, 400, (-1, 1), "FASHION_MNIST_norm", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/fashion-mnist_normalized.gz"),
    ]

    METHODS = [
        "JDE -jf 0.1 -jcr 0.1",
        "JADE -as 30 -afcr 0.5",
        "SHADE -as 30 -shah 20",
        "L-SHADE -as 30 -shah 20 -mnp {batch_size} -mnfe {half_pop_x_tot_gen}"
    ]

    MUTATIONS = ["rand/1", "curr_p_best", "degl -glnn 5 -glw 0.5"]

    CROSSOVERS = ["bin", "interm"]

    OFFSETS = [1.0, 0.5]

    out_names = []

    with open("jobs/job_2018_JAN.csv", "w") as out_file:
        ##### -> datasets
        for tot_gen, batch_size, batch_step, population_size, (clamp_min, clamp_max), dataset_name, dataset in DATASETS:
            ##### -> methods
            for method in METHODS:
                cur_method = None
                if method.find("L-SHADE") != -1:
                    cur_method = method.format(
                        batch_size=batch_size,
                        half_pop_x_tot_gen=int(population_size / 2) * tot_gen
                    )
                else:
                    cur_method = method
                ##### -> mutations
                for mutation in MUTATIONS:
                    ##### -> crossovers
                    for crossover in CROSSOVERS:
                        ##### -> offsets
                        for offset_ratio in OFFSETS:
                            ##### -> runs
                            for run in range(NUM_RUNS):
                                # Create output name
                                outname = OUTNAME.format(
                                    dataset_name=dataset_name,
                                    tot_gen=tot_gen,
                                    batch_step=batch_step,
                                    batch_offset=str(offset_ratio).replace(".", ""),
                                    population_size=population_size,
                                    method=cur_method.split(" ")[0],
                                    mutation=mutation.split(" ")[0].replace("/", "-"),
                                    crossover=crossover,
                                    run=run
                                )
                                # Add to out_names to check names after creation
                                out_names.append(outname)
                                # Create command
                                cur_cmd = CMD.format(
                                    exe=EXECUTABLE,
                                    tot_gen=tot_gen,
                                    batch_step=batch_step,
                                    batch_size=batch_size,
                                    batch_offset=int(batch_size * offset_ratio),
                                    population_size=population_size,
                                    cmax=clamp_max,
                                    cmin=clamp_min,
                                    rmax=clamp_max,
                                    rmin=clamp_min,
                                    num_threads=NUM_THREADS,
                                    dataset=dataset,
                                    method=cur_method,
                                    mutation=mutation,
                                    crossover=crossover,
                                    outname=outname
                                )
                                # Write command
                                out_file.write(cur_cmd)
                                out_file.write("\n")

    ##
    # Check if all out names are differents
    assert len(out_names) == len(set(out_names)), "{} != {}".format(
        len(out_names), len(set(out_names)))


if __name__ == '__main__':
    main()
