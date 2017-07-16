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
import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonInclude.Include;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.kakao.kakaostory.KakaoStoryService.StoryType;

import java.util.Arrays;

/**
 * 내스토리 요청에 대한 결과 객체로 스토리 정보가 담겨 있다.
 * @author MJ
 */
@JsonInclude(Include.NON_NULL)
@JsonIgnoreProperties(ignoreUnknown = true)
public class MyStoryInfo {
    @JsonProperty("id")
    private String id;
    @JsonProperty("url")
    private String url;
    @JsonProperty("media_type")
    private String mediaType;
    @JsonProperty("created_at")
    private String createdAt;

    @JsonProperty("comment_count")
    private Integer commentCount;
    @JsonProperty("like_count")
    private Integer likeCount;
    @JsonProperty("comments")
    private StoryComment[] comments;
    @JsonProperty("likes")
    private StoryLike[] likes;

    @JsonProperty("content")
    private String content;
    @JsonProperty("media")
    private MyStoryImageInfo[] medias;
    @JsonProperty("permission")
    private String permission;

    /**
     * @return 내스토리 고유 id
     */
    public String getId() {
        return id;
    }

    /**
     * @return 상세글로 바로 갈 수 있는  web url
     */
    public String getUrl() {
        return url;
    }

    /**
     * @return 내스토리의 타입. NOTE 또는 PHOTO 또는 NOT_SUPPORTED
     */
    public StoryType getMediaType() {
        return StoryType.getType(mediaType);
    }

    /**
     * @return 포스팅한 시각. yyyy-mm-ddThh:mm:ss
     */
    public String getCreatedAt() {
        return createdAt;
    }

    /**
     * @return 댓글수
     */
    public Integer getCommentCount() {
        return commentCount;
    }

    /**
     * @return 느낌수
     */
    public Integer getLikeCount() {
        return likeCount;
    }

    /**
     * @return 댓글 상세 내용. activities에는 포함되지 않는다. 개별 activity 요청시에만 포함된다.
     */
    public StoryComment[] getComments() {
        return comments;
    }

    /**
     * @return 느낌 상세 내용. activities에는 포함되지 않는다. 개별 activity 요청시에만 포함된다.
     */
    public StoryLike[] getLikes() {
        return likes;
    }

    /**
     * @return 내스토리의 텍스트 내용
     */
    public String getContent() {
        return content;
    }


    /**
     * @return {@link com.kakao.kakaostory.KakaoStoryService.StoryType#PHOTO} 스토리에 한해 이미지 파일 URL list
     */
    public MyStoryImageInfo[] getMedias() {
        return medias;
    }

    /**
     * @return 공개 범위 종류
     */
    public StoryPermission getPermission() {
        return StoryPermission.getPermission(permission);
    }

    /**
     * 내스토리의 이미지를 크기별로 얻을 수 있는 객체.
     */
    @JsonInclude(Include.NON_NULL)
    @JsonIgnoreProperties(ignoreUnknown = true)
    public static class MyStoryImageInfo {
        @JsonProperty("original")
        private String original;
        @JsonProperty("xlarge")
        private String xlarge;
        @JsonProperty("large")
        private String large;
        @JsonProperty("medium")
        private String medium;
        @JsonProperty("small")
        private String small;

        /**
         * @return 포스팅한 원본 이미지
         */
        public String getOriginal() {
            return original;
        }

        /**
         * @return 1280 * 1706 리사이징한 이미지
         */
        public String getXlarge() {
            return xlarge;
        }

        /**
         * @return 720 * 960 리사이징한 이미지
         */
        public String getLarge() {
            return large;
        }

        /**
         * @return 240 * 320 리사이징한 이미지
         */
        public String getMedium() {
            return medium;
        }

        /**
         * @return 160 * 213 리사이징한 이미지
         */
        public String getSmall() {
            return small;
        }

        @Override
        public String toString() {
            final StringBuilder sb = new StringBuilder("KakaoStoryActivityImage{");
            sb.append("original='").append(original).append('\'');
            sb.append(", xlarge='").append(xlarge).append('\'');
            sb.append(", large='").append(large).append('\'');
            sb.append(", medium='").append(medium).append('\'');
            sb.append(", small='").append(small).append('\'');
            sb.append('}');
            return sb.toString();
        }
    }

    /**
     * 덧글 또는 느낌 작성자 정보를 얻을 수 있는 객체
     */
    @JsonInclude(Include.NON_NULL)
    @JsonIgnoreProperties(ignoreUnknown=true)
    public static class StoryActor {
        @JsonProperty("display_name")
        private String displayName;
        @JsonProperty("profile_thumbnail_url")
        private String profileThumbnailUrl;

        /**
         * @return 작성자의 카카오스토리 별명.
         */
        public String getDisplayName() {
            return displayName;
        }

        /**
         * @return 작성자의 카카오스토리 썸네일 프로필 이미지 URL. 160px * 160px 크기
         */
        public String getProfileThumbnailUrl() {
            return profileThumbnailUrl;
        }

        @Override
        public String toString() {
            final StringBuilder sb = new StringBuilder("StoryActor{");
            sb.append("displayName='").append(displayName).append('\'');
            sb.append(", profileThumbnailUrl='").append(profileThumbnailUrl).append('\'');
            sb.append('}');
            return sb.toString();
        }
    }

    /**
     * 내스토리의 덧글 정보를 얻을 수 있는 객체.
     */
    @JsonInclude(Include.NON_NULL)
    @JsonIgnoreProperties(ignoreUnknown=true)
    public static class StoryComment {
        @JsonProperty("text")
        private String text;
        @JsonProperty("writer")
        private StoryActor writer;

        /**
         * @return 댓글 내용
         */
        public String getText() {
            return text;
        }

        /**
         * @return 댓글 작성자 정보
         */
        public StoryActor getWriter() {
            return writer;
        }

        @Override
        public String toString() {
            final StringBuilder sb = new StringBuilder("StoryComment{");
            sb.append("text='").append(text).append('\'');
            sb.append(", writer=").append(writer);
            sb.append('}');
            return sb.toString();
        }
    }

    /**
     * 내스토리의 느낌 정보를 얻을 수 있는 객체.
     */
    @JsonInclude(Include.NON_NULL)
    @JsonIgnoreProperties(ignoreUnknown=true)
    public static class StoryLike {
        @JsonProperty("emotion")
        private String emotion;
        @JsonProperty("actor")
        private StoryActor actor;

        /**
         * 느낌 값
         */
        public static enum Emotion {
            /**
             * 좋아요
             */
            LIKE("LIKE"),
            /**
             * 멋져요
             */
            COOL("COOL"),
            /**
             * 기뻐요
             */
            HAPPY("HAPPY"),
            /**
             * 슬퍼요
             */
            SAD("SAD"),
            /**
             * 힘내요
             */
            CHEER_UP("CHEER_UP"),
            /**
             * 정의되지 않은 느낌
             */
            NOT_DEFINED("NOT_DEFINED");

            final String papiEmotion;
            Emotion(final String papiEmotion) {
                this.papiEmotion = papiEmotion;
            }

            public static Emotion getEmotion(final String emotionString){
                for(Emotion emotion : Emotion.values()){
                    if(emotion.papiEmotion.equals(emotionString))
                        return emotion;
                }
                return NOT_DEFINED;
            }
        }

        /**
         * @return 느낌 종류
         */
        public Emotion getEmotion() {
            return Emotion.getEmotion(emotion);
        }

        /**
         * @return 느낌 작성자 정보
         */
        public StoryActor getActor() {
            return actor;
        }

        @Override
        public String toString() {
            final StringBuilder sb = new StringBuilder("StoryLike{");
            sb.append("emotion='").append(emotion).append('\'');
            sb.append(", actor=").append(actor);
            sb.append('}');
            return sb.toString();
        }
    }

    /**
     * 공개 범위 값
     */
    public static enum StoryPermission {
        /**
         * 알수 없는 공개 범위
         */
        UNKNOWN("UNKNOWN"),
        /**
         * 전체공개
         */
        PUBLIC("PUBLIC"),
        /**
         * 친구공개
         */
        FRIEND("FRIEND"),
        /**
         * 나만보기
         */
        ONLY_ME("ONLY_ME");


        final String name;
        StoryPermission(final String name) {
            this.name = name;
        }

        public static StoryPermission getPermission(final String name){
            for(StoryPermission permission : StoryPermission.values()){
                if(permission.name.equals(name))
                    return permission;
            }
            return UNKNOWN;
        }
    }

    @Override
    public String toString() {
        final StringBuilder sb = new StringBuilder("MyStoryInfo{");
        sb.append("id='").append(id).append('\'');
        sb.append(", url='").append(url).append('\'');
        sb.append(", mediaType='").append(mediaType).append('\'');
        sb.append(", createdAt='").append(createdAt).append('\'');
        sb.append(", commentCount=").append(commentCount);
        sb.append(", likeCount=").append(likeCount);
        sb.append(", comments=").append(Arrays.toString(comments));
        sb.append(", likes=").append(Arrays.toString(likes));
        sb.append(", content='").append(content).append('\'');
        sb.append(", medias=").append(Arrays.toString(medias));
        sb.append(", permission='").append(permission).append('\'');
        sb.append('}');
        return sb.toString();
    }
}

