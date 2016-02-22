﻿using ExampleApp;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Animation;
using System.Windows.Shapes;

namespace ExampleAppWPF
{
    public abstract class MenuView : Control
    {
        protected ControlClickHandler m_dragTabClickHandler = null;

        protected ListBox m_list = null;
        protected Button m_menuIcon;
        protected bool m_loggingEnabled = false;

        protected IntPtr m_nativeCallerPointer;

        protected double m_animationTimeMilliseconds = 200;

        protected Grid m_menuIconGrid;
        protected Grid m_menuViewContainer;
        protected Grid m_mainContainer;

        protected Storyboard m_openSearchIconAnim;
        protected Storyboard m_closeMenuIconAnim;

        protected Storyboard m_openSearchContainerAnim;
        protected Storyboard m_closeMenuContainerAnim;

        protected Storyboard m_openBackgroundRect;
        protected Storyboard m_closeBackgroundRect;
        protected Rectangle m_backgroundRectangle;

        protected MainWindow m_mainWindow;

        protected bool m_isAnimating;
        protected bool m_isOffScreen;
        protected float m_openState;

        protected double m_offScreenPos;
        protected double m_onScreenPos;

        protected const float MENU_CLOSED = 0.0f;
        protected const float MENU_OFFSCREEN = 0.1f;
        protected const float MENU_OPEN = 1.0f;
        protected const float MENU_CLOSING = 0.4f;
        protected const float MENU_OPENING = 0.5f;
        protected const float MENU_OFFSCREENING = 0.6f;

        private Action<object, EventArgs> m_animationCompleteCallback;

        protected abstract void RefreshListData(List<string> groups, List<bool> groupsExpandable, Dictionary<string, List<string>> groupToChildrenMap);

        static MenuView()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(MenuView), new FrameworkPropertyMetadata(typeof(MenuView)));
        }

        public MenuView(IntPtr nativeCallerPointer)
        {
            m_nativeCallerPointer = nativeCallerPointer;

            m_mainWindow = (MainWindow)Application.Current.MainWindow;

            m_openState = -1.0f;
            m_isAnimating = false;
            m_isOffScreen = true;
        }

        protected void PerformLayout(object sender, SizeChangedEventArgs e)
        {
            if (m_isOffScreen)
                m_menuViewContainer.RenderTransform = new TranslateTransform(m_offScreenPos, 0);
            else
                m_menuViewContainer.RenderTransform = new TranslateTransform(m_onScreenPos, 0);
        }

        protected void SetAnimationCompleteCallback(Action<object, EventArgs> callback)
        {
            m_animationCompleteCallback = callback;
        }
        
        public override void OnApplyTemplate()
        {
            base.OnApplyTemplate();
        }

        protected void OnDragTabMouseClick(object sender, MouseButtonEventArgs e)
        {
            if(IsAnimating())
            {
                return;
            }

            MenuViewCLIMethods.ViewClicked(m_nativeCallerPointer);
        }

        protected void OnDragTabMouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            e.Handled = true;
        }

        protected void OnDragTabMouseLeftButtonUp(object sender, MouseButtonEventArgs e)
        {
            e.Handled = false;
        }

        public void Destroy()
        {
            m_mainWindow.MainGrid.Children.Remove(this);
        }

        public virtual bool IsAnimating()
        {
            return m_isAnimating;
        }

        public virtual void UpdateAnimation(float deltaSeconds)
        {
            
        }

        void AnimateToCurrentPos(bool animationCompleteAndOpen)
        {
            m_list.IsHitTestVisible = animationCompleteAndOpen;
        }

        public virtual float NormalisedAnimationProgress()
        {
            double? progress = 0.0;

            if (m_openState == MENU_CLOSING)
            {
                progress = m_closeMenuIconAnim.GetCurrentProgress(m_menuIconGrid);

                progress = progress == null ? 0.0f : 1.0 - progress;
            }
            else if (m_openState == MENU_OPENING)
            {
                progress = m_openSearchIconAnim.GetCurrentProgress(m_menuIconGrid);

                progress = progress == null ? 0.0f : progress;
            }

            return (float)progress;
        }

        private void OnAnimCompleted(object sender, EventArgs e)
        {
            m_isAnimating = false;

            if (m_openState == MENU_CLOSING)
            {
                m_closeMenuIconAnim.Completed -= OnAnimCompleted;
                m_openState = MENU_CLOSED;

                MenuViewCLIMethods.ViewCloseCompleted(m_nativeCallerPointer);
                m_mainWindow.EnableInput();

                m_isOffScreen = false;
            }
            else if (m_openState == MENU_OPENING)
            {
                m_openSearchIconAnim.Completed -= OnAnimCompleted;
                m_openState = MENU_OPEN;

                MenuViewCLIMethods.ViewOpenCompleted(m_nativeCallerPointer);
                m_mainWindow.DisableInput();

                m_isOffScreen = false;
            }
            else if (m_openState == MENU_OFFSCREENING)
            {
                m_openState = MENU_OFFSCREEN;
                m_isOffScreen = true;
            }

            if (m_animationCompleteCallback != null)
            {
                m_animationCompleteCallback(sender, e);
            }
        }

        public virtual void AnimateToClosedOnScreen()
        {
            if (m_openState == MENU_CLOSED || m_openState == MENU_CLOSING)
                return;

            m_closeMenuIconAnim.Completed += OnAnimCompleted;
            m_closeMenuIconAnim.Begin(m_menuIconGrid, isControllable:true);
            m_closeMenuContainerAnim.Begin(m_mainContainer);
            m_closeBackgroundRect.Begin(m_backgroundRectangle);

            m_openState = MENU_CLOSING;
            m_isAnimating = true;

            m_backgroundRectangle.Visibility = Visibility.Visible;

            if (m_isOffScreen)
            {
                var mainGrid = (Application.Current.MainWindow as MainWindow).MainGrid;

                var screenWidth = mainGrid.ActualWidth;
                var totalWidth = m_mainContainer.ActualWidth + m_menuIcon.ActualWidth;

                var db = new DoubleAnimation(m_onScreenPos, TimeSpan.FromMilliseconds(m_animationTimeMilliseconds));

                try
                {
                    m_menuViewContainer.RenderTransform.BeginAnimation(TranslateTransform.XProperty, db);
                }
                catch (Exception e)
                {
                    e = null;
                }

                m_backgroundRectangle.Visibility = Visibility. Hidden;
            }
        }

        public virtual void AnimateToOpenOnScreen()
        {
            if (m_openState == MENU_OPEN || m_openState == MENU_OPENING)
                return;

            m_openSearchIconAnim.Completed += OnAnimCompleted;
            m_openSearchIconAnim.Begin(m_menuIconGrid, isControllable:true);
            m_openSearchContainerAnim.Begin(m_mainContainer);
            m_openBackgroundRect.Begin(m_backgroundRectangle);

            m_isAnimating = true;

            m_openState = MENU_OPENING;
            m_backgroundRectangle.Visibility = Visibility.Visible;
        }

        public virtual void AnimateOffScreen()
        {
            Dispatcher.Invoke(() =>
            {
                m_openState = MENU_OFFSCREENING;

                var db = new DoubleAnimation(m_offScreenPos, TimeSpan.FromMilliseconds(m_animationTimeMilliseconds));
                db.Completed += OnAnimCompleted;

                try
                {
                    m_menuViewContainer.RenderTransform.BeginAnimation(TranslateTransform.XProperty, db);
                }
                catch(Exception e)
                {

                }
            });
        }

        public void AnimateToIntermediateOnScreenState(float onScreenState)
        {
            if (IsAnimating())
            {
                return;
            }
        }

        public void AnimateToIntermediateOpenState(float openState)
        {
            if (IsAnimating())
            {
                return;
            }

            
        }

        public void PopulateData(
            IntPtr nativeCallerPointer,
            string[] groupNames,
            int[] groupSizes,
            bool[] groupIsExpandable,
            string[] childJson)
        {
            List<string> groups = groupNames.ToList();
            List<bool> groupsExpandable = groupIsExpandable.ToList();
            Dictionary<string, List<string>> childMap = new Dictionary<string, List<string>>();
            int childIndex = 0;
            for (int groupIndex = 0; groupIndex < groups.Count; groupIndex++)
            {
                int size = groupSizes[groupIndex];
                List<string> children = new List<string>();
                for (int i = 0; i < size; i++)
                {
                    children.Add(childJson[childIndex]);
                    childIndex++;
                }

                childMap[groupNames[groupIndex]] = children;
            }

            RefreshListData(groups, groupsExpandable, childMap);
        }

        protected bool CanInteract()
        {
            return IsClosed() || IsOpen();
        }

        protected bool IsClosed()
        {
            return m_openState == MENU_CLOSED;
        }

        protected bool IsOpen()
        {
            return m_openState == MENU_OPEN;
        }
    }
}
