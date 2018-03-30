from math import ceil

def gen_steps(batch_size, batch_offset, max_subgen):
    return int(ceil(((1. - float(batch_size-batch_offset)/batch_size) *  max_subgen)))


def main():
    EXECUTABLE = "DENN-float"
    NUM_THREADS = 16
    NUM_RUNS = 3

    CMD = "{exe} -t {tot_gen} -s {batch_step} -b {batch_size} -bo {batch_offset} -np {population_size} -f 0.5 -cr 0.9 -cmax {cmax} -cmin {cmin} -rmax {rmax} -rmin {rmin} -rc -1 -rd 0.001 -tp {num_threads} -i {dataset} -em {method} -m {mutation} -co {crossover} -ro bench -o results/{outname}"

    OUTNAME = "{dataset_name}_{batch_size}_{tot_gen}_{batch_step}_{batch_offset}_{population_size}_{method}_{mutation}_{crossover}_run{run}.json"

    # tot_gen, batch_size, batch_step, population_size, clamp, dataset_name, dataset
    DATASETS = [
        (2000, [10, 20, 40], [gen_steps], 123, (-150, 150), "QSAR", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/qsar.gz"),
        (2000, [10, 20, 40], [gen_steps], 123, (-1, 1), "QSAR_norm", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/qsar_normalized.gz"),
        (2000, [20, 40, 80], [gen_steps], 33, (-600, 600), "MAGIC", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/magic.gz"),
        (2000, [20, 40, 80], [gen_steps], 33, (-1, 1), "MAGIC_norm", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/magic_normalized.gz"),
        (2000, [20, 40, 80], [gen_steps], 384, (-70000, 70000), "GASS", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/gass.gz"),
        (2000, [20, 40, 80], [gen_steps], 384, (-1, 1), "GASS_norm", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/gass_normalized.gz"),
        (4000, [50, 100, 200], [gen_steps], 28*28, (-255, 255), "MNIST", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/mnist.gz"),
        (4000, [50, 100, 200], [gen_steps], 28*28, (-1, 1), "MNIST_norm", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/mnist_normalized.gz"),
        (4000, [50, 100, 200], [gen_steps], 28*28, (-255, 255), "FASHION_MNIST", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/fashion-mnist.gz"),
        (4000, [50, 100, 200], [gen_steps], 28*28, (-1, 1), "FASHION_MNIST_norm", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/fashion-mnist_normalized.gz"),
    ]

    METHODS = [
        "SAMDE -mutations_list rand/1 rand/2 best/1 curr_to_rand/1 ",
        "MAB-SHADE -as 30 -shah 20 -mutations_list rand/1 curr_p_best degl "
    ]

    #MUTATIONS = ["rand/1", "curr_p_best", "degl -glnn 5 -glw 0.5"]
    #fake
    MUTATIONS = ["degl -glnn 5 -glw 0.5"]

    CROSSOVERS = ["bin", "interm"]

    OFFSETS = [1.0, 0.5, 0.25]
    MAX_SUB_GENS = [0.5, 0.25, 0.10]

    out_names = []

    with open("jobs/job_2018_JAN_v2_MAB_SHADE_SAMDE.csv", "w") as out_file:
        ##### -> datasets
        for tot_gen, batch_sizes, batch_steps, population_size, (clamp_min, clamp_max), dataset_name, dataset in DATASETS:
            ##### -> batch_size
            for batch_size in batch_sizes:
                ##### -> batch_step
                for batch_step in batch_steps:
                    ##### -> offsets
                    for offset_ratio in OFFSETS:
                        cur_batch_offset = int(batch_size * offset_ratio)
                        for max_sub_gen in MAX_SUB_GENS:
                            cur_max_subgen = int(batch_size * max_sub_gen)
                            if callable(batch_step):
                                cur_batch_step = batch_step(batch_size, cur_batch_offset, cur_max_subgen)
                            else:
                                cur_batch_step = batch_step
                            
                            # print(dataset_name, batch_size, cur_batch_offset, cur_max_subgen, cur_batch_step)
                            ##### -> methods
                            for method in METHODS:
                                cur_method = None
                                if method.find("L-SHADE") != -1:
                                    cur_method = method.format(
                                        batch_size=batch_size,
                                        half_pop_x_tot_gen=int(float((population_size / 2.) + batch_size) * tot_gen)
                                    )
                                else:
                                    cur_method = method
                                ##### -> mutations
                                for mutation in MUTATIONS:
                                    ##### -> crossovers
                                    for crossover in CROSSOVERS:
                                        ##### -> runs
                                        for run in range(NUM_RUNS):
                                            # Create output name
                                            outname = OUTNAME.format(
                                                dataset_name=dataset_name,
                                                tot_gen=tot_gen,
                                                batch_size=batch_size,
                                                batch_step=cur_batch_step,
                                                batch_offset=cur_batch_offset,
                                                population_size=population_size,
                                                method=cur_method.split(" ")[0],
                                                mutation=mutation.split(" ")[0].replace("/", "-"),
                                                crossover=crossover,
                                                run=run
                                            )
                                            # Add to out_names to check names after creation
                                            if not outname in out_names:
                                                out_names.append(outname)
                                                # Create command
                                                cur_cmd = CMD.format(
                                                    exe=EXECUTABLE,
                                                    tot_gen=tot_gen,
                                                    batch_step=cur_batch_step,
                                                    batch_size=batch_size,
                                                    batch_offset=cur_batch_offset,
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
