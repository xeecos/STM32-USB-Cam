const BF3003 = require("./bf3003");

let sharedBuffer;
onmessage = (res)=>{
    sharedBuffer = res.data;
    BF3003.setup(640,480).then((res)=>
    {
        let buf = new Uint8Array(sharedBuffer);
        BF3003.on("data",(res)=>
        {
            let width = BF3003.width;
            let height = BF3003.height;
            for(let i=0,count=width*height;i<count;i++)
            {
                buf[i] = res.data[i];
            }
            postMessage({ line:res.line, width, height });
        });
        BF3003.start().then(res=>{
            BF3003.startTransfer();
        });
    });
}