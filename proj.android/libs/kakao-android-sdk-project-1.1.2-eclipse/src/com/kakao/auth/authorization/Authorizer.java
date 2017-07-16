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

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;

import com.kakao.auth.Session;


/**
 * @author MJ
 */
public abstract class Authorizer {
    private transient boolean hasInternetPermission;
    protected transient OnAuthorizationListener onAuthorizationListener;

    public interface OnAuthorizationListener {
        public void onAuthorizationCompletion(AuthorizationResult result);
    }

    protected boolean checkInternetPermission() {
        if (hasInternetPermission) {
            return true;
        }

        int permissionCheck = getApplicationContext().checkCallingOrSelfPermission(Manifest.permission.INTERNET);
        if (permissionCheck != PackageManager.PERMISSION_GRANTED) {
            doneOnOAuthError("This Operation needs INTERNET permission.");
            return false;
        } else {
            hasInternetPermission = true;
            return true;
        }
    }

    public Context getApplicationContext() {
        return Session.getCurrentSession().getContext();
    }

    protected void done(final AuthorizationResult result) {
        if (onAuthorizationListener != null) {
            onAuthorizationListener.onAuthorizationCompletion(result);
        }
    }

    protected abstract void doneOnOAuthError(final String errorMessage);

    public void setOnAuthorizationListener(OnAuthorizationListener onAuthorizationListener) {
        this.onAuthorizationListener = onAuthorizationListener;
    }

}
