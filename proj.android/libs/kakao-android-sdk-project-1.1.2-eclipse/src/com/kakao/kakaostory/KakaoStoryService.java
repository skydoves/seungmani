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
import com.kakao.auth.http.HttpRequestTask;
import com.kakao.auth.rest.APIHttpRequestTask;
import com.ning.http.client.AsyncHttpClient.BoundRequestBuilder;
import com.ning.http.multipart.FilePart;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.List;

/**
 * 카카오스토리 API 요청을 담당한다.
 * @author MJ
 */
public class KakaoStoryService {
    // GET
    /**
     * 카카오스토리 프로필 요청
     * @param responseHandler 프로필 요청 결과에 대한 handler
     */
    public static void requestProfile(final KakaoStoryHttpResponseHandler<KakaoStoryProfile> responseHandler) {
        requestProfile(responseHandler, null);
    }

    /**
     * 카카오스토리 프로필 요청
     * @param responseHandler 프로필 요청 결과에 대한 handler
     * @param secureResource 이미지 url을 https로 반환할지 여부
     */
    public static void requestProfile(final KakaoStoryHttpResponseHandler<KakaoStoryProfile> responseHandler, final Boolean secureResource) {
        String url = HttpRequestTask.createBaseURL(ServerProtocol.API_AUTHORITY, ServerProtocol.STORY_PROFILE_PATH);

        final Bundle params = new Bundle();
        if(secureResource != null) {
            params.putBoolean(ServerProtocol.SECURE_RESOURCE_KEY, secureResource);
        }

        APIHttpRequestTask.requestGet(responseHandler, KakaoStoryProfile.class, url, null);
    }
    /**
     * 지정한 id에 해당하는 카카오스토리 Activity 요청
     * @param responseHandler 액티비티 요청 결과에 대한 handler
     * @param parameters 얻어 오고자하는 activity id
     */
    public static void requestGetMyStory(final KakaoStoryHttpResponseHandler<MyStoryInfo> responseHandler, final Bundle parameters) {
        String url = HttpRequestTask.createBaseURL(ServerProtocol.API_AUTHORITY, ServerProtocol.STORY_ACTIVITY_PATH);
        APIHttpRequestTask.requestGet(responseHandler, MyStoryInfo.class, url, parameters);
    }

    /**
     * 지정한 id로부터 최신 약 18개 카카오스토리 Activity 요청
     * @param responseHandler 액티비티 요청 결과에 대한 handler
     * @param parameters 얻고자 하는 마지막 activity id (해당 id의 activity는 결과에 포함되지 않는다.)
     */
    public static void requestGetMyStories(final KakaoStoryHttpResponseHandler<MyStoryInfo[]> responseHandler, final Bundle parameters) {
        String url = HttpRequestTask.createBaseURL(ServerProtocol.API_AUTHORITY, ServerProtocol.STORY_ACTIVITIES_PATH);
        APIHttpRequestTask.requestGet(responseHandler, MyStoryInfo[].class, url, parameters);
    }

    /**
     * 카카오스토리에 링크 포스팅 요청전에 링크로 부터 정보를 얻어오는 과정.
     * @param responseHandler 스크랩 요청 결과에 대한 handler
     * @param linkUrl 스크랩 타켓 URL
     */
    public static void requestGetLinkInfo(final KakaoStoryHttpResponseHandler<KakaoStoryLinkInfo> responseHandler, final String linkUrl) {
        String url = HttpRequestTask.createBaseURL(ServerProtocol.API_AUTHORITY, ServerProtocol.STORY_LINK_SCRAPPER_PATH);

        final Bundle parameters = new Bundle();
        parameters.putString(ServerProtocol.SCRAP_URL_PARAM_KEY, linkUrl);

        APIHttpRequestTask.requestGet(responseHandler, KakaoStoryLinkInfo.class, url, parameters);
    }

    /**
     * 카카오스토리에 이미지 포스팅 요청전에 이미지 업로드 요청
     * @param responseHandler 이미지 업로드 요청 결과에 대한 handler
     * @param files 업로드할 이미지 파일들
     * @throws FileNotFoundException 업로드할 파일이 존재하는 않는 경우 발생한다.
     */
    public static void requestMultiUpload(final KakaoStoryHttpResponseHandler<String[]> responseHandler, final List<File> files) throws FileNotFoundException {
        String url = HttpRequestTask.createBaseURL(ServerProtocol.API_AUTHORITY, ServerProtocol.STORY_MULTI_UPLOAD_PATH);
        BoundRequestBuilder requestBuilder = HttpRequestTask.ASYNC_HTTP_CLIENT.preparePost(url);
        APIHttpRequestTask.addTokenHeader(requestBuilder);

        requestBuilder.setBodyEncoding(ServerProtocol.BODY_ENCODING);
        for(final File file : files) {
            requestBuilder.addBodyPart(new FilePart(ServerProtocol.FILE_KEY, file));
        }

        APIHttpRequestTask.checkSessionAndExecute(new APIHttpRequestTask<String[]>(requestBuilder.build(), responseHandler, String[].class));
    }

    /**
     * 카카오스토리에 포스팅 요청
     * @param type 포스팅 타입. {@link com.kakao.kakaostory.KakaoStoryService.StoryType}
     * @param responseHandler 포스팅 요청 결과에 대한 handler
     * @param parameters 포스트때 사용할 정보
     */
    public static void requestPost(final StoryType type, final KakaoStoryHttpResponseHandler<MyStoryInfo> responseHandler, final Bundle parameters) {
        String url = HttpRequestTask.createBaseURL(ServerProtocol.API_AUTHORITY, type.getRequestPath());
        APIHttpRequestTask.requestPost(responseHandler, MyStoryInfo.class, url, parameters);
    }

    /**
     * 현 사용자가 카카오스토리 가입자인지 확인하는 요청
     * @param responseHandler 카카오스토리 가입자 확인 요청 결과에 대한 handler
     */
    public static void requestIsStoryUser(final KakaoStoryHttpResponseHandler<KakaoStoryCheckUser> responseHandler) {
        String url = HttpRequestTask.createBaseURL(ServerProtocol.API_AUTHORITY, ServerProtocol.IS_STORY_USER_PATH);
        APIHttpRequestTask.requestGet(responseHandler, KakaoStoryCheckUser.class, url, null);
    }

    /**
     * 지정한 id에 해당하는 카카오스토리 Activity 삭제 요청
     * @param responseHandler 액티비티 요청 결과에 대한 handler
     * @param parameters 삭제하고자하는 activity id
     */
    public static void requestDeleteMyStory(final KakaoStoryHttpResponseHandler<Void> responseHandler, final Bundle parameters) {
        String url = HttpRequestTask.createBaseURL(ServerProtocol.API_AUTHORITY, ServerProtocol.STORY_DELETE_ACTIVITY_PATH);
        APIHttpRequestTask.requestDelete(responseHandler, Void.class, url, parameters);
    }

    /**
     * {@link #requestMultiUpload(KakaoStoryHttpResponseHandler, java.util.List)} 를 사용하도록 한다.
     */
    @Deprecated
    public static void requestUpload(final KakaoStoryHttpResponseHandler<KakaoStoryUpload> responseHandler, final File file) throws FileNotFoundException {
        String url = HttpRequestTask.createBaseURL(ServerProtocol.API_AUTHORITY, ServerProtocol.STORY_UPLOAD_PATH);
        BoundRequestBuilder requestBuilder = HttpRequestTask.ASYNC_HTTP_CLIENT.preparePost(url);
        APIHttpRequestTask.addTokenHeader(requestBuilder);

        requestBuilder.setBodyEncoding(ServerProtocol.BODY_ENCODING);
        requestBuilder.addBodyPart(new FilePart(ServerProtocol.FILE_KEY, file));

        APIHttpRequestTask.checkSessionAndExecute(new APIHttpRequestTask<KakaoStoryUpload>(requestBuilder.build(), responseHandler, KakaoStoryUpload.class));
    }

    /**
     * {@link #requestPost(com.kakao.kakaostory.KakaoStoryService.StoryType, KakaoStoryHttpResponseHandler, android.os.Bundle)}를 사용하도록 하낟.
     */
    @Deprecated
    public static void requestPost(final KakaoStoryHttpResponseHandler<Void> responseHandler, final Bundle parameters) {
        String url = HttpRequestTask.createBaseURL(ServerProtocol.API_AUTHORITY, ServerProtocol.STORY_POST_PATH);
        APIHttpRequestTask.requestPost(responseHandler, Void.class, url, parameters);
    }

    /**
     * 스토리 포스팅의 종류를 나타낸다.
     */
    public enum StoryType {
        /**
         * Note 타입의 스토리로 text가 꼭 포함되어야 한다.
         */
        NOTE("NOTE", ServerProtocol.STORY_POST_NOTE_PATH),
        /**
         * Photo 타입의 스토리로, 이미지를 먼저 업로드 한후에 이미지 url을 꼭 포함되어야 한다.
         */
        PHOTO("PHOTO", ServerProtocol.STORY_POST_PHOTO_PATH),
        /**
         * Link 타입의 스토리로, url로 부터 먼저 정보를 가지고 온 후, 그 결과가 포함되어야 한다.
         */
        LINK("LINK", ServerProtocol.STORY_POST_LINK_PATH),

        NOT_SUPPORTED("NOT_SUPPORTED", null);

        private final String requestPath;
        private final String name;

        StoryType(final String name, final String requestPath) {
            this.name = name;
            this.requestPath = requestPath;
        }

        String getRequestPath() {
            return requestPath;
        }

        protected static StoryType getType(final String name){
            for(final StoryType type : StoryType.values()){
                if(type.name.equals(name))
                    return type;
            }
            return NOT_SUPPORTED;
        }
    }

}
