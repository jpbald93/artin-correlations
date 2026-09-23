import requests,re,json,time,pathlib,concurrent.futures
out=pathlib.Path('reports/round2_astra');s=pathlib.Path('artin_correlations.tex').read_text();b=s.split('\\begin{thebibliography}')[1];items=re.split(r'\\bibitem\{([^}]+)\}',b)[1:];pairs=list(zip(items[::2],items[1::2]));
def query(pair):
 k,text=pair
 title=re.search(r'\\emph\{(.*?)\}',text,re.S).group(1);title=re.sub(r'\\[A-Za-z]+','',title);title=re.sub(r'[{}$~\\]',' ',title);title=' '.join(title.split())
 try:
  r=requests.get('https://api.crossref.org/works',params={'query.title':title,'rows':3},timeout=55);raw=r.json();(out/('crossref_'+k+'.json')).write_text(json.dumps(raw,indent=2));hits=[]
  for h in raw.get('message',{}).get('items',[]):hits.append({x:h.get(x) for x in ['title','author','DOI','published','container-title','volume','issue','page','article-number','score']})
  return {'key':k,'query':title,'http':r.status_code,'hits':hits}
 except Exception as e:return {'key':k,'error':str(e)}
with concurrent.futures.ThreadPoolExecutor(max_workers=3) as pool:res=list(pool.map(query,pairs))
(out/'bibliography_metadata.json').write_text(json.dumps(res,indent=2))
for r in res:
 print(r['key'],r.get('http',r.get('error')))
 for h in r.get('hits',[])[:2]:print(json.dumps(h,ensure_ascii=False))
for doi in ['10.5281/zenodo.22865343']:
 r=requests.get('https://api.datacite.org/dois/'+doi,timeout=55);(out/'datacite_BaldII.json').write_text(r.text);print('DATACITE',doi,r.status_code,r.text[:500])
for arxiv in ['2412.13355','2010.15988','2502.19601','2508.08996']:
 try:
  r=requests.get('https://export.arxiv.org/api/query',params={'id_list':arxiv},timeout=45);(out/('arxiv_'+arxiv+'.xml')).write_text(r.text);print('ARXIV',arxiv,r.status_code,r.text[:160])
 except Exception as e:print('ARXIV',arxiv,e)
