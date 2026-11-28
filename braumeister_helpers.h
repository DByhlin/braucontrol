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
    <button class="btn btn-amber" style="grid-column:span 2" onclick="sendCmd('ack_hop')">Silence Hop Alarm</button>
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
    for(let i=0;i<sCount;i++){
      if(steps.at(i)){sPart.push(Math.round(parseFloat(steps.at(i).querySelector('STEP_TEMP')?.textContent||'65'))+','+Math.round(parseFloat(steps.at(i).querySelector('STEP_TIME')?.textContent||'15')));}
      else{sPart.push('65,15');}
    }
    const hops=Array.from(doc.querySelectorAll('HOP')).filter(h=>['Boil','First Wort'].includes((h.querySelector('USE')?.textContent||'').trim())).slice(0,8);
    let hPart=[];
    for(let i=0;i<hops.length;i++){hPart.push(Math.round(parseFloat(hops.at(i).querySelector('TIME')?.textContent||'0')));}

    // Extract Est OG, Est FG, Yeast Name & Fermentation Stages
    const estOg=Math.round(parseFloat(doc.querySelector('RECIPE > EST_OG')?.textContent||'1.050')*1000);
    const estFg=Math.round(parseFloat(doc.querySelector('RECIPE > EST_FG')?.textContent||'1.012')*1000);
    const yeast=(doc.querySelector('YEAST > NAME')?.textContent?.trim()||'Standard Ale/Lager').replace(/\|/g,'-').slice(0,36);

    const stages=parseInt(doc.querySelector('RECIPE > FERMENTATION_STAGES')?.textContent||'1',10);
    let fStages=[];
    const pTemp=parseFloat(doc.querySelector('RECIPE > PRIMARY_TEMP')?.textContent||'');
    const pAge=Math.round(parseFloat(doc.querySelector('RECIPE > PRIMARY_AGE')?.textContent||'14'));
    if(!isNaN(pTemp)) fStages.push(pTemp.toFixed(1)+','+pAge);
    if(stages>=2){
      const sTemp=parseFloat(doc.querySelector('RECIPE > SECONDARY_TEMP')?.textContent||'');
      const sAge=Math.round(parseFloat(doc.querySelector('RECIPE > SECONDARY_AGE')?.textContent||'7'));
      if(!isNaN(sTemp)) fStages.push(sTemp.toFixed(1)+','+sAge);
    }
    if(stages>=3){
      const tTemp=parseFloat(doc.querySelector('RECIPE > TERTIARY_TEMP')?.textContent||'');
      const tAge=Math.round(parseFloat(doc.querySelector('RECIPE > TERTIARY_AGE')?.textContent||'7'));
      if(!isNaN(tTemp)) fStages.push(tTemp.toFixed(1)+','+tAge);
    }
    if(fStages.length===0) fStages.push('18.0,14');

    const packed='PACKED|'+rName+'|'+sCount+'|'+sPart.join(';')+'|'+bTime+',85|'+hops.length+'|'+hPart.join(',')+'||'+estOg+'|'+estFg+'|'+yeast+'|'+fStages.join(';');
    const res=await fetch('/brewcontrol?value='+encodeURIComponent(packed),{method:'POST'});
    if(res.ok){msg.style.color='#34d399';msg.innerText='Loaded "'+rName+'" ('+sCount+' steps, '+hops.length+' hops, OG '+(estOg/1000).toFixed(3)+').';poll();}
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

inline bool bm_unpack_recipe(
    const std::string &raw,
    std::vector<float*> &t,
    std::vector<int*> &m,
    std::vector<int*> &h,
    int &out_ms,
    int &out_bt,
    int &out_bp,
    int &out_hc) {
  for (int i = 0; i < 8; i++) { *t.at(i) = 0.0f; *m.at(i) = 0; *h.at(i) = 0; }

  size_t p1 = raw.find('|');
  size_t p2 = (p1 != std::string::npos) ? raw.find('|', p1 + 1) : std::string::npos;
  size_t p3 = (p2 != std::string::npos) ? raw.find('|', p2 + 1) : std::string::npos;
  size_t p4 = (p3 != std::string::npos) ? raw.find('|', p3 + 1) : std::string::npos;
  if (p4 == std::string::npos) return false;

  // Compact format: ms|t1,m1;t2,m2|bt,bp|hc|h1,h2
  size_t next_bar = raw.find('|', p4 + 1);
  if (next_bar == std::string::npos || raw.substr(next_bar, 2) == "||") {
    out_ms = std::max(1, std::min(8, atoi(raw.substr(0, p1).c_str())));
    std::string steps_str = raw.substr(p1 + 1, p2 - p1 - 1);
    size_t sp = 0;
    for (int i = 0; i < out_ms && sp < steps_str.size(); i++) {
      size_t next_s = steps_str.find(';', sp);
      std::string pair = (next_s == std::string::npos) ? steps_str.substr(sp) : steps_str.substr(sp, next_s - sp);
      int tv = 65, mv = 15;
      if (sscanf(pair.c_str(), "%d,%d", &tv, &mv) == 2) {
        *t.at(i) = (float)tv;
        *m.at(i) = mv;
      }
      if (next_s == std::string::npos) break;
      sp = next_s + 1;
    }
    out_bt = 60; out_bp = 85;
    sscanf(raw.substr(p2 + 1, p3 - p2 - 1).c_str(), "%d,%d", &out_bt, &out_bp);
    out_hc = std::max(0, std::min(8, atoi(raw.substr(p3 + 1, p4 - p3 - 1).c_str())));
    size_t meta_pos = raw.find("||", p4 + 1);
    std::string hops_str = (meta_pos == std::string::npos)
        ? raw.substr(p4 + 1)
        : raw.substr(p4 + 1, meta_pos - p4 - 1);
    size_t hp = 0;
    for (int i = 0; i < out_hc && hp < hops_str.size(); i++) {
      size_t next_h = hops_str.find(',', hp);
      std::string hval = (next_h == std::string::npos) ? hops_str.substr(hp) : hops_str.substr(hp, next_h - hp);
      *h.at(i) = atoi(hval.c_str());
      if (next_h == std::string::npos) break;
      hp = next_h + 1;
    }
    return true;
  }
  return false;
}

inline void bm_unpack_ferm_meta(
    const std::string &raw,
    int &out_og_pts,
    int &out_fg_pts,
    std::string &out_yeast,
    std::string &out_stages) {
  out_og_pts = 1050;
  out_fg_pts = 1012;
  out_yeast = "Standard Ale/Lager";
  out_stages = "18.0,14";

  size_t meta_pos = raw.find("||");
  if (meta_pos == std::string::npos) return;

  std::string meta = raw.substr(meta_pos + 2);
  size_t m1 = meta.find('|');
  size_t m2 = (m1 != std::string::npos) ? meta.find('|', m1 + 1) : std::string::npos;
  size_t m3 = (m2 != std::string::npos) ? meta.find('|', m2 + 1) : std::string::npos;
  if (m3 == std::string::npos) return;

  int og_p = atoi(meta.substr(0, m1).c_str());
  int fg_p = atoi(meta.substr(m1 + 1, m2 - m1 - 1).c_str());
  if (og_p >= 1000 && og_p <= 1150) out_og_pts = og_p;
  if (fg_p >= 990 && fg_p <= 1100) out_fg_pts = fg_p;

  std::string y_str = meta.substr(m2 + 1, m3 - m2 - 1);
  if (!y_str.empty()) out_yeast = y_str;

  std::string fs_str = meta.substr(m3 + 1);
  if (!fs_str.empty()) out_stages = fs_str;
}

#define BM_TEMPS std::vector<float*>{&id(mash_temp_1), &id(mash_temp_2), &id(mash_temp_3), &id(mash_temp_4), &id(mash_temp_5), &id(mash_temp_6), &id(mash_temp_7), &id(mash_temp_8)}
#define BM_TIMES std::vector<int*>{&id(mash_time_1), &id(mash_time_2), &id(mash_time_3), &id(mash_time_4), &id(mash_time_5), &id(mash_time_6), &id(mash_time_7), &id(mash_time_8)}
#define BM_HOPS  std::vector<int*>{&id(hop_time_1), &id(hop_time_2), &id(hop_time_3), &id(hop_time_4), &id(hop_time_5), &id(hop_time_6), &id(hop_time_7), &id(hop_time_8)}
#define BM_SLOT_NAMES std::vector<std::string*>{&id(slot1_name), &id(slot2_name), &id(slot3_name), &id(slot4_name), &id(slot5_name), &id(slot6_name), &id(slot7_name), &id(slot8_name)}
#define BM_SLOT_DATA  std::vector<std::string*>{&id(slot1_data), &id(slot2_data), &id(slot3_data), &id(slot4_data), &id(slot5_data), &id(slot6_data), &id(slot7_data), &id(slot8_data)}

#define BM_FERM_NAMES std::vector<std::string*>{&id(ferm1_name), &id(ferm2_name), &id(ferm3_name), &id(ferm4_name), &id(ferm5_name), &id(ferm6_name)}
#define BM_FERM_IPS   std::vector<std::string*>{&id(ferm1_ip), &id(ferm2_ip), &id(ferm3_ip), &id(ferm4_ip), &id(ferm5_ip), &id(ferm6_ip)}

inline void bm_del_fermenter(std::vector<std::string*> &names, std::vector<std::string*> &ips, int &count, int idx_1based) {
  if (count > 0 && idx_1based >= 1 && idx_1based <= count) {
    for (int i = idx_1based - 1; i < count - 1; i++) {
      *names.at(i) = *names.at(i + 1);
      *ips.at(i) = *ips.at(i + 1);
    }
    *names.at(count - 1) = "";
    *ips.at(count - 1) = "";
    count--;
  }
}

#include <cstdlib>
#include <cstring>

inline float bm_safe_stof(const std::string &s, float fallback = 0.0f) {
  if (s.empty()) return fallback;
  char *end = nullptr;
  float val = std::strtof(s.c_str(), &end);
  return (end == s.c_str()) ? fallback : val;
}

inline std::string bm_safe_ssid(const uint8_t *raw_ssid) {
  size_t len = strnlen(reinterpret_cast<const char*>(raw_ssid), 32);
  return std::string(reinterpret_cast<const char*>(raw_ssid), len);
}