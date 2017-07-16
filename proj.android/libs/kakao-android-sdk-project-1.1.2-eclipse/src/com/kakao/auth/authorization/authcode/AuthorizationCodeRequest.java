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

import android.os.Parcel;
import android.os.Parcelable;

import com.kakao.auth.authorization.AuthRequestBase;

/**
 * authorization code을 얻어오는 요청
 *
 * @author MJ
 */
public class AuthorizationCodeRequest extends AuthRequestBase {

    public static AuthorizationCodeRequest createNewRequest(final String appKey, final String redirectURI) {
        return new AuthorizationCodeRequest(appKey, redirectURI);
    }

    private AuthorizationCodeRequest(final String appKey, final String redirectURI) {
        super(appKey, redirectURI);
    }

    private AuthorizationCodeRequest(final Parcel parcel){
        readFromParcel(parcel);
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel parcel, int i) {
        super.writeToParcel(parcel, i);
    }

    public void readFromParcel(final Parcel parcel) {
        super.readFromParcel(parcel);
    }

    public static final Parcelable.Creator<AuthorizationCodeRequest> CREATOR = new Parcelable.Creator<AuthorizationCodeRequest>() {
        public AuthorizationCodeRequest createFromParcel(Parcel in) {
            return new AuthorizationCodeRequest(in);
        }

        public AuthorizationCodeRequest[] newArray(int size) {
            return new AuthorizationCodeRequest[size];
        }
    };

}
