# Clustering
Document Clustering using K-means, Spehrical K-means and E1 criterion function

Steps to run:

a)preprocess generates "reuters21578.class", "reuters21578.clabel" and three input files, named as "freq.csv", "sqrtfreq.csv", "log2freq.csv".

b)input-file: one of the above csv files,

class-file: reuters21578.class

#clusters: number of clusters

#trials: trials with different seeds. max values an be 20.

1) make preprocess
2) make kcluster
3) ./kcluster input-file criterion-function class-file #clusters #trials output-file
