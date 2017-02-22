#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


pthread_mutex_t mutex;

struct my_EGL {
    EGLDisplay *eglDpy;
    EGLSurface *eglSurf;
    EGLConfig *eglCfg;
};

  static const EGLint configAttribs[] = {
          EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
          EGL_BLUE_SIZE, 8,
          EGL_GREEN_SIZE, 8,
          EGL_RED_SIZE, 8,
          EGL_DEPTH_SIZE, 8,
          EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
          EGL_NONE
  };    


  static const int pbufferWidth = 1;
  static const int pbufferHeight = 1;

  static const EGLint pbufferAttribs[] = {

        EGL_WIDTH, pbufferWidth,
        EGL_HEIGHT, pbufferHeight,
        EGL_NONE,
  };


/* this function is run by the second thread */
void *make_current(void *ptr)
{
    my_EGL *myegl = (my_EGL*)ptr;
    EGLDisplay eglDpy = *(myegl->eglDpy);
    EGLSurface eglSurf = *(myegl->eglSurf);
    EGLConfig eglCfg = *(myegl->eglCfg);

    EGLContext eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT,
                                         NULL);
    while(1) {

        pthread_mutex_lock(&mutex);
        if (! eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx)){
            EGLint error = eglGetError();
            printf("FAILED!! %x\n",error);
            pthread_mutex_unlock(&mutex);
            return NULL;
        }
        eglMakeCurrent(eglDpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
  // 1. Initialize EGL
    //printf("%d",EGL_DEFAULT_DISPLAY);
  //EGLDisplay eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    static const int MAX_DEVICES = 4;
    EGLDeviceEXT eglDevs[MAX_DEVICES];
    EGLint numDevices;

    PFNEGLQUERYDEVICESEXTPROC eglQueryDevicesEXT =
            (PFNEGLQUERYDEVICESEXTPROC)
                    eglGetProcAddress("eglQueryDevicesEXT");
    eglQueryDevicesEXT(MAX_DEVICES, eglDevs, &numDevices);
    printf("Detected %i devices\n", numDevices);

        PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT =
                (PFNEGLGETPLATFORMDISPLAYEXTPROC)
                        eglGetProcAddress("eglGetPlatformDisplayEXT");

    EGLDisplay eglDpy = eglGetPlatformDisplayEXT(EGL_PLATFORM_DEVICE_EXT,
                                      eglDevs[0], 0);

    EGLint numConfigs;
    EGLConfig eglCfg;

    my_EGL myegl;

    EGLint major, minor;
    if (!eglInitialize(eglDpy, &major, &minor))
        printf("Could not initialize.");

    eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);

    eglBindAPI(EGL_OPENGL_API);

    EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg,
                                                 pbufferAttribs);

    pthread_mutex_init(&mutex, NULL);

    pthread_t ctx1_looper;
    pthread_t ctx2_looper;


    myegl.eglSurf=&eglSurf;
    myegl.eglDpy=&eglDpy;
    myegl.eglCfg=&eglCfg;


    //make_current((void *) &myegl);
    pthread_create(&ctx1_looper, NULL, make_current, (void *) &myegl);
    pthread_create(&ctx2_looper, NULL, make_current, (void *) &myegl);
    pthread_join(ctx1_looper, NULL);
    pthread_join(ctx2_looper, NULL);

  eglTerminate(eglDpy);
  return 0;
}
