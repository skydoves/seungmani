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
package com.kakao.usermgmt;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import android.os.Bundle;

import com.kakao.auth.helper.ServerProtocol;
import com.kakao.auth.http.HttpRequestTask;
import com.kakao.auth.rest.APIHttpRequestTask;

/**
 * UserManagement API 요청을 담당한다.
 * @author MJ
 */
public class UserManagement {

    /**
     * 사용자정보 요청
     * @param responseHandler me 요청 결과에 대한 handler
     */
    public static void requestMe(final MeResponseCallback responseHandler) {
        requestMe(responseHandler, null, null);
    }

    /**
     * 사용자 정보 일부나 이미지 리소스를 https로 받고 싶은 경우의 사용자정보 요청
     * @param responseHandler me 요청 결과에 대한 handler
     * @param propertyKeys 사용자 정보의 키 리스트
     * @param secureResource 이미지 url을 https로 반환할지 여부
     */
    public static void requestMe(final MeResponseCallback responseHandler, final ArrayList<String> propertyKeys, final Boolean secureResource) {
        final String url = HttpRequestTask.createBaseURL(ServerProtocol.API_AUTHORITY, ServerProtocol.USER_ME_PATH);

        final Bundle params = new Bundle();
        if(propertyKeys != null && !propertyKeys.isEmpty()) {
            params.putStringArrayList(ServerProtocol.PROPERTY_KEYS_KEY, propertyKeys);
        }
        if(secureResource != null) {
            params.putBoolean(ServerProtocol.SECURE_RESOURCE_KEY, secureResource);
        }

        APIHttpRequestTask.requestGet(responseHandler, Map.class, url, params);
    }

    /**
     * 로그아웃 요청
     * @param responseHandler logout 요청 결과에 대한 handler
     */
    public static void requestLogout(final LogoutResponseCallback responseHandler) {
        final String url = HttpRequestTask.createBaseURL(ServerProtocol.API_AUTHORITY, ServerProtocol.USER_LOGOUT_PATH);
        APIHttpRequestTask.requestPost(responseHandler, Map.class, url, null);
    }

    /**
     * Unlink 요청
     * @param responseHandler unlink 요청 결과에 대한 handler
     */
    public static void requestUnlink(final UnlinkResponseCallback responseHandler) {
        final String url = HttpRequestTask.createBaseURL(ServerProtocol.API_AUTHORITY, ServerProtocol.USER_UNLINK_PATH);
        APIHttpRequestTask.requestPost(responseHandler, Map.class, url, null);
    }

    /**
     * 가입 요청
     * @param responseHandler signup 요청 결과에 대한 handler
     * @param properties 가입시 받은 사용자 정보
     */
    public static void requestSignup(final SignupResponseCallback responseHandler, final HashMap properties) {
        final String url = HttpRequestTask.createBaseURL(ServerProtocol.API_AUTHORITY, ServerProtocol.USER_SIGNUP_PATH);

        final Bundle params = new Bundle();
        if(properties != null && !properties.isEmpty()) {
            params.putSerializable(ServerProtocol.PROPERTIES_KEY, properties);
        }

        APIHttpRequestTask.requestPost(responseHandler, Map.class, url, params);
    }

    /**
     * 사용자정보 저장 요청
     * @param responseHandler updateProfile 요청 결과에 대한 handler
     * @param properties 저장할 사용자 정보
     */
    public static void requestUpdateProfile(final UpdateProfileResponseCallback responseHandler, final HashMap properties) {
        final String url = HttpRequestTask.createBaseURL(ServerProtocol.API_AUTHORITY, ServerProtocol.USER_UPDATE_PROFILE_PATH);

        final Bundle params = new Bundle();
        if(properties != null && !properties.isEmpty()) {
            params.putSerializable(ServerProtocol.PROPERTIES_KEY, properties);
        }
        APIHttpRequestTask.requestPost(responseHandler, Map.class, url, params);
    }

}
