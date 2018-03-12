from math import ceil

def gen_steps(batch_size, batch_offset, max_subgen):
    return int(ceil(((1. - float(batch_size-batch_offset)/batch_size) *  max_subgen)))


def main():
    EXECUTABLE = "DENN-float"
    NUM_THREADS = 16
    NUM_RUNS = 5

    CMD = "{exe} ../template/{template}.config workers={num_threads} input={dataset} np={population_size} gens={tot_gen} batch={batch_size} sub_gens={batch_step} batch_offset={batch_offset} crossover={crossover} output={outname}.json"

    OUTNAME = "{template}_{dataset_name}_{batch_size}_{tot_gen}_{batch_step}_{batch_offset}_{population_size}_{crossover}_run{run}"

    # tot_gen, batch_size, batch_step, population_size, clamp, dataset_name, dataset
    DATASETS = [
        (2000, [20, 40], [gen_steps], 2*123, (-150, 150), "QSAR", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/qsar_balanced_classes.gz"),
        (2000, [20, 40], [gen_steps], 2*123, (-1, 1), "QSAR_norm", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/qsar_balanced_classes_normalized.gz"),
        (2000, [40, 80], [gen_steps], 2*33, (-600, 600), "MAGIC", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/magic_balanced_classes.gz"),
        (2000, [40, 80], [gen_steps], 2*33, (-1, 1), "MAGIC_norm", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/magic_balanced_classes_normalized.gz"),
        (2000, [40, 80], [gen_steps], 2*384, (-70000, 70000), "GASS", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/gass_balanced_classes.gz"),
        (2000, [40, 80], [gen_steps], 2*384, (-1, 1), "GASS_norm", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/gass_balanced_classes_normalized.gz"),
        (4000, [50, 100], [gen_steps], 2*28*28, (-255, 255), "MNIST", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/mnist_balanced_classes.gz"),
        (4000, [50, 100], [gen_steps], 2*28*28, (-1, 1), "MNIST_norm", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/mnist_balanced_classes_normalized.gz"),
        (4000, [50, 100], [gen_steps], 2*28*28, (-255, 255), "FASHION_MNIST", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/fashion-mnist_balanced_classes.gz"),
        (4000, [50, 100], [gen_steps], 2*28*28, (-1, 1), "FASHION_MNIST_norm", "../../DENN-dataset-samples/DATASETS/d_06_01_2018/fashion-mnist_balanced_classes_normalized.gz"),
    ]

    TEMPLATES = ["SaMDE", "MAB-ShaDE"]

    CROSSOVERS = ["bin", "interm"]

    OFFSETS = [
        # 1.0,
        0.5,
        # 0.25
    ]

    MAX_SUB_GENS = [
        0.5,
        # 0.25,
        # 0.10
    ]

    out_names = []

    with open("jobs/job_2018_FEB_SaMDE_MAB_ShaDE_doubleNP.csv", "w") as out_file:
        ##### -> templates
        for template in TEMPLATES:
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
                                for crossover in CROSSOVERS:
                                    ##### -> runs
                                    for run in range(NUM_RUNS):
                                        # Create output name
                                        outname = OUTNAME.format(
                                            template=template,
                                            dataset_name=dataset_name,
                                            tot_gen=tot_gen,
                                            batch_size=batch_size,
                                            batch_step=cur_batch_step,
                                            batch_offset=cur_batch_offset,
                                            population_size=population_size,
                                            crossover=crossover,
                                            run=run
                                        )
                                        # Add to out_names to check names after creation
                                        if not outname in out_names:
                                            out_names.append(outname)
                                            # Create command
                                            cur_cmd = CMD.format(
                                                exe=EXECUTABLE,
                                                template=template,
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
