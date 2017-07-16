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
package com.kakao.kakaostory;

import android.os.Bundle;

import com.kakao.auth.helper.ServerProtocol;
import com.kakao.util.KakaoParameterException;
import com.kakao.util.KakaoParameterException.ERROR_CODE;

/**
 * {@link com.kakao.kakaostory.NoteKakaoStoryPostParamBuilder} 또는 {@link com.kakao.kakaostory.PhotoKakaoStoryPostParamBuilder} 또는 {@link com.kakao.kakaostory.LinkKakaoStoryPostParamBuilder} 를 사용한다.
 * @author MJ
*/
@Deprecated
public class KakaoStoryPostParamBuilder {
    /**
     * 스토리 포스트시 공개 범위
     */
    @Deprecated
    public enum PERMISSION {
        /**
         * 전체 공개
         */
        PUBLIC("A"),
        /**
         * 친구 공개
         */
        FRIENDS("F");

        private final String value;

        PERMISSION(String value) {
            this.value = value;
        }
    }

    private final PERMISSION permission;
    private String content;
    private String imageURL;
    private String androidExecuteParam;
    private String iosExecuteParam;

    public KakaoStoryPostParamBuilder() {
        this(PERMISSION.FRIENDS);
    }

    public KakaoStoryPostParamBuilder(final PERMISSION permission) {
        this.permission = permission;
    }

    public void setContent(String content) {
        this.content = content;
    }

    public KakaoStoryPostParamBuilder setImageURL(final String imageURL) {
        this.imageURL = imageURL;
        return this;
    }

    public KakaoStoryPostParamBuilder setAndroidExecuteParam(final String androidExecuteParam) {
        this.androidExecuteParam = androidExecuteParam;
        return this;
    }

    public KakaoStoryPostParamBuilder setIOSExecuteParam(final String iosExecuteParam) {
        this.iosExecuteParam = iosExecuteParam;
        return this;
    }

    public Bundle build() throws KakaoParameterException {
        if(content == null && imageURL == null)
            throw new KakaoParameterException(ERROR_CODE.CORE_PARAMETER_MISSING, "Both content and imageURL are null or empty.");
        final Bundle parameters = new Bundle();
        parameters.putString(ServerProtocol.PERMISSION_KEY, permission.value);
        if(content != null)
            parameters.putString(ServerProtocol.CONTENT_KEY, content);
        if(imageURL != null)
            parameters.putString(ServerProtocol.IMAGE_URL_KEY, imageURL);
        if(androidExecuteParam != null)
            parameters.putString(ServerProtocol.ANDROID_EXEC_PARAM_KEY, androidExecuteParam);
        if(iosExecuteParam != null)
            parameters.putString(ServerProtocol.IOS_EXEC_PARAM_KEY, iosExecuteParam);
        return parameters;
    }
}
