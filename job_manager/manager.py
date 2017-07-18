#!/bin/bash
import csv
from subprocess import Popen
from os.path import exists
from os.path import isdir
from os import getcwd
from os import mkdir
from os import path
from os import pardir
import logging
from time import time
import sys


def pretty_cmd(cmd):
    string = cmd[0]
    for elm in cmd[1:]:
        if elm[0] == "-" and elm[1].isalpha():
            string += "\n+ {}".format(elm)
        else:
            string += "\t{}".format(elm)
    return string


def main():
    if not exists("results") or not isdir("results"):
        mkdir("results")
    logging.basicConfig(filename='./results/job-{}.log'.format(int(time())),level=logging.DEBUG)

    logging.info("Open Job file")
    job_file = path.join(pardir, "jobs", 'job.csv')
    if len(sys.argv) > 1:
        job_file = sys.argv[1]
    with open(job_file, newline='') as csvfile:
        jobs = csv.reader(csvfile, delimiter=' ')
        jobs = list(jobs)
        for idx, task in enumerate(jobs, 1):
            if len(task) != 0 and task[0] != "#":
                task[0] = path.join("Release", task[0])
                logging.info("Execute task {}/{} -> \"{}\"".format(
                    idx,
                    len(jobs),
                    " ".join(task)
                ))
                print("+++ TASK -> {}".format(pretty_cmd(task)))
                sys.stdout.flush()
                cur_task = Popen(task, cwd=getcwd(), universal_newlines=True)
                return_code = cur_task.wait()
                logging.info("Job exited with code {}".format(return_code))

if __name__ == '__main__':
    main()
