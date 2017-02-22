#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

static const EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE
};


static const int pbufferWidth = 9;
static const int pbufferHeight = 9;

static const EGLint pbufferAttribs[] = {
        EGL_WIDTH, pbufferWidth,
        EGL_HEIGHT, pbufferHeight,
        EGL_NONE,
};

int main(int argc, char *argv[])
{
    // 1. Initialize EGL
    EGLDisplay eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (eglDpy == EGL_NO_DISPLAY) {
	printf("eglGetDisplay returns EGL_NO_DISPLAY");
	exit(1);
    }
    static const int MAX_DEVICES = 4;
    EGLDeviceEXT eglDevs[MAX_DEVICES];
    EGLint numDevices;

    PFNEGLQUERYDEVICESEXTPROC eglQueryDevicesEXT =
            (PFNEGLQUERYDEVICESEXTPROC)
                    eglGetProcAddress("eglQueryDevicesEXT");
    eglQueryDevicesEXT(MAX_DEVICES, eglDevs, &numDevices);
    //printf("Detected %i devices\n", numDevices);

    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT =
            (PFNEGLGETPLATFORMDISPLAYEXTPROC)
                    eglGetProcAddress("eglGetPlatformDisplayEXT");

    eglDpy = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT,
                                          eglDevs[0], 0);
    EGLint major, minor;

    eglInitialize(eglDpy, &major, &minor);

    // 2. Select an appropriate configuration
    EGLint numConfigs;
    EGLConfig eglCfg;

    eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);

    // 3. Create a surface
    EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg,
                                                 pbufferAttribs);

    // 4. Bind the API
    eglBindAPI(EGL_OPENGL_API);

    // 5. Create a context and make it current
    EGLContext eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT,
                                         NULL);

    if (eglCtx == EGL_NO_CONTEXT) {
        perror("eglCreateContext: No context returned.\n");
        return EXIT_FAILURE;
    }
    if (! eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx)) {
        printf("eglMakeCurrent failed. EGL_FALSE\n");
/*
        // Try eglInitialize on a different display.
        if( eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx) == EGL_FALSE) {
            //fprintf(stderr, "eglMakeCurrent failed. EGL_FALSE\n");
            eglDpy = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT,
                                              eglDevs[1], 0);
            eglInitialize(eglDpy, &major, &minor);
            EGLint numConfigs;
            EGLConfig eglCfg;

            eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);

            // 3. Create a surface
            EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg,
                                                         pbufferAttribs);

            // 4. Bind the API
            eglBindAPI(EGL_OPENGL_API);

            // 5. Create a context and make it current
            EGLContext eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT,
                                                 NULL);


            if (! eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx)) {
                fprintf(stderr, "eglMakeCurrent failed twice.\n");
            } else {
                fprintf(stderr, "eglMakeCurrent success on display 2.\n");
	    }
 */       
    } else {
        //fprintf(stderr, "eglMakeCurrent success on display 0.\n");
    }
    // from now on use your OpenGL context

    // 6. Terminate EGL when finished
    //eglTerminate(eglDpy);

    while(1) {
	usleep(1000);
    }

    return 0;
}
