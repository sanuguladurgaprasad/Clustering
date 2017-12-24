// kcluster.cpp : Defines the entry point for the console application.

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <climits>
#include <cfloat>
#include <unordered_map>
#include <iterator>
#include <cmath>
#include <unistd.h>

using namespace std;

//Reading csv files and class files and storing them in local DS
//Perform 3 types of clusterings
//Computing Entropy and Purity

struct cell {
	int dim;
	double val;
};

class article {
public:
	int ID;
	vector<cell> dV;
};

class cls {
public:
	int ID;
	string topic;
};

int TOPICS;
int K,bestTrialIndex;
vector<int>* cIndicator;
unordered_map<int, string> label;
unordered_map<int, vector<int>> centroid_article_map;
double bestCFVal;
article *docVec;
ofstream myOutput;
string criteriaFn;
article globalC;

bool myfunction(cell a, cell b) {
	return (a.dim < b.dim);
}

double similarity(article A, article B) {
	int s1 = A.dV.size();
	int s2 = B.dV.size();
	double m1, m2, simNum=0, simDenomA=0, simDenomB=0;
	int i = 0, j = 0;

	while (i < s1 || j<s2) {
		if (i < s1 && j<s2) {
			if (A.dV[i].dim == B.dV[j].dim) {
				simNum += (double)A.dV[i].val*B.dV[j].val;
				simDenomA += (double)A.dV[i].val*A.dV[i].val;
				simDenomB += (double)B.dV[j].val*B.dV[j].val;
				i++; j++;
			}
			else if (A.dV[i].dim < B.dV[j].dim) {
				simDenomA += (double)A.dV[i].val*A.dV[i].val;
				i++;
			}
			else if (A.dV[i].dim > B.dV[j].dim) {
				simDenomB += (double)B.dV[j].val*B.dV[j].val;
				j++;
			}
		}
		if (i < s1 && j >= s2) {
			simDenomA += (double)A.dV[i].val*A.dV[i].val;
			i++;
		}
		if (i >= s1 && j < s2) {
			simDenomB += (double)B.dV[j].val*B.dV[j].val;
			j++;
		}
		if (i >= s1 && j >= s2)break;
	}
	return (double)simNum/(sqrt(simDenomA)*sqrt(simDenomB));
}

double dist(article A, article B) {
	int s1 = A.dV.size();
	int s2 = B.dV.size();
	int i = 0, j = 0;
	double d = 0;
	while (i < s1 || j<s2) {
		if (i < s1 && j<s2) {
			if (A.dV[i].dim == B.dV[j].dim) {
				d += (double)(A.dV[i].val - B.dV[j].val)*(A.dV[i].val - B.dV[j].val);
				i++; j++;
			}
			else if (A.dV[i].dim < B.dV[j].dim) {
				d += (double)(A.dV[i].val)*(A.dV[i].val);
				i++;
			}
			else if (A.dV[i].dim > B.dV[j].dim) {
				d += (double)(B.dV[j].val)*(B.dV[j].val);
				j++;
			}
		}
		if (i < s1 && j >= s2) {
			d += (double)(A.dV[i].val)*(A.dV[i].val);
			i++;
		}
		if (i >= s1 && j < s2) {
			d += (double)(B.dV[j].val)*(B.dV[j].val);
			j++;
		}
		if (i >= s1 && j >= s2)break;
	}
	return d;
}

bool converge(article *C1, article *C2) {
	if (C2 == NULL)return false;
	double error=0;
	for (int i = 0; i < K; i++) {
		double d = dist(C1[i], C2[i]);
		error+=d*d;
	}
	if(criteriaFn=="E1" && error<0.00001)return true;
	if (criteriaFn!="E1" && error < 0.0001)return true;
	return false;
}

//Entropy and Purity
void validity(){
	ofstream myOutput2("ClusterSolution.csv");
	unordered_map<string, int> *mIJ = new unordered_map<string, int>[K];
	unordered_map<string, int> indices;
	for(int i=0;i<K;i++){
		cIndicator[i] = centroid_article_map.at(i);
	}
	int numClasses=0;
	for (int i = 0; i < K; i++) {
		for (int k = 0; k < cIndicator[i].size(); k++) {
			string s = label[docVec[cIndicator[i][k]].ID];
			if (indices.find(s) == indices.end()) {
				indices.insert(make_pair(s, numClasses));
				numClasses++;
			}
			if (mIJ[i].find(s) == mIJ[i].end()) {
				mIJ[i].insert(make_pair(s, 1));
			}
			else {
				mIJ[i][s] += 1;
			}
		}
	}
	double entropy = 0;
	double purity = 0;
	int **matrix = new int*[K];
	for(int i=0;i<K;i++){
		matrix[i] = new int[numClasses]();
	}
	unordered_map<string, int>::iterator itr1;
	for(int i=0;i<K;i++){
		for (unordered_map<string, int>::iterator itr = mIJ[i].begin(); itr != mIJ[i].end(); itr++) {
			itr1=indices.find(itr->first);
			if(itr1!=indices.end()){
				matrix[i][itr1->second] = itr->second;
			}
		}
	}

	for (int i = 0; i < K; i++) {
		double eij = 0;
		double maxpIJ = DBL_MIN;
		for (unordered_map<string, int>::iterator itr = mIJ[i].begin(); itr != mIJ[i].end(); itr++) {
			double pij = (itr->second) / (double)cIndicator[i].size();
			eij += pij*log2(pij);
			maxpIJ = (maxpIJ < pij) ? pij : maxpIJ;
		}
		entropy += ((double)cIndicator[i].size() / TOPICS)*(-eij);
		purity += ((double)cIndicator[i].size() / TOPICS)*maxpIJ;
	}
	//printing
	//printing
	//myOutput<<";;;;;;;;;;;;;;;;;;;;; CLUSTERING SOLUTION FOR BEST TRIAL;;;;;;;;;;;;;;;;;;;;;;;;\n\n ";
	myOutput2<<",";
	for (int j=0;j<numClasses;j++){
		for(unordered_map<string, int>::iterator itr = indices.begin(); itr != indices.end(); itr++) {
			if(itr->second==j){
				myOutput2<<itr->first<<",";
				break;
			}
		}
	}
	for(int i=0;i<K;i++){
		myOutput2<<"\nK="<<i<<",";
		for (int j=0;j<numClasses;j++) {
			myOutput2<<matrix[i][j]<<",";
		}
	}

	myOutput2 << "\nENTROPY= " << entropy << " PURITY= " << purity << endl;
	cout << "\nENTROPY= " << entropy << " PURITY= " << purity << endl;
	myOutput2.close();
}

void calculate_criterion_val(int trialIndex,article *C1){
	double proximity=0;
	if(criteriaFn!="E1"){
		for (int i = 0; i < K; i++) {
			for (int j = 0; j < cIndicator[i].size(); j++) {
				if(criteriaFn=="SSE")proximity+=dist(docVec[cIndicator[i][j]],C1[i]);
				else if(criteriaFn=="I2")proximity+=similarity(docVec[cIndicator[i][j]],C1[i]);
			}
		}
	}
	else{
		for (int i= 0; i < K; i++) {
				proximity+=(double)similarity(C1[i],globalC)*cIndicator[i].size();
		}
	}
	//cout<<bestCFVal<<" "<<proximity<<endl;
	if(criteriaFn=="SSE" || criteriaFn=="E1"){
		if(bestCFVal>proximity){
			for(int i=0;i<K;i++){
				centroid_article_map[i]=cIndicator[i];
			}
			bestCFVal=proximity;
			bestTrialIndex=trialIndex;
		}
	}
	else if(criteriaFn=="I2"){
		if(bestCFVal<proximity){
			for(int i=0;i<K;i++){
				centroid_article_map[i]=cIndicator[i];
			}
			bestCFVal=proximity;
			bestTrialIndex=trialIndex;
		}
	}
}

int main(int argc, char *argv[])
{
	//int start_s,stop_s;
	string ipFile=argv[1], clsFile = argv[3], opFile=argv[6];
	criteriaFn = argv[2];
	bestCFVal = (criteriaFn=="I2")?DBL_MIN:DBL_MAX;
	K = atoi(argv[4]);
	int T = atoi(argv[5]);
	int trials[] = { 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39 };


	//temp values
	string prevID = ";;", ID, dim, val, topic;
	article *doc;
	cell temp;

	ifstream myInput, classFile(clsFile);
	myOutput.open(opFile);

	myInput.open(clsFile);
	int ind = 0;
	while (!myInput.eof()) {
		getline(myInput, ID, ',');
		getline(myInput, topic, '\n');
		if (ID == "")break;
		label.insert(make_pair(stoi(ID), topic));
		ind++;
	}
	myInput.close();
	TOPICS = ind;
	docVec = new article[TOPICS];

	myInput.open(ipFile);
	ind = -1;
	while (!myInput.eof()) {
		getline(myInput, ID, ',');
		getline(myInput, dim, ',');
		getline(myInput, val, '\n');
		if (ID == "")break;
		if (prevID != ID || prevID == ";;") {
			doc = new article;
			doc->ID = stoi(ID);
			ind++;
		}
		temp.dim = stoi(dim);
		temp.val = stof(val);
		(doc->dV).push_back(temp);
		docVec[ind] = *doc;
		prevID = ID;
	}
	delete doc;
	myInput.close();

	for (int i = 0; i < TOPICS; i++) {
		sort((docVec[i].dV).begin(), (docVec[i].dV).end(), myfunction);
	}

	if(criteriaFn=="E1"){
		unordered_map<int, double> centroid;
		for (int k = 0; k < TOPICS; k++) {
			vector<cell> cTemp = docVec[k].dV;
			for (int l = 0; l < cTemp.size(); l++) {
				if (centroid.find(cTemp[l].dim) == centroid.end()) {
					centroid.insert(make_pair(cTemp[l].dim, cTemp[l].val));
				}
				else {
					centroid[cTemp[l].dim] += cTemp[l].val;
				}
			}
		}
		if (centroid.size() > 0) {
			globalC.dV.clear();
			cell cTemp;
			for (unordered_map<int, double>::iterator itr = centroid.begin(); itr != centroid.end(); itr++) {
				cTemp.dim = itr->first;
				cTemp.val = (double)itr->second / TOPICS;
				globalC.dV.push_back(cTemp);
			}
		}
		sort((globalC.dV).begin(), (globalC.dV).end(), myfunction);
	}

	//start_s = clock();
	for (int i = 0; i < T; i++) {
		srand(trials[i]);
		article *clusters = new article[K];
		article *clustersOld = new article[K];
		cIndicator = new vector<int>[K];
		for (int j = 0; j < K; j++) {
			clusters[j] = docVec[rand() % TOPICS];
		}
		while (1) {
			//Assigning to closest entroids
			for (int k = 0; k < K; k++)
				cIndicator[k].clear();

			for (int j = 0; j < TOPICS; j++) {
				double minDist = DBL_MAX;
				double maxSim = DBL_MIN;
				int cI;
				for (int k = 0; k < K; k++) {
					if (criteriaFn == "SSE") {
						double d = sqrt(dist(docVec[j], clusters[k]));
						if (minDist > d) {
							minDist = d;
							cI = k;
						}
					}
					if (criteriaFn == "I2") {
						double sim = similarity(docVec[j], clusters[k]);
						if (maxSim < sim) {
							maxSim = sim;
							cI = k;
						}
					}
					if (criteriaFn == "E1") {
						double sim = similarity(docVec[j], clusters[k]);
						if (maxSim < sim) {
							maxSim = sim;
							cI = k;
						}
					}
				}
				cIndicator[cI].push_back(j);
			}
			for (int j = 0; j < K; j++) {
				clustersOld[j] = clusters[j];
			}
			//Recomputing centroids
			for (int j = 0; j < K; j++) {
				unordered_map<int, double> centroid;
				for (int k = 0; k < cIndicator[j].size(); k++) {
					vector<cell> cTemp = docVec[cIndicator[j][k]].dV;
					for (int l = 0; l < cTemp.size(); l++) {
						if (centroid.find(cTemp[l].dim) == centroid.end()) {
							centroid.insert(make_pair(cTemp[l].dim, cTemp[l].val));
						}
						else {
							centroid[cTemp[l].dim] += cTemp[l].val;
						}
					}
				}
				if (centroid.size() > 0) {
					clusters[j].dV.clear();
					cell cTemp;
					for (unordered_map<int, double>::iterator itr = centroid.begin(); itr != centroid.end(); itr++) {
						cTemp.dim = itr->first;
						cTemp.val = (double)itr->second / cIndicator[j].size();
						clusters[j].dV.push_back(cTemp);
					}
				}
				sort((clusters[j].dV).begin(), (clusters[j].dV).end(), myfunction);
				centroid.clear();
			}
			if (converge(clusters, clustersOld)){
				calculate_criterion_val(i,clusters);
				break;
			}
		}
	}
	validity();

	cout<<"BEST CRITERION FUNCTION VALUE = "<< bestCFVal<<endl;
	//cout<<"BEST TRIAL SEED= "<< trials[bestTrialIndex]<<endl;

	for(int i=0;i<K;i++){
		for(int j=0;j<cIndicator[i].size();j++){
			myOutput<<cIndicator[i][j]<<","<<i<<endl;
		}
	}
	//stop_s = clock();
	//cout << "Time taken for "<<T<<" Trials = " << (stop_s - start_s) / double(CLOCKS_PER_SEC)<< " seconds"<<endl;

	return 0;
}
