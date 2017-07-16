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
package com.kakao.auth.authorization.authcode;

import android.annotation.SuppressLint;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.net.Uri;
import android.net.http.SslError;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.webkit.SslErrorHandler;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.Toast;

import com.kakao.android.sdk.R;
import com.kakao.auth.Session;
import com.kakao.auth.exception.KakaoWebviewException;
import com.kakao.auth.helper.ServerProtocol;
import com.kakao.auth.http.HttpRequestTask;
import com.kakao.util.exception.KakaoException;
import com.kakao.util.exception.KakaoException.ErrorType;
import com.kakao.util.helper.MethodInvoker;
import com.kakao.util.helper.log.Logger;

/**
 * @author MJ
 */
class KakaoWebViewDialog extends Dialog {
    private static final int DEFAULT_THEME = android.R.style.Theme_Translucent_NoTitleBar;

    private final String url;
    private final OnWebViewCompleteListener onCompleteListener;
    private WebView webView;
    private ProgressDialog spinner;
    private ImageView crossImageView;
    private FrameLayout contentFrameLayout;
    private boolean listenerCalled = false;
    private boolean isDetached = true;

    public KakaoWebViewDialog(final Context context, final String url, final OnWebViewCompleteListener listener) {
        super(context, DEFAULT_THEME);
        this.url = url;
        onCompleteListener = listener;
    }

    @Override
    public void dismiss() {
        if (webView != null) {
            webView.stopLoading();
        }
        if (!isDetached) {
            if (spinner.isShowing()) {
                spinner.dismiss();
            }
            super.dismiss();
        }
    }

    @Override
    public void onDetachedFromWindow() {
        super.onDetachedFromWindow();

        isDetached = true;
        onPauseWebView();
    }

    private void onPauseWebView() {
        if (webView == null) {
            return;
        }

        try {
            MethodInvoker.invoke(webView, "onPause");
        } catch (Exception e) {
            Logger.e(e);
        }

        webView.pauseTimers();
    }

    @Override
    public void onAttachedToWindow() {
        super.onAttachedToWindow();

        isDetached = false;
        onResumeWebView();
    }

    private void onResumeWebView() {
        if (webView == null) {
            return;
        }

        try {
            MethodInvoker.invoke(webView, "onResume");
        } catch (Exception e) {
            Logger.e(e);
        }

        webView.resumeTimers();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setOnCancelListener(new OnCancelListener() {
            @Override
            public void onCancel(DialogInterface dialogInterface) {
                sendCancelToListener();
            }
        });

        spinner = new ProgressDialog(getContext());
        spinner.requestWindowFeature(Window.FEATURE_NO_TITLE);
        spinner.setMessage(getContext().getString(R.string.core_com_kakao_sdk_loading));
        spinner.setOnCancelListener(new OnCancelListener() {
            @Override
            public void onCancel(DialogInterface dialogInterface) {
                sendCancelToListener();
                KakaoWebViewDialog.this.dismiss();
            }
        });

        requestWindowFeature(Window.FEATURE_NO_TITLE);
        contentFrameLayout = new FrameLayout(getContext());

        createCrossImage();
        int crossWidth = crossImageView.getDrawable().getIntrinsicWidth();
        setUpWebView(crossWidth / 2);

        contentFrameLayout.addView(crossImageView, new ViewGroup.LayoutParams(
                ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT));
        addContentView(contentFrameLayout,
                new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
    }

    private void sendSuccessToListener(String redirectURL) {
        if (onCompleteListener != null && !listenerCalled) {
            listenerCalled = true;
            onCompleteListener.onComplete(redirectURL, null);
        }
    }

    private void sendErrorToListener(Throwable error) {
        if (onCompleteListener != null && !listenerCalled) {
            listenerCalled = true;
            KakaoException kakaoException;
            if (error instanceof KakaoException) {
                kakaoException = (KakaoException) error;
            } else {
                kakaoException = new KakaoException(error);
            }
            onCompleteListener.onComplete(null, kakaoException);
        }
    }

    private void sendCancelToListener() {
        sendErrorToListener(new KakaoException(ErrorType.CANCELED_OPERATION, ""));
    }

    private void createCrossImage() {
        crossImageView = new ImageView(getContext());
        crossImageView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                sendCancelToListener();
                KakaoWebViewDialog.this.dismiss();
            }
        });
        Drawable crossDrawable = getContext().getResources().getDrawable(R.drawable.kakao_close_button);
        crossImageView.setImageDrawable(crossDrawable);
        crossImageView.setVisibility(View.INVISIBLE);
    }

    @SuppressLint("SetJavaScriptEnabled")
    private void setUpWebView(int margin) {
        // TODO xml 설정으로 대체가 안되나?
        LinearLayout webViewContainer = new LinearLayout(getContext());
        webView = new WebView(getContext());
        webView.setVerticalScrollBarEnabled(false);
        webView.setHorizontalScrollBarEnabled(false);
        webView.setWebViewClient(new DialogWebViewClient());
        webView.getSettings().setJavaScriptEnabled(true);
        webView.loadUrl(url, HttpRequestTask.KA_HEADER);
        webView.setLayoutParams(new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT));
        webView.setVisibility(View.INVISIBLE);
        webView.getSettings().setSavePassword(false);
        webViewContainer.setPadding(margin, margin, margin, margin);
        webViewContainer.addView(webView);
        contentFrameLayout.addView(webViewContainer);
    }

    private class DialogWebViewClient extends WebViewClient {
        @Override
        @SuppressWarnings("deprecation")
        public boolean shouldOverrideUrlLoading(WebView view, String url) {
            Logger.d("Redirect URL: " + url);

            // redirect uri 
            if(url.startsWith(Session.REDIRECT_URL_PREFIX) && url.contains(Session.REDIRECT_URL_POSTFIX)){
                sendSuccessToListener(url);
                dismiss();
            } 
            // 로그인창, 동의창
            else if(url.contains(ServerProtocol.AUTH_AUTHORITY) || url.contains(ServerProtocol.API_AUTHORITY)) {
                webView.loadUrl(url, HttpRequestTask.KA_HEADER);
            } else {
                //full browser!!!
                final Context context = getContext();
                try {
                    context.startActivity(new Intent(Intent.ACTION_VIEW, Uri.parse(url)));
                } catch (ActivityNotFoundException e) {
                    Toast.makeText(context, e.getLocalizedMessage(), Toast.LENGTH_SHORT).show();
                }
            }
            return true;
        }

        @Override
        public void onReceivedError(WebView view, int errorCode,
                                    String description, String failingUrl) {
            super.onReceivedError(view, errorCode, description, failingUrl);
            sendErrorToListener(new KakaoWebviewException(errorCode, description, failingUrl));
            dismiss();
        }

        @Override
        public void onReceivedSslError(WebView view, SslErrorHandler handler, SslError error) {
            super.onReceivedSslError(view, handler, error);

            sendErrorToListener(new KakaoWebviewException(ERROR_FAILED_SSL_HANDSHAKE, null, null));
            handler.cancel();
            dismiss();
        }

        @Override
        public void onPageStarted(WebView view, String url, Bitmap favicon) {
            Logger.d("Webview loading URL: " + url);
            super.onPageStarted(view, url, favicon);
            if (!isDetached) {
                spinner.show();
            }
        }

        @Override
        public void onPageFinished(WebView view, String url) {
            super.onPageFinished(view, url);
            if (!isDetached) {
                spinner.dismiss();
            }
            contentFrameLayout.setBackgroundColor(Color.TRANSPARENT);
            webView.setVisibility(View.VISIBLE);
            crossImageView.setVisibility(View.VISIBLE);
        }
    }
}
