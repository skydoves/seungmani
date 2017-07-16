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


import com.kakao.auth.APIErrorResult;
import com.kakao.auth.Session;
import com.kakao.util.helper.log.Logger;

/**
 * 로그아웃 요청 ({@link UserManagement#requestLogout(LogoutResponseCallback)}) 호출할 때 넘겨주고 콜백을 받는다.
 * @author MJ
 */
public abstract class LogoutResponseCallback extends UserResponseCallback {
    /**
     * 로그아웃을 성공적으로 마친 경우로
     * 일반적으로 로그인창으로 이동하도록 구현한다.
     * @param userId 로그아웃된 사용자의 id
     */
    public abstract void onSuccess(final long userId);

    /**
     * 로그아웃 요청중 exception이 발생하였으나 강제로 세션을 클로즈 한 후 callback이 불리므로 로그인 창으로 이동하도록 구현한다.
     * 실패한 아래의 이유들은 디버깅용으로 사용할 수 있다.  <br/>
     * {@link com.kakao.auth.ErrorCode#INVALID_PARAM_CODE}, <br/>
     * {@link com.kakao.auth.ErrorCode#INVALID_SCOPE_CODE}, <br/>
     * {@link com.kakao.auth.ErrorCode#NOT_SUPPORTED_API_CODE}, <br/>
     * {@link com.kakao.auth.ErrorCode#INTERNAL_ERROR_CODE},<br/>
     * {@link com.kakao.auth.ErrorCode#INVALID_TOKEN_CODE},<br/>
     * {@link com.kakao.auth.ErrorCode#CLIENT_ERROR_CODE}, <br/>
     * {@link com.kakao.auth.ErrorCode#EXCEED_LIMIT_CODE}, <br/>
     * {@link com.kakao.auth.ErrorCode#KAKAO_MAINTENANCE_CODE}<br/>
     * @param errorResult 실패한 이유
     */
    public abstract void onFailure(final APIErrorResult errorResult);

    /**
     * {@link UserResponseCallback}를 구현한 것으로 로그아웃이 성공했을 때 호출된다.
     * 세션을 닫고 캐시를 삭제한 후에 사용자 콜백 {@link #onSuccess(long)}을 호출한다.
     * return값으로 온 user객체가 비정상이더라도 강제 로그아웃을 진행한다.
     * @param user 로그아웃된 사용자
     */
    @Override
    public void onSuccessUser(final User user) {
        if (user == null || user.getId() <= 0)
            forceToLogout("LogoutResponseCallback : onSuccessUser is called but the result user is null.", new APIErrorResult("the result of logout request is null."));
        else {
            Logger.d("LogoutResponseCallback: logout successfully. user = " + user);
            Session.getCurrentSession().close();
            onSuccess(user.getId());
        }
    }

    /**
     * {@link com.kakao.auth.http.HttpResponseHandler}를 구현한 것으로 로그아웃 요청 전 또는 요청 중에 세션이 닫혀 로그인이 필요할 때 호출된다.
     * 로그아웃이 세션을 닫는 것이 목적이기 때문에 강제 로그아웃을 진행한다.
     * @param errorResult 세션이 닫힌 이유
     */
    @Override
    public void onHttpSessionClosedFailure(final APIErrorResult errorResult) {
        forceToLogout("LogoutResponseCallback: session is closed before requesting logout. logout forcefully", errorResult);
    }

    /**
     * {@link com.kakao.auth.http.HttpResponseHandler}를 구현한 것으로 로그아웃 요청 중에 서버에서 로그아웃을 수행하지 못했다는 결과를 받았을 때 호출된다.
     * 로그아웃이 세션을 닫는 것이 목적이기 때문에 강제 로그아웃을 진행한다.
     * @param errorResult 실패한 결과
     */
    @Override
    public void onHttpFailure(final APIErrorResult errorResult) {
        forceToLogout("LogoutResponseCallback: server error occurred during requesting logout. logout forcefully. ", errorResult);
    }

    /**
     * 강제로 로그아웃을 진행하고 사용자가 정의한 {@link #onFailure(APIErrorResult)}를 호출한다.
     * @param msg 강제로그아웃하는 메시지
     * @param errorResult 로그아웃 실패 이유
     */
    private void forceToLogout(final String msg, final APIErrorResult errorResult) {
        Logger.d(msg + errorResult);
        Session.getCurrentSession().close();
        onFailure(errorResult);
    }
}
