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

import android.content.Intent;

import com.kakao.util.helper.TalkProtocol;

/**
 * 설치되어 있는 톡의 activity를 이용하여 id/password로 로그인 후 authorization code를 받아온다.
 * @author MJ
 */
public class LoggedOutTalkAuthHandler extends AuthorizationCodeHandler {

    public LoggedOutTalkAuthHandler(GetterAuthorizationCode authorizer) {
        super(authorizer);
    }

    @Override
    protected Intent getIntent(AuthorizationCodeRequest request) {
        return TalkProtocol.createLoggedOutActivityIntent(authorizer.getApplicationContext(),
                request.getAppKey(), request.getRedirectURI(), request.getExtras(),
                needProjectLogin(request.getExtras()));
    }

}
