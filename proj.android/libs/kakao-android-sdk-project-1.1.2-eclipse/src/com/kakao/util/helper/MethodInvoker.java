/**
 * Copyright 2015 Daum Kakao Corp.
 *
 * Redistribution and modification in source or binary forms are not permitted without specific prior written permission. 
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package com.kakao.util.helper;

import java.lang.reflect.Method;

/**
 * @author Mars
 */
public final class MethodInvoker {

    public static void invoke(Object methodOwner, String methodName, Object... args) throws Exception {
        if (null == methodOwner) {
            throw new RuntimeException("methodOwnerClass is null.");
        }

        Class<?> itr = methodOwner.getClass();
        Method[] methods;
        Method foundMethod;

        while (!itr.equals(Object.class)) {
            methods = itr.getDeclaredMethods();
            if (methods != null) {
                foundMethod = findMethod(methodName, methods);

                if (foundMethod != null) {
                    foundMethod.invoke(methodOwner, args);
                    return;
                }
            }

            itr = itr.getSuperclass();
        }
    }

    private static Method findMethod(String methodName, Method[] methods) {
        for (Method methodItr : methods) {
            if (methodItr.getName().equals(methodName)) {
                methodItr.setAccessible(true);
                return methodItr;
            }
        }

        return null;
    }

}
