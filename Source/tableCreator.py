import matplotlib.pyplot as plt
import matplotlib
import os
import shutil

def makeTable(sourceFile,outputPath,outputName):
    f=open(sourceFile,"r")

    col=f.readline().strip()[:-1].split(";")
    col.pop(0)
    col.append("Speedup")

    row=[]
    cell=[]

    lines=f.readlines()

    seqTime=0
    parallelTime=0

    for line in lines:
        elem=line.strip()[:-1].split(";")
        modality=elem.pop(0)
        row.append(modality)
        nuovo=[]
        nuovo.append(elem.pop(0))
        nuovo.append(elem.pop(0))
        for val in elem:
            nuovo.append(str(format(float(val), ".7f")))
        if modality=="Sequential":
            seqTime=float(elem[2])+float(elem[3])
            nuovo.append("1")
        else:
            parallelTime=float(elem[2])+float(elem[3])
            nuovo.append(str(format((seqTime/parallelTime),".7f")))
        cell.append(nuovo)
    
    fig, ax = plt.subplots() 
    ax.set_axis_off() 
    table = ax.table(
        cellText = cell,  
        rowLabels = row,  
        colLabels = col, 
        rowColours =["lightblue"] * len(row),  
        colColours =["lightblue"] * len(col), 
        cellLoc ='center',  
        loc ='upper center',
        rowLoc="center",
        colLoc="center")

    table.scale(0.8,0.5)

    
    name=outputName.split("_")
    title=name[0]+" nodes & "+name[1]
    ax.set_title(title+" edges", fontweight ="bold") 
    
    plt.savefig(os.path.join(outputPath, outputName)+".png",dpi=300, bbox_inches='tight')
    plt.close()
    f.close()

if os.path.exists("Tables"):#if Tables directory exist, delete it and all its elements
    shutil.rmtree('Tables')

os.mkdir("Tables")#create the tables directory

for path, currentDirectory, files in os.walk("Results"):
    for file in files:
        resultPath=path.replace("Results","Tables")
        if not(os.path.exists(resultPath)):
            os.makedirs(resultPath)
        makeTable(os.path.join(path, file),resultPath,file.replace(".csv",""))