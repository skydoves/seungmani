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

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;

/**
 * 카카오스토리 가입자 확인 요청의 결과 객체
 * @author MJ
 */
@JsonIgnoreProperties(ignoreUnknown = true)
public class KakaoStoryCheckUser {
    @JsonProperty("isStoryUser")
    private Boolean storyUser;

    // for jackson
    KakaoStoryCheckUser() {
    }

    /**
     * 카카오스토리 가입자 여부
     * @return 카카오스토리 가입자 여부
     */
    public boolean isStoryUser() {
        return storyUser;
    }

    @Override
    public String toString() {
        final StringBuilder sb = new StringBuilder("KakaoStoryCheckUser{");
        sb.append("storyUser=").append(storyUser);
        sb.append('}');
        return sb.toString();
    }
}
