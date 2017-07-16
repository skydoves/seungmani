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
package com.kakao.auth.authorization.accesstoken;

import android.os.Bundle;

import com.kakao.auth.APIErrorResult;
import com.kakao.auth.authorization.AuthorizationResult;
import com.kakao.auth.authorization.Authorizer;
import com.kakao.auth.helper.ServerProtocol;
import com.kakao.auth.http.HttpRequestTask;
import com.kakao.auth.http.HttpResponseHandler;
import com.kakao.auth.http.HttpTaskManager;
import com.kakao.auth.http.KakaoAsyncHandler;
import com.kakao.util.helper.log.Logger;
import com.ning.http.client.AsyncHttpClient.BoundRequestBuilder;
import com.ning.http.client.Request;
import com.ning.http.client.Response;

import org.apache.http.HttpStatus;

import java.io.IOException;
import java.net.URI;
import java.util.Map;
import java.util.Map.Entry;

/**
 * @author MJ
 */
public class GetterAccessToken extends Authorizer {

    private final AccessTokenRequest accessTokenRequest;

    public GetterAccessToken(final AccessTokenRequest accessTokenRequest) {
        this.accessTokenRequest = accessTokenRequest;
    }

    public void requestAccessToken() {
        final boolean permission = checkInternetPermission();
        if (!permission) {
            return;
        }

        final BoundRequestBuilder requestBuilder = makeAccessTokenRequest();
        final Request httpRequest = requestBuilder.build();
        HttpTaskManager.execute(new HttpRequestTask<Map>(httpRequest, new AccessTokenCallback(httpRequest, new HttpResponseHandler<Map>() {
            @Override
            public void onHttpSuccess(final Map resultObj) { // 200인 경우
                final AccessToken accessToken = AccessToken.createFromResponse(resultObj);
                if (accessToken == null)
                    doneOnOAuthError("AccessToken is null.");
                else
                    done(AuthorizationResult.createSuccessAccessTokenResult(accessToken));
            }

            @Override
            public void onHttpSessionClosedFailure(final APIErrorResult errorResult) {
                // API call인 경우 session 만료로 발생하므로 oauth 요청시는 발생하지 않음.
            }

            @Override
            public void onHttpFailure(final APIErrorResult errorResult) { // 200 제외
                switch (errorResult.getHttpStauts()) {
                    case HttpStatus.SC_BAD_REQUEST:
                    case HttpStatus.SC_UNAUTHORIZED:
                        doneOnOAuthError(errorResult.toString());
                        break;
                    default:
                        done(AuthorizationResult.createAccessTokenErrorResult(errorResult.toString()));
                        break;
                }
            }
        })));
    }

    public AccessTokenRequest getRequest() {
        return accessTokenRequest;
    }

    private class AccessTokenCallback extends KakaoAsyncHandler<Map> {

        public AccessTokenCallback(final Request request, final HttpResponseHandler<Map> httpResponseHandler) {
            super(request, httpResponseHandler, Map.class);
        }

        @Override
        protected Void handleFailureHttpStatus(final Response response, final URI requestUri, final int httpStatusCode) throws IOException {
            if (checkResponseBody(response)) {
                return null;
            }
            sendError(response, response.getResponseBody());
            return null;
        }
    }

    protected void doneOnOAuthError(final String errorMessage) {
        Logger.d("GetterAccessToken: " + errorMessage);
        done(AuthorizationResult.createAccessTokenOAuthErrorResult(errorMessage));
    }

    private BoundRequestBuilder makeAccessTokenRequest() {
        final BoundRequestBuilder requestBuilder = HttpRequestTask.ASYNC_HTTP_CLIENT.preparePost(
            HttpRequestTask.createBaseURL(ServerProtocol.AUTH_AUTHORITY, ServerProtocol.ACCESS_TOKEN_PATH));
        final Entry<String,String> entry = HttpRequestTask.KA_HEADER.entrySet().iterator().next();
        requestBuilder.addHeader(entry.getKey(), entry.getValue());

        if (accessTokenRequest.isAccessTokenRequestWithAuthCode()) {
            requestBuilder.addQueryParameter(ServerProtocol.GRANT_TYPE_KEY, ServerProtocol.GRANT_TYPE_AUTHORIZATION_CODE);
            requestBuilder.addQueryParameter(ServerProtocol.CODE_KEY, accessTokenRequest.getAuthorizationCode());
            requestBuilder.addQueryParameter(ServerProtocol.REDIRECT_URI_KEY, accessTokenRequest.getRedirectURI());
        } else { //if(request.isAccessTokenRequestWithRefreshToken()) {
            requestBuilder.addQueryParameter(ServerProtocol.GRANT_TYPE_KEY, ServerProtocol.REFRESH_TOKEN_KEY);
            requestBuilder.addQueryParameter(ServerProtocol.REFRESH_TOKEN_KEY, accessTokenRequest.getRefreshToken());
        }
        requestBuilder.addQueryParameter(ServerProtocol.CLIENT_ID_KEY, accessTokenRequest.getAppKey());
        requestBuilder.addQueryParameter(ServerProtocol.ANDROID_KEY_HASH, accessTokenRequest.getKeyHash());

        final Bundle extraParams = accessTokenRequest.getExtras();
        if (extraParams != null && !extraParams.isEmpty()) {
            for (String key : extraParams.keySet()) {
                String value = extraParams.getString(key);
                if (value != null) {
                    requestBuilder.addQueryParameter(key, value);
                }
            }
        }
        return requestBuilder;
    }
}