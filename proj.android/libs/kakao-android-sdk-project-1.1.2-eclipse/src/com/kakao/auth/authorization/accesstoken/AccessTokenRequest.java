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

import android.content.Context;
import android.os.Parcel;
import android.os.Parcelable;

import com.kakao.auth.authorization.AuthRequestBase;
import com.kakao.util.helper.Utility;

/**
 * authorization code 또는 refresh token을 이용하여 access token을 얻어오는 요청
 * @author MJ
 */
public class AccessTokenRequest extends AuthRequestBase {
    private String keyHash;
    private String authorizationCode;
    private String refreshToken;

    public static AccessTokenRequest createRequestWithAuthorizationCode(final Context context,
                                                                        final String appKey, final String redirectURI, final String authorizationCode){
        return new AccessTokenRequest(context, appKey, redirectURI).setAuthorizationCode(authorizationCode);
    }

    public static AccessTokenRequest createRequestWithRefreshToken(final Context context,
                                                                   final String appKey, final String redirectURI, final String refreshToken){
        return new AccessTokenRequest(context, appKey, redirectURI)
            .setRefreshToken(refreshToken);
    }

    private AccessTokenRequest(final Context context, final String appKey, final String redirectURI) {
        super(appKey, redirectURI);
        this.keyHash = Utility.getKeyHash(context);
    }

    private AccessTokenRequest(final Parcel parcel) {
        readFromParcel(parcel);
    }

    public boolean isAccessTokenRequestWithAuthCode(){
        return authorizationCode != null;
    }

    public String getKeyHash() {
        return keyHash;
    }

    public String getAuthorizationCode() {
        return authorizationCode;
    }

    public String getRefreshToken() {
        return refreshToken;
    }

    private AccessTokenRequest setAuthorizationCode(final String authorizationCode) {
        this.authorizationCode = authorizationCode;
        return this;
    }

    private AccessTokenRequest setRefreshToken(final String refreshToken) {
        this.refreshToken = refreshToken;
        return this;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(final Parcel parcel, final int i) {
        super.writeToParcel(parcel, i);
        parcel.writeString(keyHash);
        parcel.writeString(authorizationCode);
        parcel.writeString(refreshToken);
    }

    public void readFromParcel(final Parcel parcel){
        super.readFromParcel(parcel);
        keyHash = parcel.readString();
        authorizationCode = parcel.readString();
        refreshToken = parcel.readString();
    }

    public static final Parcelable.Creator<AccessTokenRequest> CREATOR = new Parcelable.Creator<AccessTokenRequest>() {
        public AccessTokenRequest createFromParcel(Parcel in) {
            return new AccessTokenRequest(in);
        }

        public AccessTokenRequest[] newArray(int size) {
            return new AccessTokenRequest[size];
        }
    };
}
