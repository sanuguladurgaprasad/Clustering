// kcluster.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
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

#define TOPICS 8095
using namespace std;

//Reading csv files and class files and storing them in local DS
//Perform 3 types of clustrings
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

int K;
vector<int>* cIndicator;
unordered_map<int, string> label;
article docVec[TOPICS];
ofstream myOutput;

bool myfunction(cell a, cell b) {
	return (a.dim < b.dim);
}

void print(article A){
	for (int j = 0; j < A.dV.size(); j++) {
		cout << A.dV[j].dim << " " << A.dV[j].val << "\n";
	}cout<<";;;;;;;\n";
}

double similarity(article A, article B) {
	int s1 = A.dV.size();
	int s2 = B.dV.size();
	double m1, m2, simNum=0, simDenomA=0, simDenomB=0;
	int i = 0, j = 0;

	/*for (int j = 0; j < A.dV.size(); j++) {
		cout << A.dV[j].dim << " " << A.dV[j].val << "\n";
	}cout<<";;;;;;;\n";
	for (int j = 0; j < B.dV.size(); j++) {
		cout << B.dV[j].dim << " " << B.dV[j].val << "\n";
	}*/
	while (i < s1 || j<s2) {
		//cout << i << " "<<j<<endl;

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
	//cout<<"s="<<simNum<<"simDenomA= "<<simDenomA<<"simDenomB "<<simDenomB<<" "<<endl;
	//sleep(10);
	return (double)simNum/(sqrt(simDenomA)*sqrt(simDenomB));
}

double dist(article A, article B) {
	int s1 = A.dV.size();
	int s2 = B.dV.size();
	int i = 0, j = 0;
	double d = 0;
	/*for (int j = 0; j < A.dV.size(); j++) {
		cout << A.dV[j].dim << " " << A.dV[j].val << "\n";
	}cout<<";;;;;;;\n";
	for (int j = 0; j < B.dV.size(); j++) {
		cout << B.dV[j].dim << " " << B.dV[j].val << "\n";
	}*/
	//cout << s1 << " " << s2 << endl;
	while (i < s1 || j<s2) {
		//cout << i << " "<<j<<endl;
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
	//cout<<"d="<<d<<endl;
	//sleep(10);
	return d;
}

bool converge(article *C1, article *C2) {
	if (C2 == NULL)return false;
	//double max = 0;
	double error=0;
	for (int i = 0; i < K; i++) {
		double d = dist(C1[i], C2[i]);
		error+=d*d;
/*		print(C1[i]);
		print(C2[i]);
		cout<<"max="<<max<<" d= "<<d<<endl;
	*/	//max = max > d ? max : d;
	}
	myOutput << "max = " << error << endl;
	cout << "max = " << error << endl;

	if (error < 0.00001)return true;
	return false;
}

//Entropy and Purity
void validity(){
	unordered_map<string, int> *mIJ = new unordered_map<string, int>[K];
	for (int i = 0; i < K; i++) {
		for (int k = 0; k < cIndicator[i].size(); k++) {
			string s = label[docVec[cIndicator[i][k]].ID];
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
	myOutput << "Entropy= " << entropy << " Purity= " << purity << endl;
	cout << "Entropy= " << entropy << " Purity= " << purity << endl;
	//delete mIJ;
}

int main()
{
	string ipFile, clsFile = "reuters21578.class", opFile,criteriaFn;
	//cin >> ipFile >> opFile>>criteriaFn;
	ipFile = "log2freq.csv";
	opFile = "op.txt";
	cin>>criteriaFn;
	ifstream myInput(ipFile), classFile(clsFile);
	myOutput.open(opFile);
	K = 60;
	int trials[] = { 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39 };

	//temp values
	string prevID = ";;", ID, dim, val, topic;
	article *doc;
	cell temp;

	int ind = -1;
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
	/*for (int i = 0; i < TOPICS; i++) {
	myOutput << docVec[i].ID << " ";
	for (int j = 0; j < docVec[i].dV.size(); j++) {
	myOutput << docVec[i].dV[j].dim << " " << docVec[i].dV[j].val << ";;";
	}myOutput << endl;
	}*/
	myInput.close();
	myInput.open(clsFile);
	ind = 0;
	while (!myInput.eof()) {
		getline(myInput, ID, ',');
		getline(myInput, topic, '\n');
		if (ID == "")break;
		label.insert(make_pair(stoi(ID), topic));
	}
	myInput.close();
	for (int i = 0; i < TOPICS; i++) {
		sort((docVec[i].dV).begin(), (docVec[i].dV).end(), myfunction);
	}
	//for(int g=0;g<2;g++){
	for (int i = 0; i < 20; i++) {
		myOutput << "\ntrials= " << i << endl;
		cout << "trials= " << i << endl;
		srand(trials[i]);
		article *clusters = new article[K];
		article *clustersOld = new article[K];
		cIndicator = new vector<int>[K];
		for (int j = 0; j < K; j++) {
			clusters[j] = docVec[rand() % TOPICS];
		}
		int yy = 0;
		while (1) {
			myOutput << "itr= " << yy<<endl;
			cout << "itr= " << yy<<endl;
yy++;
			//Assigning to closest entroids
			for (int k = 0; k < K; k++)
				cIndicator[k].clear();

			for (int j = 0; j < TOPICS; j++) {
	//			cout << j << endl;
				double minDist = DBL_MAX;
				double maxSim = DBL_MIN;
				int cI;
				for (int k = 0; k < K; k++) {
					if (criteriaFn == "sse") {
						double d = dist(docVec[j], clusters[k]);
						//double d = dist(docVec[1], docVec[1]);
						if (minDist > d) {
							minDist = d;
							cI = k;
						}
					}
					if (criteriaFn == "sph") {
						double sim = similarity(docVec[j], clusters[k]);
						//double sim = similarity(docVec[1], docVec[1]);
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

			//converge(clusters, clustersOld);
			//sleep(10);
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
			validity();
			if (converge(clusters, clustersOld))break;
		}
	}
	//criteriaFn="ssh";
//}
	//validity();
	//delete cIndicator;
	//delete mIJ;
	return 0;
}
