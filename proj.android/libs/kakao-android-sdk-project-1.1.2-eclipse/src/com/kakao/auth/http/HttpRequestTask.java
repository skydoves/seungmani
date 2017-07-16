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
package com.kakao.auth.http;

import android.net.Uri;
import android.os.Bundle;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.kakao.util.helper.CommonProtocol;
import com.kakao.util.helper.SystemInfo;
import com.kakao.util.helper.Utility;
import com.kakao.util.helper.log.Logger;
import com.ning.http.client.AsyncHttpClient;
import com.ning.http.client.AsyncHttpClient.BoundRequestBuilder;
import com.ning.http.client.AsyncHttpClientConfig;
import com.ning.http.client.Request;

import java.util.HashMap;
import java.util.Map;
import java.util.Set;

/**
 * @author MJ
 */
public class HttpRequestTask<T> implements Runnable {
    private static final ObjectMapper objectMapper = new ObjectMapper();
    public static final int SUCCESS = 1;
    public static final int ERROR = 2;
    public static final int NEED_TO_LOGIN = 4;

    private static AsyncHttpClientConfig asyncHttpClientConfig = defaultConfigure();
    public static final AsyncHttpClient ASYNC_HTTP_CLIENT = newAsyncHttpClient();   // Todo close를 어디서 하나.
    public static final HashMap<String,String> KA_HEADER = createKAHeader();

    private static final int DEFAULT_MAX_REQUEST_RETRY = 1;
    private static final int DEFAULT_CONNECTION_TO_IN_MS = 5000;
    private static final int DEFAULT_REQUEST_TO_IN_MS = 30 * 1000;
    private static final int DEFAULT_IDLE_CONNECTION_TO_IN_MS = 300 * 1000;
    private static final int DEFAULT_IDLE_CONNECTION_POOL_IN_MS = 300 * 1000;

    private final KakaoAsyncHandler<T> asyncHandler;
    protected final Request request;

    public HttpRequestTask(final Request request, final KakaoAsyncHandler<T> asyncHandler){
        this.request = request;
        this.asyncHandler = asyncHandler;
    }

    private static HashMap<String, String> createKAHeader() {
        HashMap<String, String>  kaHeader = new HashMap<String, String>();
        kaHeader.put(CommonProtocol.KA_HEADER_KEY, SystemInfo.getKAHeader());
        return kaHeader;
    }

    public static String createBaseURL(final String authority, final String requestPath) {
        Uri uri = Utility.buildUri(authority, requestPath);
        return uri.toString();
    }

    @Override
    public void run() {
        try {
            preRequest();
            ASYNC_HTTP_CLIENT.executeRequest(request, asyncHandler);
        } catch (Exception e) {
            Logger.e(e);
            failedRequest(e);
        }
    }

    protected void preRequest() {
        Logger.d("preRequest()");
        Logger.d(false, "[RequestUrl] : " + request.getUrl());
    }

    protected void failedRequest(Exception e) {
    }

    private static AsyncHttpClient newAsyncHttpClient() {
//        return new AsyncHttpClient(new GrizzlyAsyncHttpProvider(asyncHttpClientConfig), asyncHttpClientConfig);
        return new AsyncHttpClient(new SimpleAsyncHttpProvider(asyncHttpClientConfig), asyncHttpClientConfig);
    }

    // default configuration을 쓰고 싶지 않으면 HttpRequestTask를 처음 사용하기 전에 호출해 준다.
    public static void setAsyncHttpClientConfig(final AsyncHttpClientConfig asyncHttpClientConfig) {
        HttpRequestTask.asyncHttpClientConfig = asyncHttpClientConfig;
    }

    private static AsyncHttpClientConfig defaultConfigure() {
        final AsyncHttpClientConfig.Builder configBuilder = new AsyncHttpClientConfig.Builder();
        configBuilder.setAllowPoolingConnection(true);
        configBuilder.setMaxRequestRetry(DEFAULT_MAX_REQUEST_RETRY);
        configBuilder.setConnectionTimeoutInMs(DEFAULT_CONNECTION_TO_IN_MS);
        configBuilder.setRequestTimeoutInMs(DEFAULT_REQUEST_TO_IN_MS);
        configBuilder.setIdleConnectionTimeoutInMs(DEFAULT_IDLE_CONNECTION_TO_IN_MS);
        configBuilder.setIdleConnectionInPoolTimeoutInMs(DEFAULT_IDLE_CONNECTION_POOL_IN_MS);
        configBuilder.setExecutorService(HttpTaskManager.getHttpExecutor());
        return configBuilder.build();
    }

    private static void destroy() {
        ASYNC_HTTP_CLIENT.close();
    }


    /**
     * String/Boolean/Number만 지원.
     */
    public static void addQueryParams(BoundRequestBuilder requestBuilder, Bundle parameters) {
        if(parameters == null)
            return;
        Set<String> keys = parameters.keySet();
        for (String key : keys) {
            Object value = parameters.get(key);
            String valueStr = null;

            if (value == null) {
                value = "";
            }

            if (isSupportedParameterType(value)) {
                valueStr = parameterToString(value);
            } else {
                throw new IllegalArgumentException(String.format("Unsupported parameter type for GET request: %s",
                    value.getClass().getSimpleName()));
            }

            requestBuilder.addQueryParameter(key, valueStr);
        }
    }

    /**
     * String/Boolean/Number 또는 value 타입이 이와같은 Map/Set만 지원.
     */
    public static void addParams(final BoundRequestBuilder requestBuilder, final Bundle parameters) {
        if(parameters == null)
            return;

        Set<String> keys = parameters.keySet();
        for (String key : keys) {
            Object value = parameters.get(key);
            String valueStr = null;

            if (value == null) {
                value = "";
            }

            if (isSupportedParameterType(value)) {
                valueStr = parameterToString(value);
            } else if(value instanceof Map || value instanceof Set){
                try {
                    valueStr = objectMapper.writeValueAsString(value);
                } catch (JsonProcessingException e) {
                    throw new IllegalArgumentException(String.format("Unsupported parameter type : %s",
                        value.getClass().getSimpleName()));
                }
            } else {
                throw new IllegalArgumentException(String.format("Unsupported parameter type : %s",
                    value.getClass().getSimpleName()));
            }

            requestBuilder.addParameter(key, valueStr);
        }
    }

    private static boolean isSupportedParameterType(final Object value) {
        return value instanceof String || value instanceof Boolean || value instanceof Number;
    }

    private static String parameterToString(final Object value) {
        if (value instanceof String) {
            return (String) value;
        } else if (value instanceof Boolean || value instanceof Number) {
            return value.toString();
        }
        throw new IllegalArgumentException("Unsupported parameter type.");
    }


}
