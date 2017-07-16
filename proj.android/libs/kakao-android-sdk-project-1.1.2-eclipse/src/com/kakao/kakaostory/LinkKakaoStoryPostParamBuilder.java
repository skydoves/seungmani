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
 * @author MJ
 */
/*
 * Param Name   Param Type	필수      설명
 * link_info    String      o       스크랩결과
 * content	    String  	x       내용
 */

/**
 * 링크(Link) 스토리 포스트시 필요한 정보를 구성하는 Builder이다.
 * url로부터 얻어온 정보에서 url과 host값이 필수로 포함되어 있어야 한다.
 */
public class LinkKakaoStoryPostParamBuilder extends BasicKakaoStoryPostParamBuilder {

    private String content;
    private KakaoStoryLinkInfo linkInfo;


    /**
     * 스크랩 결과를 포스팅하기 위해 정보를 넘긴다.
     * @param kakaoStoryLinkInfo 스크랩 요청 결과
     */
    public LinkKakaoStoryPostParamBuilder(final KakaoStoryLinkInfo kakaoStoryLinkInfo) {
        super();
        this.linkInfo = kakaoStoryLinkInfo;
    }

    /**
     * 포스팅에 텍스트를 추가한다.
     * @param content 포스팅에 포함할 텍스트
     */
    public LinkKakaoStoryPostParamBuilder setContent(final String content) {
        this.content = content;
        return this;
    }

    /**
     * 지금까지 추가된 설정을 Bundle로 만들어준다.
     * @return 스토리 포스트 설정을 Bundle로 반환
     */
    public Bundle build() throws KakaoParameterException {
        final Bundle parameters = super.build();

        if(linkInfo == null || !linkInfo.isValidResult())
            throw new KakaoParameterException(ERROR_CODE.CORE_PARAMETER_MISSING, "Both url and host of KakaoStoryLinkInfo are required. linkInfo=" + linkInfo);

        parameters.putString(ServerProtocol.LINK_INFO_PARAM_KEY, linkInfo.toString());

        if(content != null)
            parameters.putString(ServerProtocol.CONTENT_KEY, content);
        return parameters;
    }

}
