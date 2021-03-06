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
            elif line.find("+++ TASK ->") != -1 and line.find("output=") != -1:
                chunks = line.split("\t")
                for chunk in chunks:
                    if chunk.find("output=") != -1:
                        files.append(chunk.replace("output=", "").strip())

    return files


def main():
    parser = argparse.ArgumentParser(description='Convert DENN results')
    parser.add_argument('source_folder', type=str,
                        help='result source folder')
    parser.add_argument('output', type=str,
                        help='the output format')
    parser.add_argument('--filter', type=str, dest="filter", default="", nargs="+",
                        help='the filter that have to be applied to results')
    parser.add_argument('--only-performances', type=bool, dest="only_performances", default=False,
                        help='Extract only time performances from results')

    args = parser.parse_args()

    if args.output not in ['csv']:
        raise Exception(
            "'{}' is not a supported extension... use one of this: ['csv']".format(args.output))

    data = []
    if not args.only_performances:
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
    elif args.only_performances:
        labels = [
            'dataset',
            'batch_size',
            'time'
        ]
    # Load results
    for file_ in tqdm(files_from_output(args.source_folder), desc="Extract data from json files"):
        try:
            with open(path.join(args.source_folder, file_)) as res_file:
                tmp = json.load(res_file)
                if 'mutation_list' in tmp['arguments']:
                    mutation = " ".join(tmp['arguments']['mutations_list'])
                else:
                    mutation = tmp['arguments']['mutation']

                if not args.only_performances:
                    new_data = [
                        tmp['arguments']['dataset'].split("/")[-1].replace(".gz", ""),
                        tmp['arguments']['evolution_method'],
                        mutation,
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
                elif args.only_performances:
                    new_data = [
                        tmp['arguments']['dataset'].split("/")[-1].replace(".gz", ""),
                        tmp['arguments']['batch_size'],
                        tmp['time']
                    ]

                if tmp['arguments']['output'].find("run") != -1:
                    new_data.append(
                        int(tmp['arguments']['output'].split("run")[1].split(".")[0]))
                    if "run" not in labels:
                        labels.append("run")

                data.append(new_data)
        except IOError as err:
            if err.errno == 2:
                print("[Warning]: file '{}' not found".format(file_))
            else:
                raise
        except json.decoder.JSONDecodeError as err:
            print("Error deconding file '{}' => {}".format(file_, err))

    # Load results with pandas
    print("==> Convert to pandas")
    data_frame = pd.DataFrame(
        data,
        columns=labels
    )

    print(data_frame)

    if args.output == "csv":
        print("==> Save into 'results.csv'")
        df_to_csv = data_frame
        
        if not args.only_performances:
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
        
        elif args.only_performances:

            if "mean" in args.filter:
                df_to_csv = df_to_csv.reset_index().groupby(['dataset',
                                                            'batch_size'], as_index=False).mean()
                df_to_csv = df_to_csv.drop("run", 1)
                df_to_csv = df_to_csv.drop("index", 1)
            
            if "sort" in args.filter:
                df_to_csv = df_to_csv.sort_values(["dataset", "batch_size"], ascending=True)
        
        df_to_csv.to_csv("results.csv", index=False, float_format='%.9f')


if __name__ == '__main__':
    main()
