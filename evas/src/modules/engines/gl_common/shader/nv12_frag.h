"#ifdef GL_ES\n"
"precision highp float;\n"
"#endif\n"
"uniform sampler2D tex, texuv;\n"
"varying vec4 col;\n"
"varying vec2 tex_c, tex_cuv;\n"
"void main()\n"
"{\n"
"  float y,u,v,vmu,r,g,b;\n"
"  y=texture2D(tex,tex_c).g;\n"
"  u=texture2D(texuv,tex_cuv).g;\n"
"  v=texture2D(texuv,tex_cuv).a;\n"

"  u=u-0.5;\n"
"  v=v-0.5;\n"
"  vmu=v*0.813+u*0.391;\n"
"  u=u*2.018;\n"
"  v=v*1.596;\n"
"  y=(y-0.062)*1.164;\n"

"  r=y+v;\n"
"  g=y-vmu;\n"
"  b=y+u;\n"

"  gl_FragColor=vec4(r,g,b,1.0) * col;\n"
"}\n"

