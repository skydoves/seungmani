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

/*
 * Param Name	        Param Type	필수      설명
 * last_id	            String  	o      얻고 싶은 스토리 중에 마지막 스토리 id (exclude)
*/

/**
 * 내스토리 정보 요청시 필요한 정보를 구성하는 Builder이다.
 * id가 필수 항목이다.
 * @author MJ
*/
public class KakaoStoryMyStoriesParamBuilder {
    private String lastId;


    /**
     * 설정한 스토리 아이디 바로 전 내스토리까지 반환된다.
     * 즉, 설정한 이 스토리 아이디에 해당하는 내스토리 정보는 반환되지 않는다.
     * @param lastId 정보를 원하는 마지막 내스토리 id
     */
    public KakaoStoryMyStoriesParamBuilder(final String lastId) {
        this.lastId = lastId;
    }

    /**
     * 지금까지 추가된 설정을 Bundle로 만들어준다.
     * @return 내스토리 요청 설정을 Bundle로 반환
     */
    public Bundle build() {
        if(lastId == null)
            return null;
        final Bundle parameters = new Bundle();
        parameters.putString(ServerProtocol.LAST_ACTIVITY_ID_PARAM_KEY, lastId);
        return parameters;
    }
}
