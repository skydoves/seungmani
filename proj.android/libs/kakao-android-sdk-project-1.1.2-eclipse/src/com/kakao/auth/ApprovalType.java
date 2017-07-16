/**
 * Copyright 2014 Daum Kakao Corp.
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
package com.kakao.auth;

import java.security.InvalidParameterException;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

/**
 * @author MJ
 */
public enum ApprovalType {
    INDIVIDUAL,
    PROJECT;

    private static final Map<String, ApprovalType> REVERSE_MAP = new HashMap<String, ApprovalType>();

    static {
        for (ApprovalType type : ApprovalType.values()) {
            REVERSE_MAP.put(type.toString(), type);
        }
    }

    public String toString(){
        return super.toString().toLowerCase();
    }

    public static ApprovalType getApprovalTypeByString(final String s) throws InvalidParameterException {
        if (s == null)
            return null;
        final ApprovalType type = REVERSE_MAP.get(s);
        if (type != null)
            return type;
        else
            throw new InvalidParameterException("ApprovalType is one of " + Arrays.toString(ApprovalType.values()));
    }

}
