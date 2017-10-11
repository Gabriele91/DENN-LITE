from sys import argv
from subprocess import Popen
from os import getcwd
import json

STATE_NO_INOUT_DATASET = 0
STATE_INPUT_DATASET = 1
STATE_OUTPUT_DATASET = 2

def main():
    state = STATE_NO_INOUT_DATASET
    denn_input = ''
    denn_output = ''
    denn_args = []
    denn_accuracy = []
    for i,v in enumerate(argv):
        if i == 0: 
            continue
        elif v in ('--dataset', '-d', '-i'):
            state = STATE_INPUT_DATASET
        elif  state == STATE_INPUT_DATASET:
            denn_input = v
            state = STATE_NO_INOUT_DATASET
        elif v in ('--output','-o'):
            state = STATE_OUTPUT_DATASET
        elif  state == STATE_OUTPUT_DATASET:
            denn_output = v
            state = STATE_NO_INOUT_DATASET
        else:
            denn_args.append(v)
    denn_k_end = int(denn_input.split(':')[2])
    denn_k_start = int(denn_input.split(':')[1])
    denn_input = denn_input.split(':')[0]

    for k in range(denn_k_start, denn_k_end+1):
        task = denn_args + [ '-i', denn_input.format(k) ] + [ '-o', denn_output.format(k) ]
        cur_task = Popen(task, cwd=getcwd(), universal_newlines=True)
        return_code = cur_task.wait()
        if return_code == 0:
            with open(denn_output.format(k)) as output:
                denn_accuracy.append(float(json.load(output)['accuracy']))
    
    #compute avg
    avg_accuracy = sum(denn_accuracy) / len(denn_accuracy)

    #return
    return (avg_accuracy, denn_accuracy)

if __name__ == '__main__':
    avg, all = main()
    print("avg accuracy:", avg)
    print("list of accuracy:", all)