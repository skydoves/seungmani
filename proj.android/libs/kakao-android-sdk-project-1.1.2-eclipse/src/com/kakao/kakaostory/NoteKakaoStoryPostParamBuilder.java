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

/*
 * Param Name	        Param Type	필수      설명
 * content	            String  	o      내용
*/

/**
 * 글(Note) 스토리 포스트시 필요한 정보를 구성하는 Builder이다.
 * 내용이 필수 항목이다.
 * @author MJ
*/
public class NoteKakaoStoryPostParamBuilder extends BasicKakaoStoryPostParamBuilder{
    private String content;


    /**
     * 포스팅할 텍스트를 설정한다.
     * @param content 포스팅에 포함할 텍스트
     */
    public NoteKakaoStoryPostParamBuilder(final String content) {
        this.content = content;
    }

    /**
     * 지금까지 추가된 설정을 Bundle로 만들어준다.
     * @return 스토리 포스트 설정을 Bundle로 반환
     */
    public Bundle build() throws KakaoParameterException {
        if(content == null)
            throw new KakaoParameterException(ERROR_CODE.CORE_PARAMETER_MISSING, "setContent(String) is required.");
        final Bundle parameters = super.build();
        if(content != null)
            parameters.putString(ServerProtocol.CONTENT_KEY, content);
        return parameters;
    }
}
