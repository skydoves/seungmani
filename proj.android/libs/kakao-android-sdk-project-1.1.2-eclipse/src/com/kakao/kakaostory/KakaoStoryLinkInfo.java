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
import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonInclude.Include;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.kakao.util.helper.log.Logger;

import java.util.List;

/**
 * GetLinkInfo 요청 결과로 받는 객체로 해당 url의 타이틀, 이미지 등을 포함한다.
 * @author MJ
 */
@JsonInclude(Include.NON_NULL)
@JsonIgnoreProperties(ignoreUnknown=true)
public class KakaoStoryLinkInfo {
    private static ObjectMapper objectMapper = new ObjectMapper();

    /**
     * 요청시의 URL 원본. resolution을 하기 전의 URL
     */
    @JsonProperty("requested_url")
    private String requestURL;

    /**
     * Redirect 등을 거친 최종 도착지의 URL
     */
    @JsonProperty("url")
    private String url;

    /**
     * 요청한 주소의 Host 명
     */
    @JsonProperty("host")
    private String host;

    /**
     * 웹페이지 타이틀
     */
    @JsonProperty("title")
    private String title;

    /**
     * 해당 웹 페이지의 대표 이미지 주소. 최대 3개
     */
    @JsonProperty("image")
    private List<String> image;

    /**
     * 해당 웹 페이지의 설명
     */
    @JsonProperty("description")
    private String description;

    /**
     * @return 요청시의 URL 원본. resolution을 하기 전의 URL
     */
    public String getRequestURL() {
        return requestURL;
    }

    /**
     * @return  Redirect 등을 거친 최종 도착지의 URL
     */
    public String getUrl() {
        return url;
    }

    /**
     * @return 요청한 주소의 Host 명
     */
    public String getHost() {
        return host;
    }

    /**
     * @return 웹페이지 타이틀
     */
    public String getTitle() {
        return title;
    }

    /**
     * @return 해당 웹 페이지의 대표 이미지 주소. 최대 3개
     */
    public List<String> getImage() {
        return image;
    }

    /**
     * @return 해당 웹 페이지의 설명
     */
    public String getDescription() {
        return description;
    }

    /**
     * @return 스크랩 결과 해당 url로 링크 포스팅이 가능한지 여부
     */
    @JsonIgnore
    public boolean isValidResult(){
        return url != null && host != null;
    }

    @Override
    public String toString() {
        try {
            return objectMapper.writeValueAsString(this);
        } catch (JsonProcessingException e) {
            Logger.e(e);
            return null;
        }
    }
}

