// Copyright eeGeo Ltd (2012-2014), All Rights Reserved

#include "MyPinCreationConfirmationViewController.h"
#include "MyPinCreationConfirmationViewModel.h"
#include "IMyPinCreationModel.h"
#include "MyPinCreationStage.h"
#include "IMyPinCreationDetailsViewModel.h"
#include "AndroidAppThreadAssertionMacros.h"
#include "MyPinCreationViewStateChangedMessage.h"

namespace ExampleApp
{
    namespace MyPinCreation
    {
    	MyPinCreationConfirmationViewController::MyPinCreationConfirmationViewController(
    			AndroidNativeState& nativeState,
    			MyPinCreation::IMyPinCreationModel& model,
  				MyPinCreation::IMyPinCreationConfirmationViewModel& viewModel,
  				MyPinCreationDetails::IMyPinCreationDetailsViewModel& detailsViewModel,
				ExampleAppMessaging::UiToNativeMessageBus& uiToNativeMessageBus
  			)
    	: m_nativeState(nativeState)
       	, m_model(model)
    	, m_viewModel(viewModel)
    	, m_detailsViewModel(detailsViewModel)
    	, m_uiToNativeMessageBus(uiToNativeMessageBus)
    	, m_pOnScreenStateChangedCallback(Eegeo_NEW((
    	    			Eegeo::Helpers::TCallback2<MyPinCreationConfirmationViewController, ScreenControlViewModel::IScreenControlViewModel&, float>))(this, &MyPinCreationConfirmationViewController::OnScreenStateChangedCallback))
    	{
    		ASSERT_UI_THREAD

			m_viewModel.InsertOnScreenStateChangedCallback(*m_pOnScreenStateChangedCallback);

			AndroidSafeNativeThreadAttachment attached(m_nativeState);
			JNIEnv* env = attached.envForThread;

			jstring strClassName = env->NewStringUTF("com.eegeo.mypincreation.MyPinCreationConfirmationView");
			jclass uiClass = m_nativeState.LoadClass(env, strClassName);
			env->DeleteLocalRef(strClassName);

			m_uiViewClass = static_cast<jclass>(env->NewGlobalRef(uiClass));
			jmethodID uiViewCtor = env->GetMethodID(m_uiViewClass, "<init>", "(Lcom/eegeo/mobileexampleapp/MainActivity;J)V");

			jobject instance = env->NewObject(
				m_uiViewClass,
				uiViewCtor,
				m_nativeState.activity,
				(jlong)(this)
			);

			m_uiView = env->NewGlobalRef(instance);
    	}

    	MyPinCreationConfirmationViewController::~MyPinCreationConfirmationViewController()
    	{
    		ASSERT_UI_THREAD

    		m_viewModel.RemoveOnScreenStateChangedCallback(*m_pOnScreenStateChangedCallback);

			Eegeo_DELETE m_pOnScreenStateChangedCallback;

			AndroidSafeNativeThreadAttachment attached(m_nativeState);
			JNIEnv* env = attached.envForThread;
			jmethodID removeHudMethod = env->GetMethodID(m_uiViewClass, "destroy", "()V");
			env->CallVoidMethod(m_uiView, removeHudMethod);
			env->DeleteGlobalRef(m_uiView);
			env->DeleteGlobalRef(m_uiViewClass);
    	}

    	void MyPinCreationConfirmationViewController::HandleCancelSelected()
    	{
    		ASSERT_UI_THREAD

    		m_uiToNativeMessageBus.Publish(MyPinCreationViewStateChangedMessage(ExampleApp::MyPinCreation::Inactive));
    	}

    	void MyPinCreationConfirmationViewController::HandleConfirmSelected()
		{
    		ASSERT_UI_THREAD

    		m_uiToNativeMessageBus.Publish(MyPinCreationViewStateChangedMessage(ExampleApp::MyPinCreation::Details));
			m_viewModel.RemoveFromScreen();
			m_detailsViewModel.Open();
		}

    	void MyPinCreationConfirmationViewController::OnScreenStateChangedCallback(ScreenControlViewModel::IScreenControlViewModel& viewModel, float& onScreenState)
    	{
    		ASSERT_UI_THREAD

			AndroidSafeNativeThreadAttachment attached(m_nativeState);
			JNIEnv* env = attached.envForThread;

			if(m_viewModel.IsFullyOnScreen())
			{
				jmethodID animateToClosedOnScreen = env->GetMethodID(m_uiViewClass, "animateToActive", "()V");
				env->CallVoidMethod(m_uiView, animateToClosedOnScreen);
			}
			else if (m_viewModel.IsFullyOffScreen())
			{
				jmethodID animateOffScreen = env->GetMethodID(m_uiViewClass, "animateToInactive", "()V");
				env->CallVoidMethod(m_uiView, animateOffScreen);
			}
			else
			{
				jmethodID animateToIntermediateOpenStateOnScreen = env->GetMethodID(m_uiViewClass, "animateToIntermediateOnScreenState", "(F)V");
				env->CallVoidMethod(m_uiView, animateToIntermediateOpenStateOnScreen, onScreenState);
			}
    	}
    }
}
