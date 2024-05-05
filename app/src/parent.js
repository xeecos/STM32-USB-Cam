const bmp = require('bmp-js');
const jpeg = require("jpeg-js");
const fs = require("fs")
const Demosaic = require('demosaic');
const sharedBuffer = new SharedArrayBuffer(640*480);
let worker = new Worker('./child.js',{
    type: 'module'
  });
worker.postMessage(sharedBuffer);
worker.onmessage = (res)=>{
    console.log(res.data);
};
let buf = Buffer.from(sharedBuffer);
let canvas = document.getElementById("canvas");
let ctx = canvas.getContext("2d");
canvas.width = 640;
canvas.height = 480;
ctx.width = 640;
ctx.height = 480;
worker.onmessage =  (res)=>{
    
    let width = res.data.width, height = res.data.height;
    let line = res.data.line;
    // if(line!=height)return;
    // console.time("time");
    let rgb = Demosaic.bilinear({data: Buffer.from(buf), width, height, bayer:Demosaic.Bayer.BGGR});
    let imageData = ctx.getImageData((640-width)/2, (480-height)/2, width, height);
    for(let i=0,count=width*height;i<count;i++)
    {
        let n = i<<2;
        imageData.data[n] = rgb[i*3];
        imageData.data[n+1] = rgb[i*3+1];
        imageData.data[n+2] = rgb[i*3+2];
        imageData.data[n+3] = 0xff;
    }
    ctx.putImageData(imageData,(640-width)/2, (480-height)/2);
    // let img = bmp.encode({data,width,height});
    // let img = jpeg.encode({data,width,height},80);
    // fs.writeFileSync("tmp.jpg",img.data);
    // console.timeEnd("time");
};