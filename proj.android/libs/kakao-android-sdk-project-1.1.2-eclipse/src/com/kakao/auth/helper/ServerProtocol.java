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
package com.kakao.auth.helper;

import com.kakao.util.helper.log.Logger.DeployPhase;

/**
 * @author MJ
 */
public final class ServerProtocol {
//    public static enum DeployPhase {
//        Local, Alpha, Sandbox, Beta, Release;
//
//        public static DeployPhase convert(String i) {
//            for (DeployPhase current : values()) {
//                if (current.toString().toLowerCase().equals(i)) {
//                    return current;
//                }
//            }
//            return Release;
//        }
//
//        public static DeployPhase current() {
//            return Release;
//        }
//    }

    private static final DeployPhase DEPLOY_PHASE = DeployPhase.current();
    public static final String AUTH_AUTHORITY = initAuthAuthority();
    public static final String API_AUTHORITY = initAPIAuthority();

    //Authorization: Bearer
    public static final String AUTHORIZATION_HEADER_KEY ="Authorization";
    public static final String AUTHORIZATION_BEARER ="Bearer";
    public static final String AUTHORIZATION_HEADER_DELIMITER = " ";

    // oauth url
    public static final String AUTHORIZE_CODE_PATH = "oauth/authorize";
    public static final String ACCESS_TOKEN_PATH = "oauth/token";

    //oauth param
    public static final String CLIENT_ID_KEY = "client_id";
    public static final String REDIRECT_URI_KEY = "redirect_uri";
    public static final String RESPONSE_TYPE_KEY = "response_type";
    public static final String GRANT_TYPE_KEY = "grant_type";
    public static final String CODE_KEY = "code";
    public static final String ACCESS_TOKEN_KEY = "access_token";
    public static final String REFRESH_TOKEN_KEY = "refresh_token";
    public static final String EXPIRES_AT_KEY = "expires_in";
    public static final String GRANT_TYPE_AUTHORIZATION_CODE = "authorization_code";
    public static final String ANDROID_KEY_HASH = "android_key_hash";
    public static final String APPROVAL_TYPE = "approval_type";

    // api url
    private static final String API_VERSION = "v1";
    // usermgmt
    public static final String USER_ME_PATH = API_VERSION + "/user/me";
    public static final String USER_LOGOUT_PATH = API_VERSION + "/user/logout";
    public static final String USER_SIGNUP_PATH = API_VERSION + "/user/signup";
    public static final String USER_UNLINK_PATH = API_VERSION + "/user/unlink";
    public static final String USER_UPDATE_PROFILE_PATH = API_VERSION + "/user/update_profile";
    public static final String USER_ACCESS_TOKEN_INFO_PATH = API_VERSION + "/user/access_token_info";
    // push
    public static final String PUSH_REGISTER_PATH = API_VERSION + "/push/register";
    public static final String PUSH_TOKENS_PATH = API_VERSION + "/push/tokens";
    public static final String PUSH_DEREGISTER_PATH = API_VERSION + "/push/deregister";
    public static final String PUSH_SEND_PATH = API_VERSION + "/push/send";

    // story
    //POST
    public static final String STORY_MULTI_UPLOAD_PATH = API_VERSION + "/api/story/upload/multi";
    public static final String STORY_POST_NOTE_PATH = API_VERSION + "/api/story/post/note";
    public static final String STORY_POST_PHOTO_PATH = API_VERSION + "/api/story/post/photo";
    public static final String STORY_POST_LINK_PATH = API_VERSION + "/api/story/post/link";
    //GET
    public static final String STORY_PROFILE_PATH = API_VERSION + "/api/story/profile";
    public static final String STORY_ACTIVITIES_PATH = API_VERSION + "/api/story/mystories";
    public static final String STORY_ACTIVITY_PATH = API_VERSION + "/api/story/mystory";
    public static final String STORY_LINK_SCRAPPER_PATH = API_VERSION + "/api/story/linkinfo";
    public static final String IS_STORY_USER_PATH = API_VERSION + "/api/story/isstoryuser";
    //DELETE
    public static final String STORY_DELETE_ACTIVITY_PATH = API_VERSION + "/api/story/delete/mystory";

    @Deprecated
    public static final String STORY_UPLOAD_PATH = API_VERSION + "/api/story/upload";
    @Deprecated
    public static final String STORY_POST_PATH = API_VERSION + "/api/story/post";

    //talk
    public static final String TALK_PROFILE_PATH = API_VERSION + "/api/talk/profile";

    // usermgmt param
    public static final String USER_ID_KEY ="id";
    public static final String PROPERTIES_KEY = "properties";
    public static final String PROPERTY_KEYS_KEY = "propertyKeys";
    public static final String SECURE_RESOURCE_KEY = "secure_resource";
    /**
     * Policy.RESERVED_PROPERTY_NAMES 와 match
     */
    public static final String NICK_NAME_KEY = "nickname";
    public static final String PROFILE_THUMBNAIL_IMAGE_KEY = "thumbnail_image";
    public static final String PROFILE_IMAGE_KEY = "profile_image";

    //push param
    public static final String DEVICE_ID_PARAM_NAME = "device_id";
    public static final String PUSH_TYPE_PARAM_NAME = "push_type";
    public static final String PUSH_TOKEN_PARAM_NAME = "push_token";
    public static final String PUSH_MESSAGE_PARAM_NAME = "push_message";
    public static final String PUSH_GCM_TYPE = "gcm";

    // story param
    public static final String BODY_ENCODING ="UTF-8";
    public static final String FILE_KEY ="file";
    public static final String CONTENT_KEY ="content";
    public static final String PERMISSION_KEY ="permission";
    public static final String ENABLE_SHARE_KEY ="enable_share";
    public static final String IMAGE_URL_LIST_KEY ="image_url_list";

    public static final String ANDROID_EXEC_PARAM_KEY ="android_exec_param";
    public static final String IOS_EXEC_PARAM_KEY ="ios_exec_param";
    public static final String ANDROID_MARKET_PARAM_KEY ="android_market_param";
    public static final String IOS_MARKET_PARAM_KEY ="ios_market_param";

    @Deprecated
    public static final String IMAGE_URL_KEY ="image_url";

    public static final String ACTIVITY_ID_PARAM_KEY ="id";
    public static final String LAST_ACTIVITY_ID_PARAM_KEY ="last_id";

    public static final String SCRAP_URL_PARAM_KEY ="url";
    public static final String LINK_INFO_PARAM_KEY ="link_info";

    private static String initAuthAuthority() {
        switch (DEPLOY_PHASE) {
            case Local:
                return "localhost:";
            case Alpha:
                return "alpha-kauth.kakao.com";
            case Sandbox:
                return "sandbox-kauth.kakao.com";
            case Beta:
                return "beta-kauth.kakao.com";
            case Release:
                return "kauth.kakao.com";
            default:
                return null;
        }
    }

    private static String initAPIAuthority() {
        switch (DEPLOY_PHASE) {
            case Local:
                return "localhost:";
            case Alpha:
                return "alpha-kapi.kakao.com";
            case Sandbox:
                return "sandbox-kapi.kakao.com";
            case Beta:
                return "beta-kapi.kakao.com";
            case Release:
                return "kapi.kakao.com";
            default:
                return null;
        }
    }

}
