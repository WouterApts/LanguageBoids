import re

def parse_data(file_path):
    with open(file_path, 'r') as file:
        string_data = file.read()

    run_number = re.search(r'Run: (\d+)', string_data).group(1)
    distribution = tuple(map(int, re.search(r'Distribution: \((\d+), ?(\d+)\)', string_data).groups()))

    outcome = re.search(r'Outcome: \[([\d,\s-]+)]', string_data).group(1)
    outcome = list(map(int, outcome.split(',')))

    time = re.search(r'Time: \[([\d,\s]+)]', string_data).group(1)
    time = list(map(int, time.split(',')))

    final_distributions = re.findall(r'Final-Distributions: \[([\d,\s]+)]', string_data)
    final_distributions = [tuple(map(int, dist.split(', '))) for dist in final_distributions]

    return {
        'Run Number': int(run_number),
        'Distribution': distribution,
        'Outcome': outcome,
        'Time': time,
        'Final Distributions': final_distributions
    }

file_path = 'data.txt'  # Replace 'data.txt' with the path to your text file
data = parse_data(file_path)
print(data)


