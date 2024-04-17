
for n in [1,32,64,128]:
    for i in range(5):
        print(n,i+1)
        with open(f"./results/results_{n}_{i+1}.txt", "r") as file:
            averages = []
            for k,l in enumerate(file):
                if k in (26,27,28,29):
                    averages.append(l.split()[1])
        file.close()
        with open("./avg.csv", "a") as file:
            file.writelines(",".join(averages))
            file.writelines("\n")