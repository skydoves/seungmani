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
package com.kakao.auth.rest;

import android.os.Bundle;
import android.os.Message;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.kakao.auth.APIErrorResult;
import com.kakao.auth.Session;
import com.kakao.auth.SessionCallback;
import com.kakao.auth.helper.ServerProtocol;
import com.kakao.auth.http.HttpRequestTask;
import com.kakao.auth.http.HttpResponseHandler;
import com.kakao.auth.http.HttpTaskManager;
import com.kakao.auth.http.KakaoAsyncHandler;
import com.kakao.util.exception.KakaoException;
import com.ning.http.client.AsyncHttpClient.BoundRequestBuilder;
import com.ning.http.client.Request;
import com.ning.http.client.Response;

import org.apache.http.HttpStatus;

import java.io.IOException;
import java.net.URI;
import java.util.ArrayList;
import java.util.List;
import java.util.Map.Entry;

///**
// * API 요청을 위한 HttpRequestTask로 세션 만료시 재갱신하여 재요청해주고,
// * 서버 에러가 발생한 경우 APIErrorResult 객체를 반환하도록 한다.
// * @param <T> : 요청이 성공한 경우 return 객체 type. httpResponseHandler<T>, Class<T>
// * @author MJ
// */

public class APIHttpRequestTask<T> extends HttpRequestTask<T> implements SessionCallback {
    private final HttpResponseHandler<T> httpResponseHandler;

    public APIHttpRequestTask(final Request request, final HttpResponseHandler<T> httpResponseHandler, final Class<T> returnType) {
        super(request, new APIAsyncHandler<T>(request, httpResponseHandler, returnType));
        this.httpResponseHandler = httpResponseHandler;
    }

    public APIHttpRequestTask(final Request request, final KakaoAsyncHandler<T> asyncTaskHandler, final HttpResponseHandler<T> httpResponseHandler) {
        super(request, asyncTaskHandler);
        this.httpResponseHandler = httpResponseHandler;
    }

    public static void checkSessionAndExecute(final APIHttpRequestTask requestTask) {
        if (Session.getCurrentSession().isOpened())
            HttpTaskManager.execute(requestTask);
        else if (!requestAccessTokenWithRefreshToken(requestTask)) {
            failedToRefreshAccessToken(requestTask, requestTask.httpResponseHandler, "session is closed before sending the request");
        }
    }

    public static <T> void requestGet(final HttpResponseHandler<T> responseHandler, final Class<T> responseClass, final String url, final Bundle queryParams) {
        final BoundRequestBuilder requestBuilder = HttpRequestTask.ASYNC_HTTP_CLIENT.prepareGet(url);
        addTokenHeader(requestBuilder);
        if(queryParams != null && !queryParams.isEmpty()) {
            addQueryParams(requestBuilder, queryParams);
        }
        checkSessionAndExecute(new APIHttpRequestTask<T>(requestBuilder.build(), responseHandler, responseClass));
    }

    public static <T> void requestPost(final HttpResponseHandler<T> responseHandler, final Class<T> responseClass, final String url, final Bundle queryParams) {
        final BoundRequestBuilder requestBuilder = HttpRequestTask.ASYNC_HTTP_CLIENT.preparePost(url);
        addTokenHeader(requestBuilder);
        if (queryParams != null && !queryParams.isEmpty()) {
            addParams(requestBuilder, queryParams);
        }
        checkSessionAndExecute(new APIHttpRequestTask<T>(requestBuilder.build(), responseHandler, responseClass));
    }

    public static <T> void requestDelete(final HttpResponseHandler<T> responseHandler, final Class<T> responseClass, final String url, final Bundle queryParams) {
        final BoundRequestBuilder requestBuilder = HttpRequestTask.ASYNC_HTTP_CLIENT.prepareDelete(url);
        addTokenHeader(requestBuilder);
        if(queryParams != null && !queryParams.isEmpty()) {
            addQueryParams(requestBuilder, queryParams);
        }
        checkSessionAndExecute(new APIHttpRequestTask<T>(requestBuilder.build(), responseHandler, responseClass));
    }

    private static void failedToRefreshAccessToken(final APIHttpRequestTask requestTask, final HttpResponseHandler responseHandler, final String errorMsg) {
        String requestUrl = null;
        if(requestTask != null && requestTask.request != null)
            requestUrl = requestTask.request.getUrl();
        final APIErrorResult clientError = new APIErrorResult(requestUrl, errorMsg);
        responseHandler.sendMessage(Message.obtain(responseHandler, NEED_TO_LOGIN, 0, 0, clientError));
    }

    @Override
    public void onSessionOpened() {
        HttpTaskManager.execute(updateSession());
        Session.getCurrentSession().removeCallback(this);
    }

    @Override
    public void onSessionClosed(KakaoException exception) {
        failedToRefreshAccessToken(this, httpResponseHandler, "session is closed during refreshing token for the request");
        Session.getCurrentSession().removeCallback(this);
    }

    @Override
    public void onSessionOpening() {
    }

    private static class APIAsyncHandler<T> extends KakaoAsyncHandler<T> {

        public APIAsyncHandler(final Request request, final HttpResponseHandler<T> httpResponseHandler, final Class<T> returnType) {
            super(request, httpResponseHandler, returnType);
        }

        protected Void handleFailureHttpStatus(final Response response, final URI requestUri, final int httpStatusCode) throws IOException {
            switch (httpStatusCode) {
                case HttpStatus.SC_UNAUTHORIZED:
                    requestAccessTokenWithRefreshToken(new APIHttpRequestTask<T>(request, new TokenRefreshAsyncHandler<T>(request, httpResponseHandler, returnType), httpResponseHandler));
                    return null;
                case HttpStatus.SC_BAD_REQUEST:
                case HttpStatus.SC_FORBIDDEN:
                case HttpStatus.SC_INTERNAL_SERVER_ERROR:
                    sendError(response, requestUri, httpStatusCode, ERROR);
                    return null;
                default:
                    sendError(response, "not expected http status");
                    return null;
            }
        }

        protected void sendError(Response response, URI requestUri, int httpStatusCode, int errorCode) throws IOException {
            if (checkResponseBody(response)) return;

            final APIErrorResult result =  new ObjectMapper().readValue(response.getResponseBody(request.getBodyEncoding()), APIErrorResult.class);
            result.setRequestURL(requestUri == null ? null : requestUri.toString());
            result.setHttpStauts(httpStatusCode);
            httpResponseHandler.sendMessage(Message.obtain(httpResponseHandler, errorCode, 0, 0, result));
        }
    }

    private static final class TokenRefreshAsyncHandler<T> extends APIAsyncHandler<T> {

        public TokenRefreshAsyncHandler(final Request request, final HttpResponseHandler<T> httpResponseHandler, final Class<T> returnType) {
            super(request, httpResponseHandler, returnType);
        }

        protected Void handleFailureHttpStatus(final Response response, final URI requestUri, final int httpStatusCode) throws IOException {
            switch (httpStatusCode) {
                case HttpStatus.SC_BAD_REQUEST:
                case HttpStatus.SC_UNAUTHORIZED:
                    Session.getCurrentSession().invalidateAccessToken();
                    sendError(response, requestUri, httpStatusCode, NEED_TO_LOGIN);
                    return null;
            }

            return super.handleFailureHttpStatus(response, requestUri, httpStatusCode);
        }
    }

    private static boolean requestAccessTokenWithRefreshToken(final APIHttpRequestTask requestTask) {
        final Session session = Session.getCurrentSession();
        session.addCallback(requestTask);
        return Session.getCurrentSession().implicitOpen();
    }

    private HttpRequestTask updateSession() {
        final List<String> token = new ArrayList<String>();
        token.add(getTokenAuthHeaderValue());
        request.getHeaders().put(ServerProtocol.AUTHORIZATION_HEADER_KEY, token);
        return this;
    }

    protected void preRequest() {
        httpResponseHandler.sendMessage(Message.obtain(httpResponseHandler, -1, 0, 0));
    }

    protected void failedRequest(Exception e) {
        httpResponseHandler.sendMessage(Message.obtain(httpResponseHandler, ERROR, 0, 0, e));
    }

    public static void addTokenHeader(final BoundRequestBuilder requestBuilder) {
        final Entry<String,String> entry = KA_HEADER.entrySet().iterator().next();
        requestBuilder.addHeader(entry.getKey(), entry.getValue());

        requestBuilder.addHeader(ServerProtocol.AUTHORIZATION_HEADER_KEY, getTokenAuthHeaderValue());
    }

    private static String getTokenAuthHeaderValue() {
        final StringBuilder autorizationValue = new StringBuilder();
        autorizationValue.append(ServerProtocol.AUTHORIZATION_BEARER).append(ServerProtocol.AUTHORIZATION_HEADER_DELIMITER).append(Session.getCurrentSession().getAccessToken());
        return autorizationValue.toString();
    }
}
