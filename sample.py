import glob,os,re
from nltk.stem import PorterStemmer
ps=PorterStemmer()
#print(stopList)
x='jd*****23*23'
x = re.sub('[^0-9a-zA-Z]+',' ',x)
x = x.split(' ')
print(x)
for y in x:
    x=[g for g in x if g.isdigit()==0]
print(x)
'''os.chdir("./Reuters")
for file in sorted(glob.glob("*.sgm")):
    with open(file) as fp:
        for line in fp:
            fp.seek(12345,os.SEEK_SET)
            print(fp.readline())
            break
        break
'''
