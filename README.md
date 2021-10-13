# UFreS
Algorithms for mining frequent subgraph patterns from Uncertain Graph Databases.

The Datasets Folder contains the datasets used in our empirical analysis, while the implementations of the MUSE, UFreS and UGRAP Algorithms can be found in the respective folders. To run the codes, simply run the "*.cpp" files in each folder, after ensuring that all supporting header files for the same algorithm are in the same folder. Running via any C++ IDE/g++17 compiler will do.

Among the datasets, files with the name format "DatasetName_0.8_0.1.txt" have uncertainty values synthetically injected using a normal distribution having a mean of 0.8 and a standard deviation of 0.1 respectively, file names having the format "DatasetName_0.4_0.05.txt" have uncertainty values injected using a normal distribution with mean 0.4 and standard deviation 0.05. File names having the format "DatasetName_Exp_4.0.txt" have uncertainty values injected using an exponential distribution with lambda=4. Files starting with the prefix "Porto" denote the "Taxi" Dataset represented in the paper.


The attached sheet contains the result of our empirical analysis on each dataset. Accuracy results, as reported on our paper, may somewhat vary due to the stochastic nature of UGRAP and MUSE.

[Experimental Results.xlsx](https://github.com/sogeking124/UFreS/files/6994795/Experimental.Results.xlsx)

