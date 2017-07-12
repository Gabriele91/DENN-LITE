import argparse
from os import walk
from os import path
import json
import csv

FEATURES = [
    "generation",
    "sub_gens",
    "number_parents",
    "evolution_method",
    "mutation",
    "crossover",
    "f_default",
    "cr_default",
    "f_jde",
    "cr_jde",
    "archive_size",
    "f_cr_adapt",
    "perc_of_best",
    "clamp_min",
    "clamp_max",
    "random_min",
    "random_max",
    "restart_count",
    "restart_delta",
    "hidden_layers",
    "dataset",
    "output"
]


def extract(data):
    row = []
    row.append(data['accuracy'])
    row.append(data['time'])

    for feature in FEATURES:
        if feature == "hidden_layers":
            row.append(str(data['arguments']['hidden_layers']))
        elif feature == "dataset" or feature == "output":
            row.append(data['arguments'][feature].split("/")[-1])
        else:
            row.append(data['arguments'][feature])

    return row


def main():
    parser = argparse.ArgumentParser(description='Convert results to CSV.')
    parser.add_argument('folder', metavar='folder', type=str,
                        help='The results folder')

    args, unparsed = parser.parse_known_args()

    if args.folder is not None:
        with open('results.csv', 'w', newline='') as csvfile:
            writer = csv.writer(csvfile, quotechar='"',
                                quoting=csv.QUOTE_NONNUMERIC)
            writer.writerow([
                "accuracy",
                "time",
                "generation",
                "sub_gens",
                "number_parents",
                "evolution_method",
                "mutation",
                "crossover",
                "f_default",
                "cr_default",
                "f_jde",
                "cr_jde",
                "archive_size",
                "f_cr_adapt",
                "perc_of_best",
                "clamp_min",
                "clamp_max",
                "random_min",
                "random_max",
                "restart_count",
                "restart_delta",
                "hidden_layers",
                "dataset",
                "output"
            ])
            for root, dirs, files in walk(args.folder):
                for file_ in files:
                    filename, extension = path.split(file_)
                    if extension.find(".json") != -1:
                        print(file_)
                        with open(path.join(root, file_)) as cur_file:
                            cur_res = json.load(cur_file)
                            writer.writerow(extract(cur_res))
    else:
        parser.print_help()

if __name__ == '__main__':
    main()
