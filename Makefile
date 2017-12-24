preprocess : script.py
	python script.py

kcluster : kcluster.cpp
	g++ -std=c++11 -o kcluster kcluster.cpp
