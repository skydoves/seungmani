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

import org.json.JSONArray;

/*
 * Param Name	        Param Type	필수      설명
 * imageURLs	        String  	o       이미지 URL 리스트
 * content	            String  	x       내용
*/

/**
 * 사진(Photo) 스토리 포스트시 필요한 정보를 구성하는 Builder이다.
 * 사진은 필수 항목이다.
 * @author MJ
*/
public class PhotoKakaoStoryPostParamBuilder extends BasicKakaoStoryPostParamBuilder{
    private String content;
    private final JSONArray imageURLs = new JSONArray();

    /**
     * 업로드 결과를 받아 이미지 url을 설정해 준다.
     * @param imageURLs 업로드 결과
     */
    public PhotoKakaoStoryPostParamBuilder (final String[] imageURLs) {
        for(final String imagePath : imageURLs)
            this.imageURLs.put(imagePath);
    }

    /**
     * 포스팅에 텍스트를 추가한다.
     * @param content 포스팅에 포함할 텍스트
     */
    public PhotoKakaoStoryPostParamBuilder setContent(final String content) {
        this.content = content;
        return this;
    }

    /**
     * 지금까지 추가된 설정을 Bundle로 만들어준다.
     * @return 스토리 포스트 설정을 Bundle로 반환
     */
    public Bundle build() throws KakaoParameterException {
        if(imageURLs.length() == 0)
            throw new KakaoParameterException(ERROR_CODE.CORE_PARAMETER_MISSING, "addImageURL(String) is required.");
        final Bundle parameters = super.build();
        if(content != null)
            parameters.putString(ServerProtocol.CONTENT_KEY, content);
        if(imageURLs.length() != 0)
            parameters.putString(ServerProtocol.IMAGE_URL_LIST_KEY, imageURLs.toString());
        return parameters;
    }
}
