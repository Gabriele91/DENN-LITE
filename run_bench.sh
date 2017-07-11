#!/bin/bash

CUR_TIME=$(python -c "from time import time;print(int(time()))")
mv results results_$CUR_TIME 2>/dev/null ;
rm -fR results ;
mkdir -p results ;
touch results/OUTPUT.out ;
nohup python3 job_manager.py >results/OUTPUT.out 2>&1 &