/*
Copyright 2022 Lee Taek Hee (Tech University of Korea)
This program is free software: you can redistribute it and/or modify
it under the terms of the What The Hell License. Do it plz.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY.
*/
#include "stdafx.h"
#include "Renderer.h"
#include "PrototypeWorld.h"
#include "Dependencies/freeglut.h"
#include <chrono>
#include <iostream>
#include <memory>
#include <cctype>

namespace {
    std::unique_ptr<Renderer> renderer;
    PrototypeWorld world;
    bool keys[256] = {}, arrows[256] = {}, paused = false;
    int width=1280, height=720;
    auto previous=std::chrono::steady_clock::now();
    void ClearInput() {
        for(bool& k:keys) k=false;
        for(bool& k:arrows) k=false;
    }
    void Close() { renderer.reset(); }
    void Display() {
        if (!renderer) return;
        world.Draw(*renderer,width,height);
        glutSwapBuffers();
    }
    void Resize(int w,int h) {
        width=w>0?w:1; height=h>0?h:1;
        if(renderer) renderer->Resize(width,height);
    }
    void KeyDown(unsigned char key,int,int) {
        key=static_cast<unsigned char>(std::tolower(key));
        if(keys[key]) return;
        keys[key]=true;
        if(key==27) { glutLeaveMainLoop(); return; }
        if(!world.Started() && key>='1' && key<='2') world.Select(key-'1');
        if(key=='p') { paused=!paused; world.SetPaused(paused); }
        if(key=='r') world.Reset();
        if(renderer && key=='[') {
            auto& exposure=renderer->Effects().exposure;
            exposure-=.1f; if(exposure<.1f) exposure=.1f;
        }
        if(renderer && key==']') {
            auto& exposure=renderer->Effects().exposure;
            exposure+=.1f; if(exposure>5.f) exposure=5.f;
        }
    }
    void KeyUp(unsigned char key,int,int) { keys[static_cast<unsigned char>(std::tolower(key))]=false; }
    void SpecialDown(int key,int,int) {
        if(key<0 || key>=256 || arrows[key]) return;
        arrows[key]=true;
        if(!renderer) return;
        auto& fx=renderer->Effects();
        if(key==GLUT_KEY_F1) fx.enabled=!fx.enabled;
        if(key==GLUT_KEY_F2) fx.bloom=!fx.bloom;
        if(key==GLUT_KEY_F3) fx.vignette=!fx.vignette;
        if(key==GLUT_KEY_F4) fx.edgeBlur=!fx.edgeBlur;
    }
    void SpecialUp(int key,int,int) { if(key>=0 && key<256) arrows[key]=false; }
    void Entry(int state) { if(state==GLUT_LEFT) ClearInput(); }
    void Timer(int) {
        if(!renderer) return;
        auto now=std::chrono::steady_clock::now();
        float dt=std::chrono::duration<float>(now-previous).count(); previous=now;
        if(dt>.05f) dt=.05f;
        bool focused=GetForegroundWindow()==GetActiveWindow();
        if(!focused) ClearInput();
        float dx=float(keys['d']||arrows[GLUT_KEY_RIGHT])-float(keys['a']||arrows[GLUT_KEY_LEFT]);
        float dy=float(keys['s']||arrows[GLUT_KEY_DOWN])-float(keys['w']||arrows[GLUT_KEY_UP]);
        bool sprint=focused && (GetAsyncKeyState(VK_SHIFT)&0x8000)!=0;
        world.SetPaused(paused || !focused);
        world.Update(dt,dx,dy,sprint);
        glutPostRedisplay(); glutTimerFunc(16,Timer,0);
    }
}
int main(int argc,char** argv)
{
    glutInit(&argc,argv);
    glutInitContextVersion(3,3);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize(width,height);
    int window=glutCreateWindow("The Long War - Rendering Prototype");
    if(window<=0) return 1;
    glewExperimental=GL_TRUE;
    if(glewInit()!=GLEW_OK || !GLEW_VERSION_3_3) {
        std::cerr<<"OpenGL 3.3 compatibility support is required.\n";
        glutDestroyWindow(window); return 1;
    }
    renderer.reset(new Renderer(width,height));
    if(!renderer->IsInitialized() || !renderer->LoadCharacterSprites()) {
        std::cerr<<"Renderer or character sprite initialization failed. See the asset error above.\n";
        renderer.reset(); glutDestroyWindow(window); return 1;
    }
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);
    glutCloseFunc(Close);
    glutDisplayFunc(Display); glutReshapeFunc(Resize);
    glutKeyboardFunc(KeyDown); glutKeyboardUpFunc(KeyUp);
    glutSpecialFunc(SpecialDown); glutSpecialUpFunc(SpecialUp);
    glutEntryFunc(Entry); glutIgnoreKeyRepeat(1);
    world.Reset(); previous=std::chrono::steady_clock::now();
    glutTimerFunc(0,Timer,0);
    std::cout<<"Choose 1/2. Move: WASD/arrows. Run: Shift. Pause: P. Home: R. Exit: Esc.\n";
    std::cout<<"Post FX: F1 all, F2 bloom, F3 vignette, F4 edge blur, [/] exposure.\n";
    glutMainLoop();
    // Freeglut close callback releases GPU objects before context destruction.
    return 0;
}
