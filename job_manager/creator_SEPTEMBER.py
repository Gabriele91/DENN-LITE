

def main():
    EXECUTABLE = "DENN-float"

    CMD = "{exe} -t 20000 -s {batch_step} -np {population_size} -f 0.5 -cr 0.9 -cmax {cmax} -cmin {cmin} -rmax {rmax} -rmin {rmin} -rc 10 -rd 0.001 -tp 17 -i {dataset} -o results/{outname} -co bin -m {mutation} -em {method} --compute_test_per_pass no -ro bench"

    OUTNAME = "{method}_{dataset_name}_{batch_size}_{batch_step}_{population_size}_{mutation}_hl{hl}_run{run}.json"

    DATASETS = [
        # (10, 10, "MAGIC"),
        # # (10, 20, "MAGIC"),
        # # (10, 40, "MAGIC"),
        # (41, 10, "QSAR"),
        ## (41, 20, "QSAR"),
        ## (41, 40, "QSAR"),
        # (19, 10, "BANK"),
        ## (19, 20, "BANK"),
        ## (19, 40, "BANK"),
        # (128, 30, "GASS"),
        ## (128, 60, "GASS"),
        ## (128, 120, "GASS"),
        (784, 50, "MNIST"),
        ## (784, 100, "MNIST"),
        ## (784, 200, "MNIST"),
    ]

    DATASETS_DICT = {
        "MAGIC": {
            10: "../DENN-dataset-samples/JULY_MAGIC_1_1521x10_1s.gz",
            20: "../DENN-dataset-samples/JULY_MAGIC_1_760x20_1s.gz",
            40: "../DENN-dataset-samples/JULY_MAGIC_1_380x40_1s.gz"
        },
        "QSAR": {
            10: "../DENN-dataset-samples/JULY_QSAR_1_84x10_1s.gz",
            20: "../DENN-dataset-samples/JULY_QSAR_1_42x20_1s.gz",
            40: "../DENN-dataset-samples/JULY_QSAR_1_21x40_1s.gz"
        },
        "GASS": {
            30: "../DENN-dataset-samples/JULY_GasSensorArrayDrift_1_370x30_1s.gz",
            60: "../DENN-dataset-samples/JULY_GasSensorArrayDrift_1_185x60_1s.gz",
            120: "../DENN-dataset-samples/JULY_GasSensorArrayDrift_1_92x120_1s.gz"
        },
        "MNIST": {
            50: "../DENN-dataset-samples/JULY_MNIST_1_1080x50_1s.gz",
            100: "../DENN-dataset-samples/JULY_MNIST_1_540x100_1s.gz",
            200: "../DENN-dataset-samples/JULY_MNIST_1_270x200_1s.gz"
        }
    }

    METHODS = ["JDE"]
    MUTATIONS = [
        "rand/1",
        # "degl"
    ]
    HLS = []
    FACTORS = [1, 2, 4]

    out_names = []

    with open("jobs/job_SEPTEMBER.csv", "w") as out_file:
        for features, batch_size, dataset_name in DATASETS:
            for batch_factor in FACTORS:
                for step_factor in FACTORS:
                    for np_factor in [elm * 2 for elm in FACTORS]:
                        for method in METHODS:
                            for mutation in MUTATIONS:
                                for run in range(5):
                                    hl = 0
                                    outname = OUTNAME.format(
                                        batch_step=batch_size * step_factor,
                                        population_size=batch_size * np_factor,
                                        dataset_name=dataset_name,
                                        method=method,
                                        mutation=mutation.replace("/", ""),
                                        hl=hl,
                                        batch_size=batch_size * batch_factor,
                                        run=run
                                    )
                                    out_names.append(outname)
                                    max_val = 1.0
                                    min_val = -1.0
                                    if dataset_name == "MNIST":
                                        max_val = 0.5
                                        min_val = -0.5
                                    cur_cmd = CMD.format(
                                        exe=EXECUTABLE,
                                        batch_step=batch_size * step_factor,
                                        population_size=batch_size * np_factor,
                                        dataset=DATASETS_DICT[dataset_name][
                                            batch_size * batch_factor],
                                        outname=outname,
                                        method=method,
                                        mutation=mutation,
                                        cmax=max_val,
                                        cmin=min_val,
                                        rmax=max_val,
                                        rmin=min_val
                                    )
                                    out_file.write(cur_cmd)
                                    out_file.write("\n")
        # for features, batch_size, dataset_name, dataset in DATASETS:
        #     for method in METHODS:
        #         for mutation in MUTATIONS:
        #             for hl in HLS:
        #                 for run in range(5):
        #                     if dataset_name != "MNIST":
        #                         outname = OUTNAME.format(
        #                             batch_step=batch_size,
        #                             population_size=batch_size * 2,
        #                             dataset_name=dataset_name,
        #                             method=method,
        #                             mutation=mutation.replace("/", ""),
        #                             hl=hl,
        #                             run=run
        #                         )
        #                         out_names.append(outname)
        #                         max_val = 1.0
        #                         min_val = -1.0
        #                         cur_cmd = CMD.format(
        #                             exe=EXECUTABLE,
        #                             batch_step=batch_size,
        #                             population_size=batch_size * 2,
        #                             dataset=dataset,
        #                             outname=outname,
        #                             method=method,
        #                             mutation=mutation,
        #                             cmax=max_val,
        #                             cmin=min_val,
        #                             rmax=max_val,
        #                             rmin=min_val
        #                         )
        #                         if hl > 0:
        #                             levels = [str(features * 2)] * hl
        #                             cur_cmd += " -hl {}".format(
        #                                 " ".join(levels))
        #                         out_file.write(cur_cmd)
        #                         out_file.write("\n")

    ##
    # Check if all out names are differents
    assert len(out_names) == len(set(out_names)), "{} != {}".format(
        len(out_names), len(set(out_names)))

    print(len(out_names))


if __name__ == '__main__':
    main()
