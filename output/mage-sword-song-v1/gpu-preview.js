'use strict';

// The effect shader is exported from the game, while this class owns browser-only composition.
class ArcaneGpuPreview
{
    constructor(canvas)
    {
        this.canvas=canvas;
        this.gl=canvas.getContext('webgl2',{alpha:false,antialias:false,premultipliedAlpha:false});
        if(!this.gl)throw new Error('WebGL2를 사용할 수 없습니다.');
        this.resources=[];this.targets=[];this.ready=false;
        canvas.addEventListener('webglcontextlost',event=>{
            event.preventDefault();this.ready=false;
            document.getElementById('gpu-status').textContent='그래픽 컨텍스트가 중단되었습니다. 페이지를 새로 열어 주세요.';
        });
        const gl=this.gl;
        try
        {
            this.hdr=!!gl.getExtension('EXT_color_buffer_float');
            this.effect=this.program(ArcaneShaders.vertex,ArcaneShaders.fragment);
            this.post=this.program(`#version 300 es
                precision highp float;
                out vec2 uv;
                void main(){vec2 c=vec2((gl_VertexID<<1)&2,gl_VertexID&2);uv=c;gl_Position=vec4(c*2.-1.,0,1);}`,`#version 300 es
                precision highp float;
                in vec2 uv;out vec4 color;
                uniform sampler2D source;uniform sampler2D bloom;
                uniform int mode;uniform vec2 direction;uniform vec4 shock;uniform vec2 size;
                vec3 display(vec3 c){return mix(c*12.92,1.055*pow(max(c,vec3(0)),vec3(1./2.4))-.055,step(vec3(.0031308),c));}
                void main(){
                    if(mode==0){vec4 s=texture(source,vec2(uv.x,1.-uv.y));vec3 c=s.rgb;c=mix(c/12.92,pow((c+.055)/1.055,vec3(2.4)),step(vec3(.04045),c));color=vec4(c,s.a);}
                    else if(mode==1){vec3 c=vec3(0);vec2 tx=1./vec2(textureSize(source,0));
                        for(int y=0;y<2;y++)for(int x=0;x<2;x++){vec3 s=texture(source,uv+(vec2(float(x),float(y))-.5)*tx).rgb;float p=max(s.r,max(s.g,s.b));c+=s*max(p-1.,0.)/max(p,.0001)*.25;}color=vec4(c,1);}
                    else if(mode==2){float w[5]=float[5](.227027,.1945946,.1216216,.054054,.016216);vec2 tx=direction/vec2(textureSize(source,0));vec3 c=texture(source,uv).rgb*w[0];for(int i=1;i<5;i++)c+=(texture(source,uv+tx*float(i)).rgb+texture(source,uv-tx*float(i)).rgb)*w[i];color=vec4(c,1);}
                    else {vec2 coord=uv; if(shock.w>0.){vec2 d=(vec2(uv.x,1.-uv.y)*size-shock.xy)/vec2(1,.58);float r=length(d);float front=300.*(.06+(1.-exp(-shock.z*7.))*1.32);vec2 shift=d/max(r,1.)*vec2(1,.58)*exp(-pow((r-front)/16.,2.))*shock.w;coord-=vec2(shift.x,-shift.y)/size;coord=clamp(coord,.5/size,1.-.5/size);}
                        vec3 c=texture(source,coord).rgb+texture(bloom,coord).rgb*.32;
                        c=1.-exp(-max(c,vec3(0))*1.15);color=vec4(display(c),1);}
                }`);
            this.vao=gl.createVertexArray();
            this.upload=this.texture(gl.RGBA8,1,1,gl.LINEAR);
            this.scene=this.target(canvas.width,canvas.height);
            this.blur=[this.target(canvas.width/2,canvas.height/2),this.target(canvas.width/2,canvas.height/2)];
            gl.bindFramebuffer(gl.FRAMEBUFFER,null);
            this.ready=true;
            document.getElementById('gpu-status').textContent=this.hdr?
                'WebGL2 · 게임과 동일한 효과 셰이더 · HDR / 블룸 / 충격파 굴절':
                'WebGL2 · HDR 미지원: 일반 정밀도로 표시합니다. 빛의 번짐이 제한됩니다.';
        }
        catch(error){this.dispose();throw error;}
    }
    program(vertex,fragment)
    {
        const gl=this.gl, shaders=[];
        try
        {
            for(const [type,text] of [[gl.VERTEX_SHADER,vertex],[gl.FRAGMENT_SHADER,fragment]])
            {
                const shader=gl.createShader(type);shaders.push(shader);
                gl.shaderSource(shader,text);gl.compileShader(shader);
                if(!gl.getShaderParameter(shader,gl.COMPILE_STATUS))throw new Error(gl.getShaderInfoLog(shader));
            }
            const program=gl.createProgram();this.resources.push(program);
            shaders.forEach(s=>gl.attachShader(program,s));gl.linkProgram(program);
            if(!gl.getProgramParameter(program,gl.LINK_STATUS))throw new Error(gl.getProgramInfoLog(program));
            return {program,uniforms:new Map()};
        }
        finally{shaders.forEach(s=>gl.deleteShader(s));}
    }
    uniform(program,name)
    {
        if(!program.uniforms.has(name))program.uniforms.set(name,this.gl.getUniformLocation(program.program,name));
        return program.uniforms.get(name);
    }
    texture(format,w,h,filter)
    {
        const gl=this.gl,texture=gl.createTexture();
        gl.bindTexture(gl.TEXTURE_2D,texture);
        gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MIN_FILTER,filter);gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_MAG_FILTER,filter);
        gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_WRAP_S,gl.CLAMP_TO_EDGE);gl.texParameteri(gl.TEXTURE_2D,gl.TEXTURE_WRAP_T,gl.CLAMP_TO_EDGE);
        gl.texImage2D(gl.TEXTURE_2D,0,format,w,h,0,gl.RGBA,format===gl.RGBA16F?gl.HALF_FLOAT:gl.UNSIGNED_BYTE,null);
        return texture;
    }
    target(w,h)
    {
        const gl=this.gl,t={width:w,height:h,texture:this.texture(this.hdr?gl.RGBA16F:gl.RGBA8,w,h,gl.LINEAR),fbo:gl.createFramebuffer()};
        this.targets.push(t);gl.bindFramebuffer(gl.FRAMEBUFFER,t.fbo);gl.framebufferTexture2D(gl.FRAMEBUFFER,gl.COLOR_ATTACHMENT0,gl.TEXTURE_2D,t.texture,0);
        if(gl.checkFramebufferStatus(gl.FRAMEBUFFER)!==gl.FRAMEBUFFER_COMPLETE)throw new Error('미리보기 렌더 타깃 생성 실패');
        return t;
    }
    bind(target)
    {
        const gl=this.gl;gl.bindFramebuffer(gl.FRAMEBUFFER,target?target.fbo:null);gl.viewport(0,0,target?target.width:this.canvas.width,target?target.height:this.canvas.height);gl.bindVertexArray(this.vao);
    }
    pass(mode,source,target,direction=[0,0],shock=[0,0,0,0])
    {
        const gl=this.gl,p=this.post;this.bind(target);gl.useProgram(p.program);
        gl.activeTexture(gl.TEXTURE0);gl.bindTexture(gl.TEXTURE_2D,source);gl.uniform1i(this.uniform(p,'source'),0);
        // Even unused sampler branches must not reference the current render target.
        gl.activeTexture(gl.TEXTURE1);gl.bindTexture(gl.TEXTURE_2D,mode===3?this.blur[0].texture:source);
        gl.uniform1i(this.uniform(p,'bloom'),1);
        gl.uniform1i(this.uniform(p,'mode'),mode);gl.uniform2fv(this.uniform(p,'direction'),direction);gl.uniform4fv(this.uniform(p,'shock'),shock);
        gl.uniform2f(this.uniform(p,'size'),this.canvas.width,this.canvas.height);
        gl.drawArrays(gl.TRIANGLES,0,3);
    }
    layer(canvas)
    {
        const gl=this.gl;gl.activeTexture(gl.TEXTURE0);gl.bindTexture(gl.TEXTURE_2D,this.upload);
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL,false);gl.pixelStorei(gl.UNPACK_PREMULTIPLY_ALPHA_WEBGL,false);
        gl.texImage2D(gl.TEXTURE_2D,0,gl.RGBA,gl.RGBA,gl.UNSIGNED_BYTE,canvas);
        gl.enable(gl.BLEND);gl.blendFunc(gl.SRC_ALPHA,gl.ONE_MINUS_SRC_ALPHA);this.pass(0,this.upload,this.scene);
    }
    drawEffect(kind,center,radius,depth,rotation,age,ink)
    {
        if(radius<=.5||ink[3]<=0)return;
        const gl=this.gl,p=this.effect,extent=kind===0?radius*1.17:480,ey=kind===0?extent*Math.max(.1,depth):extent;
        this.bind(this.scene);gl.useProgram(p.program);
        const left=Math.max(0,Math.floor(center[0]-extent)),right=Math.min(this.canvas.width,Math.ceil(center[0]+extent));
        const top=Math.max(0,Math.floor(center[1]-ey)),bottom=Math.min(this.canvas.height,Math.ceil(center[1]+ey));
        if(right<=left||bottom<=top)return;
        gl.enable(gl.SCISSOR_TEST);gl.scissor(left,this.canvas.height-bottom,right-left,bottom-top);
        gl.uniform2f(this.uniform(p,'viewport'),this.canvas.width,this.canvas.height);
        gl.uniform4f(this.uniform(p,'effectRect'),...center,extent,ey);
        gl.uniform4f(this.uniform(p,'parameters'),radius,depth,rotation,age);
        gl.uniform4fv(this.uniform(p,'ink'),ink);gl.uniform1i(this.uniform(p,'effectKind'),kind);gl.uniform1i(this.uniform(p,'linearOutput'),1);
        gl.enable(gl.BLEND);gl.blendFunc(gl.ONE,gl.ONE);gl.drawArrays(gl.TRIANGLES,0,3);gl.disable(gl.SCISSOR_TEST);
    }
    compose(background,actors,swords,t,c)
    {
        if(!this.ready)return;
        const gl=this.gl,G=SongGeometry,g=G.ease(t/.7),m=G.ease((t-4.7)/.55),core=[c[0],c[1]-115];
        this.bind(this.scene);gl.clearColor(0,0,0,1);gl.clear(gl.COLOR_BUFFER_BIT);this.layer(background);
        if(t<5.25)this.drawEffect(0,[c[0],c[1]-115*m],300*g*(1-.78*m),.866+(.34-.866)*m,t*.27,0,[.65,.45,1,g*.8]);
        this.layer(actors);
        if(t<5.25)
        {
            this.drawEffect(1,core,300,1,0,t,[1,1,1,g]);
            this.drawEffect(0,[c[0],c[1]-230+115*m],300*g*(1-.78*m),.34,-t*.34,0,[.32,.80,1,g*.9]);
        }
        else if(t<5.8)this.drawEffect(2,core,300,1,0,t-5.25,[1,1,1,1]);
        this.layer(swords);
        gl.disable(gl.BLEND);this.pass(1,this.scene.texture,this.blur[0]);
        for(let i=0;i<4;i++)this.pass(2,this.blur[i%2].texture,this.blur[1-i%2],i%2?[0,2.5]:[2.5,0]);
        const age=t-5.25,strength=age>=0&&age<.55?7*G.ease(age/.025)*(1-G.ease(age/.55)):0;
        this.pass(3,this.scene.texture,null,[0,0],[...core,Math.max(0,age),strength]);
    }
    dispose()
    {
        const gl=this.gl;this.targets.forEach(t=>{gl.deleteTexture(t.texture);gl.deleteFramebuffer(t.fbo);});
        this.resources.forEach(p=>gl.deleteProgram(p));if(this.vao)gl.deleteVertexArray(this.vao);if(this.upload)gl.deleteTexture(this.upload);this.ready=false;
    }
}
