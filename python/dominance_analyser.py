def parse_file(filename):
    data = []
    with open(filename, 'r') as file:
        current_section = {}
        for line in file:
            line = line.strip()
            if line.startswith("Distribution-Number"):
                if current_section:
                    data.append(current_section)
                distribution_number = int(line.split(":")[1].strip().split(",")[0])
                distribution = tuple(map(int, line.split("(")[1].split(")")[0].split(",")))
                current_section = {"Distribution-Number": distribution_number, "Distribution": distribution, "Outcome": [], "Time": []}
            elif line.startswith("Outcome:"):
                outcomes_str = line.split(":")[1].strip()
                outcomes_str = outcomes_str.replace("[", "").replace("]", "")
                outcomes = list(map(int, outcomes_str.split(",")))
                current_section["Outcome"] = outcomes
            elif line.startswith("Time:"):
                times_str = line.split(":")[1].strip()
                times_str = times_str.replace("[", "").replace("]", "")
                times = list(map(float, times_str.split(",")))
                current_section["Time"] = times
            elif line.startswith("Final-Distribution:"):
                final_distribution = tuple(map(int, line.split(":")[1].strip().split("(")[1].split(")")[0].split(",")))
                current_section["Final-Distribution"] = final_distribution
        if current_section:
            data.append(current_section)
    return data

# Example usage
filename = "../cmake-build-debug/output/symmetric_population_500_output.txt"
parsed_data = parse_file(filename)
print(parsed_data)


