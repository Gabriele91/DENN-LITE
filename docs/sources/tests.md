# Test bench system

A bench job list is a `csv` file that is used by the **job manager**. You have some bench generator inside the tests folder in `job_manager` and the relative `csv` files are in the folder `jobs`.

To use such bench list you can input this command from the `tests` folder:

```bash
./run_bench.sh jobs/job_2018_MAR_speed_test.csv
```

As you can see you have to specify the csv to use and the **job manager** will execute all the tasks in the list. You will find the results in a folder named `results`. This folder is not overwritten if you terminated the tasks and you use again the `run_bench` command, but you will break the execution if you tempt to lunch consecutive times the bench because the program rename the folder to make a fresh one.

> **NOTE**: inside the folder `results` you will find the json files for each task plus an `OUTPUT.out` file (that is the output of the **DENN framework**) and a `job-xxxxx.log` (that is the **job_manager** output). These files are used to have a feedback and check eventual problems.