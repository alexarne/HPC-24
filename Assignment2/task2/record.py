
averages = []

with open("./results.txt", "r") as file:
    for i,n in enumerate(file):
       if i in (26,27,28,29):
           averages.append(n.split()[1])

with open("./avg.csv", "a") as file:
    file.writelines(",".join(averages))
    file.writelines("\n")