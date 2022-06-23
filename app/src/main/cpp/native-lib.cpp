#include <jni.h>
#include <string>
#include "Log.h"

const char *APP_PACKAGE_NAME = "com.wangxingxing.secretkeysave";

// 验证是否通过
static jboolean auth = JNI_FALSE;

extern "C" JNIEXPORT jstring JNICALL
Java_com_wangxingxing_secretkeysave_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

//动态  效率高
//    反编译  安全性高
void regist(JNIEnv *env, jobject thiz, jobject jCallback) {
    LOGD("--动态注册调用成功-->");
}

jint RegisterNatives(JNIEnv *env) {
    jclass activityClass = env->FindClass("com/wangxingxing/secretkeysave/MainActivity");
    if (activityClass == NULL) {
        return JNI_ERR;
    }
    JNINativeMethod methods_MainActivity[] = {
            {
                    "setAntiBiBCallback",
                    "(Lcom/wangxingxing/secretkeysave/IAntiDebugCallback;)V",
                    (void *) regist
            }
    };

    return env->RegisterNatives(activityClass, methods_MainActivity, sizeof(methods_MainActivity) / sizeof(methods_MainActivity[0]));
}

//JNI_OnLoad   java
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    //    手机app   手机开机了
    JNIEnv *env = NULL;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    jint result = RegisterNatives(env);
    //    函数注册

    return JNI_VERSION_1_6;
}

//default  hiden  静态注册   动态
extern "C"
__attribute__ ((visibility ("default"))) void
Java_com_wangxingxing_secretkeysave_MainActivity_func1(JNIEnv *env, jobject thiz) {

}

extern "C"
JNIEXPORT void JNICALL
Java_com_wangxingxing_secretkeysave_MainActivity_func2(JNIEnv *env, jobject thiz, jstring data) {
    //data  env 转换方法
    const char *data1 = env->GetStringUTFChars(data, 0);
    env->ReleaseStringChars(data, reinterpret_cast<const jchar *>(data1));
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_wangxingxing_secretkeysave_MainActivity_func3(JNIEnv *env, jobject thiz) {
    std::string hello = "123123密钥";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wangxingxing_secretkeysave_MainActivity_func4(JNIEnv *env, jobject thiz) {
    //jobject--> class
    jclass activityClass = env->GetObjectClass(thiz);
    //方法签名 String 对象1    基本类型
    jfieldID idText = env->GetFieldID(activityClass, "text", "Ljava/lang/String;");
    //C字符串   1  java 字符串2
    jstring text1 = env->NewStringUTF("native层修改");

    env->SetObjectField(thiz, idText, text1);
}

/**
 * public void getSignInfo() {
       try {
           PackageInfo packageInfo = getPackageManager().getPackageInfo(
                   getPackageName(), PackageManager.GET_SIGNATURES);
           Signature[] signs = packageInfo.signatures;
           Signature sign = signs[0];
           System.out.println(sign.toCharsString());
       } catch (Exception e) {
           e.printStackTrace();
       }
   }
 */

/**
 * 获取全局 Application
 *
 * @param env
 * @return
 */
jobject getApplicationContext(JNIEnv *env) {
    jclass activityThread = env->FindClass("android/app/ActivityThread");
    jmethodID currentActivityThread = env->GetStaticMethodID(activityThread, "currentActivityThread", "()Landroid/app/ActivityThread;");
    jobject at = env->CallStaticObjectMethod(activityThread, currentActivityThread);
    jmethodID getApplication = env->GetMethodID(activityThread, "getApplication", "()Landroid/app/Application;");
    return env->CallObjectMethod(at, getApplication);
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_wangxingxing_secretkeysave_MainActivity_init(JNIEnv *env, jclass clazz) {

    jclass binderClass = env->FindClass("android/os/Binder");
    jclass contextClass = env->FindClass("android/content/Context");
    jclass signatureClass = env->FindClass("android/content/pm/Signature");
    jclass packageNameClass = env->FindClass("android/content/pm/PackageManager");
    jclass packageInfoClass = env->FindClass("android/content/pm/PackageInfo");

    jmethodID packageManager = env->GetMethodID(contextClass, "getPackageManager", "()Landroid/content/pm/PackageManager;");
    jmethodID packageName = env->GetMethodID(contextClass, "getPackageName", "()Ljava/lang/String;");
    jmethodID toCharsString = env->GetMethodID(signatureClass, "toCharsString", "()Ljava/lang/String;");
    jmethodID packageInfo = env->GetMethodID(packageNameClass, "getPackageInfo", "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    jmethodID nameForUid = env->GetMethodID(packageNameClass, "getNameForUid", "(I)Ljava/lang/String;");
    jmethodID callingUid = env->GetStaticMethodID(binderClass, "getCallingUid", "()I");

    jint uid = env->CallStaticIntMethod(binderClass, callingUid);

    // 获取全局 Application
    jobject context = getApplicationContext(env);

    jobject packageManagerObject = env->CallObjectMethod(context, packageManager);
    jstring packNameString = (jstring) env->CallObjectMethod(context, packageName);
    jobject packageInfoObject = env->CallObjectMethod(packageManagerObject, packageInfo, packNameString, 64);
    jfieldID signatureFieldID = env->GetFieldID(packageInfoClass, "signatures", "[Landroid/content/pm/Signature;");
    jobjectArray signatureArray = (jobjectArray) env->GetObjectField(packageInfoObject, signatureFieldID);
    jobject signatureObject = env->GetObjectArrayElement(signatureArray, 0);
    jstring runningPackageName = (jstring) env->CallObjectMethod(packageManagerObject, nameForUid, uid);

    if (runningPackageName) {
        // 正在运行应用的包名
        const char *charPackageName = env->GetStringUTFChars(runningPackageName, 0);
        if (strcmp(charPackageName, APP_PACKAGE_NAME) != 0) {
            return JNI_FALSE;
        }
        env->ReleaseStringUTFChars(runningPackageName, charPackageName);
    } else {
        return JNI_FALSE;
    }

    jstring signatureStr = (jstring) env->CallObjectMethod(signatureObject, toCharsString);
    const char *signature = env->GetStringUTFChars(
            (jstring) env->CallObjectMethod(signatureObject, toCharsString), NULL);

    env->DeleteLocalRef(binderClass);
    env->DeleteLocalRef(contextClass);
    env->DeleteLocalRef(signatureClass);
    env->DeleteLocalRef(packageNameClass);
    env->DeleteLocalRef(packageInfoClass);

    LOGE("current apk signature %s", signature);

    // 应用签名，通过 JNIDecryptKey.getSignature(getApplicationContext())
    // 获取，注意开发版和发布版的区别，发布版需要使用正式签名打包后获取
    const char *SIGNATURE_KEY = "308202e4308201cc020101300d06092a864886f70d010105050030373116301406035504030c0d416e64726f69642044656275673110300e060355040a0c07416e64726f6964310b30090603550406130255533020170d3231303632393034333332315a180f32303531303632323034333332315a30373116301406035504030c0d416e64726f69642044656275673110300e060355040a0c07416e64726f6964310b300906035504061302555330820122300d06092a864886f70d01010105000382010f003082010a02820101008c11e7f91eb41c4ea6dd80f6b70f2b99a1aa00b65877403cea2789f87d7dc6d52a756bbb4a1ada14d38daa7ebbd0ba8ad1feae7252c0816e3d18564f5841c4e6170c2bc67e14ec77adad8bf8d744eb3771a913d6a631b78001604008ed82af33ef9d187b36ba538d026958ff65f3cf483e0be7dbcfab966cef74f1dcd4e9c7519a3c375f828e16d7e3909b163038d01a41029807f55c6d44f4a9dac8b9ab974abf9fa9481bfc2365f5c846d1167a6c2ae517c94f3afa3a879fca04a0013773e7734c83ec956329c6960d45d86f51ab062ab27b24f5403fe50562064ec41776d3bd3f4ea33cb177fed7785f38270dddd27ee02ad124f8c4f34dd8c18482ee28910203010001300d06092a864886f70d0101050500038201010001e38099307194bb4a753c43ec6b07fafd93f24b68895eb160ca52ed816c52989dfc5cb947d1a7dac0e2a33055debbcac17e0fc78c645c81950ac72b8594fcb691c05756976d9a9a8aaded1968a444d0fd27786f75da49ea7ea27c39e2e1fa2ab83c8804cec1c597f3c1a9dce98f70d47500dfb24ed03d74cc93045649a7f37d08e539f281a37929e12fafb489afc2d906ce2659e58083080d3aa433b3b10ac239aee3e6c12a629084e6a2b088660d2af8115ef3d92b22c51767f406239c716d806aab98d44d94634c029addd2f875740d5fb9dac908d1fc3bab85542e628bb32788a79a6e7ebab5d683465f6cef57d413a98b9191b95a64adf4ce09d2b85d3c";
    if (strcmp(signature, SIGNATURE_KEY) == 0) {
        LOGE("verification passed");
        env->ReleaseStringUTFChars(signatureStr, signature);
        auth = JNI_TRUE;
        return JNI_TRUE;
    } else {
        LOGE("verification failed");
        auth = JNI_FALSE;
        return JNI_FALSE;
    }
    return auth;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_wangxingxing_secretkeysave_MainActivity_getKey(JNIEnv *env, jclass clazz) {
    const char *DECRYPT_KEY = "successful return 666";
    if (auth) {
        return env->NewStringUTF(DECRYPT_KEY);
    } else {
        // 你没有权限，验证没有通过。
        return env->NewStringUTF("You don't have permission, the verification didn't pass.");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_wangxingxing_secretkeysave_MainActivity_func5(JNIEnv *env, jobject thiz) {
    jclass activityClass=env->GetObjectClass(thiz);
//  idText1  =    ArtMethod  sophix  andfix 修复原理
    jmethodID idText = env->GetMethodID(activityClass, "callBack", "(I)V");
//    方法字节码指令   快鱼  普通的java 反射
    jmethodID idText1 = env->GetMethodID(activityClass, "callBack", "(Ljava/lang/String;I)Ljava/lang/String;");
    jmethodID idText2 = env->GetMethodID(activityClass, "callBack", "([B)V");
    env->CallVoidMethod(thiz, idText, 2);
}