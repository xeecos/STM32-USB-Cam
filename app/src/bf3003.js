const { findByIds, WebUSBDevice } = require('usb');
const EventEmitter = require('node:events');
class Utils
{
    static async delay(ms)
    {
        return new Promise(resolve => setTimeout(resolve, ms));
    }
}
class BF3003
{
    constructor()
    {
        this._width = 640;
        this._height = 480;
        this._emitter = new EventEmitter();
        this._epOut = 2;
    }
    async setup(width=640,height=480)
    {
        let device = findByIds(0x20B1, 0x1DE0);
        if(!device)return 1;
        this._usb = new WebUSBDevice(device);
        this._usb.open();

        device.interfaces[0].claim();
        if(this._epOut>0)
        {
            device.interfaces[1].claim();
        }
        await this.setFrameSize(width,height);
        await this.setMode(1,1,0);
        await this.setGain(5,9,5);
        await this.setDummy(this._width==640?0x20:0x0);
        await this.setExposure(0xf);
        await this.setFrequency(12,12);
        let debug = {time:Date.now(),successCount:0,failCount:0,enable:true};

        let line = 0;
        let imageIdx = 0;
        let imageBuf = [new Array(640*480),new Array(640*480)];
        let lineCount = 0;

        let epIn = device.interfaces[0].endpoints[0];
        let cc = 0;
        let now = Date.now();
        epIn.on("data", (buffer)=>{ 
            // cc += buffer.length;
            // if(cc>1024*1024)
            // {
            //     console.log("time:",Date.now()-now,1000/(Date.now()-now));
            //     now = Date.now();
            //     cc = 0;
            // }
            // console.log("n:"+buffer.length+" ,");
            // return;
            if(buffer.length==2)
            {
                if(line>1)
                {
                    this.emit("data",{line,data:imageBuf[imageIdx]});
                    imageIdx = 1 - imageIdx;

                    if(debug.enable)
                    {
                        let t = Date.now()-debug.time;
                        console.log("encode:",t/1000, line);
                        debug.time = Date.now();
                        if(line>this._height-1)
                        {
                            debug.successCount++;
                        }
                        else
                        {
                            debug.failCount++;
                        }
                        // console.log("percent:",(debug.successCount/(debug.successCount+debug.failCount)*100).toFixed(2),"success:", debug.successCount, " fail:", debug.failCount);
                    }
                }
                line = 0;
                lineCount = 0;
                return;
            }
            if(buffer.length>0)
            {
                let n = buffer.length / this._width;
                let line0 = line;
                for(let j=0;j<n;j++)
                {
                    // line0 = ((buffer[j*this._width]<<8)+buffer[j*this._width+1]);
                    for(let i=0;i<this._width;i++)
                    {
                        imageBuf[imageIdx][(line0+j)*this._width+lineCount+i] = buffer[i+j*this._width];
                    }
                }
                lineCount = 0;
                line+=n;
            }
        });
        return 0;
    }
    get width()
    {
        return this._width;
    }
    get height()
    {
        return this._height;
    }
    emit(type,data)
    {
        this._emitter.emit(type, data);
    }
    on(type,callback)
    {
        this._emitter.on(type, callback);
    }
    transferOut(data)
    {
        return new Promise(resolve=>
        {
            if(this._epOut<0) return resolve(0);
            this._usb.transferOut(this._epOut,data).then((res)=>{
                resolve(res);
            });
        })
    }
    setWindow(x,y,w,h)
    {
        return new Promise(resolve=>{
            this._width = w;
            this._height = h;
            this.transferOut(Buffer.from([1,1,x>>8,x&0xff,y>>8,y&0xff,w>>8,w&0xff,h>>8,h&0xff])).then((res)=>{
                resolve(res);
            });
        })
    }
    setDummy(dummy=0x0)
    {
        return new Promise(resolve=>{
            this.transferOut(Buffer.from([1,2,0x00,dummy])).then((res)=>{
                resolve(res);
            });
        })
    }
    setFrameSize(w,h)
    {
        let x = (640-w)/2+6;
        let y = (480-h)/2;
        return this.setWindow(x, y, w, h);
    }
    setExposure(exp=0x10)
    {
        return new Promise(resolve=>{
            this.transferOut(Buffer.from([1,3,exp>>8,exp&0xff])).then((res)=>{
                resolve(res);
            });
        })
    }
    setGain(red=0x15, green=0x12, blue=0x17)
    {
        return new Promise(resolve=>{
            this.transferOut(Buffer.from([1,4,red,green,blue])).then((res)=>{
                resolve(res);
            });
        })
    }
    setMode(gain=0,colorbalance=0,exposure=0)
    {
        return new Promise(resolve=>{
            this.transferOut(Buffer.from([1, 8, gain, colorbalance, exposure])).then((res)=>{
                resolve(res);
            })
        });
    }
    setFrequency(freqBase=8,freqSkip=8)
    {
        return new Promise(resolve=>{
            this.transferOut(Buffer.from([1,7,freqBase, freqSkip])).then((res)=>{
                resolve(res);
            });
        })
    }
    start()
    {
        return new Promise(resolve=>{
            console.log("start")
            this.transferOut(Buffer.from([1,0,0x1])).then((res)=>{
                resolve(res);
            });
        });
    }
    stop()
    {
        return new Promise(resolve=>{
            this.transferOut(Buffer.from([1,0,0x0])).then((res)=>{
                resolve(res);
            });
        });
    }
    startTransfer()
    {
        return new Promise(async resolve=>{
            await Utils.delay(100);
            let epIn = this._usb.device.interfaces[0].endpoints[0];
            epIn.startPoll(1,1280);
            resolve();
        });
    }
}
module.exports = new BF3003();