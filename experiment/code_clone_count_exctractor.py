import multiprocessing

def process_lines(chunk):
    print("Processing {} lines".format(len(chunk)))
    """Extract numbers from a chunk of lines."""
    numbers = []
    for line in chunk:
        if line.startswith("Number of clones:"):
            number = int(line.split(':')[1].strip())
            numbers.append(number)
    return numbers

def chunkify(lst, n):
    """Split the list into n chunks."""
    return [lst[i::n] for i in range(n)]

def main(input_filename, output_filename):
    with open(input_filename, 'r') as file:
        lines = file.readlines()

    print("Number of lines: {}".format(len(lines)))

    # Number of processes
    num_processes = multiprocessing.cpu_count()
    print("Number of processes: {}".format(num_processes))

    # Split lines into chunks for each process
    chunks = chunkify(lines, num_processes)

    # Create a pool of processes
    pool = multiprocessing.Pool(processes=num_processes)

    # Map the process_lines function to each chunk
    results = pool.map(process_lines, chunks)

    # Close the pool and wait for the work to finish
    pool.close()
    pool.join()

    # Flatten the list of results
    all_numbers = [num for sublist in results for num in sublist]

    # Write the results to a file
    with open(output_filename, 'w+') as output_file:
        for number in all_numbers:
            output_file.write(str(number) + '\n')

if __name__ == "__main__":
    main("/home/eilgun/bio2se/experiment/kmer30v2.txt", "/home/eilgun/bio2se/experiment/code_clone_counts.txt")
