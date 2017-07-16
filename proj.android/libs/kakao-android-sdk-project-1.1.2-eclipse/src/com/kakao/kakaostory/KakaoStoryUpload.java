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

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonIgnoreProperties;

/**
 * deprecated {@link com.kakao.kakaostory.KakaoStoryService#requestUpload(KakaoStoryHttpResponseHandler, java.io.File)}의 결과 값으로 더이상 사용되지 않는다.
 * 새로운 API인 {@link com.kakao.kakaostory.KakaoStoryService#requestMultiUpload(KakaoStoryHttpResponseHandler, java.util.List)} 는 String[]를 반환한다.
 * @author MJ
 */
@Deprecated
@JsonIgnoreProperties(ignoreUnknown = true)
public class KakaoStoryUpload {
    private String url;

    public String getUrl() {
        return url;
    }

    @JsonIgnore
    public boolean isValidResult() {
        return url != null;
    }

    @Override
    public String toString() {
        final StringBuilder sb = new StringBuilder("StoryUpload{");
        sb.append("url='").append(url).append('\'');
        sb.append('}');
        return sb.toString();
    }
}
