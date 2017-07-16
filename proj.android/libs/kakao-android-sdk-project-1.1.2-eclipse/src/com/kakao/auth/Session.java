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
package com.kakao.auth;

import android.app.Activity;
import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Looper;
import android.os.SystemClock;
import android.webkit.CookieManager;
import android.webkit.CookieSyncManager;

import com.kakao.auth.authorization.AuthRequestBase;
import com.kakao.auth.authorization.AuthorizationResult;
import com.kakao.auth.authorization.Authorizer;
import com.kakao.auth.authorization.accesstoken.AccessToken;
import com.kakao.auth.authorization.accesstoken.AccessTokenRequest;
import com.kakao.auth.authorization.accesstoken.GetterAccessToken;
import com.kakao.auth.authorization.authcode.AuthorizationCode;
import com.kakao.auth.authorization.authcode.AuthorizationCodeRequest;
import com.kakao.auth.authorization.authcode.GetterAuthorizationCode;
import com.kakao.auth.helper.ServerProtocol;
import com.kakao.auth.helper.SharedPreferencesCache;
import com.kakao.auth.http.HttpRequestTask;
import com.kakao.auth.http.HttpResponseHandler;
import com.kakao.auth.rest.APIHttpRequestTask;
import com.kakao.util.exception.KakaoException;
import com.kakao.util.exception.KakaoException.ErrorType;
import com.kakao.util.helper.CommonProtocol;
import com.kakao.util.helper.SystemInfo;
import com.kakao.util.helper.Utility;
import com.kakao.util.helper.log.Logger;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

/**
 * 로그인 상태를 유지 시켜주는 객체로 access token을 관리한다.
 *
 * @author MJ
 */
public class Session implements Authorizer.OnAuthorizationListener {
    private static Session currentSession;
    public static final String REDIRECT_URL_PREFIX = "kakao";
    public static final String REDIRECT_URL_POSTFIX = "://oauth";
    private static final String COOKIE_SEPERATOR = ";";
    private static final String COOKIE_NAME_VALUE_DELIMITER = "=";
    private static final int DEFAULT_TOKEN_REQUEST_TIME_MILLIS = 3 * 60 * 60 * 1000; // 3 hours
    private static final int RETRY_TOKEN_REQUEST_TIME_MILLIS = 5 * 60 * 1000; // 5 minutes
    private static final String EXPIRES_IN_MILLIS = "expiresInMillis";
    public static final int AUTHORIZATION_CODE_REQUEST = 1;
    public static final int ACCESS_TOKEN_REQUEST = 2;

    private final Object INSTANCE_LOCK = new Object();

    private final Context context;
    private final String appKey;
    private final String redirectUri;
    private final AuthType[] authTypes;
    private final ApprovalType approvalType;
    private final SharedPreferencesCache appCache;
    private final List<SessionCallback> sessionCallbacks;
    private final Handler sessionCallbackHandler;
    private final AlarmManager tokenAlarmManager;
    private final PendingIntent alarmIntent;

    // 아래 값들은 변경되는 값으로 INSTANCE_LOCK의 보호를 받는다.
    private SessionState state;
    // close시 삭제
    private RequestType requestType;
    private AuthorizationCode authorizationCode;
    private AccessToken accessToken;

    private GetterAuthorizationCode getterAuthorizationCode;
    private GetterAccessToken getterAccessToken;


    /**
     * Application 이 최초 구동시, Session 을 초기화 합니다.
     *
     * @param context   세션을 접근하는 context. 여기로 부터 app key와 redirect uri를 구해온다.
     */
    public static synchronized void initialize(final Context context) {
        initialize(context, ApprovalType.INDIVIDUAL);
    }

    /**
     * Application 이 최초 구동시, Session 을 초기화 합니다.
     *
     * @param context   세션을 접근하는 context. 여기로 부터 app key와 redirect uri를 구해온다.
     * @param authTypes 로그인시 인증받을 타입을 지정한다. 지정하지 않을 시 가능한 모든 옵션이 지정된다. 예시) AuthType.KAKAO_TALK
     */
    public static synchronized void initialize(final Context context, final AuthType... authTypes) {
        initialize(context, ApprovalType.INDIVIDUAL, authTypes);
    }

    public static synchronized void initialize(final Context context, final ApprovalType approvalType, final AuthType... authTypes) {
        if (currentSession != null) {
            currentSession.clearCallbacks();
            currentSession.close();
        }
        SystemInfo.initialize(context);
        currentSession = new Session(context, approvalType, authTypes);
    }

    /**
     * 현재 세션을 반환한다.
     *
     * @return 현재 세션 객체
     */

    public static synchronized Session getCurrentSession() {
        if (currentSession == null) {
            throw new IllegalStateException("Session is not initialized. Call Session#initializeSession first.");
        }

        return currentSession;
    }

    /**
     * 세션 오픈을 진행한다.
     * {@link SessionState#OPENED} 상태이면 바로 종료.
     * {@link SessionState#CLOSED} 상태이면 authorization code 요청. 에러/취소시 {@link SessionState#CLOSED}
     * {@link SessionState#OPENABLE} 상태이면 code 또는 refresh token 이용하여  access token 을 받아온다. 에러/취소시 {@link SessionState#CLOSED}, refresh 취소시에만 {@link SessionState#OPENABLE} 유지.
     * param으로 받은 콜백으로 그 결과를 전달한다.
     *
     * @param authType 인증받을 타입. 예를 들어, 카카오톡 또는 카카오스토리 또는 직접 입력한 카카오계정
     * @param callerActivity 세션오픈을 호출한 activity
     */
    public void open(final AuthType authType, final Activity callerActivity) {
         internalOpen(authType, callerActivity);
    }

    /**
     * 토큰 갱신이 가능한지 여부를 반환한다.
     * 토큰 갱신은 background로 사용자가 모르도록 진행한다.
     *
     * @return 토큰 갱신을 진행할 때는 true, 토큰 갱신을 하지 못할때는 false를 return 한다.
     */
    public boolean implicitOpen() {
        if (isOpened() || (isOpenable() && accessToken.hasRefreshToken())) {
            internalOpen(null, null);
            return true;
        } else {
            return false;
        }
    }

    /**
     * 명시적 강제 close(로그아웃/탈퇴). request중 인 것들은 모두 실패를 받게 된다.
     * token을 삭제하기 때문에 authorization code부터(로그인 버튼) 다시 받아서 세션을 open 해야한다.
     */
    public void close() {
        internalClose(null, true);
    }

    /**
     * 토큰 유효성을 검사하고 만료된 경우 갱신시켜 준다.
     */
    public void checkAccessTokenInfo() {
        if (isClosed()) {
            deregisterTokenManger();
        } else if (isOpenable()) {
            implicitOpen();
        } else {
            requestAccessTokenInfo();
        }
    }

    public AuthType[] getAuthTypes() {
        return authTypes;
    }

    public boolean isProjectLogin() {
        return approvalType != null && approvalType == ApprovalType.PROJECT;
    }

    /**
     * 현재 세션이 가지고 있는 access token이 유효한지를 검사후 세션의 상태를 반환한다.
     * 만료되었다면 opened 상태가 아닌 opening상태가 반환된다.
     * @return 세션의 상태
     */
    public final SessionState checkState() {
        synchronized (INSTANCE_LOCK) {
            if (state.isOpened() && !accessToken.hasValidAccessToken() && accessToken.hasRefreshToken()) {
                synchronized (INSTANCE_LOCK) {
                    state = SessionState.OPENABLE;
                    requestType = null;
                    authorizationCode = AuthorizationCode.createEmptyCode();
                }
            }
            return state;
        }
    }

    /**
     * 현재 세션의 상태
     * @return 세션의 상태
     */
    private SessionState getState() {
        synchronized (INSTANCE_LOCK) {
            return state;
        }
    }

    /**
     * 현재 세션이 열린 상태인지 여부를 반환한다.
     * @return 세션이 열린 상태라면 true, 그외의 경우 false를 반환한다.
     */
    public final boolean isOpened() {
        final SessionState state = checkState();
        return state == SessionState.OPENED;
    }

    /**
     * 현재 세션이 오픈중(갱신 포함) 상태인지 여부를 반환한다.
     *
     * @return 세션 오픈 진행 중이면 true, 그외 경우는 false를 반환한다.
     */
    public boolean isOpenable() {
        final SessionState state = checkState();
        return state == SessionState.OPENABLE;
    }

    /**
     * 현재 세션이 닫힌 상태인지 여부를 반환한다.
     * @return 세션이 닫힌 상태라면 true, 그외의 경우 false를 반환한다.
     */
    public final boolean isClosed() {
        final SessionState state = checkState();
        return state == SessionState.CLOSED;
    }

    /**
     * 현재 진행 중인 요청 타입
     * @return 현재 진행 중인 요청 타입
     */
    private final RequestType getRequestType() {
        synchronized (INSTANCE_LOCK) {
            return requestType;
        }
    }

    /**
     * 현재 세션이 가지고 있는 access token을 반환한다.
     * @return access token
     */
    public final String getAccessToken() {
        synchronized (INSTANCE_LOCK) {
            return (accessToken == null) ? null : accessToken.getAccessTokenString();
        }
    }

    /**
     * 앱 캐시를 반환한다.
     * @return 앱 캐시
     */
    public static SharedPreferencesCache getAppCache() {
        final Session session = Session.getCurrentSession();
        return session.appCache;
    }

    /**
     * authorization code 결과를 받아 처리한다. (authcode 저장, state 변경, accesstoken요청)
     */
    public void onAuthCodeCompleted(final AuthorizationResult result) {
        AuthorizationCode authCode = null;
        KakaoException exception = null;
        if (getRequestType() == null) {
            exception = new KakaoException(ErrorType.AUTHORIZATION_FAILED, "session is closed during requesting authorization code. result will be ignored. state = " + getState());
        } else if (result == null) {
            exception = new KakaoException(ErrorType.AUTHORIZATION_FAILED, "the result of authorization code request is null.");
        } else {
            final String resultRedirectURL = result.getRedirectURL();
            if (result.isSuccess()) {
                // 기대 했던 redirect uri 일치
                if (resultRedirectURL != null && resultRedirectURL.startsWith(redirectUri)) {
                    authCode = AuthorizationCode.createFromRedirectedUri(result.getRedirectUri());
                    // authorization code가 포함되지 않음
                    if (!authCode.hasAuthorizationCode()) {
                        authCode = null;
                        exception = new KakaoException(ErrorType.AUTHORIZATION_FAILED, "the result of authorization code request does not have authorization code.");
                    }
                    // 기대 했던 redirect uri 불일치
                } else {
                    exception = new KakaoException(ErrorType.AUTHORIZATION_FAILED, "the result of authorization code request mismatched the registered redirect uri. msg = " + result.getResultMessage());
                }
            } else if (result.isCanceled()) {
                exception = new KakaoException(ErrorType.CANCELED_OPERATION, result.getResultMessage());
            } else {
                exception = new KakaoException(ErrorType.AUTHORIZATION_FAILED, result.getResultMessage());
            }
        }

        synchronized (INSTANCE_LOCK) {
            final SessionState previousState = state;
            if (authCode != null) {
                this.authorizationCode = authCode;
                state = SessionState.OPENABLE;
                // log만 남기고 callback은 호출되지 않는다.
                onStateChange(previousState, state, requestType, null, false);
                //  request가 성공적으로 끝났으니 request는 reset
                requestType = null;
            } else {
                internalClose(exception, false);
                return;
            }
        }
        // request AccessToken
        open(null, null);
    }

    /**
     * access token 결과를 받아 처리한다. (access token 저장, state 변경)
     */
    public void onAccessTokenCompleted(final AuthorizationResult result) {
        AccessToken resultAccessToken = null;
        KakaoException exception = null;
        if (getRequestType() == null) {
            exception = new KakaoException(ErrorType.AUTHORIZATION_FAILED, "session is closed during requesting access token. result will be ignored. state = " + getState());
        } else if (result == null) {
            exception = new KakaoException(ErrorType.AUTHORIZATION_FAILED, "the result of access token request is null.");
        } else {
            if (result.isSuccess()) {
                resultAccessToken = result.getAccessToken();
                if (!resultAccessToken.hasValidAccessToken()) {
                    resultAccessToken = null;
                    exception = new KakaoException(ErrorType.AUTHORIZATION_FAILED, "the result of access token request is invalid access token.");
                }
            } else if (result.isCanceled()) {
                exception = new KakaoException(ErrorType.CANCELED_OPERATION, result.getResultMessage());
            } else if (result.isAuthError()) {
                exception = new KakaoException(ErrorType.AUTHORIZATION_FAILED, result.getResultMessage());
            } else {
                exception = new KakaoException(ErrorType.UNSPECIFIED_ERROR, result.getResultMessage());
            }
        }

        synchronized (INSTANCE_LOCK) {
            final SessionState previousState = state;
            if (resultAccessToken != null) {
                //authorization code는 한번 밖에 사용하지 못한다.
                authorizationCode = AuthorizationCode.createEmptyCode();
                // refresh 요청에는 refresh token이 내려오지 않을 수 있으므로 accessToken = resultAccessToken을 하면 안된다.
                updateAccessToken(resultAccessToken);
                state = SessionState.OPENED;
            } else {
                state = SessionState.OPENABLE;
                // code로 요청한 경우는 code는 일회성이므로 재사용 불가. exception 종류에 상관 없이 무조건 close
                // refresh token으로 요청한 경우는 서버에서 refresh token을 재사용할 수 없다고 에러를 준 경우만 close.
                if ((requestType != null && requestType.isAccessTokenRequest()) || exception.getErrorType() == ErrorType.AUTHORIZATION_FAILED) {
                    internalClose(exception, false);
                }
            }
            requestType = null;
            onStateChange(previousState, state, requestType, null, false);
        }
    }

    private void updateAccessToken(AccessToken resultAccessToken) {
        synchronized (INSTANCE_LOCK) {
            accessToken.updateAccessToken(resultAccessToken);
            saveTokenToCache(accessToken);
        }
    }

    /**
     * 현재 세션이 가지고 있는 access token 을 무효화 시킨다.
     */
    public void invalidateAccessToken() {
        synchronized (INSTANCE_LOCK) {
            accessToken.updateAccessToken(AccessToken.createEmptyToken());
            saveTokenToCache(accessToken);
        }
    }

    private Session(final Context context, final ApprovalType approvalType, final AuthType... authTypes) {
        if (context == null)
            throw new KakaoException(ErrorType.ILLEGAL_ARGUMENT, "cannot create Session without Context.");

        this.context = context;
        this.approvalType = approvalType;
        this.appKey = Utility.getMetadata(context, CommonProtocol.APP_KEY_PROPERTY);
        if (appKey == null) {
            throw new KakaoException(ErrorType.MISS_CONFIGURATION, String.format("need to declare %s in your AndroidManifest.xml", CommonProtocol.APP_KEY_PROPERTY));
        }
        this.redirectUri = REDIRECT_URL_PREFIX + this.appKey + REDIRECT_URL_POSTFIX;

        if (authTypes == null || authTypes.length == 0) {
            this.authTypes = AuthType.values();
        } else {
            this.authTypes = authTypes;
        }

        this.appCache = new SharedPreferencesCache(context, appKey);
        this.sessionCallbacks = new ArrayList<SessionCallback>();
        this.sessionCallbackHandler = new Handler(Looper.getMainLooper()); //세션 callback은 main thread에서 호출되도록 한다.
        this.tokenAlarmManager = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
        this.alarmIntent = PendingIntent.getBroadcast(context, 0, new Intent(context, TokenAlarmReceiver.class), 0);

        appCache.reloadAll();
        synchronized (INSTANCE_LOCK) {
            authorizationCode = AuthorizationCode.createEmptyCode();
            accessToken = AccessToken.createFromCache(appCache);
            if (accessToken.hasValidAccessToken()) {
                this.state = SessionState.OPENED;
            } else if (accessToken.hasRefreshToken()) {
                this.state = SessionState.OPENABLE;
            } else {
                this.state = SessionState.CLOSED;
                internalClose(null, false);
            }
        }
    }

    private void internalOpen(final AuthType authType, final Activity callerActivity) {
        final SessionState currentState = getState();

        Logger.d("++ currentState : " + currentState);
        if (currentState.isOpened()) {
            // 이미 open이 되어 있다.
            notifySessionState(currentState, null);
            return;
        }

        //끝나지 않은 request가 있다.
        if (getRequestType() != null) {
            Logger.d(getRequestType() + " is still doing.");
            return;
        }
        try {
            synchronized (INSTANCE_LOCK) {
                switch (state) {
                    case CLOSED:
                        if (appKey != null) {
                            this.requestType = RequestType.GETTING_AUTHORIZATION_CODE;
                            requestAuthCode(authType, callerActivity);
                        } else {
                            internalClose(new KakaoException(ErrorType.AUTHORIZATION_FAILED, "can not request authorization code because appKey or redirectUri is invalid."), false);
                        }
                        break;
                    case OPENABLE:
                        if (accessToken.hasRefreshToken()) {
                            requestType = RequestType.REFRESHING_ACCESS_TOKEN;
                            requestAccessToken();
                        } else if (authorizationCode.hasAuthorizationCode()) {
                            requestType = RequestType.GETTING_ACCESS_TOKEN;
                            requestAccessToken();
                        } else {
                            internalClose(new KakaoException(ErrorType.AUTHORIZATION_FAILED, "can not request access token because both authorization code and refresh token are invalid."), false);
                        }
                        break;
                    default:
                        throw new KakaoException(ErrorType.AUTHORIZATION_FAILED, "current session state is not possible to open. state = " + state);
                }
            }
        } catch (KakaoException e) {
            internalClose(e, false);
        }
    }

    private void requestAuthCode(final AuthType authType, final Activity callerActivity) {
        getterAuthorizationCode = new GetterAuthorizationCode((AuthorizationCodeRequest) createAuthRequest(), authType);
        getterAuthorizationCode.setOnAuthorizationListener(this);
        getterAuthorizationCode.setCallerActivity(callerActivity);
        getterAuthorizationCode.tryNextHandler();
    }

    private void requestAccessToken() {
        getterAccessToken = new GetterAccessToken((AccessTokenRequest) createAuthRequest());
        getterAccessToken.setOnAuthorizationListener(this);
        getterAccessToken.requestAccessToken();
    }

    private AuthRequestBase createAuthRequest() {
        final AuthRequestBase authRequest;
        if (requestType == RequestType.GETTING_AUTHORIZATION_CODE) {
            authRequest = AuthorizationCodeRequest.createNewRequest(appKey, redirectUri);
        } else if(requestType == RequestType.REFRESHING_ACCESS_TOKEN){
            authRequest = AccessTokenRequest.createRequestWithRefreshToken(context, appKey, redirectUri, accessToken.getRefreshTokenString());
        } else if (requestType == RequestType.GETTING_ACCESS_TOKEN)  {
            authRequest = AccessTokenRequest.createRequestWithAuthorizationCode(context, appKey, redirectUri, authorizationCode.getAuthorizationCode());
        } else {
            throw new KakaoException(ErrorType.AUTHORIZATION_FAILED, "unknown requst type. requestType = " + requestType);
        }
        authRequest.addApprovalType(approvalType);

        final SessionState previous = state;
        state = SessionState.OPENING;
        onStateChange(previous, state, requestType, null, false);

        return authRequest;
    }

    @Override
    public void onAuthorizationCompletion(final AuthorizationResult result) {
        if (result.isAuthorizationCodeRequest()) {
            getterAuthorizationCode = null;
            onAuthCodeCompleted(result);
        } else if (result.isAccessTokenRequest()) {
            getterAccessToken = null;
            onAccessTokenCompleted(result);
        }
    }

    /**
     * 로그인 activity를 이용하여 sdk에서 필요로 하는 activity를 띄운다.
     * 따라서 해당 activity의 결과를 로그인 activity가 받게 된다.
     * 해당 결과를 세션이 받아서 다음 처리를 할 수 있도록 로그인 activity의 onActivityResult에서 해당 method를 호출한다.
     * @param requestCode
     * @param resultCode
     * @param data
     * @return
     */
    public boolean handleActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == AUTHORIZATION_CODE_REQUEST) {
            getterAuthorizationCode.onActivityResult(requestCode, resultCode, data);
            return true;
        }
        return false;
    }

    /**
     * 세션 상태 변화 콜백을 받고자 할때 콜백을 등록한다.
     * @param callback 추가할 세션 콜백
     */
    public void addCallback(final SessionCallback callback) {
        synchronized (sessionCallbacks) {
            if (callback != null && !sessionCallbacks.contains(callback)) {
                sessionCallbacks.add(callback);
            }
        }
    }

    /**
     * 더이상 세션 상태 변화 콜백을 받고 싶지 않을 때 삭제한다.
     * @param callback 삭제할 콜백
     */
    public void removeCallback(final SessionCallback callback) {
        synchronized (sessionCallbacks) {
            if (callback != null) {
                sessionCallbacks.remove(callback);
            }
        }
    }

    private void clearCallbacks() {
        synchronized (sessionCallbacks) {
            sessionCallbacks.clear();
        }
    }

    /**
     * 세션을 close하여 처음부터 새롭게 세션 open을 진행한다.
     * @param kakaoException exception이 발생하여 close하는 경우 해당 exception을 넘긴다.
     * @param forced         강제 close 여부. 강제 close이면 이미 close가 되었어도 callback을 호출한다.
     */
    private void internalClose(final KakaoException kakaoException, final boolean forced) {
        synchronized (INSTANCE_LOCK) {
            final SessionState previous = state;
            state = SessionState.CLOSED;
            requestType = null;
            authorizationCode = AuthorizationCode.createEmptyCode();
            accessToken = AccessToken.createEmptyToken();
            onStateChange(previous, state, requestType, kakaoException, forced);
        }
        if (this.appCache != null) {
            this.appCache.clearAll();
        }

        removeCookiesForKakaoDomain(context);
    }

    private static void removeCookiesForKakaoDomain(final Context context) {
        removeCookiesForDomain(context, "kakao.com");
        removeCookiesForDomain(context, ".kakao.com");
        removeCookiesForDomain(context, "https://kakao.com");
        removeCookiesForDomain(context, "https://.kakao.com");
        removeCookiesForDomain(context, "kakao.co.kr");
        removeCookiesForDomain(context, ".kakao.co.kr");
        removeCookiesForDomain(context, "https://kakao.co.kr");
        removeCookiesForDomain(context, "https://.kakao.co.kr");
    }

    private static void removeCookiesForDomain(final Context context, final String domain) {
        // CookieManager를 쓰려면 CookieSyncManager를 만들어야 하는 버그가 있다.
        CookieSyncManager syncManager = CookieSyncManager.createInstance(context);
        syncManager.sync();

        CookieManager cookieManager = CookieManager.getInstance();

        // domain으로 쿠키를 삭제하는 API가 제공되지 않으므로 삭제하고 싶은 쿠키를 강제로 expire 시킨다음 removeExpiredCookie 호출한다.
        String cookieForDomain = cookieManager.getCookie(domain);
        if (cookieForDomain == null) {
            return;
        }

        String[] cookiesForDomain = cookieForDomain.split(COOKIE_SEPERATOR);
        for (String currentCookie : cookiesForDomain) {
            String[] cookieNameAndValue = currentCookie.split(COOKIE_NAME_VALUE_DELIMITER);
            if (cookieNameAndValue.length > 0) {
                // value를 reset하고 expire 시각을 강제로 set.
                String revisedCookie = cookieNameAndValue[0].trim() + "=;expires=Web, 18 Mar 2010 09:00:01 GMT;";
                cookieManager.setCookie(domain, revisedCookie);
            }
        }
        cookieManager.removeExpiredCookie();
    }

    private void saveTokenToCache(final AccessToken newToken) {
        if (newToken != null && appCache != null) {
            newToken.saveAccessTokenToCache(appCache);
        }
    }

    private void onStateChange(final SessionState previousState, final SessionState newState,
                               final RequestType requestType, final KakaoException exception,
                               boolean forced) {
        if (!forced && (previousState == newState) && exception == null) {
            return;
        }

        Logger.d("Session State changed : %s -> %s \n ex = %s, request_type = %s", previousState, newState, (exception != null ? ", ex=" + exception.getMessage() : ""), requestType);

        // 사용자에게 opening을 state를 알려줄 필요는 없는듯.
        if (newState.isOpenable())
            return;

        notifySessionState(newState, exception);
    }

    private void notifySessionState(final SessionState newState, final KakaoException exception) {
        final List<SessionCallback> dumpSessionCallbacks = new ArrayList<SessionCallback>(sessionCallbacks);
        Runnable runCallbacks = new Runnable() {
            public void run() {
                if (newState.isOpened()) {
                    final int interval = Math.min(DEFAULT_TOKEN_REQUEST_TIME_MILLIS, accessToken.getRemainedExpiresInAccessTokenTime());
                    registerTokenManger(interval);
                } else if (newState.isClosed()) {
                    deregisterTokenManger();
                }

                for (SessionCallback callback : dumpSessionCallbacks) {
                    if (newState.isOpened()) {
                        callback.onSessionOpened();
                    } else if (newState.isClosed()) {
                        callback.onSessionClosed(exception);
                    }

                }
            }
        };
        //세션 callback은 main thread에서 호출되도록 한다.
        sessionCallbackHandler.post(runCallbacks);
    }

    private void registerTokenManger(final int interval) {
        tokenAlarmManager.cancel(alarmIntent);
        tokenAlarmManager.setInexactRepeating(AlarmManager.ELAPSED_REALTIME, SystemClock.elapsedRealtime() + interval, interval, alarmIntent);
    }

    private void deregisterTokenManger() {
        tokenAlarmManager.cancel(alarmIntent);
    }

    private void requestAccessTokenInfo() {
        final String url = HttpRequestTask.createBaseURL(ServerProtocol.API_AUTHORITY, ServerProtocol.USER_ACCESS_TOKEN_INFO_PATH);
        APIHttpRequestTask.requestGet(new HttpResponseHandler<Map>() {
            @Override
            public void onHttpSuccess(Map resultObj) {
                Object expiresInMillis = resultObj.get(EXPIRES_IN_MILLIS);
                if (expiresInMillis instanceof Integer) {
                    final int interval = Math.min(DEFAULT_TOKEN_REQUEST_TIME_MILLIS, (Integer) expiresInMillis);
                    registerTokenManger(interval);
                } else {
                    Logger.e(String.format("%s of response is incompatible", url));
                }
            }

            @Override
            public void onHttpSessionClosedFailure(APIErrorResult errorResult) {
                if (isOpenable()) {
                    implicitOpen();
                }
            }

            @Override
            public void onHttpFailure(APIErrorResult errorResult) {
                registerTokenManger(RETRY_TOKEN_REQUEST_TIME_MILLIS);
            }
        }, Map.class, url, null);
    }

    public Context getContext() {
        return context;
    }

    /**
     * @author MJ
     */
    private enum SessionState {
        /**
         * memory와 cache에 session 정보가 없는 전혀 상태.
         * 처음 session에 접근할 때 또는 session을 close(예를 들어 로그아웃, 탈퇴)한 상태.
         * open({@link com.kakao.auth.Session.RequestType#GETTING_AUTHORIZATION_CODE}) : 성공 - {@link #OPENABLE}, 실패 - 그대로 CLOSED
         * close(명시적 close) : 그대로 CLOSED
         */
        CLOSED,
        /**
         * {@link #CLOSED}상태에서 token을 발급 받기 위해 authorization code를 발급 받아 valid한 authorization code를 가지고 있는 상태.
         * 또는 토큰이 만료되었으나 refresh token을 가지고 있는 상태.
         * open({@link com.kakao.auth.Session.RequestType#GETTING_ACCESS_TOKEN} 또는 {@link com.kakao.auth.Session.RequestType#REFRESHING_ACCESS_TOKEN}) :  성공 - {@link #OPENED}, 실패 - {@link #CLOSED}
         * close(명시적 close) : {@link #CLOSED}
         */
        OPENABLE,

        /**
         * access token을 성공적으로 발급 받아 valid access token을 가지고 있는 상태.
         * 토큰 만료 : {@link #OPENABLE}
         * close(명시적 close) : {@link #CLOSED}
         */
        OPENED,

        /**
         * {@link #CLOSED} 상태에서 authcode를 요청하는 경우, code나 refresh token으로 token을 요청하는 경우 중간 단계.
         *
         */
        OPENING;

        private boolean isClosed() {
            return this == SessionState.CLOSED;
        }

        private boolean isOpenable() {
            return this == SessionState.OPENABLE;
        }

        private boolean isOpened() {
            return this == SessionState.OPENED;
        }

        private boolean isOpening() {
            return this == SessionState.OPENING;
        }
    }

    private enum RequestType {
        GETTING_AUTHORIZATION_CODE,
        GETTING_ACCESS_TOKEN,
        REFRESHING_ACCESS_TOKEN;

        private boolean isAuthorizationCodeRequest() {
            return this == RequestType.GETTING_AUTHORIZATION_CODE;
        }

        private boolean isAccessTokenRequest() {
            return this == RequestType.GETTING_ACCESS_TOKEN;
        }

        private boolean isRefreshingTokenRequest() {
            return this == RequestType.REFRESHING_ACCESS_TOKEN;
        }
    }
}
