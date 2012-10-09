/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//#define LOG_NDEBUG 0
#define LOG_TAG "DisplayManagerService"

#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"

#include <utils/misc.h>
#include <utils/Log.h>
#include <hardware/hardware.h>
#include <hardware/display.h>
#include <gui/SurfaceComposerClient.h>
#include <ui/DisplayDispatcher.h>
#include <stdio.h>

namespace android
{
    struct DisplayFieldIds
    {
        // members
        jfieldID mDisplayOpen0;
        jfieldID mDisplayOpen1;
        jfieldID mDisplayMaster;
        jfieldID mDisplayMode;
        jfieldID mDisplayPixelFormat0;
        jfieldID mDisplayPixelFormat1;
        jfieldID mDisplayType0;
        jfieldID mDisplayType1;
        jfieldID mDisplayFormat0;
        jfieldID mDisplayFormat1;
    };

    static DisplayFieldIds gFieldIds;

    class NativeDisplayManager: public virtual RefBase
    {
        public:
            NativeDisplayManager();
            ~NativeDisplayManager();

            int      changeDisplayMode(int displayno, int value0,int value1);
            int      setDisplayParameter(int displayno, int value0,int value1);
            int      setDisplayMode(int mode);
            int        openDisplay(int displayno);
            int     closeDisplay(int displayno);
            int     getHdmiStatus(void);
            int     getDisplayParameter(int displayno, int param);
            int     setMasterDisplay(int displayno);
            int     getMasterDisplay();
            int     getDisplayMode();
            int     getDisplayCount();
            int        getMaxWidthDisplay();
            int     getMaxHdmiMode();

            bool     mDisplayOpen0;
            bool     mDisplayOpen1;
            int     mDisplayMaster;
            int     mDisplayMode;
            int     mDisplayPixelFormat0;
            int     mDisplayPixelFormat1;
            int     mDisplayType0;
            int     mDisplayType1;
            int     mDisplayFormat0;
            int     mDisplayFormat1;
        private:
            hw_module_t*         disp_module;
            display_device_t*    disp_device;
    };

    NativeDisplayManager::NativeDisplayManager()
    {
        int                 err;

        err = hw_get_module(DISPLAY_HARDWARE_MODULE_ID, (hw_module_t const**)&disp_module);
        if (err == 0)
        {
            err = display_open(disp_module, &disp_device);
            if (err == 0) {
                ALOGE("Open Display Device Failed!\n");
            }
        }
    }

    NativeDisplayManager::~NativeDisplayManager()
    {
        int                 err;

        if(disp_device)
        {
            display_close(disp_device);
        }
    }

    int NativeDisplayManager::changeDisplayMode(int displayno, int value0,int value1)
    {
	#ifdef ALLWINNER_HARDWARE
        return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_CHANGEDISPMODE,displayno,value0,value1);
	#else
	return 0;
	#endif
    }

    int NativeDisplayManager::setDisplayParameter(int displayno, int value0,int value1)
    {
        if(displayno == 0)
        {
            if(value0 == DISPLAY_DEVICE_LCD)
            {
                mDisplayType0     = DISPLAY_DEVICE_LCD;
            }
            else if(value0 == DISPLAY_DEVICE_HDMI)
            {
                mDisplayType0     = DISPLAY_DEVICE_HDMI;
                mDisplayFormat0 = value1;
            }
            else if(value0 == DISPLAY_DEVICE_VGA)
            {
                mDisplayType0     = DISPLAY_DEVICE_VGA;
                mDisplayFormat0 = value1;
            }
            else
            {
                mDisplayPixelFormat0 = value1;
            }
        }
        else
        {
            if(value0 == DISPLAY_DEVICE_LCD)
            {
                mDisplayType1     = DISPLAY_DEVICE_LCD;
            }
            else if(value0 == DISPLAY_DEVICE_HDMI)
            {
                mDisplayType1     = DISPLAY_DEVICE_HDMI;
                mDisplayFormat1 = value1;
            }
            else
            {
                mDisplayPixelFormat1 = value1;
            }
        }
        #ifdef ALLWINNER_HARDWARE
        return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_SETDISPPARA,displayno,value0,value1);
        #else
        return 0;
        #endif
    }

    int NativeDisplayManager::setDisplayMode(int mode)
    {
        #ifdef ALLWINNER_HARDWARE
        return SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_SETDISPMODE,mode,0,0);
        #else
        return 0;
        #endif
    }

    int NativeDisplayManager::openDisplay(int displayno)
    {
        if(displayno == 0)
        {
            mDisplayOpen0 = true;
        }
        else
        {
            mDisplayOpen1 = true;
        }

        #ifdef ALLWINNER_HARDWARE
        return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_OPENDISP,displayno,0,0);
        #else
        return 0;
        #endif
    }

    int NativeDisplayManager::closeDisplay(int displayno)
    {
        if(displayno == 0)
        {
            mDisplayOpen0 = false;
        }
        else
        {
            mDisplayOpen1 = false;
        }

        #ifdef ALLWINNER_HARDWARE
        return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_CLOSEDISP,displayno,0,0);
        #else
        return 0;
        #endif
    }

    int NativeDisplayManager::getHdmiStatus(void)
    {
        if(disp_device)
        {
            return  disp_device->gethdmistatus(disp_device);
        }

        return  -1;
    }

    int NativeDisplayManager::getDisplayParameter(int displayno, int param)
    {
        #ifdef ALLWINNER_HARDWARE
        return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETDISPPARA,displayno,param,0);
        #else
        return 0;
        #endif
    }

    int NativeDisplayManager::setMasterDisplay(int displayno)
    {
        #ifdef ALLWINNER_HARDWARE
        return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_SETMASTERDISP,displayno,0,0);
        #else
        return 0;
        #endif
    }

    int NativeDisplayManager::getMasterDisplay()
    {
        #ifdef ALLWINNER_HARDWARE
        return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETMASTERDISP,0,0,0);
        #else
        return 0;
        #endif
    }

    int NativeDisplayManager::getMaxWidthDisplay()
    {
        #ifdef ALLWINNER_HARDWARE
        return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETMAXWIDTHDISP,0,0,0);
        #else
        return 0;
        #endif
    }

    int NativeDisplayManager::getMaxHdmiMode()
    {
        #ifdef ALLWINNER_HARDWARE
        return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETMAXHDMIMODE,0,0,0);
        #else
        return 0;
        #endif
    }

    int NativeDisplayManager::getDisplayMode()
    {
        #ifdef ALLWINNER_HARDWARE
        return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETDISPLAYMODE,0,0,0);
        #else
        return 0;
        #endif
    }

    int NativeDisplayManager::getDisplayCount()
    {
        #ifdef ALLWINNER_HARDWARE
        return  SurfaceComposerClient::setDisplayProp(DISPLAY_CMD_GETDISPCOUNT,0,0,0);
        #else
        return 0;
        #endif
    }

    static sp<NativeDisplayManager> gNativeDisplayManager;

    static bool checkDisplayManagerUnitialized(JNIEnv* env)
    {
        if (gNativeDisplayManager == NULL)
        {
            ALOGE("Display manager not initialized.");

            jniThrowRuntimeException(env, "Display manager not initialized.");

            return true;
        }
        return false;
    }

    static void init_native(JNIEnv *env, jobject clazz)
    {
        if(gNativeDisplayManager == NULL)
        {
            gNativeDisplayManager = new NativeDisplayManager();
        }
        else
        {
            ALOGE("Native Display manager already initialized.");
            jniThrowRuntimeException(env, "Display manager already initialized.");
        }
    }

    static void finalize_native(JNIEnv *env, jobject clazz)
    {
        /* STUB */
    }

    static void updateStatus(JNIEnv *env, jobject clazz)
    {
        env->SetIntField(clazz, gFieldIds.mDisplayFormat0, gNativeDisplayManager->mDisplayFormat0);
        env->SetIntField(clazz, gFieldIds.mDisplayFormat1, gNativeDisplayManager->mDisplayFormat1);
        env->SetIntField(clazz, gFieldIds.mDisplayType0, gNativeDisplayManager->mDisplayType0);
        env->SetIntField(clazz, gFieldIds.mDisplayType1, gNativeDisplayManager->mDisplayType1);
        env->SetIntField(clazz, gFieldIds.mDisplayPixelFormat0, gNativeDisplayManager->mDisplayPixelFormat0);
        env->SetIntField(clazz, gFieldIds.mDisplayPixelFormat1, gNativeDisplayManager->mDisplayPixelFormat1);
        env->SetIntField(clazz, gFieldIds.mDisplayMaster, gNativeDisplayManager->mDisplayMaster);
        env->SetIntField(clazz, gFieldIds.mDisplayMode, gNativeDisplayManager->mDisplayMode);
        env->SetBooleanField(clazz, gFieldIds.mDisplayOpen0, gNativeDisplayManager->mDisplayOpen0);
        env->SetBooleanField(clazz, gFieldIds.mDisplayOpen1, gNativeDisplayManager->mDisplayOpen1);
    }

    static jint changeMode_native(JNIEnv *env, jobject clazz,int displayno, int value0,int value1)
    {
        int   ret;

        //ALOGE("Native Display manager already initialized.");
        if (checkDisplayManagerUnitialized(env))
        {
            return -1;
        }

        ALOGE("changeMode_native.");

        ret = gNativeDisplayManager->changeDisplayMode(displayno,value0,value1);
        if(ret == 0)
        {
            updateStatus(env, clazz);
        }

        return  (jint)ret;
    }

    static jint setDisplayParameter_native(JNIEnv *env, jobject clazz,int displayno, int value0,int value1)
    {
        int   ret;

        //ALOGE("Native Display manager already initialized.");

        if (checkDisplayManagerUnitialized(env))
        {
            return -1;
        }

        ALOGE("setDisplayParameter_native.");

        ret = gNativeDisplayManager->setDisplayParameter(displayno,value0,value1);
        if(ret == 0)
        {
            updateStatus(env, clazz);
        }

        return  (jint)ret;
    }

    static jint setDisplayMode_native(JNIEnv *env, jobject clazz,int mode)
    {
        int   ret;

        //ALOGE("Native Display manager already initialized.");

        if (checkDisplayManagerUnitialized(env))
        {
            return -1;
        }

        ALOGE("setDisplayMode_native.");

        ret = gNativeDisplayManager->setDisplayMode(mode);
        if(ret == 0)
        {
            updateStatus(env, clazz);
        }

        return  (jint)ret;
    }

    static jint openDisplay_native(JNIEnv *env, jobject clazz,int displayno)
    {
        int   ret;

        //ALOGE("Native Display manager already initialized.");

        if (checkDisplayManagerUnitialized(env))
        {
            return -1;
        }

        ALOGE("openDisplay_native.");

        ret = gNativeDisplayManager->openDisplay(displayno);
        if(ret == 0)
        {
            updateStatus(env, clazz);
        }

        return  (jint)ret;
    }


    static jint closeDisplay_native(JNIEnv *env, jobject clazz, int displayno)
    {
        int   ret;

        //ALOGE("Native Display manager already initialized.");

        if (checkDisplayManagerUnitialized(env))
        {
            return -1;
        }

        ALOGE("closeDisplay_native.");

        ret = gNativeDisplayManager->closeDisplay(displayno);
        if(ret == 0)
        {
            updateStatus(env, clazz);
        }

        return  (jint)ret;
    }


    static jint getHdmiStatus_native(JNIEnv *env, jobject clazz)
    {
        int   ret;

        //ALOGE("Native Display manager already initialized.");

        if (checkDisplayManagerUnitialized(env))
        {
            return -1;
        }

       // ALOGE("getHdmiStatus_native.");

        return  (jint)gNativeDisplayManager->getHdmiStatus();
    }

    static jint getMaxHdmiMode_native(JNIEnv *env, jobject clazz)
    {
        int   ret;

        //ALOGE("Native Display manager already initialized.");

        if (checkDisplayManagerUnitialized(env))
        {
            return -1;
        }

       // ALOGE("getHdmiStatus_native.");

        return  (jint)gNativeDisplayManager->getMaxHdmiMode();
    }


    static jint getDisplayParameter_native(JNIEnv *env, jobject clazz,int displayno, int param)
    {
        int   ret;

        //ALOGE("Native Display manager already initialized.");

        if (checkDisplayManagerUnitialized(env))
        {
            return -1;
        }

        //ALOGE("getDisplayParameter_native.");

        return  (jint)gNativeDisplayManager->getDisplayParameter(displayno,param);
    }

    static jint getDisplayMode_native(JNIEnv *env, jobject clazz)
    {
        int   ret;

        //ALOGE("Native Display manager already initialized.");

        if (checkDisplayManagerUnitialized(env))
        {
            return -1;
        }

        //ALOGE("getDisplayMode_native.");

        return  (jint)gNativeDisplayManager->getDisplayMode();
    }

    static jint getDisplayCount_native(JNIEnv *env, jobject clazz)
    {
        int   ret;

        //ALOGE("Native Display manager already initialized.");

        if (checkDisplayManagerUnitialized(env))
        {
            return -1;
        }

        //ALOGE("getDisplayCount_native.");

        return  (jint)gNativeDisplayManager->getDisplayCount();
    }

    static jint setDisplayMaster_native(JNIEnv *env, jobject clazz,int displayno)
    {
        int   ret;

        //ALOGE("Native Display manager already initialized.");

        if (checkDisplayManagerUnitialized(env))
        {
            return -1;
        }

        ALOGE("setDisplayMaster_native.");

        ret = gNativeDisplayManager->setMasterDisplay(displayno);
        if(ret == 0)
        {
            updateStatus(env, clazz);
        }

        return  (jint)ret;
    }

    static jint getDisplayMaster_native(JNIEnv *env, jobject clazz)
    {
        int   ret;

        //ALOGE("Native Display manager already initialized.");

        if (checkDisplayManagerUnitialized(env))
        {
            return -1;
        }

        //ALOGE("getDisplayMaster_native.");

        return  (jint)gNativeDisplayManager->getMasterDisplay();
    }

    static jint getMaxWidthDisplay_native(JNIEnv *env, jobject clazz)
    {
        int   ret;

        //ALOGE("Native Display manager already initialized.");

        if (checkDisplayManagerUnitialized(env))
        {
            return -1;
        }

        //ALOGE("getDisplayMaster_native.");

        return  (jint)gNativeDisplayManager->getMaxWidthDisplay();
    }

    static jint getDisplayOutputType_native(JNIEnv *env, jobject clazz,int displayno)
    {
        int   ret;

        //ALOGE("Native Display manager already initialized.");

        if (checkDisplayManagerUnitialized(env))
        {
            return -1;
        }

        //ALOGE("getDisplayOutputType_native.");

        return  (jint)gNativeDisplayManager->getDisplayParameter(displayno,DISPLAY_OUTPUT_TYPE);
    }

    static jint getDisplayOutputFormat_native(JNIEnv *env, jobject clazz,int displayno)
    {
        int   ret;

        //ALOGE("Native Display manager already initialized.");

        if (checkDisplayManagerUnitialized(env))
        {
            return -1;
        }

        //ALOGE("getDisplayOutputFormat_native.");

        return  (jint)gNativeDisplayManager->getDisplayParameter(displayno,DISPLAY_OUTPUT_FORMAT);
    }

    static jint getDisplayOutputWidth_native(JNIEnv *env, jobject clazz,int displayno)
    {
        int   ret;

        //ALOGE("Native Display manager already initialized.");

        if (checkDisplayManagerUnitialized(env))
        {
            return -1;
        }

        //ALOGE("getDisplayOutputWidth_native.");

        return  (jint)gNativeDisplayManager->getDisplayParameter(displayno,DISPLAY_OUTPUT_WIDTH);
    }

    static jint getDisplayOutputHeight_native(JNIEnv *env, jobject clazz,int displayno)
    {
        int   ret;

        //ALOGE("Native Display manager already initialized.");

        if (checkDisplayManagerUnitialized(env))
        {
            return -1;
        }

        //ALOGE("getDisplayOutputHeight_native.");

        return  (jint)gNativeDisplayManager->getDisplayParameter(displayno,DISPLAY_OUTPUT_HEIGHT);
    }

    static jint getDisplayOutputPixelFormat_native(JNIEnv *env, jobject clazz,int displayno)
    {
        int   ret;

        //ALOGE("Native Display manager already initialized.");

        if (checkDisplayManagerUnitialized(env))
        {
            return -1;
        }

        //ALOGE("getDisplayOutputPixelFormat_native.");

        return  (jint)gNativeDisplayManager->getDisplayParameter(displayno,DISPLAY_OUTPUT_PIXELFORMAT);
    }

    static jint getDisplayOutputOpen_native(JNIEnv *env, jobject clazz,int displayno)
    {
        int   ret;

        //ALOGE("Native Display manager already initialized.");

        if (checkDisplayManagerUnitialized(env))
        {
            return -1;
        }

        //ALOGE("getDisplayOutputOpen_native.");

        return  (jint)gNativeDisplayManager->getDisplayParameter(displayno,DISPLAY_OUTPUT_ISOPEN);
    }

    static jint getDisplayOutputHotPlug_native(JNIEnv *env, jobject clazz,int displayno)
    {
        int   ret;

        //ALOGE("Native Display manager already initialized.");

        if (checkDisplayManagerUnitialized(env))
        {
            return -1;
        }

        //ALOGE("getDisplayOutputHotPlug_native!\n");

        return  (jint)gNativeDisplayManager->getDisplayParameter(displayno,DISPLAY_OUTPUT_HOTPLUG);
    }

static JNINativeMethod method_table[] = {
    { "nativeInit", "()V", (void*)init_native },
    { "nativeSetDisplayOutputType", "(III)I", (void*)changeMode_native },
    { "nativeSetDisplayParameter", "(III)I", (void*)setDisplayParameter_native },
    { "nativeSetDisplayMode", "(I)I", (void*)setDisplayMode_native },
    { "nativeOpenDisplay", "(I)I", (void*)openDisplay_native },
    { "nativeCloseDisplay", "(I)I", (void*)closeDisplay_native },
    { "nativeGetHdmiHotPlug", "()I", (void*)getHdmiStatus_native },
    { "nativeGetDisplayMode", "()I", (void*)getDisplayMode_native },
    { "nativeGetDisplayCount", "()I", (void*)getDisplayCount_native },
    { "nativeSetDisplayMaster", "(I)I", (void*)setDisplayMaster_native },
    { "nativeGetDisplayMaster", "()I", (void*)getDisplayMaster_native },
    { "nativeGetMaxWidthDisplay", "()I", (void*)getMaxWidthDisplay_native },
    { "nativeGetMaxHdmiMode", "()I", (void*)getMaxHdmiMode_native },
    { "nativeGetDisplayOutputType", "(I)I", (void*)getDisplayOutputType_native },
    { "nativeGetDisplayOutputFormat", "(I)I", (void*)getDisplayOutputFormat_native},
    { "nativeGetDisplayWidth", "(I)I", (void*)getDisplayOutputWidth_native },
    { "nativeGetDisplayHeight", "(I)I", (void*)getDisplayOutputHeight_native },
    { "nativeGetDisplayPixelFormat", "(I)I", (void*)getDisplayOutputPixelFormat_native },
    { "nativeGetDisplayOpen", "(I)I", (void*)getDisplayOutputOpen_native },
    { "nativeGetDisplayHotPlug", "(I)I", (void*)getDisplayOutputHotPlug_native },
};

int register_android_server_DisplayManagerService(JNIEnv *env)
{
    jclass clazz = env->FindClass("com/android/server/DisplayManagerService");

    if (clazz == NULL)
    {
        ALOGE("Can't find com/android/server/DisplayManagerService");

        return -1;
    }

    gFieldIds.mDisplayMode             = env->GetFieldID(clazz, "mDisplayMode", "I");
    gFieldIds.mDisplayOpen0         = env->GetFieldID(clazz, "mDisplayOpen0", "Z");
    gFieldIds.mDisplayOpen1         = env->GetFieldID(clazz, "mDisplayOpen1", "Z");
    gFieldIds.mDisplayMaster         = env->GetFieldID(clazz, "mDisplayMaster", "I");
    gFieldIds.mDisplayPixelFormat0     = env->GetFieldID(clazz, "mDisplayPixelFormat0", "I");
    gFieldIds.mDisplayPixelFormat1     = env->GetFieldID(clazz, "mDisplayPixelFormat1", "I");
    gFieldIds.mDisplayType0         = env->GetFieldID(clazz, "mDisplayType0", "I");
    gFieldIds.mDisplayType1         = env->GetFieldID(clazz, "mDisplayType1", "I");
    gFieldIds.mDisplayFormat0         = env->GetFieldID(clazz, "mDisplayFormat0", "I");
    gFieldIds.mDisplayFormat1         = env->GetFieldID(clazz, "mDisplayFormat1", "I");

    return jniRegisterNativeMethods(env, "com/android/server/DisplayManagerService",
            method_table, NELEM(method_table));
}

};
