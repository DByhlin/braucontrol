#pragma once
#include <vector>
#include <string>
#include <algorithm>

static const char* BM_UPLOAD_HTML = R"HTML(<!DOCTYPE html>
<html><head><meta name="viewport" content="width=device-width,initial-scale=1">
<title>BrauControl - Universal Brew Controller</title>
<style>
  *{box-sizing:border-box}
  body{font-family:Roboto,-apple-system,BlinkMacSystemFont,"Segoe UI",sans-serif;background:#0b0f17;color:#f8fafc;margin:0;padding:0}
  .appbar{background:#111827;color:#f8fafc;padding:14px 20px;display:flex;align-items:center;justify-content:space-between;border-bottom:1px solid #1f2937}
  .appbar h1{margin:0;font-size:18px;font-weight:600;color:#facc15}
  .appbar a{color:#f8fafc;text-decoration:none;background:#1e293b;border:1px solid #334155;padding:6px 12px;border-radius:8px;font-size:13px;font-weight:500}
  .appbar a:hover{background:#334155}
  .wrap{max-width:620px;margin:18px auto;padding:0 14px;display:flex;flex-direction:column;gap:14px}
  .card{background:#111827;border:1px solid #1f2937;border-radius:12px;overflow:hidden}
  .card-hdr{padding:11px 16px;font-size:12px;font-weight:600;color:#38bdf8;border-bottom:1px solid #1f2937;background:#0f172a;text-transform:uppercase;letter-spacing:.5px}
  .card-body{padding:14px 16px}
  .grid2{display:grid;grid-template-columns:1fr 1fr;gap:10px}
  .grid4{display:grid;grid-template-columns:1fr 1fr 1fr 1fr;gap:8px}
  .stat{background:#1e293b;border:1px solid #334155;border-radius:8px;padding:10px 12px}
  .stat-lbl{font-size:11px;color:#94a3b8;text-transform:uppercase}
  .stat-val{font-size:18px;font-weight:600;color:#f8fafc;margin-top:2px}
  input[type=file]{display:block;width:100%;padding:12px;margin:10px 0 14px;background:#1e293b;border:1px dashed #475569;border-radius:8px;color:#cbd5e1;font-size:14px}
  .btn{width:100%;padding:12px 10px;background:#1d4ed8;color:#fff;border:1px solid #3b82f6;border-radius:8px;font-size:13px;font-weight:600;cursor:pointer;text-transform:uppercase;transition:opacity .15s}
  .btn:hover{opacity:.9}
  .btn-green{background:#059669;border-color:#34d399}
  .btn-amber{background:#d97706;border-color:#fbbf24}
  .btn-red{background:#dc2626;border-color:#f87171}
  .btn-off{background:#1e293b;border-color:#334155;color:#cbd5e1}
  .btn-htr-on{background:#dc2626;border-color:#f87171;color:#fff}
  .btn-pmp-on{background:#2563eb;border-color:#60a5fa;color:#fff}
  .btn-dis{background:#0f172a;border-color:#1e293b;color:#475569;cursor:not-allowed}
  #msg{margin-top:10px;font-size:13px;font-weight:500}
</style></head><body>
<div class="appbar"><h1>BrauControl</h1><a href="/">Full Entity View &rarr;</a></div>
<div class="wrap">
    <div class="card"><div class="card-hdr">Live Brew Status</div><div class="card-body"><div class="grid2">
    <div class="stat"><div class="stat-lbl">Kettle Temp</div><div class="stat-val" id="st_temp" style="color:#f59e0b">--.- &deg;C</div></div>
    <div class="stat"><div class="stat-lbl">Step Timer</div><div class="stat-val" id="st_time" style="color:#34d399">00:00</div></div>
    <div class="stat"><div class="stat-lbl">Power Draw</div><div class="stat-val" id="st_pwr" style="color:#f87171">0 W (0.0 A)</div></div>
    <div class="stat"><div class="stat-lbl">Active Recipe</div><div class="stat-val" id="st_rec" style="font-size:13px;color:#38bdf8">--</div></div>
    <div class="stat" style="grid-column:span 2"><div class="stat-lbl">Status</div><div class="stat-val" id="st_stat" style="font-size:13px">Idle</div></div>
  </div></div></div>
  <div class="card"><div class="card-hdr">Recipe Progression</div><div class="card-body"><div class="grid2">
    <button class="btn btn-green" onclick="sendCmd('start')">Start / Resume</button>
    <button class="btn btn-amber" id="btn_conf" onclick="sendCmd('confirm')">Confirm Step</button>
    <button class="btn" onclick="sendCmd('boil')">Start Boil</button>
    <button class="btn btn-red" onclick="sendCmd('stop')">Stop All</button>
    <button class="btn btn-off" onclick="sendCmd('delay_add')">Delay Start +30m</button>
    <button class="btn btn-off" onclick="sendCmd('delay_clr')">Clear Delay</button>
  </div></div></div>
  <div class="card"><div class="card-hdr">Manual Pump &amp; Element Override</div><div class="card-body"><div id="io_grid" style="display:grid;grid-template-columns:repeat(auto-fit,minmax(110px,1fr));gap:8px">
    <button class="btn btn-off" id="b_h1" onclick="sendCmd('htr1')">HTR 1</button>
    <button class="btn btn-off" id="b_h2" onclick="sendCmd('htr2')">HTR 2</button>
    <button class="btn btn-off" id="b_p1" onclick="sendCmd('pump1')">PUMP 1</button>
    <button class="btn btn-off" id="b_p2" onclick="sendCmd('pump2')">PUMP 2</button>
  </div></div></div>
  <div class="card"><div class="card-hdr">BeerXML Recipe Importer</div><div class="card-body">
    <p style="margin:0;font-size:13px;color:#94a3b8">Select a <b>.xml</b> recipe exported from Brewfather, BeerSmith, or Grainfather:</p>
    <input type="file" id="f" accept=".xml,text/xml">
    <button class="btn" onclick="sendXml()">Upload &amp; Load Recipe</button>
    <div id="msg"></div>
  </div></div>
</div>
<script>
async function sendCmd(c){ await fetch('/brewcontrol?cmd='+c,{method:'POST'}); setTimeout(poll,200); }
function updPill(id,l,s,cls){ const e=document.getElementById(id); if(s===-1){e.style.display='none';}else{e.style.display='block';if(s===1){e.className='btn '+cls;e.innerText=l+': ON';}else{e.className='btn btn-off';e.innerText=l+': OFF';}} }
async function poll(){
  try{
    const io = await fetch('/brewcontrol?cmd=state',{method:'POST'}).then(r=>r.json()).catch(()=>null);
    if(!io) return;
    if(io.temp) document.getElementById('st_temp').innerText=io.temp;
    if(io.time) document.getElementById('st_time').innerText=io.time;
    if(io.pwr)  document.getElementById('st_pwr').innerText=io.pwr;
    if(io.stat) document.getElementById('st_stat').innerText=io.stat;
    if(io.rec)  document.getElementById('st_rec').innerText=io.rec;
    updPill('b_h1','HTR 1',io.h1,'btn-htr-on');
    updPill('b_h2','HTR 2',io.h2,'btn-htr-on');
    updPill('b_p1','PUMP 1',io.p1,'btn-pmp-on');
    updPill('b_p2','PUMP 2',io.p2,'btn-pmp-on');
    document.getElementById('btn_conf').style.opacity=(io.bs===6)?'1.0':'0.55';
  }catch(e){}
}
setInterval(poll,2000); poll();
async function sendXml(){
  const msg=document.getElementById('msg');
  try{
    const file=document.getElementById('f').files.item(0);
    if(!file){alert('Please choose a .xml file first.');return;}
    msg.style.color='#38bdf8'; msg.innerText='Parsing XML...';
    const doc=new DOMParser().parseFromString(await file.text(),'text/xml');
    const rName=(doc.querySelector('RECIPE > NAME')?.textContent?.trim()||file.name).replace(/\|/g,'-').slice(0,32);
    const bTime=Math.round(parseFloat(doc.querySelector('RECIPE > BOIL_TIME')?.textContent||'60'));
    const steps=Array.from(doc.querySelectorAll('MASH_STEP')).slice(0,8);
    const sCount=steps.length||1;
    let sPart=[];
    for(let i=0;i<8;i++){
      if(steps.at(i)){sPart.push(Math.round(parseFloat(steps.at(i).querySelector('STEP_TEMP')?.textContent||'65'))+','+Math.round(parseFloat(steps.at(i).querySelector('STEP_TIME')?.textContent||'15')));}
      else{sPart.push('0,0');}
    }
    const hops=Array.from(doc.querySelectorAll('HOP')).filter(h=>['Boil','First Wort'].includes((h.querySelector('USE')?.textContent||'').trim())).slice(0,8);
    let hPart=[];
    for(let i=0;i<8;i++){hPart.push(hops.at(i)?Math.round(parseFloat(hops.at(i).querySelector('TIME')?.textContent||'0')):0);}
    const packed='PACKED|'+rName+'|'+sCount+'|'+sPart.join('|')+'|'+bTime+',85|'+hops.length+'|'+hPart.join(',');
    const res=await fetch('/brewcontrol?value='+encodeURIComponent(packed),{method:'POST'});
    if(res.ok){msg.style.color='#34d399';msg.innerText='Loaded "'+rName+'" ('+sCount+' steps, '+hops.length+' hops).';poll();}
    else{msg.style.color='#f87171';msg.innerText='Upload failed: HTTP '+res.status;}
  }catch(err){msg.style.color='#f87171';msg.innerText='Error: '+err.message;}
}
</script></body></html>)HTML";

inline void bm_adj_float(std::vector<float*> &vec, int idx, float delta, float min_v, float max_v) {
  if (idx >= 1 && idx <= (int)vec.size()) {
    float *v = vec.at(idx - 1);
    *v = std::max(min_v, std::min(max_v, *v + delta));
  }
}

inline void bm_adj_int(std::vector<int*> &vec, int idx, int delta, int min_v, int max_v) {
  if (idx >= 1 && idx <= (int)vec.size()) {
    int *v = vec.at(idx - 1);
    *v = std::max(min_v, std::min(max_v, *v + delta));
  }
}

inline void bm_del_step(std::vector<float*> &temps, std::vector<int*> &times, int &count, int &sel) {
  if (count > 1 && sel >= 1 && sel <= count) {
    for (int i = sel - 1; i < count - 1; i++) {
      *temps.at(i) = *temps.at(i + 1);
      *times.at(i) = *times.at(i + 1);
    }
    *temps.at(count - 1) = 0.0f;
    *times.at(count - 1) = 0;
    count--;
    if (sel > count) sel = count;
  }
}

inline void bm_del_hop(std::vector<int*> &hops, int &count, int &sel) {
  if (count > 0 && sel >= 1 && sel <= count) {
    for (int i = sel - 1; i < count - 1; i++) {
      *hops.at(i) = *hops.at(i + 1);
    }
    *hops.at(count - 1) = 0;
    count--;
    if (sel > count) sel = (count > 0) ? count : 1;
  }
}
#define BM_TEMPS std::vector<float*>{&id(mash_temp_1), &id(mash_temp_2), &id(mash_temp_3), &id(mash_temp_4), &id(mash_temp_5), &id(mash_temp_6), &id(mash_temp_7), &id(mash_temp_8)}
#define BM_TIMES std::vector<int*>{&id(mash_time_1), &id(mash_time_2), &id(mash_time_3), &id(mash_time_4), &id(mash_time_5), &id(mash_time_6), &id(mash_time_7), &id(mash_time_8)}
#define BM_HOPS  std::vector<int*>{&id(hop_time_1), &id(hop_time_2), &id(hop_time_3), &id(hop_time_4), &id(hop_time_5), &id(hop_time_6), &id(hop_time_7), &id(hop_time_8)}