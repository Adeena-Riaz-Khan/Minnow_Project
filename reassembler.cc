#include "reassembler.hh"
#include "debug.hh"
using namespace std;

Reassembler::Reassembler(ByteStream&& out) 
:out_(std::move(out)),next_idx(0),end_seen_flag(false),end_pos(0),frags(){}

void Reassembler::flush(){
bool wrote_any=true;
while (wrote_any){
wrote_any=false;
map<uint64_t, string>::iterator it;
for (it=frags.begin();it!=frags.end();++it){
uint64_t start=it->first;
string &s=it->second;
if (start==next_idx){
size_t cap=out_.writer().available_capacity();
if (cap == 0) return;
if (s.size()>cap){
out_.writer().push(s.substr(0, cap));
next_idx+=cap;
s.erase(0, cap);
} else{
out_.writer().push(s);
next_idx+=s.size();
frags.erase(it); }
wrote_any=true;
break; }
}
}
}

void Reassembler::insert(uint64_t idx,string s,bool last){
if (last){
end_seen_flag=true;
end_pos=idx+s.size();
}
if (idx+s.size()<=next_idx){ 
if (end_seen_flag && next_idx==end_pos && count_bytes_pending()==0) out_.writer().close();
return;
}
if (idx<next_idx){ 
size_t cut=next_idx-idx;
s.erase(0, cut);
idx=next_idx;
}
size_t cap=out_.writer().available_capacity();
if (idx>=next_idx+cap) 
return; 
if (idx+s.size()>next_idx+cap) s.resize(next_idx+cap-idx);
uint64_t ns=idx;
uint64_t ne=idx+s.size();
string merged=s;
map<uint64_t, string>::iterator it=frags.lower_bound(ns);
if (it!= frags.begin()) 
--it;
while (it!= frags.end()) {
uint64_t a=it->first;
uint64_t b=a+it->second.size();
if (ne<a) 
break;
if (b<ns){
++it;
continue;
}
uint64_t lo=min(ns, a);
uint64_t hi=max(ne, b);
string tmp(hi-lo, '\0');
for (uint64_t i=ns;i<ne;i++) 
tmp[i-lo]=merged[i-ns];
for (uint64_t i=a;i<b;i++)
tmp[i-lo]=it->second[i-a];
ns=lo;
ne=hi;
merged=tmp;
it=frags.erase(it);
}
frags[ns]=merged;
flush(); 
if(end_seen_flag && next_idx==end_pos && count_bytes_pending()==0){
out_.writer().close();
}
}

uint64_t Reassembler::count_bytes_pending() const{
uint64_t total=0;
for (const map<uint64_t,string>::value_type &entry:frags){
total+=entry.second.size();
}
return total;
}

