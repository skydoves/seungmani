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
package com.kakao.kakaolink.internal;


import android.text.TextUtils;

import com.kakao.kakaolink.AppActionBuilder.DEVICE_TYPE;

import org.json.JSONException;
import org.json.JSONObject;

public class AppActionInfo {
    public enum ACTION_INFO_OS {
        ANDROID("android"),
        IOS("ios");

        private final String value;

        ACTION_INFO_OS(String value) {
            this.value = value;
        }
    }

    private final ACTION_INFO_OS os;
    private final DEVICE_TYPE deviceType;
    private final String executeParam;
    private final String marketParam;

    public AppActionInfo(final ACTION_INFO_OS os, final DEVICE_TYPE deviceType, final String executeParam, final String marketParam) {
        this.os = os;
        this.executeParam = executeParam;
        this.marketParam = marketParam;
        this.deviceType = deviceType;
    }

    @Deprecated
    public static AppActionInfo createAndroidActionInfo(final DEVICE_TYPE deviceType, final String executeParam, final String marketParam){
        return new AppActionInfo(ACTION_INFO_OS.ANDROID, deviceType, executeParam, marketParam);
    }

    @Deprecated
    public static AppActionInfo createIOSActionInfo(final DEVICE_TYPE deviceType, final String executeParam, final String marketParam){
        return new AppActionInfo(ACTION_INFO_OS.IOS, deviceType, executeParam, marketParam);
    }

    public JSONObject createJSONObject() throws JSONException {
        JSONObject json = new JSONObject();
        json.put(KakaoTalkLinkProtocol.ACTIONINFO_OS, os.value);

        if (deviceType != null) {
            json.put(KakaoTalkLinkProtocol.ACTIONINFO_DEVICETYPE, deviceType.getValue());
        }

        if (!TextUtils.isEmpty(executeParam)) {
            json.put(KakaoTalkLinkProtocol.ACTIONINFO_EXEC_PARAM, executeParam);
        }

        if (!TextUtils.isEmpty(marketParam)) {
            json.put(KakaoTalkLinkProtocol.ACTIONINFO_MARKET_PARAM, marketParam);
        }

        return json;
    }
}

