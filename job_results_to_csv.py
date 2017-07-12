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
    "degl_scalar_weight",
    "degl_neighborhood"
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
            row.append(data['arguments'].pop(feature, None))

    return row

CMD_PARAMS = {
    ("--generation", "-t"): "generation",
    ("--sub_genssub_gens", "-s"): "sub_gens",
    ("--number_parents", "-np"): "number_parents",
    ("--evolution_method", "--evolution", "-em"): "evolution_method",
    ("--mutation", "-m"): "mutation",
    ("--crossover", "-co"): "crossover",
    ("--f_default", "-f"): "f_default",
    ("--cr_default", "-cr"): "cr_default",
    ("--f_jde", "-jf"): "f_jde",
    ("--cr_jde", "-jcr"): "cr_jde",
    ("--archive_size", "-as"): "archive_size",
    ("--f_cr_adapt", "-afcr"): "f_cr_adapt",
    ("--perc_of_best", "-pob"): "perc_of_best",
    ("--degl_scalar_weight", "-glw"): "degl_scalar_weight",
    ("--degl_neighborhood", "-glnn"): "degl_neighborhood",
    ("--clamp_min", "-cmin"): "clamp_min",
    ("--clamp_max", "-cmax"): "clamp_max",
    ("--random_min", "-rmin"): "random_min",
    ("--random_max", "-rmax"): "random_max",
    ("--restart_count", "-rc"): "restart_count",
    ("--restart_delta", "-rd"): "restart_delta",
    ("--hidden_layers", "-hl"): "hidden_layers",
    ("--dataset", "-d", "-i"): "dataset",
    ("--output", "-o"): "output"
}


def parse_file(text):
    rows = []
    status = "START"
    parsed_values = {}

    for line in text.readlines():
        if status == "START":
            if line.find("+++ TASK ->") != -1:
                status = "NEW_TASK"
                parsed_values = {}
                continue
            ##
            # TO UNCOMMENT AFTER OUTPUT IS STABLE!!!
            # raise Exception("NO NEW TASK!")
        elif status == "NEW_TASK":
            if line.find("=== START ===") == -1:
                print(line)
                ##
                # TRY CATCH TO BE REMOVED!!!
                try:
                    cmd, cmd_value = line.replace("+ ", "").split("\t")
                except ValueError:
                    status = "START"
                    parsed_values = {}
                    continue
                for key, key_value in CMD_PARAMS.items():
                    if cmd in key:
                        if key_value == "dataset" or key_value == "output":
                            parsed_values[key_value] = cmd_value.strip().split("/")[-1]
                        else:
                            try:
                                parsed_values[key_value] = float(cmd_value.strip())
                            except ValueError:
                                parsed_values[key_value] = cmd_value.strip()
                continue
            status = "GET_RESULTS"
            continue
        elif status == "GET_RESULTS":
            if line.find("+ TEST") == 0:
                parsed_values['accuracy'] = float(line.split("\t")[-1].strip())
                continue
            elif line.find("+ TIME") == 0:
                parsed_values['time'] = float(line.split("\t")[-1].strip())
                continue
            elif line.find("=== END ===") == 0:
                cur_row = []
                cur_row.append(parsed_values['accuracy'])
                cur_row.append(parsed_values['time'])
                for feature in FEATURES:
                    cur_row.append(parsed_values.get(feature, None))
                rows.append(cur_row)
                status = "START"
                parsed_values = {}
                continue
    print(rows)
    return rows


def main():
    parser = argparse.ArgumentParser(description='Convert results to CSV.')
    parser.add_argument('--folder', metavar='folder', type=str,
                        help='The results folder')
    parser.add_argument('--outfile', metavar='outfile', type=str,
                        help='The output file')

    args, unparsed = parser.parse_known_args()

    if args.folder is not None or args.outfile is not None:
        with open('results.csv', 'w', newline='') as csvfile:
            writer = csv.writer(csvfile, quotechar='"',
                                quoting=csv.QUOTE_NONNUMERIC)
            writer.writerow(['accuracy', 'time'] + FEATURES)
            if args.folder is not None:
                for root, dirs, files in walk(args.folder):
                    for file_ in files:
                        filename, extension = path.split(file_)
                        if extension.find(".json") != -1:
                            print(file_)
                            with open(path.join(root, file_)) as cur_file:
                                cur_res = json.load(cur_file)
                                writer.writerow(extract(cur_res))
            elif args.outfile is not None:
                with open(args.outfile) as output_file:
                    rows = parse_file(output_file)
                    writer.writerows(rows)
    else:
        parser.print_help()

if __name__ == '__main__':
    main()
