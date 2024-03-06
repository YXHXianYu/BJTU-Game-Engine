// Copyright Inigo Quilez, 2021 - https://iquilezles.org/
// I am the sole copyright owner of this Work.
// You cannot host, display, distribute or share this Work neither
// as it is or altered, here on Shadertoy or anywhere else, in any
// form including physical and digital. You cannot use this Work in any
// commercial or non-commercial product, website or project. You cannot
// sell this Work and you cannot mint an NFTs of it or train a neural
// network with it without permission. I share this Work for educational
// purposes, and you can link to it, through an URL, proper attribution
// and unmodified screenshot, as part of your educational material. If
// these conditions are too restrictive please contact me and we'll
// definitely work it out.
// -------------------------------------------------------

// I made this shader in 2008 for the Euskal Party
// competition in Spain. The original Pouet entry is 
// here: https://www.pouet.net/prod.php?which=51074

// It's a raymarched SDF, and while I had raymarched some
// fractal SDFs back in 2005, this was the first time I
// used the technique to produce art-directed content and
// "paint with maths" (as opposed to doing "mathematical art").

// This is also the shader where I first used the now
// popular ambient occlusion estimation. You'll also find
// my first attemtps at smooth-minumum (used to blend the
// creature's head and tentacles together), and soft 
// shadows (still not as good as the ones we use today).
// You'll find most of the other usual Shadertoy/raymarching
// stuff - domain repetition, Value Noise, box SDFs and of
// course the raymarching machinery.

// Surprisngly, copying the code from my original project
// to Shadertoy here required very little modifications.

// === Add by YXH_XianYu - Begin ===

#version 410 core

#extension GL_EXT_gpu_shader4 : enable

layout(location = 0) in vec2 texcoord;
layout(location = 0) out vec4 fragcolor;

uniform float iTime;
uniform vec2 iResolution;
uniform vec2 iMouse;

// shader toy compatibility

#define fragCoord texcoord
#define fragColor fragcolor

// === Add by YXH_XianYu - End ===


#define AA 1

#define ZERO 0

float dot2( in vec3 v ) { return dot(v,v); }

// https://iquilezles.org/articles/sfrand
float sfrand( inout int mirand )
{
    mirand = mirand*0x343fd+0x269ec3;
    float res = uintBitsToFloat((uint(mirand)>>9)|0x40000000u ); 
    return( res-3.0 );
}

// hash by Hugo Elias
uint ihash( uint n )
{
	n = (n << 13) ^ n;
    n = (n*(n*n*15731u+789221u)+1376312589u)&0x7fffffffu;
    return n;
}

// hash by Hugo Elias
float hash( int n )
{
	n = (n << 13) ^ n;
    n = (n*(n*n*15731+789221)+1376312589)&0x7fffffff;
    return 1.0 - float(n)*(1.0/1073741824.0);
}

float noise( vec3 p )
{
	ivec3 ip = ivec3(floor(p));
     vec3 fp = fract(p);

    vec3 w = fp*fp*(3.0-2.0*fp);
    
    int n = ip.x + ip.y*57 + 113*ip.z;

	return mix(mix(mix( hash(n+(0+57*0+113*0)),
                        hash(n+(1+57*0+113*0)),w.x),
                   mix( hash(n+(0+57*1+113*0)),
                        hash(n+(1+57*1+113*0)),w.x),w.y),
               mix(mix( hash(n+(0+57*0+113*1)),
                        hash(n+(1+57*0+113*1)),w.x),
                   mix( hash(n+(0+57*1+113*1)),
                        hash(n+(1+57*1+113*1)),w.x),w.y),w.z);
}

// https://iquilezles.org/articles/fbm
float fbm( vec3 p )
{
#if 0
    // original code
    return 0.5000*noise( p*1.0 ) + 
           0.2500*noise( p*2.0 ) + 
           0.1250*noise( p*4.0 ) +
           0.0625*noise( p*8.0 );
#else
    // equivalent code, but compiles MUCH faster
    float f = 0.0;
    float s = 0.5;
    for( int i=ZERO; i<4; i++ )
    {
        f += s*noise( p );
        s *= 0.5;
        p *= 2.0;
    }
    return f;
#endif
}

// https://iquilezles.org/articles/distfunctions
float sdBox( in vec3 p, in vec3 b ) 
{
    vec3 q = abs(p) - b;
    return min(max(q.x,max(q.y,q.z)),0.0) + length(max(q,0.0));
}

// https://iquilezles.org/articles/distfunctions
float udSqBox( vec3 p, vec3 b )
{
	float di = 0.0;
	float dx = abs(p.x)-b.x; if( dx>0.0 ) di+=dx*dx;
    float dy = abs(p.y)-b.y; if( dy>0.0 ) di+=dy*dy;
    float dz = abs(p.z)-b.z; if( dz>0.0 ) di+=dz*dz;
    return di;
}

//-------------------------------------------------------------------------------

float columna( vec3 pos, float offx )
{
    float x = pos.x;
    float y = pos.y;
    float z = pos.z;
    
	float y2=y-0.40;
    float y3=y-0.35;
    float y4=y-1.00;

    float di =    udSqBox( vec3(x, y , z), vec3(0.10, 1.00, 0.10) );
    di = min( di, udSqBox( vec3(x, y , z), vec3(0.12, 0.40, 0.12) ) );
    di = min( di, udSqBox( vec3(x, y , z), vec3(0.05, 0.35, 0.14) ) );
    di = min( di, udSqBox( vec3(x, y , z), vec3(0.14, 0.35, 0.05) ) );
    di = min( di, udSqBox( vec3(x, y4, z), vec3(0.14, 0.02, 0.14) ) );
    di = min( di, udSqBox( vec3((x-y2)*0.7071, (y2+x)*0.7071, z), vec3(0.10*0.7071,  0.10*0.7071, 0.12) ) );
    di = min( di, udSqBox( vec3(x, (y2+z)*0.7071, (z-y2)*0.7071), vec3(0.12,  0.10*0.7071, 0.1*0.7071) ) );
    di = min( di, udSqBox( vec3((x-y3)*0.7071, (y3+x)*0.7071, z), vec3(0.10*0.7071,  0.10*0.7071, 0.14) ) );
    di = min( di, udSqBox( vec3(x, (y3+z)*0.7071, (z-y3)*0.7071), vec3(0.14,  0.10*0.7071, 0.10*0.7071) ) );

    #if 1
    float fb = fbm(vec3(10.1*x+offx,10.1*y,10.1*z));
    if( fb>0.0 )
        di = di + 0.00000003*fb;
    #endif

    return sqrt(di);
}

float bicho( vec3 pos, float mindist )
{
    pos -= vec3(0.64,0.50,1.5);
    
    float r2 = dot2(pos);

	float sa = smoothstep(0.0,0.5,r2);
    pos *= vec3(0.75,0.80,0.75) + sa*vec3(0.25,0.20,0.25);
    r2 = dot2(pos);

    if( r2>5.0 ) return mindist;
    if( pos.y >0.5 ) return mindist;
	if( pos.y>-0.2 && dot(pos.xz,pos.xz)>0.6 ) return mindist;
    if( r2>(1.7+mindist)*(1.7+mindist)  ) return mindist;	// algo ayuda, poco, pero algo oiga

	float r = sqrt(r2);

	if( r<0.75 )
    {
	float a1 = 1.0-smoothstep( 0.0, 0.75, r );
	a1 *= 0.6;
	vec2 sico = vec2( sin(a1), cos(a1) );
	float nx = pos.x;
	float ny = pos.y;
	pos.x = nx*sico.y - ny*sico.x;
	pos.y = nx*sico.x + ny*sico.y;
    }
	#define TENTACURA 0.045
	float mindist2 = 100000.0;
    vec3  p = pos;
	float rr = 0.05+length(pos.xz);
	float ca = (0.5-TENTACURA*0.75) -6.0*rr*exp2(-10.0*rr);
    for( int j=1+ZERO; j<7; j++ )
	{
		float an = (6.2831/7.0) * float(j);
		float aa = an + 0.40*rr*noise(vec3(4.0*rr, 2.5, an)) + 0.29;
		float rc = cos(aa);
        float rs = sin(aa);
		vec3 q = vec3( p.x*rc-p.z*rs, p.y+ca, p.x*rs+p.z*rc );
		float dd = dot(q.yz,q.yz);
		if( q.x>0.0 && q.x<1.5 && dd<mindist2 ) mindist2=dd;
	}
	float dtentacles = sqrt(mindist2) - TENTACURA;

    // proto-smooth-minimum
	float dhead = r-0.15;

    float h = clamp( r*3.0, 0.0, 1.0 );
    return mix(dhead-0.15,dtentacles,h);
}

float techo2( float x, float y, float z )
{
    y = 1.0 - y;
    x = x - 0.5;
    z = z - 0.5;
    return 0.4 - sqrt(y*y+min(x*x,z*z) );
}

float map( vec3 pos, out uint sid, out uint submat )
{
    submat = 0u;
    float mindist;
    
    //-----------------------
    // suelo
    //-----------------------
    {
	float dis = pos.y;
	
	float ax = 128.0 + (pos.x+pos.z)*6.0;
	float az = 128.0 + (pos.x-pos.z)*6.0;
    uint ix = uint(floor(ax));
    uint iz = uint(floor(az));
    submat = ihash(ix+53u*iz);
    bool ba = ( ((submat>>10)&7u)>6u );
    float peld = max(fract(ax),fract(az));
    peld = smoothstep( 0.975, 1.0, peld );
    if( ba ) peld = 1.0;
    dis += 0.005*peld;
	mindist = dis;
    if( peld>0.0000001 ) sid = 2u; else sid = 0u;
    }
    
    //-----------------------
    // techo
    //-----------------------
	if( pos.y>1.0 ) 
    {
        float dis = techo2( fract(pos.x), pos.y, fract(pos.z) );
        if( dis<mindist )
        {
            mindist = dis;
            sid = 5u;
        }
    }

    //-----------------------
    // columnas
    //-----------------------
    {
	vec3 fc = vec3(fract( pos.x+0.5 )-0.5, pos.y,
	               fract( pos.z+0.5 )-0.5);
    float bb = udSqBox( fc, vec3(0.14, 1.0, 0.14) );
    if( bb < mindist*mindist )
    {
        float dis = columna( fc, 13.1*floor(pos.x)+17.7*floor(pos.z) );
        if( dis<mindist )
        {
            mindist = dis;
            sid = 1u;
        }
    }
    }
    
    //-----------------------
    // bicho
    //-----------------------
    //if( length( pos-vec3(0.64,-2.1,1.5) ) - 2.8 < mindist )
    if( dot2( pos-vec3(0.64,-2.1,1.5) ) < (mindist+2.8)*(mindist+2.8) )
    {
    float dis = bicho( pos, mindist );
    if( dis<mindist )
    {   
        mindist = dis;
        sid = 4u;
    }
    }

    //-----------------------
    // baldosas
    //-----------------------
    if( pos.y-0.02 < mindist )
	{
	int kkr = 65535;
    for( int i=ZERO; i<10; i++ )
    {
        float a = 3.14*sfrand(kkr);
        float co = cos(a);
        float si = sin(a);
        float xc = pos.x - (0.5 + 1.5*sfrand(kkr));
        float zc = pos.z - (1.0 + 1.0*sfrand(kkr));
        float xo = xc*co-si*zc;
        float zo = zc*co+si*xc;
        float dis = sdBox( vec3(xo, pos.y-0.008, zo), vec3(0.07, 0.008, 0.07) );
        if( dis < mindist ) 
        {
            mindist = dis;
            sid = 1u;
        }
    }
	}
    
    return mindist;
}

void raycast( vec3 ro, vec3 rd, float to, float tMax, out float resT, out vec3 resPos, out uint resSurface, out uint submat )
{
    float t = to;
    for( int i=ZERO; i<256; i++ )
	{
        resPos = ro + t*rd;
		float h = map( resPos, resSurface, submat );
		if( h<0.0001 )
		{
		    resT = t;
			return;
		}
        t += h*0.75;
        if( t>tMax ) break;
	}
    resSurface = 666u;
}

vec3 calcNormal( vec3 pos )
{
#if 0
    // original code
    const float eps = 0.0002;
    uint kk, kk2;
    vec3 nor = vec3(
      map( pos.x+eps, pos.y, pos.z, kk, kk2 ) - map( pos.x-eps, pos.y, pos.z, kk, kk2 ),
      map( pos.x, pos.y+eps, pos.z, kk, kk2 ) - map( pos.x, pos.y-eps, pos.z, kk, kk2 ),
      map( pos.x, pos.y, pos.z+eps, kk, kk2 ) - map( pos.x, pos.y, pos.z-eps, kk, kk2 ));
    return normalize(nor);
#else
    // inspired by tdhooper and klems - a way to prevent the compiler from inlining map() 4 times
    vec3 n = vec3(0.0);
    for( int i=ZERO; i<4; i++ )
    {
        uint kk, kk2;
        vec3 e = 0.5773*(2.0*vec3((((i+3)>>1)&1),((i>>1)&1),(i&1))-1.0);
        n += e*map(pos+0.0002*e, kk, kk2);
        if( n.x+n.y+n.z>100.0 ) break;
    }
    return normalize(n);
#endif    
}

vec3 shade( vec3 pos, vec3 rd, uint matID, uint subMatID, float dis )
{
    if( matID==666u ) return vec3(0.0,0.0,0.0);

    vec3 col = vec3(0.1*dis);

    // calc normal
    vec3 nor = calcNormal( pos );

    // bump mapping
    #if 1
    const float kke = 0.0001;
    float bumpa = 0.0075;
	if( matID!=5u ) bumpa *= 0.75;
    if( matID==4u ) bumpa *= 0.50;
    bumpa /= kke;
    float kk = fbm( 32.0*pos );
    nor.x += bumpa*(fbm( 32.0*vec3(pos.x+kke, pos.y, pos.z) )-kk);
    nor.y += bumpa*(fbm( 32.0*vec3(pos.x, pos.y+kke, pos.z) )-kk);
    nor.z += bumpa*(fbm( 32.0*vec3(pos.x, pos.y, pos.z+kke) )-kk);
    nor = normalize(nor);
    #endif


    // light
	float spe = 0.0;
    vec3 lig = vec3( 0.5, 0.8, 1.5 ) - pos;
    float llig = dot2(lig);
    float im = 1.0/sqrt(llig);
    lig *= im;
    float dif = dot( nor, lig ); 
    if( matID==4u )
        dif = 0.5+0.5*dif;
    else
        dif = 0.10+0.90*dif;
    dif = max( dif, 0.0 );
	dif *= 2.5*exp2( -1.75*llig );
	float dif2 = (nor.x+nor.y)*0.075;

    // materials
    if( matID==0u )
    {
        float xoff = 13.1 * float(subMatID&255u);
        float fb = fbm( 16.0*vec3(pos.x+xoff, pos.y, pos.z) );

        col = 0.7 + vec3(0.20,0.22,0.25)*fb;

        float baldscale = float((subMatID>>9)&15u)/14.0;
        baldscale = 0.51 + 0.34*baldscale;
        col *= baldscale;
        float fx = 1.0;
        if( (subMatID&256u)!=0u ) fx = -1.0;
        float m = sin( 64.0*pos.z*fx +  64.0*pos.x + 4.0*fb );
        m = smoothstep( 0.25, 0.5, m ) - smoothstep( 0.5, 0.75, m );

        col += m*0.15;
    }
    else if( matID==2u )
    {
        col = vec3(0.0,0.0,0.0);
    }
    else if( matID==1u )
    {
        float fb = fbm( 16.0*pos );
        float m = sin( 64.0*pos.z +  64.0*pos.x + 4.0*fb );
        m = smoothstep( 0.3, 0.5, m ) - smoothstep( 0.5, 0.7, m );
        col = 0.59 + fb*vec3(0.17,0.18,0.21) + m*0.15 + dif2;
    }   
    else if( matID==4u )
    {
        float ft = fbm( 16.0*pos );
        col = vec3(0.82,0.73,0.65) + ft*vec3(0.10,0.10,0.15);
        float fs = 0.90+0.10*fbm( 32.0*pos );
        col *= fs;

        float fre = -dot( nor, rd );
        if( fre<0.0 ) fre=0.0;
        fre = fre*fre*0.45;
        col -= fre;

		spe = (nor.y-nor.z)*0.707;
        spe = spe*spe;
        spe = spe*spe;
        spe = spe*spe;
        spe = spe*spe;
        spe = spe*spe;
        spe *= 0.20;
     }   
    // techo
    else //if( matID==5 )
    {
        float fb = fbm( 16.0*pos );
        col = vec3(0.64,0.61,0.59) + fb*vec3(0.21,0.19,0.19) + dif2;
    }   

    // AO
    float ao;
    #if 1
    float totao = 0.0;
    float sca = 10.0;
    for( int aoi=ZERO; aoi<5; aoi++ )
    {
        float hr = 0.01 + 0.015*float(aoi*aoi);
        vec3 aopos = pos + hr*nor;
        uint kk, kk2;
        float dd = map( aopos, kk, kk2 );
        ao = -(dd-hr);
        totao += ao*sca;
        sca *= 0.5;
        //if( totao>1000.0+sin(iTime) ) break;
    }
    ao = 1.0 - clamp( totao, 0.0, 1.0 );
    #else
    ao = 1.0;
    #endif

    // shadow
    #if 1
    float so = 0.0;
	for( int i=ZERO; i<6; i++ )
    {
		float h = float(i)/6.0;
        float hr = 0.01 + 1.0*h;
        vec3 aopos = pos + hr*lig;
        uint kk, kk2;
        float dd = map( aopos, kk, kk2 );
        so += (1.0-h)*dd*20.0/6.0;
        //if( so>1000.0+iTime ) break;
    }
    dif *= clamp( (so-0.40)*1.5, 0.0, 1.0 );
    #endif

    // shade
	float ao2 = 0.05 + 0.95*ao;
    col *= vec3(0.25,0.30,0.35)*ao + dif*ao2*vec3(1.95,1.65,1.05);
    col += spe;
    col *= exp2( -0.40*pos.z );

    return col;
}

vec3 post( vec3 col, vec2 p )
{
    // flare
	float rr = (p.x+0.10)*(p.x+0.10) + (p.y-0.40)*(p.y-0.40);
    col += vec3( 1.0, 0.95, 0.90 )*0.75*exp2( -64.0*rr );

    // color correct
    col = sqrt(col)*0.3 + 0.7*col;
    col *= vec3(0.83,1.00,0.83);

    // vignetting
    col *= 0.25 + 0.75*clamp( 0.60*abs(p.x-1.77)*abs(p.x+1.77), 0.0, 1.0 );
    
    return clamp( col, 0.0, 1.0 );
}

vec3 computePixel( vec2 p, vec2 res, float time )
{
    vec2 s = (2.0*p-res)/res.y;
    
    // fisheye distortion
    float r2 = s.x*s.x*0.32 + s.y*s.y;
	float tt = (7.0-sqrt(37.5-11.5*r2))/(r2+1.0);

    // create camera and ray
    vec2 d = s*tt;
#if 0
    vec3 ro = vec3( 0.195, 0.5, 0.0 );
	vec3 rd = normalize( vec3( d.x*0.955336 + 0.29552, d.y, 0.955336 - d.x*0.29552 ) );
#else
    float an = 4.4 + 0.2*sin(0.4*time);
    vec3 ta = vec3(0.64,0.50,1.5);
    vec3 ro = ta + 1.55*vec3( cos(an), 0.0, sin(an));
    vec3 ww = normalize( ta-ro );
    vec3 vv = vec3(0.0,1.0,0.0);
    vec3 uu = normalize(cross(vv,ww));
    vec3 rd = normalize( uu*d.x + vv*d.y + 1.0*ww );
#endif
	float t;
	uint matID, sumMatID;
	vec3 xyz;
    
    // raycast
	raycast( ro, rd, 0.5, 24.0, t, xyz, matID, sumMatID );
    
    // shade
	vec3 col = shade(  xyz, rd, matID, sumMatID, t  );
    
    // postprocess
	col = post( col, s );

	return col;
}


void main()
{
#if AA<2    
    vec3 col = computePixel(fragCoord, iResolution.xy, iTime );
#else
    vec3 col = vec3(0.0);
    for( int j=0; j<AA; j++ )
    for( int i=0; i<AA; i++ )
    {
        vec2 of = vec2(i,j)/float(AA);
        float time = iTime + (0.5/30.0)*float(j*AA+i)/float(AA*AA);
        col += computePixel(fragCoord+of, iResolution.xy, time );
    }
    col /= float(AA*AA);
#endif
    fragColor = vec4(col,1.0);
}