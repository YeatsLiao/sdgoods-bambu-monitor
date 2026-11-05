/* SPDX-License-Identifier: MIT; deterministic PackBits roundtrip and palette bounds. */
const assert=require('assert'),{pack,quantize}=require('./encode_batch3.cjs');
function unpack(data){const out=[];for(let i=0;i<data.length;){const c=data[i++],n=(c&127)+1;if(c&128){assert(i<data.length);out.push(...Array(n).fill(data[i++]));}else{assert(i+n<=data.length);out.push(...data.slice(i,i+n));i+=n;}}return out;}
let seed=123;const rand=()=>{seed=(Math.imul(seed,1664525)+1013904223)>>>0;return seed&255;};
for(const n of [0,1,2,3,127,128,129,255,256,10000]){
 for(const mode of [0,1,2]){const input=Array.from({length:n},(_,i)=>mode===0?7:mode===1?i&255:rand());assert.deepStrictEqual(unpack(pack(input)),input);}
}
for(const input of [Array(100).fill(0xffff),[0,0,0,0xffff],Array.from({length:5000},()=>rand()<<8|rand())]){
 const q=quantize(input);assert.strictEqual(q.palette.length,512);assert.strictEqual(q.indices.length,input.length);assert(q.indices.every(i=>i>=0&&i<=255));assert.deepStrictEqual(unpack(pack(q.indices)),q.indices);
}
console.log('PASS: PackBits boundaries 0/1/2/3/127/128/129/255/256/10000, runs/literals, deterministic data, uniform and skewed palettes.');
