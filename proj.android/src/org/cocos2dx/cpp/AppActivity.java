package org.cocos2dx.cpp;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;
import org.cocos2dx.lib.Cocos2dxRenderer;

import com.skydoves.seungmani.*;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiManager.WifiLock;
//import android.R;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.PowerManager;
import android.os.PowerManager.WakeLock;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnKeyListener;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.EditText;
import android.widget.ProgressBar;
import android.widget.Toast;

import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdView;
import com.kakao.kakaolink.AppActionBuilder;
import com.kakao.kakaolink.KakaoLink;
import com.kakao.kakaolink.KakaoTalkLinkMessageBuilder;
import com.kakao.util.KakaoParameterException;

import com.parse.Parse;
import com.parse.PushService;

public class AppActivity extends Cocos2dxActivity
{
	//define global variables
	private Cocos2dxGLSurfaceView mGLView;
	static public Handler handler;
	private EditText edit01; // Nick Name Edit
	private WebView myWebView; // Virtual WebView
	private ProgressBar progressBar1;
	AdView ad; // Advertise View
    private KakaoLink mKakaoLink;
    private KakaoTalkLinkMessageBuilder mKakaoTalkLinkMessageBuilder;
	
	//C++ Functions - jni
    private native int MakeID(String data);
    private native int GetRank(int data1, int data2, String data3);
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        //Regist Parse.com
        Parse.initialize(this, "UjlvYaHQ0qT5lnZXpXORF4cnpXSIAjSpIVkYZrTV" , "i6Gr2HInX5YtkWPuiCxCmo1stZItdVMKaGxx59Hu");
        PushService.subscribe(this, "", AppActivity.class);
        PushService.setDefaultPushCallback((Context) getBaseContext(), AppActivity.class);   
        
        // Check Rooting
        try 
        {
           Runtime.getRuntime().exec("su");
        } 
        catch ( Exception e)  // If no Rooting mood, occur exception
        {
            //cocos2d-x layout
            mGLView = (Cocos2dxGLSurfaceView)findViewById(R.id.game_gl_surface);
            this.mGLView.setEGLContextClientVersion(2);
            this.mGLView.setCocos2dxRenderer(new Cocos2dxRenderer());
        }
        
        // set virtual WebView
        myWebView = (WebView) findViewById(R.id.myWebView);
        myWebView.getSettings().setJavaScriptEnabled(true);
        myWebView.addJavascriptInterface(this, "CommunicateApp");
        
        // set advertise
        ad = (AdView)findViewById(R.id.adView);
        AdRequest adRequest = new AdRequest.Builder().build();
        ad.loadAd(adRequest);
        
        //EditText0
        edit01  = (EditText)findViewById(R.id.edit01);
        progressBar1 = (ProgressBar)findViewById(R.id.progressBar1);           
        
        //edit01에 대한 키입력 처리
        edit01.setOnKeyListener(new OnKeyListener() {
			@Override
			public boolean onKey(View v, int keyCode, KeyEvent event)		
			{
				
				if((event.getAction() == KeyEvent.ACTION_DOWN) 
						&& (keyCode == KeyEvent.KEYCODE_ENTER))
				{
					//네트워크 상태를 확인한다
			    	ConnectivityManager connmgr = (ConnectivityManager)getSystemService(Context.CONNECTIVITY_SERVICE);
			    	NetworkInfo netinfo = connmgr.getActiveNetworkInfo();	

			    	if(netinfo != null && netinfo.isAvailable())	 	    
			    	{ 
			    		handler.sendEmptyMessage(2);
			    		newweb("http://skydoves.cafe24.com/seungmani/seungmani_Login.php?user_id=" + edit01.getText());
			    	}
			    	else
			    	 Toast.makeText(getBaseContext(), "네트워크 상태를 확인해주세요!",Toast.LENGTH_SHORT).show();
				}
				return false;
			}
		});
        
        //Hander(As Delegate)
        handler = new Handler()
        {
        public void handleMessage(Message msg)
        {
        	    //Name Edit - visible
        		if(msg.what == 0)
        	        edit01.setVisibility(View.VISIBLE);
     		
        		//Name Edit - invisible
        		else if(msg.what == 1)
        			edit01.setVisibility(View.GONE);
        		   		
        		//Prograssbar - visible
        		else if(msg.what == 2)
        			progressBar1.setVisibility(View.VISIBLE);
        		
        		//Prograssbar - invisible
        		else if(msg.what == 3)
        			progressBar1.setVisibility(View.GONE);
       
        		//Ranking - visible
        		else if(msg.what == 4)
        		{
					//네트워크 상태를 확인한다
			    	ConnectivityManager connmgr = (ConnectivityManager)getSystemService(Context.CONNECTIVITY_SERVICE);
			    	NetworkInfo netinfo = connmgr.getActiveNetworkInfo();	

			    	if(netinfo != null && netinfo.isAvailable())	 	    
			    	{ 
			    	newweb("http://skydoves.cafe24.com/seungmani/seungmani_Game1_GetRank.php");
			    	}
			    	else
				     Toast.makeText(getBaseContext(), "네트워크 상태가 좋지 않아요!",Toast.LENGTH_SHORT).show();
        		}
        		
        		// Set Advertise - Visible
        		else if(msg.what == 5)
        			ad.setVisibility(View.VISIBLE);
     
        		// Set Advertise - Gone
        		else if(msg.what == 6)
        			ad.setVisibility(View.GONE);
        		
        		// Kakao link
        		// Touchmani
        		else if(msg.what == 98)
        		{
        			String Data_String = msg.getData().getString("data_kakao");
        			String[] Datas;      			
        			Datas = Data_String.split("&");
        			
        	        InitKakaoLink();
        	        SendKakaoMessage(Datas[0], Datas[1]);
        		}
        		
        		// Catchmani
        		else if(msg.what == 99)
        		{
        			String Data_String = msg.getData().getString("data_kakao");
        			String[] Datas;      			
        			Datas = Data_String.split("&");
        			
        	        InitKakaoLink();
        	        SendKakaoMessage2(Datas[0], Datas[1], Datas[2]);
        		}
        		
        		 		
        		// Rank
        		else if(msg.what == 100)
        		{
					//네트워크 상태를 확인한다
			    	ConnectivityManager connmgr = (ConnectivityManager)getSystemService(Context.CONNECTIVITY_SERVICE);
			    	NetworkInfo netinfo = connmgr.getActiveNetworkInfo();	

			    	if(netinfo != null && netinfo.isAvailable())	 	    
			    	{ 
        			String Data_String = msg.getData().getString("data");
        			String[] Datas;
        			
        			Datas = Data_String.split("&");
        			
        			newweb("http://skydoves.cafe24.com/seungmani/seungmani_Game1_ScoreUpdate.php?user_id=" + Datas[2]+ "&MainChar=" + Datas[0] + "&Score=" + Datas[1]);
			    	}
			    	else
				    	 Toast.makeText(getBaseContext(), "네트워크 상태가 좋지않아 점수 등록에 실패했습니다.",Toast.LENGTH_SHORT).show();
			    	}
        }
        };
        
        
    }
    
    public void receive( int data)
    {
    	//이름이 이미 존재할 때
    	if(data == 0)
    	{
        handler.sendEmptyMessage(3); //Prograssbar Invisible
    	Toast.makeText(this, "이미 존재하는 이름입니다.", Toast.LENGTH_SHORT).show();
    	}
    	//이름이 존재 하지않고 유효한 경우 않는경우 - jni
    	else if(data == 1)
    	{
        Toast.makeText(this, "이름 생성완료!", Toast.LENGTH_SHORT).show();
        handler.sendEmptyMessage(1);
        handler.sendEmptyMessage(3); //Prograssbar Invisible
        MakeID(edit01.getText().toString());
    	}
    	
    	//이름이 존재 하지 않지만 문자가 유효하지 않은경우
    	else if(data == 2)
    	{
    	handler.sendEmptyMessage(3); //Prograssbar Invisible
        Toast.makeText(this, "만들수 없는 아이디 입니다!", Toast.LENGTH_SHORT).show();
    	}
    }
    
    public void receive_Game1_Data (int data1, int data2, String data3)
    {
    	GetRank(data1, data2, data3);
    }
    
    
    /*
    // Jni receive Functions
    */
    
    // Jni receive Function - <System>
    public static void jniFunc(int value)
    {
    	handler.sendEmptyMessage(value);
    }
    
    // Ganking System
    public static void jniFunc2(String value)
    {
    	Message msg = Message.obtain( handler, 100);
    	Bundle data = new Bundle();
 		data.putString("data", value);
 		msg.setData(data);
 		handler.sendMessage(msg); 
    }
    
    // kakao link
    // Touchmani
    public static void jniFunc3(String value)
    {
    	Message msg = Message.obtain( handler, 98);
    	Bundle data = new Bundle();
 		data.putString("data_kakao", value);
 		msg.setData(data);
 		handler.sendMessage(msg); 
    }
    
    // Catchmani
    public static void jniFunc4(String value)
    {
    	Message msg = Message.obtain( handler, 99);
    	Bundle data = new Bundle();
 		data.putString("data_kakao", value);
 		msg.setData(data);
 		handler.sendMessage(msg); 
    }
    
    /*
    private native int cppSum(int a, int b);
    
    protected void onResume() // onResume 어플의 완전 종료와 관계 없이 다시 열기만 하면 바로 실행되는 메소드.
    {
    	super.onResume();
    	
    	int sum = cppSum(1, 2);
    	
    	Toast.makeText(getBaseContext(), "총 합은 : " + sum,Toast.LENGTH_SHORT).show();
    }
    */
    
    
    /*
    // Kakao - link  Functions
    */
    private void InitKakaoLink(){
        try{
          mKakaoLink = KakaoLink.getKakaoLink(this);
          mKakaoTalkLinkMessageBuilder = mKakaoLink.createKakaoTalkLinkMessageBuilder();
        }catch(KakaoParameterException e){
          e.printStackTrace();
        }
      }
    // Touchmani
    private void SendKakaoMessage(String UserId, String Score){
        try{
          mKakaoTalkLinkMessageBuilder.addText("[승마니]\n현실을 이탈한 승마니의 이야기!\n" + UserId + " 님께서 " + Score + "점이나 달성하셨습니다!\n함께 참여해 보세요~><");
          mKakaoTalkLinkMessageBuilder.addImage("http://skydoves.cafe24.com/seungmani/img/background4.png", 300, 200);
          mKakaoTalkLinkMessageBuilder.addWebLink("구글 플레이 스토어", "https://play.google.com/store/apps/details?id=com.skydoves.seungmani");
          mKakaoTalkLinkMessageBuilder.addAppButton("앱으로 이동", new AppActionBuilder()
                                              .setAndroidExecuteURLParam("target=main")
                                              .setIOSExecuteURLParam("target=main", AppActionBuilder.DEVICE_TYPE.PHONE).build());
          mKakaoLink.sendMessage(mKakaoTalkLinkMessageBuilder.build(), this);
        }catch(KakaoParameterException e){
          e.printStackTrace();
        }
      }
    
    // Catchmani
    private void SendKakaoMessage2(String UserId, String Time, String Stage){
        try{
          mKakaoTalkLinkMessageBuilder.addText("[승마니]\n현실을 이탈한 승마니의 이야기!\n" + UserId + " 님께서 " + Stage + "번 스테이지를 " + Integer.parseInt(Time)/60+ "분 " + Integer.parseInt(Time)%60 + "초만에 클리어 하셨습니다! \n함께 참여해 보세요~><");
          mKakaoTalkLinkMessageBuilder.addImage("http://skydoves.cafe24.com/seungmani/img/background4.png", 300, 200);
          mKakaoTalkLinkMessageBuilder.addWebLink("구글 플레이 스토어", "https://play.google.com/store/apps/details?id=com.skydoves.seungmani");
          mKakaoTalkLinkMessageBuilder.addAppButton("앱으로 이동", new AppActionBuilder()
                                              .setAndroidExecuteURLParam("target=main")
                                              .setIOSExecuteURLParam("target=main", AppActionBuilder.DEVICE_TYPE.PHONE).build());
          mKakaoLink.sendMessage(mKakaoTalkLinkMessageBuilder.build(), this);
        }catch(KakaoParameterException e){
          e.printStackTrace();
        }
      }
    
    // 혹시나 웹뷰가 떠버리는 경우 대비
    public void newweb(String data)
    {
        WebSettings ws = myWebView.getSettings();
        ws.setSupportMultipleWindows(true);
          
         myWebView.setWebViewClient(new WebViewClient(){
             @Override
             public boolean shouldOverrideUrlLoading(WebView view, String url) {
                 // .... 내용 처리....
                 return super.shouldOverrideUrlLoading(view, url);
             }
         });
          
         //javascript의 _blank 웹뷰 하나에 계속 띄워주고 싶을때
         myWebView.setWebChromeClient(new WebChromeClient() {
             @Override
             public boolean onCreateWindow(WebView view, boolean dialog, boolean userGesture, android.os.Message resultMsg) {
          
                 WebView newWebView = new WebView(getBaseContext());
                  
                 WebView.WebViewTransport transport = (WebView.WebViewTransport)resultMsg.obj;
                 transport.setWebView(newWebView);
                 resultMsg.sendToTarget();
                  
                 return true;
             }
         });
  	   
     myWebView.loadUrl( data ); 	    
    }
     
}
