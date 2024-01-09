import matplotlib.pyplot as plt
from collections import Counter
import numpy as np
import os

def read_checkpoint(filename):
    """Read the last checkpoint from a file and return the lines."""
    with open(filename, 'r') as file:
        lines = file.readlines()[-4000:]

    for i, line in enumerate(lines):
        if line.startswith("Checkpoint"):
            return lines[i:i+10]
    
    return []

def print_checkpoint(lines):
    """Print the checkpoint lines."""
    for line in lines:
        print(line.strip())

def read_numbers(filename):
    """Read numbers from a file and return them as a list."""
    with open(filename, 'r') as file:
        return [int(line.strip()) for line in file]
    
def plot_histogram(numbers):
    # Create the histogram
    plt.hist(numbers, bins=max(numbers)-min(numbers)+1, range=(min(numbers), max(numbers)+1))

    # Set the labels and title
    plt.xlabel('Number of Kmer Counts')
    plt.ylabel('Frequency')
    plt.title('Kmer Count Histogram')

    plt.savefig("kmer_count_histogram2.png")

#TODO. Not working. Fix it.
def plot_histogram_w_freq_2(numbers):
    # Get numbers with frequency more than 1
    counter = Counter(numbers)
    numbers_with_frequency_more_than_1 = [num for num, count in counter.items() if count > 1]

    # Determine the range of the histogram bins based on the original numbers list
    min_value = min(numbers)
    max_value = max(numbers)

    # Plot numbers with frequency more than 1
    plt.figure()
    plt.hist(numbers_with_frequency_more_than_1, bins=max_value-min_value+1, range=(min_value, max_value+1))
    plt.xlabel('Number of Code Clones')
    plt.ylabel('Frequency')
    plt.title('Code Clone Histogram (Numbers with Frequency > 1)')
    plt.savefig("code_clone_histogram_frequency_more_than_1.png")

def plot_log_histogram(numbers):
    """Plot a histogram of the numbers with a logarithmic y-scale and save to a file."""
    plt.figure()
       
    # Using numpy to create bins more suited for log scale
    bins = np.logspace(np.log10(min(numbers)), np.log10(max(numbers)), 50)
    plt.hist(numbers, bins=bins, edgecolor='black')

    plt.xscale('log')  # Set x-axis to logarithmic scale
    plt.yscale('log')  # Set y-axis to logarithmic scale

    plt.xlabel('Number of Kmer Count')
    plt.ylabel('Frequency')
    plt.title('Histogram of Kmer Counts (Log Scale)')
    plt.grid(True)

    plt.savefig("kmer_count_log_histogram.png")

def analyse_code_clone_numbers(numbers):
    """Analyse the numbers and print the results."""
    print("Number of numbers: {}".format(len(numbers)))
    print("Minimum number: {}".format(min(numbers)))
    print("Maximum number: {}".format(max(numbers)))
    print("Average number: {}".format(sum(numbers) / len(numbers)))
    print("Median number: {}".format(np.median(numbers)))
    print("Standard deviation: {}".format(np.std(numbers)))
    print("Variance: {}".format(np.var(numbers)))

    #print the number with the highest frequency
    counter = Counter(numbers)
    most_common = counter.most_common(1)
    print("Number with the highest frequency: {}".format(most_common[0][0]))

    #print the numbers around 2standard deviations from the mean
    mean = np.mean(numbers)
    std = np.std(numbers)
    print("2 standard deviations smaller from the mean: {}".format(mean - 2*std))
    print("2 standard deviations larger from the mean: {}".format(mean + 2*std))

    #99% of the numbers are in this range
    print("1% of the numbers are in this range: {}".format(np.percentile(numbers, 1)))
    print("99% of the numbers are in this range: {}".format(np.percentile(numbers, 99)))

    print("3.33% of the numbers are in this range: {}".format(np.percentile(numbers, 3.33)))
    print("96.66% of the numbers are in this range: {}".format(np.percentile(numbers, 96.66)))

    print("5% of the numbers are in this range: {}".format(np.percentile(numbers, 5)))
    print("95% of the numbers are in this range: {}".format(np.percentile(numbers, 95)))


    print("10% of the numbers are in this range: {}".format(np.percentile(numbers, 10)))
    print("90% of the numbers are in this range: {}".format(np.percentile(numbers, 90)))

def analyse_kmer_experiments():
    # Get the directory path
    directory = "/home/eilgun/bio2se/experiment/"

    # Iterate through all files in the directory
    for filename in os.listdir(directory):
        if filename.startswith("kmer") and filename.endswith(".txt"):
            file_path = os.path.join(directory, filename)
            print(file_path)
            with open("checkpoint.txt", "a") as file:
                file.write("File name: {}\n".format(filename))
                file.write(str(read_checkpoint(file_path)))
                file.write("\n")

def prec_rec_f1(tp, fp, fn, tn):
    precision = tp / (tp + fp) if (tp + fp) != 0 else -1
    recall = tp / (tp + fn) if (tp + fn) != 0 else -1
    f1 = 2 * (precision * recall) / (precision + recall) if (precision + recall) != 0 else -1

    return precision, recall, f1

def sort_the_effectiveness_measures(results):
    """Sort the measures according to their current_file key as numbers."""
    return {k: v for k, v in sorted(results.items(), key=lambda item: int(item[0]))}

def compute_effectiveness_measures(checkpoints_filename):
    """Compute the effectiveness measures from the checkpoints file."""

    # Read the checkpoints file
    with open(checkpoints_filename, 'r') as file:
        text = file.read()
    
    lines = text.strip().split("\n")
    results = {}
    current_file = None
    tp, fp, fn, tn = 0, 0, 0, 0

    for line in lines:
        if line.startswith("File name:"):
            # If not the first file, store the previous file's results
            if current_file is not None:
                precision, recall, f1 = prec_rec_f1(tp, fp, fn, tn)
                results[current_file[4:-6]] = {"TP": tp, "FP": fp, "FN": fn, "TN": tn, "Precision": precision, "Recall": recall, "F1": f1}

            current_file = line.split(":")[1].strip()
            tp, fp, fn, tn = 0, 0, 0, 0
        elif line.startswith("Type"):
            continue
        else:
            # Read and sum the TP, FP, FN, TN values
            values = line.split()
            if len(values) == 4:
                if values[0] == "TP":
                    continue
                else:
                    tp += int(values[0])
                    fp += int(values[1])
                    fn += int(values[2])
                    tn += int(values[3])

    # Store the last file's results
    if current_file is not None:
        precision, recall, f1 = prec_rec_f1(tp, fp, fn, tn)
        results[current_file[4:-6]] = {"TP": tp, "FP": fp, "FN": fn, "TN": tn, "Precision": precision, "Recall": recall, "F1": f1}

    sort_the_effectiveness_measures(results)

    return results 

def write_effectiveness_measures(results):
    """Write the effectiveness measures to a file."""
    with open("effectiveness_measures.txt", "w+") as file:
        for filename, values in results.items():
            file.write("kmer size: {}\n".format(filename))
            file.write("TP: {}\n".format(values["TP"]))
            file.write("FP: {}\n".format(values["FP"]))
            file.write("FN: {}\n".format(values["FN"]))
            file.write("TN: {}\n".format(values["TN"]))

            print("kmer size: {}".format(filename))
            print("TP: {}".format(values["TP"]))
            print("FP: {}".format(values["FP"]))
            print("FN: {}".format(values["FN"]))
            print("TN: {}".format(values["TN"]))
            print()

            file.write("Precision: {}\n".format(values["Precision"]))
            file.write("Recall: {}\n".format(values["Recall"]))
            file.write("F1: {}\n".format(values["F1"]))
            file.write("\n")

def get_highest_f1(results):
    """Return the file name with the highest F1 score."""
    highest_f1 = -1
    highest_f1_file = None

    for filename, values in results.items():
        if values["F1"] > highest_f1:
            highest_f1 = values["F1"]
            highest_f1_file = filename

    return highest_f1_file

def get_highest_precision(results):
    """Return the file name with the highest precision."""
    highest_precision = -1
    highest_precision_file = None

    for filename, values in results.items():
        if values["Precision"] > highest_precision:
            highest_precision = values["Precision"]
            highest_precision_file = filename

    return highest_precision_file

def get_highest_recall(results):
    """Return the file name with the highest recall."""
    highest_recall = -1
    highest_recall_file = None

    for filename, values in results.items():
        if values["Recall"] > highest_recall:
            highest_recall = values["Recall"]
            highest_recall_file = filename

    return highest_recall_file

def plot_effectiveness_measures(results):
    "Given results, plot the effectiveness measures, i.e. precision, recall, and F1."

    plot = plt.figure()

    # Get the kmer sizes as x-labels
    kmer_sizes = list(results.keys())

    # Plot the precision
    plt.subplot(3, 1, 1)
    plt.plot(kmer_sizes, [values["Precision"] for values in results.values()])
    plt.ylabel("Precision")
    plt.xlabel("Kmer Size")
    plt.title("Precision")
    plt.xticks(rotation=45)  # Rotate x-axis labels

    # Plot the recall
    plt.subplot(3, 1, 2)
    plt.plot(kmer_sizes, [values["Recall"] for values in results.values()])
    plt.ylabel("Recall")
    plt.xlabel("Kmer Size")
    plt.title("Recall")
    plt.xticks(rotation=45)  # Rotate x-axis labels

    # Plot the F1
    plt.subplot(3, 1, 3)
    plt.plot(kmer_sizes, [values["F1"] for values in results.values()])
    plt.ylabel("F1")
    plt.xlabel("Kmer Size")
    plt.title("F1")
    plt.xticks(rotation=45)  # Rotate x-axis labels

    plt.savefig("effectiveness_measures_plot.png")

    return

'''
def plot_finished_experiments_effectiveness_measures(results):
    "Given results, plot the effectiveness measures, i.e. precision, recall, and F1."
    # Get the kmer sizes as x-labels
    kmer_sizes = [1, 10, 30, 50, 80, 100, 120, 150, 180, 200]

    values_list = [results[kmer_size] for kmer_size in kmer_sizes if kmer_size in results]
    print(values_list)

    plot = plt.figure()
    
    # Plot the precision
    plt.subplot(3, 1, 1)
    plt.plot(kmer_sizes, [values["Precision"] for values in results.values()])
    plt.ylabel("Precision")
    plt.xlabel("Kmer Size")
    plt.title("Precision")
    
    # Plot the recall
    plt.subplot(3, 1, 2)
    plt.plot(kmer_sizes, [values["Recall"] for values in results.values()])
    plt.ylabel("Recall")
    plt.xlabel("Kmer Size")
    plt.title("Recall")

    # Plot the F1
    plt.subplot(3, 1, 3)
    plt.plot(kmer_sizes, [values["F1"] for values in results.values()])
    plt.ylabel("F1")
    plt.xlabel("Kmer Size")
    plt.title("F1")

    plt.savefig("finished_experiments_effectiveness_measures_plot.png")

    return
'''


def read_type_line(line):
    # Read and sum the TP, FP, FN, TN values
    values = line.split()
    tp = int(values[0])
    fp = int(values[1])
    fn = int(values[2])
    tn = int(values[3])
    precision, recall, f1 = prec_rec_f1(tp, fp, fn, tn)
    typex = {precision, recall, f1}

    return typex

def compute_effectiveness_measures_for_types(checkpoints_filename):
    """Compute the effectiveness measures from the checkpoints file."""

    # Read the checkpoints file
    with open(checkpoints_filename, 'r') as file:
        text = file.read()
    
    lines = text.strip().split("\n")
    results = {}
    current_file = None
    type1, type2, type3, type4 = {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}

    for i, line in enumerate(lines):
        if line.startswith("File name:"):
            # If not the first file, store the previous file's results
            if current_file is not None:
                results[current_file[4:-6]] = {"Type1": type1, "Type2": type2, "Type3": type3, "Type4": type4}
            
            current_file = line.split(":")[1].strip()
            type1, type2, type3, type4 = {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}
        
        elif line.startswith("Type 1"):
            next_line = lines[i+1]
            type1 = read_type_line(next_line)
        elif line.startswith("Type 2"):
            next_line = lines[i+1]            
            type2 = read_type_line(next_line)
        elif line.startswith("Type 3"):
            next_line = lines[i+1]
            type3 = read_type_line(next_line)
        elif line.startswith("Type 4"):
            next_line = lines[i+1]
            type4 = read_type_line(next_line)

    # Store the last file's results
    if current_file is not None:
        results[current_file[4:-6]] = {"Type1": type1, "Type2": type2, "Type3": type3, "Type4": type4}

    sort_the_effectiveness_measures(results)

    return results

def write_effectiveness_measures_for_types(results):
    """Write the effectiveness measures to a file."""
    with open("effectiveness_measures_for_types.txt", "w+") as file:
        for filename, values in results.items():
            file.write("kmer size: {}\n".format(filename))
            file.write("Type 1: {}\n".format(values["Type1"]))
            file.write("Type 2: {}\n".format(values["Type2"]))
            file.write("Type 3: {}\n".format(values["Type3"]))
            file.write("Type 4: {}\n".format(values["Type4"]))
            file.write("\n")

def get_highest_f1_for_types(results):
    """Return the file name with the highest F1 score."""
    highest_f1 = -1
    highest_f1_file = None

    for filename, values in results.items():
        if values["F1"] > highest_f1:
            highest_f1 = values["F1"]
            highest_f1_file = filename

    return highest_f1_file

if __name__ == "__main__":
    numbers = read_numbers("code_clone_counts.txt")
    plot_histogram(numbers)
    #plot_histogram_w_freq_2(numbers)
    plot_log_histogram(numbers)

    #analyse_code_clone_numbers(numbers)
        
    # Get the directory path
    #directory = "/home/eilgun/bio2se/experiment/"

    # Iterate through all files in the directory
    #for filename in os.listdir(directory):
    #    if filename.startswith("kmer") and filename.endswith(".txt"):
    #        file_path = os.path.join(directory, filename)
    #        print("File name: {}".format(filename))
    #        print_checkpoint(read_checkpoint(file_path))
    #        print()

    #filename = "checkpoint.txt"
    #file_path = os.path.join(directory, filename)
    #result = compute_effectiveness_measures(filename)
    #write_effectiveness_measures(result)
    #plot_effectiveness_measures(result)

    #result_types = compute_effectiveness_measures_for_types(filename)
    #write_effectiveness_measures_for_types(result_types)

    #print("Highest F1: {}".format(get_highest_f1(result)))
    #print("Highest Precision: {}".format(get_highest_precision(result)))
    #print("Highest Recall: {}".format(get_highest_recall(result)))