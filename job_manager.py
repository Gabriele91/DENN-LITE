#!/bin/bash
import csv
from subprocess import Popen
from subprocess import PIPE
from os.path import abspath
from os.path import dirname
from os.path import exists
from os.path import isdir
from os import mkdir
import logging
from time import time


def main():
    if not exists("results") or not isdir("results"):
        mkdir("results")
    logging.basicConfig(filename='results/job-{}.log'.format(int(time())),level=logging.DEBUG)

    working_dir = dirname(abspath(__file__))
    logging.info("Open Job file")
    with open('job.csv', newline='') as csvfile:
        jobs = csv.reader(csvfile, delimiter=' ')
        for task in jobs:
            if len(task) != 0 and task[0] != "#":
                logging.info("Execute command -> \"{}\"".format(" ".join(task)))
                cur_task = Popen(task, cwd=working_dir, universal_newlines=True)
                return_code = cur_task.wait()
                logging.info("Job exited with code {}".format(return_code))

if __name__ == '__main__':
    main()
