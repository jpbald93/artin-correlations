import re,requests,json,time,pathlib
root=pathlib.Path(__file__).resolve().parents[2]
s=(root/'artin_correlations.tex').read_text()
items=re.split(r'\\bibitem\{([^}]+)\}',s)[1:]
out=[]
for key,text in zip(items[::2],items[1::2]):
 title=re.search(r'\\emph\{([\s\S]*?)\}',text).group(1)
 query=title.replace('\\','').replace('$','').replace('{','').replace('}','')
 try:
  r=requests.get('https://api.crossref.org/works',params={'query.bibliographic':query,'rows':2},timeout=40)
  data=r.json()['message']['items']
  result={'key':key,'query':query,'matches':[{k:x.get(k) for k in ['title','author','DOI','published','volume','issue','page','article-number','container-title','URL']} for x in data]}
 except Exception as e:result={'key':key,'error':str(e)}
 out.append(result);print(json.dumps(result),flush=True)
 (root/'reports/astra_evidence/citations.json').write_text(json.dumps(out,indent=2))
 time.sleep(.15)
