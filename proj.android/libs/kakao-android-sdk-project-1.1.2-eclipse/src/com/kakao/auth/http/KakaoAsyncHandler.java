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

import android.os.Message;

import com.fasterxml.jackson.databind.ObjectMapper;
import com.kakao.auth.APIErrorResult;
import com.kakao.util.helper.log.Logger;
import com.ning.http.client.AsyncCompletionHandler;
import com.ning.http.client.Request;
import com.ning.http.client.Response;

import org.apache.http.HttpStatus;

import java.io.IOException;
import java.net.URI;

/**
 * response를 받아 response에 따라 사용자가 등록한 HttpResponseHandler<T>을 불러주는 역할을 담당한다.
 * @param <T> : 요청이 성공한 경우 HttpResponseHandler<T>가 받게되는 return type
* @author MJ
*/
public abstract class KakaoAsyncHandler<T> extends AsyncCompletionHandler<Void> {
    protected final HttpResponseHandler<T> httpResponseHandler;
    protected final Request request;
    protected final Class<T> returnType;

    public KakaoAsyncHandler(final Request request, final HttpResponseHandler<T> httpResponseHandler, final Class<T> returnType) {
        this.request = request;
        this.httpResponseHandler = httpResponseHandler;
        this.returnType = returnType;
    }

    @Override
    public Void onCompleted(final Response response) throws Exception {
        final URI requestUri = response.getUri();
        try {
            if (!response.hasResponseStatus()) {
                sendError(response, "the response didn't have a response status");
                return null;
            }

            final int httpStatusCode = response.getStatusCode();
            if (httpStatusCode != HttpStatus.SC_OK) {
                return handleFailureHttpStatus(response, requestUri, httpStatusCode);
            } else {
                if(returnType.equals(Void.class)){
                    httpResponseHandler.sendMessage(Message.obtain(httpResponseHandler, HttpRequestTask.SUCCESS, 0, 0));
                } else {
                    if (checkResponseBody(response)) return null;
                    final String responseValue = response.getResponseBody(request.getBodyEncoding());
                    Logger.d(false, "[ResponseBody] : " + responseValue);

                    Object result;
                    if(returnType.equals(String.class)) {
                        result = responseValue;
                    } else if(returnType.equals(Character.class)) {
                        result = responseValue.charAt(0);
                    } else {
                        result = new ObjectMapper().readValue(responseValue, returnType);
                    }
                    httpResponseHandler.sendMessage(Message.obtain(httpResponseHandler, HttpRequestTask.SUCCESS, 0, 0, result));
                }
                return null;
            }
        } catch (Exception e) {
            sendError(response, e.toString());
            return null;
        }
    }

    public void onThrowable(final Throwable t) {
        httpResponseHandler.sendMessage(Message.obtain(httpResponseHandler, HttpRequestTask.ERROR, 0, 0,
            new APIErrorResult(request.getUrl(), "error occurred during http request. t= "+ t.toString())));
        Logger.e(t);
    }

    protected void sendError(final Response response, final String msg) {
        final APIErrorResult result = new APIErrorResult(request.getUrl(), msg);
        result.setHttpStauts(response.getStatusCode());
        httpResponseHandler.sendMessage(Message.obtain(httpResponseHandler, HttpRequestTask.ERROR, 0, 0, result));
    }

    protected boolean checkResponseBody(final Response response) {
        if (!response.hasResponseBody()) {
            sendError(response, "the response didn't have a body");
            return true;
        }
        return false;
    }

    protected abstract Void handleFailureHttpStatus(Response response, URI requestUri, int httpStatusCode) throws IOException;
}
