         R"glsl(#version 120
        uniform sampler2D uDataTexture;
        uniform vec2 uRes;
        uniform float steps;
        float cell_width = 1/steps;

        vec4 getColor(float non)
        {
            float rk = mod(non*12, 12);
            float r = 1.0 - 0.8 * max(-1 , min(rk-3, min(9-rk, 1.0) ) );

            float gk = mod(8 + non*12, 12);
            float g = 1.0 - 0.8 * max(-1 , min(gk-3, min(9-gk, 1.0) ) );

            float bk = mod(4 + non*12, 12);
            float b = 1.0 - 0.8 * max(-1 , min(bk-3, min(9-bk, 1.0) ) );

            return vec4(r, g, b, 1.0);
        }

        vec4 getCircle(float radius, vec2 barTop, vec2 fragC, float window, float non)
        {
            float r = radius*window;
            float power = 3.0;
            vec2 po = fragC - barTop;
            vec2 p = vec2(po.x*(uRes.x/uRes.y), po.y); 
            vec4 fragColor = getColor(non);
            vec4 outp;

            if (length(p) <= window) {
                float s = length(p) - r;
                if (s <= 0) {
                    outp = fragColor*1.5;
                } else {
                    float t = 1 - (s/(window-r));
                    outp = mix(vec4(0.0, 0.0, 0.0, 1.0), fragColor*1.5, pow(t, power));
                }
            } else {
                outp = vec4(0);
            }

            return outp;
        }

        vec4 getSmear(float radius, vec4 data, vec4 dataS, vec2 uv)
        {
            vec4 outp;
            float start = dataS.x;
            float end = data.x;
            float power = 1.5;

            vec2 barT = vec2(data.z + (cell_width/2), data.x);
            vec2 barTS = vec2(dataS.z + (cell_width/2), dataS.x);

            vec4 fragColor = getColor(data.z);

            if(start > end)
            {
                float t = (start - uv.y)/(start- end) * (1 - abs(uv.x-barT.x) / (cell_width*2.5*(data.x + 0.2) ));
                if(end < uv.y && uv.y < start){
                    outp = mix(vec4(0.0, 0.0, 0.0, 1.0), fragColor*1.5, pow(t, power));
                }
                else outp = vec4(0);
            }
            else
            {
                float t = (start - uv.y)/(start - end) * (1 - abs(uv.x-barT.x) / (cell_width*2.5*(data.x + 0.2) ));
                if(start < uv.y && uv.y < end)
                {
                    outp = mix(vec4(0.0, 0.0, 0.0, 1.0), fragColor*1.5, pow(t, power));
                }
                else outp = vec4(0);
            }

            return outp;
        }

        vec4 getBarData(vec2 uvog, int offset)
        {
            vec2 uv = uvog - vec2(cell_width*offset, 0.0);
            float height = texture2D(uDataTexture, vec2(uv.x, 0.0)).r*2/3;
            float no = floor(uv.x*steps);
            float non = no/steps;
            float ra = (uv.x*steps - no);

            return vec4(height, no, non, ra);
        }

        vec4 getBarDataS(vec2 uvog, int offset)
        {
            vec2 uv = uvog - vec2(cell_width*offset, 0.0);
            float height = texture2D(uDataTexture, vec2(uv.x , 1.0)).r*2/3;
            float no = floor(uv.x*steps);
            float non = no/steps;
            float ra = (uv.x*steps - no);

            return vec4(height, no, non, ra);
        }

        vec4 drawBar(vec2 uv)
        {
            vec4 outp;

            vec4 data = getBarData(uv, 0);

            float height = data.x;
            float no     = data.y;
            float non    = data.z;
            float ra     = data.a;

            vec4 rainbow = getColor(non);
            float thick = 0.23*sqrt(height);
            float insideBar = floor(abs(thick/(ra-0.5) ) );
            float isVisible = clamp( (floor(height/uv.y)*insideBar ), 0.0, 1.0 );

            outp = vec4(rainbow.x*isVisible, rainbow.y*isVisible, rainbow.z*isVisible, 1.0);

            return outp;
        }

        vec4 getMaxByAlpha(vec4 left, vec4 right)
        {
            if(left.a >= right.a)
            {
                return left;
            }
            else 
            {
                return right;
            }
        }

        vec4 drawCircle(vec2 uv, int offset)
        {

            vec4 data = getBarData(uv, offset);

            vec2 barT = vec2(data.z + (cell_width/2), data.x);

            return getCircle(0.3, barT, uv, cell_width*5*sqrt(data.x), data.z);
        }

        vec4 drawSmear(vec2 uv, int offset)
        {

            vec4 data = getBarData(uv, offset);
            vec4 dataS = getBarDataS(uv, offset);

            return getSmear(0.3, data, dataS, uv);
        }

        void main() 
        {
            vec2 uv = gl_FragCoord.xy / uRes;

            vec4 barColor = drawBar(uv);
            vec4 circlesColor = vec4(0.0, 0.0, 0.0, 0.0); 
            vec4 smearColor = vec4(0.0, 0.0, 0.0, 0.0); 
                circlesColor += drawCircle(uv, 0);
                smearColor += drawSmear(uv, 0);
                for(int i=-3; i<4; i++)
                {
                    circlesColor = max(circlesColor, drawCircle(uv ,i) );
                    smearColor = max(smearColor, drawSmear(uv ,i) );
                }

            
            gl_FragColor = max(circlesColor, max(barColor, smearColor) );

        }
    )glsl" ;
