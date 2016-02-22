/*!
   \file JavaStaticCaller.hpp
   \brief A utility header to call static java methods.
   \author Denis Sorokin
   \date January 24 2016
   \copyright Zeptolab, 2016
 */

#ifndef JH_JAVA_STATIC_CALLER_HPP
#define JH_JAVA_STATIC_CALLER_HPP

#include <jni.h>
#include <string>
#include "../core/ToJavaType.hpp"
#include "../core/ErrorHandler.hpp"
#include "../core/JNIEnvironment.hpp"
#include "../core/JavaMethodSignature.hpp"

namespace jh
{
    /**
    * Classes that describe internal implementation for Java static calls.
    * Each class represents different return type of Java method.
    */
    template<class ReturnType, class ... ArgumentTypes>
    struct StaticCaller
    {
        static jobject call(JNIEnv* env, jclass javaClass, jmethodID javaMethod, ArgumentTypes ... arguments)
        {
            return env->CallStaticObjectMethod(javaClass, javaMethod, arguments...);
        }
    };

    template<class ... ArgumentTypes>
    struct StaticCaller<void, ArgumentTypes...>
    {
        static void call(JNIEnv* env, jclass javaClass, jmethodID javaMethod, ArgumentTypes ... arguments)
        {
            env->CallStaticVoidMethod(javaClass, javaMethod, arguments...);
        }
    };

    template<class ... ArgumentTypes>
    struct StaticCaller<jboolean, ArgumentTypes...>
    {
        static jboolean call(JNIEnv* env, jclass javaClass, jmethodID javaMethod, ArgumentTypes ... arguments)
        {
            return env->CallStaticBooleanMethod(javaClass, javaMethod, arguments...);
        }
    };

    template<class ... ArgumentTypes>
    struct StaticCaller<jint, ArgumentTypes...>
    {
        static jint call(JNIEnv* env, jclass javaClass, jmethodID javaMethod, ArgumentTypes ... arguments)
        {
            return env->CallStaticIntMethod(javaClass, javaMethod, arguments...);
        }
    };

    template<class ... ArgumentTypes>
    struct StaticCaller<jlong, ArgumentTypes...>
    {
        static jlong call(JNIEnv* env, jclass javaClass, jmethodID javaMethod, ArgumentTypes ... arguments)
        {
            return env->CallStaticLongMethod(javaClass, javaMethod, arguments...);
        }
    };

    template<class ... ArgumentTypes>
    struct StaticCaller<jfloat, ArgumentTypes...>
    {
        static jfloat call(JNIEnv* env, jclass javaClass, jmethodID javaMethod, ArgumentTypes ... arguments)
        {
            return env->CallStaticFloatMethod(javaClass, javaMethod, arguments...);
        }
    };

    template<class ... ArgumentTypes>
    struct StaticCaller<jdouble, ArgumentTypes...>
    {
        static jdouble call(JNIEnv* env, jclass javaClass, jmethodID javaMethod, ArgumentTypes ... arguments)
        {
            return env->CallStaticDoubleMethod(javaClass, javaMethod, arguments...);
        }
    };

    /**
    * Calls a static method of some Java class. Programmer should explicitly
    * specify the return type and argument types via template arguments.
    *
    * @param className Java class name as string.
    * @param methodName Method name as string.
    * @param arguments List of arguments to the java method call.
    *
    * Possible usage example:
    *
    * @code{.cpp}
    *
    * // Declaring some custom Java class:
    * JH_JAVA_CUSTOM_CLASS(Example, "com/class/path/Example");
    *
    * // Calling void static method without arguments:
    * jh::callStaticMethod<void>(ExampleClass::name(), "voidMethodName");
    *
    * // Calling int static method with two arguments:
    * int sum = jh::callStaticMethod<int, int, int>(ExampleClass::name(), "sumMethod", 4, 5);
    *
    * // Calling static factory method that returns some custom Java object:
    * jobject newObject = jh::callStaticMethod<Example, double>(ExampleClass::name(), "factoryMethodName", 3.1415);
    *
    * @endcode
    */
    template<class ReturnType, class ... ArgumentTypes>
    typename ToJavaType<ReturnType>::Type callStaticMethod(std::string className, std::string methodName, typename ToJavaType<ArgumentTypes>::Type ... arguments)
    {
        using RealReturnType = typename ToJavaType<ReturnType>::Type;

        JNIEnv* env = getCurrentJNIEnvironment();

        std::string methodSignature = getJavaMethodSignature<ReturnType, ArgumentTypes...>();

        jclass javaClass = env->FindClass(className.c_str());
        if (javaClass == nullptr) {
            reportInternalError("class not found [" + className + "]");
            return RealReturnType();
        }

        jmethodID javaMethod = env->GetStaticMethodID(javaClass, methodName.c_str(), methodSignature.c_str());
        if (javaMethod == nullptr) {
            reportInternalError("method [" + methodName + "] for class [" + className + "] not found, tried signature [" + methodSignature + "]");
            return RealReturnType();
        }

        return static_cast<RealReturnType>(StaticCaller<typename ToJavaType<ReturnType>::CallReturnType, typename ToJavaType<ArgumentTypes>::Type ...>::call(env, javaClass, javaMethod, arguments...));
    }

    template<class JavaClassType, class ReturnType, class ... ArgumentTypes>
    typename ToJavaType<ReturnType>::Type callStaticMethod(std::string methodName, typename ToJavaType<ArgumentTypes>::Type ... arguments)
    {
        return callStaticMethod<ReturnType, ArgumentTypes ...>(JavaClassType::className(), methodName, arguments...);
    }
}

#endif