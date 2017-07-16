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
package com.kakao.auth.authorization;

import android.os.Bundle;
import android.os.Parcel;
import android.os.Parcelable;

import com.kakao.auth.ApprovalType;
import com.kakao.auth.Session;
import com.kakao.auth.helper.ServerProtocol;


/**
 * authorization code을 얻어오는 요청
 *
 * @author MJ
 */
public class AuthRequestBase implements Parcelable {
    private String appKey;
    private String redirectURI;
    private Bundle extras;

    protected AuthRequestBase(final String appKey, final String redirectURI) {
        this.appKey = appKey;
        this.redirectURI = redirectURI;
    }

    protected AuthRequestBase() {
    }

    public void addApprovalType(final ApprovalType approvalType) {
        if(approvalType !=null) {
            addExtraString(ServerProtocol.APPROVAL_TYPE, approvalType.toString());
        }
    }

    protected void addExtraString(final String key, final String value){
        if(extras == null){
            extras = new Bundle();
        }
        extras.putString(key, value);
    }

    public int getRequestCode() {
        return Session.AUTHORIZATION_CODE_REQUEST;
    }

    public String getAppKey() {
        return appKey;
    }

    public String getRedirectURI() {
        return redirectURI;
    }

    public Bundle getExtras() { return extras; }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel parcel, int i) {
        parcel.writeString(appKey);
        parcel.writeString(redirectURI);
        parcel.writeBundle(extras);
    }

    public void readFromParcel(final Parcel parcel) {
        appKey = parcel.readString();
        redirectURI = parcel.readString();
        extras = parcel.readBundle();
    }

}
