#include "wrapping_integers.hh"
#include "debug.hh"

using namespace std;

Wrap32 Wrap32::wrap(uint64_t n,Wrap32 zero_point){
return Wrap32{static_cast<uint32_t>(zero_point.raw_value_+n)};
}

uint64_t Wrap32::unwrap(Wrap32 zero_point,uint64_t checkpoint) const{

uint64_t WRAP=1ULL<<32;
uint32_t offset=raw_value_-zero_point.raw_value_;
uint64_t base=(checkpoint/WRAP)*WRAP+offset;
uint64_t best=base;
uint64_t best_diff;

if(checkpoint>base){
best_diff=checkpoint-base;
}
else{
best_diff=base-checkpoint;
}

uint64_t up=base+WRAP;
uint64_t up_diff;

if(checkpoint>up){
up_diff=checkpoint-up;
}
else{
up_diff=up-checkpoint;
}
if(up_diff<best_diff){
best=up;
best_diff=up_diff;
}

if(base>=WRAP){
uint64_t down=base-WRAP;
uint64_t down_diff;

if(checkpoint>down){
down_diff=checkpoint-down;
}
else{
down_diff=down-checkpoint;
}
if(down_diff<best_diff){
best=down;
}
}
return best;
}

