import json
from math import ceil
from os import path


def gen_steps(batch_size, batch_offset, max_subgen):
    return int(ceil(((1. - float(batch_size-batch_offset)/batch_size) *  max_subgen)))


def main():
    EXECUTABLE = "DENN-float"
    NUM_THREADS = 16
    NUM_RUNS = 5

    CMD = "{exe} ../template/{template}.config workers={num_threads} input={dataset} np={population_size} gens={tot_gen} batch={batch_size} sub_gens={batch_step} batch_offset={batch_offset} crossover={crossover} output={outname}.json"

    OUTNAME = "{template}_{dataset_name}_{batch_size}_{tot_gen}_{batch_step}_{batch_offset}_{population_size}_{crossover}_run{run}"

    TEMPLATES = ["SaMDE", "MAB-ShaDE"]

    DATASET_CONVERSIONS = {
        'qsar': ("../../DENN-dataset-samples/DATASETS/d_06_01_2018/qsar.gz", "QSAR"),
        'qsar_normalized': ("../../DENN-dataset-samples/DATASETS/d_06_01_2018/qsar_normalized.gz", "QSAR_norm"),
        'magic': ("../../DENN-dataset-samples/DATASETS/d_06_01_2018/magic.gz", "MAGIC"),
        'magic_normalized': ("../../DENN-dataset-samples/DATASETS/d_06_01_2018/magic_normalized.gz", "MAGIC_norm"),
        'gass': ("../../DENN-dataset-samples/DATASETS/d_06_01_2018/gass.gz", "GASS"),
        'gass_normalized': ("../../DENN-dataset-samples/DATASETS/d_06_01_2018/gass_normalized.gz", "GASS_norm"),
        'mnist': ("../../DENN-dataset-samples/DATASETS/d_06_01_2018/mnist.gz", "MNIST"),
        'mnist_normalized': ("../../DENN-dataset-samples/DATASETS/d_06_01_2018/mnist_normalized.gz", "MNIST_norm")
    }

    out_names = []

    with open(path.join("jobs", "2018_march", "all_bests.json")) as all_bests:
        bests = json.load(all_bests)

    with open("jobs/job_2018_MAR_lost_SaMDE_MAB_ShaDE.csv", "w") as out_file:
        ##### -> templates
        for dataset, best in bests.items():
            for template in TEMPLATES:
                for run in range(NUM_RUNS):
                    # Create output name
                    dataset_file_name, dataset_name = DATASET_CONVERSIONS[dataset]
                    outname = OUTNAME.format(
                        template=template,
                        dataset_name=dataset_name,
                        tot_gen=best['generations'],
                        batch_size=best['batch_size'],
                        batch_step=best['sub_gens'],
                        batch_offset=best['batch_offset'],
                        population_size=best['number_parents'],
                        crossover=best['crossover'],
                        run=run
                    )
                    # Add to out_names to check names after creation
                    if not outname in out_names:
                        out_names.append(outname)
                        # Create command
                        cur_cmd = CMD.format(
                            exe=EXECUTABLE,
                            template=template,
                            tot_gen=best['generations'],
                            batch_step=best['sub_gens'],
                            batch_size=best['batch_size'],
                            batch_offset=best['batch_offset'],
                            population_size=best['number_parents'],
                            cmax=best['clamp_max'],
                            cmin=best['clamp_min'],
                            rmax=best['clamp_max'],
                            rmin=best['clamp_min'],
                            num_threads=NUM_THREADS,
                            dataset=dataset_file_name,
                            crossover=best['crossover'],
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
