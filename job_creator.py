 

def main():
    EXECUTABLE = "./Release/DENN-float"
    
    CMD_JDE = "{exe} -t {total_step} -s {batch_step} -np {population_size} -f 0.1 -cr 0.5 -jf 0.1 -jcr 0.1 -cmax {cmax} -cmin {cmin} -rmax {cmax} -rmin {cmin} -rc {restart} -rd {restart_delta} -tp 16 -i {dataset} -o results/{outname} -co {crossover} -m {mutation}"
    CMD_JADE = "{exe} -t {total_step} -s {batch_step} -np {population_size} -f 0.1 -cr 0.5 -jf 0.1 -jcr 0.1 -cmax {cmax} -cmin {cmin} -rmax {cmax} -rmin {cmin} -rc {restart} -rd {restart_delta} -tp 16 -i {dataset} -o results/{outname} -co {crossover} -m curr_p_best -em JADE -as {archive_size}"
    
    OUTNAME_JDE = "JDE_{dataset_name}_{total_step}_{batch_step}_{population_size}_{mutation}_{crossover}_{clamp}.json"
    OUTNAME_JADE = "JADE_{dataset_name}_{total_step}_{batch_step}_{population_size}_{archive_size}_{crossover}_{clamp}.json"

    DATASETS = [
        "../DENN-dataset-samples/JULY_MNIST_0_540x100_1s.gz",
        "../DENN-dataset-samples/JULY_MNIST_1_540x100_1s.gz",
        "../DENN-dataset-samples/JULY_GasSensorArrayDrift_0_185x60_1s",
        "../DENN-dataset-samples/JULY_GasSensorArrayDrift_1_185x60_1s"
    ]
    DATASET_NAMES = [
        "MNIST_0",
        "MNIST_1",
        "GASS_0",
        "GASS_1"
    ]

    CLAMPS = [1.0, 0.5, 0.25]
    CROSS = ["bin", "interm"]
    MUTATION = ["rand/1", "best/1"]
    POPULATION = [50, 100, 150]
    ARCHIVE_SIZE = [50, 100, 150]
    BATCH_STEP = [50, 100]
    TOTAL_STEPS = [8000]

    with open("job_JDE.csv", "w") as jde_file:
        for num_db, dataset in enumerate(DATASETS):
            for total_step in TOTAL_STEPS:
                for batch_step in BATCH_STEP:
                    for pop_size in POPULATION:
                        for mutation in MUTATION:
                            for cross in CROSS:
                                for clamp in CLAMPS:
                                    outname = OUTNAME_JDE.format(
                                        dataset_name=DATASET_NAMES[num_db],
                                        total_step=total_step,
                                        batch_step=batch_step,
                                        population_size=pop_size,
                                        mutation=mutation,
                                        crossover=cross,
                                        clamp=clamp
                                    )
                                    jde_file.write(CMD_JDE.format(
                                        dataset=dataset,
                                        exe=EXECUTABLE,
                                        total_step=total_step,
                                        batch_step=batch_step,
                                        population_size=pop_size,
                                        mutation=mutation,
                                        crossover=cross,
                                        cmax=clamp,
                                        cmin=-clamp,
                                        rmax=clamp,
                                        rmin=-clamp,
                                        restart=4,
                                        restart_delta=0.001,
                                        outname=outname
                                    ))
                                    jde_file.write("\n")
    
    with open("job_JADE.csv", "w") as jde_file:
        for num_db, dataset in enumerate(DATASETS):
            for total_step in TOTAL_STEPS:
                for batch_step in BATCH_STEP:
                    for pop_size in POPULATION:
                        for archive_size in ARCHIVE_SIZE:
                            for cross in CROSS:
                                for clamp in CLAMPS:
                                    outname = OUTNAME_JADE.format(
                                        dataset_name=DATASET_NAMES[num_db],
                                        total_step=total_step,
                                        batch_step=batch_step,
                                        population_size=pop_size,
                                        archive_size=archive_size,
                                        crossover=cross,
                                        clamp=clamp
                                    )
                                    jde_file.write(CMD_JADE.format(
                                        dataset=dataset,
                                        exe=EXECUTABLE,
                                        total_step=total_step,
                                        batch_step=batch_step,
                                        population_size=pop_size,
                                        archive_size=archive_size,
                                        crossover=cross,
                                        cmax=clamp,
                                        cmin=-clamp,
                                        rmax=clamp,
                                        rmin=-clamp,
                                        restart=4,
                                        restart_delta=0.001,
                                        outname=outname
                                    ))
                                    jde_file.write("\n")
                        


if __name__ == '__main__':
    main()