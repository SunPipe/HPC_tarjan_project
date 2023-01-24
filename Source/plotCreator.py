import matplotlib.pyplot as plt
import numpy as np
import os
import shutil

def makeTable(sourceFile,outputPath,outputName):
    f=open(sourceFile,"r")

    col=f.readline().strip()[:-1].split(";")
    col.pop(0)

    lines=f.readlines()

    sequential=0
    mpiOnly=dict()
    hybrid=dict()
    
    for line in lines:
        elem=line.strip()[:-1].split(";")
        elem.pop(0)
        omp=int(elem.pop(0))
        mpi=int(elem.pop(0))

        if(omp==0 and mpi==0):
            sequential=float(elem[2])+float(elem[3])
        elif(omp==0):
            mpiOnly[mpi]=sequential/(float(elem[2])+float(elem[3]))
        else:
            if mpi not in hybrid:
                hybrid[mpi]={}
            val1=float(elem[2])
            val2=float(elem[3])
            tot=val1+val2
            hybrid[mpi][omp]=sequential/tot
        
    #mpi
    x=[]
    y=[]
    xIdeal=[1,2,3,4,5,6,7,8,9]
    mpiList=list(mpiOnly.keys())
    mpiList.sort()

    for mpi in mpiList:
        x.append(mpi)
        y.append(mpiOnly[mpi])

    xpoints = np.array(x)
    ypoints = np.array(y)

    fig, ax = plt.subplots(3,2,constrained_layout = True)
    fig.tight_layout()
    
    ax[2][0].set_xlabel('Processes')
    ax[1][1].set_xlabel('Processes')
    ax[0][0].set_ylabel('Speedup')
    ax[1][0].set_ylabel('Speedup')
    ax[2][0].set_ylabel('Speedup')


    ax[0][0].plot(xpoints,ypoints,marker="o", label="real",  linewidth='1', ms=3)
    ax[0][0].plot(xIdeal,xIdeal,marker="o", label="ideal",  linewidth='1', ms=3)
    # plt.xlabel("Processes")
    # plt.ylabel("Speedup")
    ax[0][0].grid()
    ax[0][0].tick_params()
    ax[0][0].legend()
    ax[0][0].set_title("MPIOnly",fontsize=10)
    # plt.savefig(os.path.join(outputPath, outputName)+"_MPIOnly.png",dpi=300, bbox_inches='tight')
    # plt.close()

    #hybrid
    mpiList=list(hybrid.keys())
    mpiList.sort()
    k=0
    j=1
    for mpi in mpiList:
        ompList=list(hybrid[mpi].keys())
        ompList.sort()
        x=[]
        y=[]
        xIdeal=[]
        
        maxOMP=0
        for omp in ompList:
            if omp> maxOMP:
                maxOMP=omp
            x.append(omp*mpi)
            y.append(hybrid[mpi][omp])
        
        for i in range(1,maxOMP+1):
            xIdeal.append(i*mpi)

        xpoints = np.array(x)
        ypoints = np.array(y)

        ax[k][j].plot(xpoints,ypoints,marker="o", label="real",  linewidth='1', ms=3)
        ax[k][j].plot(xIdeal,xIdeal,marker="o", label="ideal",  linewidth='1', ms=3)
        # plt.xlabel("Processes")
        # plt.ylabel("Speedup")
        ax[k][j].grid()
        ax[k][j].tick_params()
        ax[k][j].legend()
        ax[k][j].set_title(str(mpi)+"MPI+OMP",fontsize=10)
        if j==1:
            k+=1
            j=0
        else:
            j+=1
    
    ax[2][1].axis("off")
    plt.savefig(os.path.join(outputPath, outputName)+".png", bbox_inches='tight')
    plt.close()
    f.close()

if os.path.exists("Plots"):#if Plots directory exist, delete it and all its elements
    shutil.rmtree('Plots')

os.mkdir("Plots")#create the plots directory

for path, currentDirectory, files in os.walk("Results"):
    for file in files:
        resultPath=path.replace("Results","Plots")
        if not(os.path.exists(resultPath)):
            os.makedirs(resultPath)
        makeTable(os.path.join(path, file),resultPath,file.replace(".csv",""))