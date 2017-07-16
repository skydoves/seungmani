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
package com.kakao.auth.authorization.authcode;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;
import android.webkit.CookieSyncManager;

import com.kakao.auth.authorization.AuthorizationResult;
import com.kakao.auth.helper.ServerProtocol;
import com.kakao.util.exception.KakaoException;
import com.kakao.util.helper.Utility;
import com.kakao.util.helper.log.Logger;

/**
 * When KakaoTalk isn't installed in the device, get authorization code through webView,
 *
 * @author MJ
 */
public class WebViewAuthHandler extends AuthorizationCodeHandler {

    private transient KakaoWebViewDialog loginDialog;

    public WebViewAuthHandler(final GetterAuthorizationCode authorizer) {
        super(authorizer);
    }

    @Override
    public void cancel() {
        if (loginDialog != null) {
            loginDialog.dismiss();
            loginDialog = null;
        }
    }

    public boolean onActivityResult(final int requestCode, final int resultCode, final Intent data) {
        return false;
    }

    @Override
    public boolean tryAuthorize(final AuthorizationCodeRequest request) {
        OnWebViewCompleteListener listener = new OnWebViewCompleteListener() {
            @Override
            public void onComplete(String redirectURL, KakaoException exception) {
                onWebViewCompleted(redirectURL, exception);
            }
        };
        try {
            final Bundle parameters = new Bundle();
            parameters.putString(ServerProtocol.CLIENT_ID_KEY, request.getAppKey());
            parameters.putString(ServerProtocol.REDIRECT_URI_KEY, request.getRedirectURI());
            parameters.putString(ServerProtocol.RESPONSE_TYPE_KEY, ServerProtocol.CODE_KEY);

            final Bundle extraParams = request.getExtras();
            if(extraParams != null && !extraParams.isEmpty()){
                for(String key : extraParams.keySet()){
                    String value = extraParams.getString(key);
                    if(value != null){
                        parameters.putString(key, value);
                    }
                }
            }

            final Activity currentContext = authorizer.getCallerActivity();
            synchronizeCookies(currentContext);

            Uri uri = Utility.buildUri(ServerProtocol.AUTH_AUTHORITY, ServerProtocol.AUTHORIZE_CODE_PATH, parameters);
            loginDialog = new KakaoWebViewDialog(currentContext, uri.toString(), listener);
            loginDialog.show();
        } catch (Throwable t) {
            Logger.e("WebViewAuthHandler is failed", t);
            return false;
        }
        return true;
    }

    private void synchronizeCookies(Context currentContext) {
        Context applicationContext = currentContext.getApplicationContext();
        Context context = (applicationContext != null) ? applicationContext : currentContext;
        CookieSyncManager syncManager = CookieSyncManager.createInstance(context);
        syncManager.sync();
    }

    void onWebViewCompleted(final String redirectURL, final KakaoException exception) {
        AuthorizationResult result;
        if (redirectURL != null) {
            result = AuthorizationResult.createSuccessAuthCodeResult(redirectURL);
        } else {
            if (exception.isCancledOperation()) {
                result = AuthorizationResult.createAuthCodeCancelResult(exception.getMessage());
            } else {
                result = AuthorizationResult.createAuthCodeOAuthErrorResult(exception.getMessage());
            }
        }
        authorizer.done(result);
    }
}
