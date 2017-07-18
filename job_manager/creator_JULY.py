 

def main():
    EXECUTABLE = "DENN-float"
    
    CMD = "{exe} -t 20000 -s {batch_step} -np {population_size} -f 0.5 -cr 0.9 -jf 0.5 -jcr 0.9 -cmax 1.0 -cmin -1.0 -rmax 1.0 -rmin -1.0 -rc 10 -rd 0.001 -tp 16 -i {dataset} -o results/{outname} -co bin -m {mutation} -em {method}"
    
    OUTNAME = "{method}_{dataset_name}_{batch_step}_{population_size}_{mutation}_hl{hl}_run{run}.json"

    DATASETS = [
        (10, 10, "MAGIC", "../DENN-dataset-samples/JULY_MAGIC_1_1521x10_1s.gz"),
        (10, 20, "MAGIC", "../DENN-dataset-samples/JULY_MAGIC_1_760x20_1s.gz"),
        (10, 40, "MAGIC", "../DENN-dataset-samples/JULY_MAGIC_1_380x40_1s.gz"),
        (41, 10, "QSAR", "../DENN-dataset-samples/JULY_QSAR_1_84x10_1sdb.gz"),
        (41, 20, "QSAR", "../DENN-dataset-samples/JULY_QSAR_1_42x20_1s.gz"),
        (41, 40, "QSAR", "../DENN-dataset-samples/JULY_QSAR_1_21x40_1s.gz"),
        (19, 10, "BANK", "../DENN-dataset-samples/JULY_BANK_1_3707x10_1s.gz"),
        (19, 20, "BANK", "../DENN-dataset-samples/JULY_BANK_1_1853x20_1s.gz"),
        (19, 40, "BANK", "../DENN-dataset-samples/JULY_BANK_1_926x40_1s.gz"),
        (128, 30, "GASS", "../DENN-dataset-samples/JULY_GasSensorArrayDrift_1_370x30_1s.gz"),
        (128, 60, "GASS", "../DENN-dataset-samples/JULY_GasSensorArrayDrift_1_185x60_1s.gz"),
        (128, 120, "GASS", "../DENN-dataset-samples/JULY_GasSensorArrayDrift_1_92x120_1s.gz"),
        (784, 50, "MNIST", "../DENN-dataset-samples/JULY_MNIST_1_1080x50_1s.gz"),
        (784, 100, "MNIST", "../DENN-dataset-samples/JULY_MNIST_1_540x100_1s.gz"),
        (784, 200, "MNIST", "../DENN-dataset-samples/JULY_MNIST_1_270x200_1s.gz")
    ]

    METHODS = ["DE", "JDE"]
    MUTATIONS = ["rand/1", "curr_best", "degl"]
    HLS = [1, 2]

    out_names = []

    with open("jobs/job_JULY.csv", "w") as out_file:
        for run in range(5):
            for features, batch_size, dataset_name, dataset in DATASETS:
                for method in METHODS:
                    for mutation in MUTATIONS:
                        hl = 0
                        outname = OUTNAME.format(
                            batch_step=batch_size,
                            population_size=batch_size*2,
                            dataset_name=dataset_name,
                            method=method,
                            mutation=mutation.replace("/", ""),
                            hl=hl,
                            run=run
                        )
                        out_names.append(outname)
                        cur_cmd = CMD.format(
                            exe=EXECUTABLE,
                            batch_step=batch_size,
                            population_size=batch_size*2,
                            dataset=dataset,
                            outname=outname,
                            method=method,
                            mutation=mutation
                        )
                        out_file.write(cur_cmd)
                        out_file.write("\n")
        for run in range(5):
            for features, batch_size, dataset_name, dataset in DATASETS:
                for method in METHODS:
                    for mutation in MUTATIONS:
                        for hl in HLS:
                            if dataset_name != "MNIST":
                                outname = OUTNAME.format(
                                    batch_step=batch_size,
                                    population_size=batch_size*2,
                                    dataset_name=dataset_name,
                                    method=method,
                                    mutation=mutation.replace("/", ""),
                                    hl=hl,
                                    run=run
                                )
                                out_names.append(outname)
                                cur_cmd = CMD.format(
                                    exe=EXECUTABLE,
                                    batch_step=batch_size,
                                    population_size=batch_size*2,
                                    dataset=dataset,
                                    outname=outname,
                                    method=method,
                                    mutation=mutation
                                )
                                if hl > 0:
                                    levels = [str(features*2)]*hl
                                    cur_cmd += " hl {}".format(" ".join(levels))
                                out_file.write(cur_cmd)
                                out_file.write("\n")

    ##
    # Check if all out names are differents
    assert len(out_names) == len(set(out_names)), "{} != {}".format(len(out_names), len(set(out_names)))
                        


if __name__ == '__main__':
    main()