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

package com.kakao.kakaolink;

/**
* @author MJ
*/

import com.kakao.kakaolink.AppActionBuilder.DEVICE_TYPE;
import com.kakao.kakaolink.internal.AppActionInfo;
import com.kakao.kakaolink.internal.AppActionInfo.ACTION_INFO_OS;

/**
 * 기기에서 앱으로 연결시 해당 앱이 설치되어 있으면 실행 커스텀 URL, 설치되어 있지 않으면 마켓 URL이 호출된다.
 * 이때 개발자 사이트에 등록된 URL에 OS별, device type별 파람을 추가하고 싶을 때 이를 이용하여 AppActionInfo를 생성한다.
 */
public class AppActionInfoBuilder {

    private final ACTION_INFO_OS os;
    private final DEVICE_TYPE deviceType;
    private String executeParam;
    private String marketParam;

    private AppActionInfoBuilder(final ACTION_INFO_OS os, final DEVICE_TYPE deviceType) {
        this.os = os;
        this.deviceType = deviceType;
    }

    /**
     * iOS 기기에서의 AppActionInfo를 설정하고 싶은 경우에 사용한다.
     * @return iOS 기기에서의 AppActionInfoBuilder
     */
    public static AppActionInfoBuilder createiOSActionInfoBuilder(){
        return new AppActionInfoBuilder(ACTION_INFO_OS.IOS, null);
    }

    /**
     * iOS 특정 device type별 AppActionInfo를 설정하고 싶은 경우에 사용한다.
     * @param deviceType {@link DEVICE_TYPE}
     * @return iOS 특정 device type 기기에서의 AppActionInfoBuilder
     */
    public static AppActionInfoBuilder createiOSActionInfoBuilder(final DEVICE_TYPE deviceType){
        return new AppActionInfoBuilder(ACTION_INFO_OS.IOS, deviceType);
    }

    /**
     * Android 기기에서의 AppActionInfo를 설정하고 싶은 경우에 사용한다.
     * @return Android 기기에서의 AppActionInfoBuilder
     */
    public static AppActionInfoBuilder createAndroidActionInfoBuilder(){
        return new AppActionInfoBuilder(ACTION_INFO_OS.ANDROID, null);
    }

    /**
     * Android 특정 device type별 AppActionInfo를 설정하고 싶은 경우에 사용한다.
     * @param deviceType {@link DEVICE_TYPE}
     * @return Android 특정 device type 기기에서의 AppActionInfoBuilder
     */
    public static AppActionInfoBuilder createAndroidActionInfoBuilder(final DEVICE_TYPE deviceType){
        return new AppActionInfoBuilder(ACTION_INFO_OS.ANDROID, deviceType);
    }

    /**
     * 기기에서 앱으로 연결시 기본 커스텀 스킴 URL(kakao[appkey]://kakaolink)에 추가할 파라미터를 설정한다.
     * @param executeParam 앱 연결 URL에 추가할 파라미터
     * @return 계속해서 앱 연결 설정할 수 있는 빌더를 반환한다.
     */
    public AppActionInfoBuilder setExecuteParam(final String executeParam) {
        this.executeParam = executeParam;
        return this;
    }

    /**
     * [NOTICE] 안드로이드 마켓 파람은 안드로이드 카카오톡 4.5.0 이상부터 지원된다. 그 이하 버젼에서는 파람이 붙지 않고 개발자 싸이트에 등록된 마켓 url로 이동된.
     * 앱연결 링크 클릭시 해당앱이 설치되지 않았을 때 호출되는 마켓 url에 추가할 파라미터 설정한다.
     * 기본적으로는 개발자 싸이트에 등록한 안드로이드 마켓 url로 연결되나 그 뒤에 파라미터를 붙이고 싶을 때 사용한다.
     * 개발자 싸이트에 등록한 안드로이드 마켓 url + "?" + ${marketParam}
     * @param marketParam 추가할 파라미터
     * @return 계속해서 앱 연결 설정할 수 있는 빌더를 반환한다.
     */
    public AppActionInfoBuilder setMarketParam(final String marketParam) {
        this.marketParam = marketParam;
        return this;
    }

    /**
     * 하나의 OS, device type별 파람 설정이 끝났을 때 이를 통해 AppActionInfo를 생성한다.
     * @return 설정이 완료된 AppActionInfo
     */
    public AppActionInfo build(){
        return new AppActionInfo(os, deviceType, executeParam, marketParam);
    }
}
