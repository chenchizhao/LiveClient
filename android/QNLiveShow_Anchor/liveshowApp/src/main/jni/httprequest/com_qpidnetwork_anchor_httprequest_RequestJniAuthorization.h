/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_qpidnetwork_anchor_httprequest_RequestJniAuthorization */

#ifndef _Included_com_qpidnetwork_anchor_httprequest_RequestJniAuthorization
#define _Included_com_qpidnetwork_anchor_httprequest_RequestJniAuthorization
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_qpidnetwork_anchor_httprequest_RequestJniAuthorization
 * Method:    Login
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Lcom/qpidnetwork/livemodule/httprequest/OnRequestLoginCallback;)J
 */
JNIEXPORT jlong JNICALL Java_com_qpidnetwork_anchor_httprequest_RequestJniAuthorization_Login
  (JNIEnv *, jclass, jstring, jstring, jstring, jstring, jstring, jstring, jobject);

/*
 * Class:     com_qpidnetwork_anchor_httprequest_RequestJniAuthorization
 * Method:    UploadPushTokenId
 * Signature: (Ljava/lang/String;Lcom/qpidnetwork/livemodule/httprequest/OnRequestCallback;)J
 */
JNIEXPORT jlong JNICALL Java_com_qpidnetwork_anchor_httprequest_RequestJniAuthorization_UploadPushTokenId
        (JNIEnv *, jclass, jstring, jobject);

/*
 * Class:     com_qpidnetwork_anchor_httprequest_RequestJniAuthorization
 * Method:    LSGetVerificationCode
 * Signature: (IZLcom/qpidnetwork/livemodule/httprequest/OnRequestGetVerificationCodeCallback;)J
 */
JNIEXPORT jlong JNICALL Java_com_qpidnetwork_anchor_httprequest_RequestJniAuthorization_LSGetVerificationCode
        (JNIEnv *, jclass, jobject);


#ifdef __cplusplus
}
#endif
#endif
