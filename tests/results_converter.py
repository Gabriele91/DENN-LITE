"""DENN result converter."""
import argparse
from os import path

from tqdm import tqdm
import json
import pandas as pd


def files_from_output(folder):
    """Get list of result files from output log."""

    files = []
    with open(path.join(folder, "OUTPUT.out")) as out_file:
        for line in tqdm(out_file.readlines(), desc="Read files from output"):
            if line.find("+ -o") != -1:
                files.append(line.replace(
                    "+ -o\t", "").replace("results/", "").strip())

    return files


def main():
    parser = argparse.ArgumentParser(description='Convert DENN results')
    parser.add_argument('source_folder', type=str,
                        help='result source folder')
    parser.add_argument('output', type=str,
                        help='the output format')
    parser.add_argument('--filter', type=str, dest="filter", default="", nargs="+",
                        help='the filter that have to be applied to results')

    args = parser.parse_args()

    if args.output not in ['csv']:
        raise Exception(
            "'{}' is not a supported extension...".format(args.output))

    data = []
    labels = [
        'dataset',
        'evolution_method',
        'mutation',
        'crossover',
        'generations',
        'sub_gens',
        'batch_size',
        'batch_offset',
        'number_parents',
        'clamp_min',
        'clamp_max',
        'accuracy',
    ]
    # Load results
    for file_ in tqdm(files_from_output(args.source_folder), desc="Extract data from json files"):
        with open(path.join(args.source_folder, file_)) as res_file:
            tmp = json.load(res_file)
            new_data = [
                tmp['arguments']['dataset'].split("/")[-1].replace(".gz", ""),
                tmp['arguments']['evolution_method'],
                tmp['arguments']['mutation'],
                tmp['arguments']['crossover'],
                tmp['arguments']['generations'],
                tmp['arguments']['sub_gens'],
                tmp['arguments']['batch_size'],
                tmp['arguments']['batch_offset'],
                tmp['arguments']['number_parents'],
                tmp['arguments']['clamp_min'],
                tmp['arguments']['clamp_max'],
                tmp['accuracy']
            ]

            if tmp['arguments']['output'].find("run") != -1:
                new_data.append(
                    int(tmp['arguments']['output'].split("run")[1].split(".")[0]))
                if "run" not in labels:
                    labels.append("run")

            data.append(new_data)

    # Load results with pandas
    print("==> Convert to pandas")
    data_frame = pd.DataFrame(
        data,
        columns=labels
    )

    # print(data_frame)

    if args.output == "csv":
        print("==> Save into 'results.csv'")
        df_to_csv = data_frame

        if "mean" in args.filter:
            df_to_csv = df_to_csv.reset_index().groupby(['dataset',
                                                          'evolution_method',
                                                          'mutation',
                                                          'crossover',
                                                          'generations',
                                                          'sub_gens',
                                                          'batch_size',
                                                          'batch_offset',
                                                          'number_parents',
                                                          'clamp_min',
                                                          'clamp_max', ], as_index=False).mean()
            df_to_csv = df_to_csv.drop("run", 1)
            df_to_csv = df_to_csv.drop("index", 1)
        
        if "sort" in args.filter:
            df_to_csv = df_to_csv.sort_values(["dataset", "accuracy"], ascending=False)
        
        df_to_csv.to_csv("results.csv", index=False)


if __name__ == '__main__':
    main()
