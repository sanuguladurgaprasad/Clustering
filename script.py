import glob, os, re
import collections
import math

from operator import itemgetter
from nltk.stem import PorterStemmer
ps=PorterStemmer()
class offsets:
    reuters = 0
    topics = 0
    start = 0
    end = 0

class article:
    newId=0
    topic=""
    body=""

stopList = []
stop=[]
with open("stoplist.txt") as fp:
    stopList = fp.readlines()

stopList = [x.strip() for x in stopList]
stopList= [x.split(" ") for x in stopList]

for x in stopList:
    for y in x:
        stop.append(y)


os.chdir("./reuters21578")
offset = 0
list1d = []
list2d =[]
list2 = []
dict={}
hm=0
for file in sorted(glob.glob("*.sgm")):
    offset=0
    num=0
    v=0
    m1=0#for storing relevant topics
    list1d=[]
    with open(file) as fp:
        obj = offsets()
        for line in fp:
            if num>0:
                num=num+1

            m=re.search('<REUTERS TOPICS=(.*)NEWID="(.*)"',line)
            if m:
                obj.reuters = offset
                offset = offset + len(line)
                v=0
                continue

            m=(re.search('<TOPICS><D>(.*)</D><D>',line) or  re.search('<TOPICS><D></D></TOPICS>',line))
            if m:
                obj=offsets()
                offset = offset + len(line)
                continue
            else:
                m=re.search('<TOPICS><D>(.+)</D></TOPICS>',line)
                if m:
                    obj.topics =offset
                    offset = offset + len(line)
                    m1=m
                    v=1
                    continue

            if v==1:
                m=re.search('<BODY>',line)
                if m:
                    obj.start=offset
                    num=num+1
                    offset = offset + len(line)
                    continue
                m=re.search('</BODY>',line)
                if m:
                    obj.end =num
                    if(dict.has_key(m1.group(1))==0):
                        dict.update({m1.group(1):1})
                    else:
                        dict.update({m1.group(1):(dict.get(m1.group(1))+1)})
                    list1d.append(obj)
                    hm=hm+1
                    obj = offsets()
                    v=0#for getting relevant bodies
                    num=0
            offset = offset + len(line)
        list2d.append(list1d)

i=0
length = len(dict)

freq=[]
for w in sorted(dict, key=dict.get):
  i=i+1
  if(i>length-20):
      freq.append(w)

x=0
for file in sorted(glob.glob("*.sgm")):
    with open(file) as fp:
        for y in list2d[x]:
            fp.seek(y.topics,os.SEEK_SET)
            line = fp.readline()

            m=re.search('<TOPICS><D>(.+)</D></TOPICS>',line)
            if m:
                if m.group(1) in freq:
                    obj = article()
                    obj.topic = m.group(1)

                    fp.seek(y.reuters,os.SEEK_SET)
                    line = fp.readline()
                    m=re.search('<REUTERS TOPICS=(.*)NEWID="(.*)"',line)
                    if m:
                        obj.newId=m.group(2)

                    fp.seek(y.start,os.SEEK_SET)
                    num=0
                    s=''
                    for line in fp:
                        s=s+line
                        num=num+1
                        if num>y.end:
                            break
                    st=s.find('<BODY>')
                    end = s.find('</BODY>')
                    obj.body = s[st+6:end]
                    list2.append(obj)
        x=x+1
        if x==len(list2d):
            break
oo=0
for x in list2:
    x.body = ''.join([i if ord(i) < 128 else '' for i in x.body])
    x.body = (x.body).lower()
    x.body = re.sub('[^0-9a-zA-Z]+',' ',x.body)
    x.body = x.body.split(' ')
    x.body = [g for g in x.body if g!='']
    x.body=[g for g in x.body if g.isdigit()==0]
    x.body = [g for g in x.body if g not in stop]
    x.body = [ps.stem(f) for f in x.body]

dict={}
for x in list2:
    for y in x.body:
        if(dict.has_key(y)==0):
            dict.update({y:1})
        else:
            dict.update({y:(dict.get(y)+1)})

for x in list2:
    x.body = [g for g in x.body if(dict.get(g)>=5)]
    if not x.body:
        print(x.newId)

dict1 = collections.OrderedDict()
for w in dict:
    if(dict[w]>=5):
        dict1.update({w:dict[w]})

fp1=open("../freq.csv","w")
fp2=open("../sqrtfreq.csv","w")
fp3=open("../log2freq.csv","w")
fp4=open("../reuters21578.class","w")
fp5=open("../reuters21578.clabel","w")

for w in dict1:
    fp5.write(w+os.linesep)
u=0
for x in list2:
    fp4.write(x.newId+","+x.topic+os.linesep)
    u=u+1
    dict2=collections.OrderedDict()
    for y in x.body:
        if(dict2.has_key(y)==0):
            dict2.update({y:1})
        else:
            dict2.update({y:(dict2.get(y)+1)})
    norm1=0
    norm2=0
    norm3=0
    for w in dict2:
        h=dict2[w]
        norm1=norm1+h**2
        norm2=norm2+(1+math.sqrt(h))**2
        norm3=norm3+(1+math.log(h,2))**2
    norm1=norm1**(0.5)
    norm2=norm2**(0.5)
    norm3=norm3**(0.5)
    for w in dict2:
        h=dict2[w]
        ind = dict1.keys().index(w)
        fp1.write(x.newId+",")
        fp1.write(str(ind)+",")
        fp1.write(str(h/float(norm1))+os.linesep)
        fp2.write(x.newId+",")
        fp2.write(str(ind)+",")
        fp2.write(str((1+math.sqrt(h))/float(norm2))+os.linesep)
        fp3.write(x.newId+",")
        fp3.write(str(ind)+",")
        fp3.write(str((1+math.log(h,2))/float(norm3))+os.linesep)

fp1.close()
fp2.close()
fp3.close()
fp4.close()
fp5.close()
