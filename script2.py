import glob, os, re
import collections
import math

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


os.chdir("./Reuters")
offset = 0
list1d = []
list2d =[]
list2 = []
dict={}
for file in sorted(glob.glob("*.sgm")):
    offset=0
    #print(file)
    list1d=[]
    with open(file) as fp:
        for line in fp:
            obj = offsets()
            m=re.search('<REUTERS TOPICS="YES"(.*)NEWID="(.*)"',line)
            if m:
                obj.reuters = offset
                #print(obj.reuters)
                offset = offset + len(line)
                num=0
                for line in fp:
                    #print(line)
                    #break
                    m=re.search('<TOPICS><D>(.*)</D><D>',line)
                    if m:
                        break
                    else:
                        m=re.search('<TOPICS><D>(.+)</D></TOPICS>',line)
                        if m:
                            if(m.group(1)!=''):
                                obj.topics =offset
                                if(dict.has_key(m.group(1))==0):
                                    dict.update({m.group(1):1})
                                else:
                                    dict.update({m.group(1):(dict.get(m.group(1))+1)})
                            else:
                                break
                    if num>0:
                        num=num+1
                    m=re.search('<BODY>',line)
                    if m:
                        if(obj.topics==0):
                            break
                        obj.start=offset
                        num=num+1
                    m=re.search('</BODY>',line)
                    if m:
                        obj.end =num
                        list1d.append(obj)
                        break
                    offset = offset + len(line)
                #found = m.group(2)
            offset = offset + len(line)
        #break
        list2d.append(list1d)
        #break

for file in sorted(glob.glob("*.sgm")):
    #print(file)
    with open(file) as fp:
        for x in list2d:
            for y in x:
                #print(y.reuters)
                """fp.seek(y.reuters,os.SEEK_SET)
                print(fp.readline())
                #print(y.topics)
                fp.seek(y.topics,os.SEEK_SET)
                print(fp.readline())
                #print(y.start)
                fp.seek(y.start,os.SEEK_SET)
                print(fp.readline())
                #print(y.en
                fp.seek(y.end,os.SEEK_SET)
                print(fp.readline())"""
        #break

    #<TOPICS><D>cocoa</D></TOPICS>
    #<TOPICS><D>grain</D><D>wheat</D><D>corn</D><D>barley</D><D>oat</D><D>sorghum</D></TOPICS>
    #<BODY>
    #</BODY>
i=0
length = len(dict)
freq=[]
for w in sorted(dict, key=dict.get):
  #i=i+dict[w]
  i=i+1
  if(i>length-20):
      freq.append(w)
  print w, dict[w]
print(freq)
x=0
for file in sorted(glob.glob("*.sgm")):
    with open(file) as fp:
        #print(len(list2d[x]))
        #kk=0
        for y in list2d[x]:
            fp.seek(y.topics,os.SEEK_SET)
            line = fp.readline()

            m=re.search('<TOPICS><D>(.+)</D></TOPICS>',line)
            if m:
                #print(m.group(1))
                if m.group(1) in freq:
                    obj = article()
                    obj.topic = m.group(1)

                    fp.seek(y.reuters,os.SEEK_SET)
                    line = fp.readline()
                    m=re.search('<REUTERS TOPICS="YES"(.*)NEWID="(.*)"',line)
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
                    #kk=kk+1
        #print(kk)
        x=x+1
        #print(x)
        #break
        if x==len(list2d):
            break
oo=0
for x in list2:
    #print(oo)
    #oo=oo+1
    #print(x.newId)
    #print(x.topic)
    #print(x.body)
    #break
    x.body = ''.join([i if ord(i) < 128 else '' for i in x.body])
    #print(x.body)
    x.body = (x.body).lower()
    x.body = re.sub('[^0-9a-zA-Z]+',' ',x.body)
    x.body = x.body.split(' ')
    #print(x.body)
    x.body=[g for g in x.body if g.isdigit()==0]
    x.body = [g for g in x.body if g not in stop]
    x.body = [ps.stem(f) for f in x.body]
    #print(x.body)
    #break
dict={}
for x in list2:
    for y in x.body:
        if(dict.has_key(y)==0):
            dict.update({y:1})
        else:
            dict.update({y:(dict.get(y)+1)})
    #break
'''with open("op.txt","w") as fp:
    for w in sorted(dict, key=dict.get):
        #print w, dict[w]
        fp.write(w+" ")
        fp.write(str(dict[w]))
        fp.write(os.linesep)'''

for x in list2:
    x.body = [g for g in x.body if(dict.get(g)>=5)]
    if not x.body:
        print(x.newId)
    #print(x.body)
#    break
dict1 = collections.OrderedDict()
for w in dict:
    if(dict[w]>=5):
        dict1.update({w:dict[w]})
#print(dict)
#print(dict1)
'''fp1=open("f1.txt","w")
fp2=open("f2.txt","w")
fp3=open("f3.txt","w")
u=0
for x in list2:
    u=u+1
    print(u)
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
        norm2=norm2+(1+math.sqrt(h))
        norm3=norm3+(1+math.log(h,2))
    norm1=norm1**(0.5)
    norm2=norm2**(0.5)
    norm3=norm3**(0.5)
    for w in dict2:
        h=dict2[w]
        ind = dict1.keys().index(w)
        fp1.write(x.newId+" ")
        fp1.write(str(ind)+" ")
        #fp1.write(w+" ")
        fp1.write(str(h/float(norm1))+os.linesep)
        fp2.write(x.newId+" ")
        fp2.write(str(ind)+" ")
        fp2.write(str((1+math.sqrt(h))/float(norm2))+os.linesep)
        fp3.write(x.newId+" ")
        fp3.write(str(ind)+" ")
        fp3.write(str((1+math.log(h,2))/float(norm3))+os.linesep)
    #break
fp1.close()
fp2.close()
fp3.close()'''
