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
import android.content.Intent;

import com.kakao.auth.AuthType;
import com.kakao.auth.authorization.AuthorizationResult;
import com.kakao.auth.authorization.Authorizer;
import com.kakao.util.helper.log.Logger;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

/**
 * authorization code를 얻는 세가지 방법을 중계하는 역할
 * 우선순위에 따라 try하며 실패하면 다음 방법으로 넘어감.
 *
 * @author MJ
 */
public class GetterAuthorizationCode extends Authorizer {
    private final AuthorizationCodeRequest authorizationCodeRequest;
    private final List<AuthorizationCodeHandler> authenticationCodeHandlers = new ArrayList<AuthorizationCodeHandler>();
    private final AuthType authType;

    private AuthorizationCodeHandler currentHandler;
    private Activity callerActivity;

    public GetterAuthorizationCode(final AuthorizationCodeRequest authCodeRequest, final AuthType authType) {
        authorizationCodeRequest = authCodeRequest;
        if(authType == null)
            this.authType = AuthType.KAKAO_TALK;
        else
            this.authType = authType;

        switch (authType) {
            case KAKAO_TALK:
                authenticationCodeHandlers.add(new LoggedInTalkAuthHandler(this));  // talk에 login되어 있는 계정이 있는 경우
                authenticationCodeHandlers.add(new LoggedOutTalkAuthHandler(this)); // talk에 login되어 있는 계정이 없는 경우(신규생성 버튼없는 native login 화면)
                break;
            case KAKAO_STORY:
                authenticationCodeHandlers.add(new LoggedInStoryAuthHandler(this)); // story에 login되어 있는 계정이 있는 경우
                break;
            case KAKAO_TALK_EXCLUDE_NATIVE_LOGIN:
                authenticationCodeHandlers.add(new LoggedInTalkAuthHandler(this));  // talk에 login되어 있는 계정이 있는 경우
                break;
        }

        // default action
        authenticationCodeHandlers.add(new WebViewAuthHandler(this)); // talk이 install되어 있지 않는 경우
    }

    public void tryNextHandler() {
        final Iterator<AuthorizationCodeHandler> iterator = authenticationCodeHandlers.iterator();
        while (iterator.hasNext()) {
            currentHandler = iterator.next();
            iterator.remove();
            if (tryCurrentHandler()) {
                return;
            }
        }
        // handler를 끝까지 돌았는데도 authorization code를 얻지 못했으면 error
        doneOnOAuthError("Failed to get Authorization Code.");
    }

    public AuthorizationCodeRequest getRequest() {
        return authorizationCodeRequest;
    }

    public void setCallerActivity(final Activity callerActivity) {
        this.callerActivity = callerActivity;
    }

    public Activity getCallerActivity() {
        return callerActivity;
    }

    public int getAuthTypeNumber() {
        return authType.getNumber();
    }

    private boolean tryCurrentHandler() {
        if (currentHandler.needsInternetPermission() && !checkInternetPermission()) {
            return false;
        } else {
            return currentHandler.tryAuthorize(authorizationCodeRequest);
        }
    }

    public boolean onActivityResult(final int requestCode, final int resultCode, final Intent data) {
        if (requestCode == authorizationCodeRequest.getRequestCode()) {
            return currentHandler.onActivityResult(requestCode, resultCode, data);
        } else {
            return false;
        }
    }

    protected void done(final AuthorizationResult result) {
        super.done(result);

        clear();
    }

    protected void doneOnOAuthError(final String errorMessage) {
        Logger.e("GetterAuthorizationCode : " + errorMessage);

        done(AuthorizationResult.createAuthCodeOAuthErrorResult(errorMessage));
        clear();
    }

    void clear() {
        currentHandler = null;
        callerActivity = null;
    }

    public void startActivityForResult(final Intent intent, final int requestCode) {
        if (callerActivity != null) {
            callerActivity.startActivityForResult(intent, requestCode);
        }
    }
}
