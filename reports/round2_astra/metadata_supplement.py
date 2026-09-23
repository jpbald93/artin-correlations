import requests,json,pathlib,concurrent.futures
out=pathlib.Path('reports/round2_astra')
dois={'ConwayGuy1996':'10.1007/978-1-4612-4072-3','Erdos1935':'10.1093/qmath/os-6.1.205','FanPollack2025':'10.1112/mtk.70055','GuptaMurty1984':'10.1007/BF01388719','HardyLittlewood1923':'10.1007/BF02403921','HeathBrown1986':'10.1093/qmath/37.1.27','Hooley1967':'10.1515/crll.1967.225.209','Moree2012':'10.1515/integers-2012-0043','JarviniemiPeruccaSgobba2025':'10.1007/s40993-025-00620-2','Lenstra1977':'10.1007/BF01389788','Matthews1976':'10.4064/aa-29-2-113-146'}
def run(item):
 k,d=item
 try:
  r=requests.get('https://api.crossref.org/works/'+d,timeout=50);(out/('doi_'+k+'.json')).write_text(r.text);h=r.json()['message'];return {'key':k,'http':r.status_code,**{x:h.get(x) for x in ['title','author','DOI','published','published-print','container-title','volume','issue','page','article-number']}}
 except Exception as e:return {'key':k,'error':str(e)}
with concurrent.futures.ThreadPoolExecutor(max_workers=2) as p:res=list(p.map(run,dois.items()))
for x in res:print(json.dumps(x,ensure_ascii=False))
for a in ['2412.13355','2010.15988','2502.19601','2508.08996']:
 r=requests.get('https://api.datacite.org/dois/10.48550/arXiv.'+a,timeout=45);(out/('datacite_'+a+'.json')).write_text(r.text)
 try:
  x=r.json()['data']['attributes'];print('ARXIV_DATACITE',a,r.status_code,json.dumps({k:x.get(k) for k in ['titles','creators','publicationYear','relatedIdentifiers']},ensure_ascii=False))
 except Exception as e:print(a,r.status_code,str(e))
# Original edition review metadata, not the later Springer edition
r=requests.get('https://api.crossref.org/works/10.1112/jlms/s1-42.1.189b',timeout=45);(out/'Artin_review.json').write_text(r.text)
print('ARTIN_REVIEW',r.status_code)
